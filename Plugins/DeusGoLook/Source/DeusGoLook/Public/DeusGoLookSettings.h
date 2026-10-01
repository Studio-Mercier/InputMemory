#pragma once

#include "CoreMinimal.h"
#include "DeusGoLookTypes.h"
#include "Engine/DeveloperSettings.h"
#include "DeusGoLookSettings.generated.h"

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Deus Go Look"))
class DEUSGOLOOK_API UDeusGoLookSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override { return TEXT("Rendering"); }
	virtual void PostInitProperties() override;

	UPROPERTY(EditAnywhere, config, Category = "General", meta = (ToolTip = "Master switch for the whole project, wins over any Deus Go Look actor"))
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, config, Category = "Look", meta = (ShowOnlyInnerProperties))
	FDeusGoLookStyle Style;

	// editor only preview, never saved
	UPROPERTY(EditAnywhere, Transient, Category = "Preview", meta = (DisplayName = "Before/After Split", ClampMin = "0", ClampMax = "1", ToolTip = "Left of the line shows the scene without the look. 0 turns the split off. Not saved"))
	float CompareSplit = 0.f;

	// named looks, managed from the toolbar dropdown
	UPROPERTY(config)
	TMap<FName, FDeusGoLookStyle> Presets;

	UPROPERTY(config)
	FName ActivePreset;

	// runtime switch is in memory only, the editor dropdown is what saves
	UFUNCTION(BlueprintCallable, Category = "Deus Go Look", meta = (DisplayName = "Apply Deus Go Look Preset"))
	static bool ApplyPreset(FName PresetName);

	UFUNCTION(BlueprintPure, Category = "Deus Go Look", meta = (DisplayName = "Get Deus Go Look Presets"))
	static TArray<FName> GetPresetNames();

	void SavePreset(FName PresetName);
	void DeletePreset(FName PresetName);
};
