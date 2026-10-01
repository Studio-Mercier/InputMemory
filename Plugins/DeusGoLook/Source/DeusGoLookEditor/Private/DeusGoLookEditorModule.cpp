#include "DeusGoLookGroupCustomization.h"
#include "DeusGoLookSettings.h"
#include "DeusGoLookTypes.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Notifications/NotificationManager.h"
#include "HAL/IConsoleManager.h"
#include "IDetailsView.h"
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Styling/AppStyle.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#define LOCTEXT_NAMESPACE "DeusGoLookEditor"

namespace DeusGoLookEditor
{
	static const FName PanelTabName("DeusGoLookPanel");

	static FSlateIcon GetIcon()
	{
		return FSlateIcon(FAppStyle::GetAppStyleSetName(), "ShowFlagsMenu.Fog");
	}

	static UDeusGoLookSettings& Settings()
	{
		return *GetMutableDefault<UDeusGoLookSettings>();
	}

	static void OpenRenderingSettings()
	{
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->ShowViewer("Project", "Engine", "Rendering");
		}
	}

	// neon reads the custom stencil, which is off unless r.CustomDepth is 3
	static void WarnIfStencilOff()
	{
		static bool bWarned = false;
		if (bWarned || !Settings().Style.Neon.bNeon)
		{
			return;
		}

		const IConsoleVariable* CustomDepth = IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepth"));
		if (!CustomDepth || CustomDepth->GetInt() == 3)
		{
			return;
		}

		bWarned = true;
		FNotificationInfo Info(LOCTEXT("StencilOff", "Neon Zones need Custom Depth-Stencil Pass set to Enabled with Stencil"));
		Info.SubText = LOCTEXT("StencilOffSub", "Project Settings > Rendering > Postprocessing");
		Info.Hyperlink = FSimpleDelegate::CreateStatic(&OpenRenderingSettings);
		Info.HyperlinkText = LOCTEXT("StencilOffLink", "Open Rendering Settings");
		Info.ExpireDuration = 10.f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}

	// same file Project Settings writes, keeps both views in sync
	static void Save()
	{
		Settings().TryUpdateDefaultConfigFile();
		WarnIfStencilOff();
	}

	static void OpenPanel()
	{
		FGlobalTabmanager::Get()->TryInvokeTab(PanelTabName);
	}

	static void OpenProjectSettings()
	{
		const UDeusGoLookSettings& S = Settings();
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->ShowViewer(S.GetContainerName(), S.GetCategoryName(), S.GetSectionName());
		}
	}

	using FFlagGetter = bool& (*)(UDeusGoLookSettings&);

	static void AddToggle(FToolMenuSection& Section, FName Name, const FText& Label, FFlagGetter Flag)
	{
		Section.AddMenuEntry(
			Name,
			Label,
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([Flag]()
				{
					bool& Value = Flag(Settings());
					Value = !Value;
					Save();
				}),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([Flag]() { return Flag(Settings()); })),
			EUserInterfaceActionType::ToggleButton);
	}

	static void AddPresetSection(UToolMenu* Menu)
	{
		FToolMenuSection& Section = Menu->AddSection("Presets", LOCTEXT("Presets", "Presets"));

		// saved config overrides the built in defaults, this is the only way back to them
		Section.AddMenuEntry(
			"PluginDefaults",
			LOCTEXT("PluginDefaults", "Plugin Defaults"),
			LOCTEXT("PluginDefaultsTip", "Reset every look setting to the plugin's built in values"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([]()
			{
				Settings().Style = FDeusGoLookStyle();
				Settings().ActivePreset = NAME_None;
				Save();
			})));

		for (const FName PresetName : UDeusGoLookSettings::GetPresetNames())
		{
			Section.AddMenuEntry(
				FName(*(TEXT("Preset_") + PresetName.ToString())),
				FText::FromName(PresetName),
				LOCTEXT("ApplyPresetTip", "Switch to this look"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateLambda([PresetName]()
					{
						UDeusGoLookSettings::ApplyPreset(PresetName);
						Save();
					}),
					FCanExecuteAction(),
					FIsActionChecked::CreateLambda([PresetName]() { return Settings().ActivePreset == PresetName; })),
				EUserInterfaceActionType::RadioButton);
		}

		const FName Active = Settings().ActivePreset;
		if (!Active.IsNone() && Settings().Presets.Contains(Active))
		{
			Section.AddMenuEntry(
				"UpdatePreset",
				FText::Format(LOCTEXT("UpdatePreset", "Update \"{0}\""), FText::FromName(Active)),
				LOCTEXT("UpdatePresetTip", "Overwrite this preset with the current settings"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([Active]()
				{
					Settings().SavePreset(Active);
					Save();
				})));
		}

		// typing in a menu works, Enter commits and closes it
		TSharedRef<SWidget> SaveBox = SNew(SBox)
			.MinDesiredWidth(180.f)
			[
				SNew(SEditableTextBox)
				.HintText(LOCTEXT("SaveHint", "New name, then Enter"))
				.OnTextCommitted_Lambda([](const FText& Text, ETextCommit::Type Commit)
				{
					const FString NewName = Text.ToString().TrimStartAndEnd();
					if (Commit != ETextCommit::OnEnter || NewName.IsEmpty())
					{
						return;
					}

					Settings().SavePreset(FName(*NewName));
					Save();
					FSlateApplication::Get().DismissAllMenus();
				})
			];
		Section.AddEntry(FToolMenuEntry::InitWidget("SavePresetAs", SaveBox, LOCTEXT("SaveAs", "Save As")));

		if (Settings().Presets.Num() > 0)
		{
			Section.AddSubMenu(
				"DeletePreset",
				LOCTEXT("DeletePreset", "Delete"),
				LOCTEXT("DeletePresetTip", "Remove a saved preset"),
				FNewToolMenuDelegate::CreateLambda([](UToolMenu* SubMenu)
				{
					FToolMenuSection& Sub = SubMenu->AddSection("Delete");
					for (const FName PresetName : UDeusGoLookSettings::GetPresetNames())
					{
						Sub.AddMenuEntry(
							FName(*(TEXT("Delete_") + PresetName.ToString())),
							FText::FromName(PresetName),
							FText::GetEmpty(),
							FSlateIcon(),
							FUIAction(FExecuteAction::CreateLambda([PresetName]()
							{
								Settings().DeletePreset(PresetName);
								Save();
							})));
					}
				}));
		}
	}

	static void FillQuickMenu(UToolMenu* Menu)
	{
		AddPresetSection(Menu);

		FToolMenuSection& Toggles = Menu->AddSection("Toggles", LOCTEXT("Toggles", "Quick Toggles"));
		AddToggle(Toggles, "Enabled", LOCTEXT("Enabled", "Look Enabled"), [](UDeusGoLookSettings& S) -> bool& { return S.bEnabled; });
		AddToggle(Toggles, "Fog", LOCTEXT("Fog", "Fog"), [](UDeusGoLookSettings& S) -> bool& { return S.Style.Fog.bFog; });
		AddToggle(Toggles, "Sky", LOCTEXT("Sky", "Replace Sky"), [](UDeusGoLookSettings& S) -> bool& { return S.Style.Background.bBackground; });
		AddToggle(Toggles, "Outlines", LOCTEXT("Outlines", "Outlines"), [](UDeusGoLookSettings& S) -> bool& { return S.Style.Edges.bOutlines; });
		AddToggle(Toggles, "Creases", LOCTEXT("Creases", "Creases"), [](UDeusGoLookSettings& S) -> bool& { return S.Style.Edges.bCreases; });
		AddToggle(Toggles, "Mist", LOCTEXT("Mist", "Ground Mist"), [](UDeusGoLookSettings& S) -> bool& { return S.Style.Mist.bMist; });
		AddToggle(Toggles, "Neon", LOCTEXT("Neon", "Neon Zones"), [](UDeusGoLookSettings& S) -> bool& { return S.Style.Neon.bNeon; });
		AddToggle(Toggles, "Posterize", LOCTEXT("Posterize", "Posterize"), [](UDeusGoLookSettings& S) -> bool& { return S.Style.Stylize.bPosterize; });
		AddToggle(Toggles, "TiltShift", LOCTEXT("TiltShift", "Tilt Shift"), [](UDeusGoLookSettings& S) -> bool& { return S.Style.Stylize.bTiltShift; });

		FToolMenuSection& Open = Menu->AddSection("Open", LOCTEXT("Open", "Open"));
		Open.AddMenuEntry("OpenPanel", LOCTEXT("OpenPanel", "Look Panel"), LOCTEXT("OpenPanelTip", "Every setting in a dockable tab"), GetIcon(), FUIAction(FExecuteAction::CreateStatic(&OpenPanel)));
		Open.AddMenuEntry("OpenProjectSettings", LOCTEXT("OpenProjectSettings", "Project Settings"), FText::GetEmpty(), FSlateIcon(), FUIAction(FExecuteAction::CreateStatic(&OpenProjectSettings)));
	}

	static TSharedRef<SDockTab> SpawnPanel(const FSpawnTabArgs& Args)
	{
		FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		FDetailsViewArgs ViewArgs;
		ViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
		ViewArgs.bHideSelectionTip = true;

		TSharedRef<IDetailsView> DetailsView = PropertyEditor.CreateDetailView(ViewArgs);
		DetailsView->SetObject(&Settings());
		DetailsView->OnFinishedChangingProperties().AddLambda([](const FPropertyChangedEvent&) { Save(); });

		return SNew(SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				DetailsView
			];
	}
}

class FDeusGoLookEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		for (const UScriptStruct* Group : GetGroups())
		{
			PropertyEditor.RegisterCustomPropertyTypeLayout(Group->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateLambda([Group]()
			{
				return MakeShared<FDeusGoLookGroupCustomization>(Group);
			}));
		}
		PropertyEditor.NotifyCustomizationModuleChanged();

		// Project Settings edits do not go through Save
		SettingChangedHandle = GetMutableDefault<UDeusGoLookSettings>()->OnSettingChanged().AddLambda([](UObject*, FPropertyChangedEvent&) { DeusGoLookEditor::WarnIfStencilOff(); });

		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(DeusGoLookEditor::PanelTabName, FOnSpawnTab::CreateStatic(&DeusGoLookEditor::SpawnPanel))
			.SetDisplayName(LOCTEXT("PanelTitle", "Deus Go Look"))
			.SetIcon(DeusGoLookEditor::GetIcon())
			.SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorCategory());

		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FDeusGoLookEditorModule::RegisterMenus));
	}

	virtual void ShutdownModule() override
	{
		if (FPropertyEditorModule* PropertyEditor = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
		{
			for (const UScriptStruct* Group : GetGroups())
			{
				PropertyEditor->UnregisterCustomPropertyTypeLayout(Group->GetFName());
			}
		}

		if (UObjectInitialized())
		{
			GetMutableDefault<UDeusGoLookSettings>()->OnSettingChanged().Remove(SettingChangedHandle);
		}

		UToolMenus::UnRegisterStartupCallback(this);
		UToolMenus::UnregisterOwner(this);

		if (FSlateApplication::IsInitialized())
		{
			FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(DeusGoLookEditor::PanelTabName);
		}
	}

private:
	static TArray<const UScriptStruct*> GetGroups()
	{
		return
		{
			FDeusGoLookFog::StaticStruct(), FDeusGoLookBackground::StaticStruct(), FDeusGoLookMist::StaticStruct(), FDeusGoLookGrade::StaticStruct(),
			FDeusGoLookEdges::StaticStruct(), FDeusGoLookNeon::StaticStruct(), FDeusGoLookStylize::StaticStruct()
		};
	}

	// main toolbar, right after Cinematics
	void RegisterMenus()
	{
		FToolMenuOwnerScoped OwnerScoped(this);

		UToolMenu* Toolbar = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.AssetsToolBar");
		FToolMenuSection& Section = Toolbar->FindOrAddSection("Content");

		FToolMenuEntry Button = FToolMenuEntry::InitToolBarButton(
			"DeusGoLook",
			FUIAction(FExecuteAction::CreateStatic(&DeusGoLookEditor::OpenPanel)),
			LOCTEXT("ButtonLabel", "Deus Go Look"),
			LOCTEXT("ButtonTip", "Open the Deus Go Look panel"),
			DeusGoLookEditor::GetIcon());
		Button.StyleNameOverride = "AssetEditorToolbar";
		Button.InsertPosition = FToolMenuInsert("EditCinematics", EToolMenuInsertType::After);
		Section.AddEntry(Button);

		// bare arrow next to the button, quick toggles
		FToolMenuEntry Arrow = FToolMenuEntry::InitComboButton(
			"DeusGoLookOptions",
			FUIAction(),
			FNewToolMenuDelegate::CreateStatic(&DeusGoLookEditor::FillQuickMenu),
			LOCTEXT("OptionsLabel", "Deus Go Look Options"),
			LOCTEXT("OptionsTip", "Quick toggles for the Deus Go Look"),
			FSlateIcon(),
			true);
		Arrow.StyleNameOverride = "AssetEditorToolbar";
		Arrow.InsertPosition = FToolMenuInsert("DeusGoLook", EToolMenuInsertType::After);
		Section.AddEntry(Arrow);
	}

	FDelegateHandle SettingChangedHandle;
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDeusGoLookEditorModule, DeusGoLookEditor)
