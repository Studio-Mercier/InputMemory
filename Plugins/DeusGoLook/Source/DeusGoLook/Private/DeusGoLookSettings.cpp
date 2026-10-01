#include "DeusGoLookSettings.h"

#include "Misc/ConfigCacheIni.h"

namespace DeusGoLook
{
	// pre struct layout wrote every property flat into the section
	static void ImportFlatKeys(const UStruct* Struct, void* Data, const FString& Section)
	{
		const UPackage* OurPackage = FDeusGoLookStyle::StaticStruct()->GetOutermost();

		for (TFieldIterator<FProperty> It(Struct); It; ++It)
		{
			void* Value = It->ContainerPtrToValuePtr<void>(Data);

			// recurse into our own groups only, FLinearColor is a struct too
			const FStructProperty* StructProp = CastField<FStructProperty>(*It);
			if (StructProp && StructProp->Struct->GetOutermost() == OurPackage)
			{
				ImportFlatKeys(StructProp->Struct, Value, Section);
				continue;
			}

			FString Text;
			if (GConfig->GetString(*Section, *It->GetName(), Text, GGameIni))
			{
				It->ImportText_Direct(*Text, Value, nullptr, PPF_None);
			}
		}
	}
}

bool UDeusGoLookSettings::ApplyPreset(FName PresetName)
{
	UDeusGoLookSettings* Settings = GetMutableDefault<UDeusGoLookSettings>();
	const FDeusGoLookStyle* Preset = Settings->Presets.Find(PresetName);
	if (!Preset)
	{
		return false;
	}

	Settings->Style = *Preset;
	Settings->ActivePreset = PresetName;
	return true;
}

TArray<FName> UDeusGoLookSettings::GetPresetNames()
{
	TArray<FName> Names;
	GetDefault<UDeusGoLookSettings>()->Presets.GetKeys(Names);
	Names.Sort(FNameLexicalLess());
	return Names;
}

void UDeusGoLookSettings::SavePreset(FName PresetName)
{
	if (PresetName.IsNone())
	{
		return;
	}

	Presets.Add(PresetName, Style);
	ActivePreset = PresetName;
}

void UDeusGoLookSettings::DeletePreset(FName PresetName)
{
	Presets.Remove(PresetName);
	if (ActivePreset == PresetName)
	{
		ActivePreset = NAME_None;
	}
}

void UDeusGoLookSettings::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject) || !GConfig)
	{
		return;
	}

	const FString Section = GetClass()->GetPathName();
	FString Existing;
	if (GConfig->GetString(*Section, TEXT("Style"), Existing, GGameIni))
	{
		return;
	}

	DeusGoLook::ImportFlatKeys(FDeusGoLookStyle::StaticStruct(), &Style, Section);

	// legacy used 0 strength as off, toggles default off so keep a usable strength
	const FDeusGoLookEdges Defaults;
	if (Style.Edges.OutlineStrength <= 0.f)
	{
		Style.Edges.OutlineStrength = Defaults.OutlineStrength;
	}
	if (Style.Edges.CreaseStrength <= 0.f)
	{
		Style.Edges.CreaseStrength = Defaults.CreaseStrength;
	}
}
