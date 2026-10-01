#include "DeusGoLookGroupCustomization.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "IDetailPropertyRow.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyHandle.h"
#include "Widgets/Text/STextBlock.h"

FDeusGoLookGroupCustomization::FDeusGoLookGroupCustomization(const UScriptStruct* InStruct)
	: Defaults(InStruct)
{
}

void FDeusGoLookGroupCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	HeaderRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	];
}

void FDeusGoLookGroupCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if (!Defaults.IsValid())
	{
		return;
	}

	const FString Help = Defaults.GetStruct()->GetMetaData(TEXT("GroupHelp"));
	if (!Help.IsEmpty())
	{
		const FText HelpText = FText::FromString(Help);
		ChildBuilder.AddCustomRow(HelpText)
			.WholeRowContent()
			[
				SNew(STextBlock)
				.Text(HelpText)
				.Font(IDetailLayoutBuilder::GetDetailFontItalic())
				.AutoWrapText(true)
			];
	}

	AddChildren(PropertyHandle, Defaults.GetStructMemory(), ChildBuilder, nullptr);
}

void FDeusGoLookGroupCustomization::AddChildren(TSharedRef<IPropertyHandle> Parent, const uint8* ParentDefaults, IDetailChildrenBuilder& ChildBuilder, IDetailGroup* Group)
{
	const UPackage* OurPackage = Defaults.GetStruct()->GetOutermost();

	uint32 NumChildren = 0;
	Parent->GetNumChildren(NumChildren);

	for (uint32 Index = 0; Index < NumChildren; ++Index)
	{
		TSharedPtr<IPropertyHandle> Child = Parent->GetChildHandle(Index);
		const FProperty* Property = Child.IsValid() ? Child->GetProperty() : nullptr;
		if (!Property)
		{
			continue;
		}

		const uint8* ChildDefault = Property->ContainerPtrToValuePtr<uint8>(ParentDefaults);

		const FStructProperty* StructProperty = CastField<FStructProperty>(Property);
		if (StructProperty && StructProperty->Struct->GetOutermost() == OurPackage)
		{
			IDetailGroup& SubGroup = Group
				? Group->AddGroup(Property->GetFName(), Child->GetPropertyDisplayName())
				: ChildBuilder.AddGroup(Property->GetFName(), Child->GetPropertyDisplayName());
			AddChildren(Child.ToSharedRef(), ChildDefault, ChildBuilder, &SubGroup);
			continue;
		}

		IDetailPropertyRow& Row = Group ? Group->AddPropertyRow(Child.ToSharedRef()) : ChildBuilder.AddProperty(Child.ToSharedRef());
		Row.OverrideResetToDefault(FResetToDefaultOverride::Create(
			FIsResetToDefaultVisible::CreateSP(this, &FDeusGoLookGroupCustomization::IsResetVisible, static_cast<const void*>(ChildDefault)),
			FResetToDefaultHandler::CreateSP(this, &FDeusGoLookGroupCustomization::Reset, static_cast<const void*>(ChildDefault))));
	}
}

bool FDeusGoLookGroupCustomization::IsResetVisible(TSharedPtr<IPropertyHandle> Handle, const void* DefaultValue) const
{
	const FProperty* Property = Handle.IsValid() ? Handle->GetProperty() : nullptr;
	if (!Property || !DefaultValue)
	{
		return false;
	}

	TArray<void*> RawData;
	Handle->AccessRawData(RawData);
	for (const void* Value : RawData)
	{
		if (Value && !Property->Identical(Value, DefaultValue))
		{
			return true;
		}
	}
	return false;
}

void FDeusGoLookGroupCustomization::Reset(TSharedPtr<IPropertyHandle> Handle, const void* DefaultValue) const
{
	const FProperty* Property = Handle.IsValid() ? Handle->GetProperty() : nullptr;
	if (!Property || !DefaultValue)
	{
		return;
	}

	FString DefaultText;
	Property->ExportTextItem_Direct(DefaultText, DefaultValue, nullptr, nullptr, PPF_None);

	// goes through the handle so undo and saving behave like a normal edit
	Handle->SetValueFromFormattedString(DefaultText);
}
