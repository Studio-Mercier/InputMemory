#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "UObject/StructOnScope.h"

class IDetailGroup;

// help line and per row reset arrows for the look groups
// Project Settings only adds arrows to top level config properties, and our rows are nested
class FDeusGoLookGroupCustomization : public IPropertyTypeCustomization
{
public:
	explicit FDeusGoLookGroupCustomization(const UScriptStruct* InStruct);

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

private:
	// our own nested structs become sub groups so their rows get arrows too
	void AddChildren(TSharedRef<IPropertyHandle> Parent, const uint8* ParentDefaults, IDetailChildrenBuilder& ChildBuilder, IDetailGroup* Group);

	bool IsResetVisible(TSharedPtr<IPropertyHandle> Handle, const void* DefaultValue) const;
	void Reset(TSharedPtr<IPropertyHandle> Handle, const void* DefaultValue) const;

	// plugin defaults, not the config values
	FStructOnScope Defaults;
};
