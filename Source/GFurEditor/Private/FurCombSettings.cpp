// Copyright 2019 GiM s.r.o. All Rights Reserved.

#include "FurCombSettings.h"
#include "UObject/UnrealType.h"

#include "Misc/ConfigCacheIni.h"

#include "SFurCombModeWidget.h"

UFurCombSettings::UFurCombSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	Radius(16.0f),
	Strength(0.5f),
	FalloffAmount(0.5f),
	ApplyHeight(1.0f),
	ApplySpread(0.0f),
	TwistCount(2.0f),
	bEnableFlow(true),
	bMirrorX(false),
	bMirrorY(false),
	bMirrorZ(false),
	bShowSplines(false)
{
	RadiusMin = 0.01f, RadiusMax = 250.0f;

}

UFurCombSettings::~UFurCombSettings()
{
}

void UFurCombSettings::SetRadius(float InRadius)
{
	Radius = (float)FMath::Clamp(InRadius, RadiusMin, RadiusMax);
	GConfig->SetFloat(TEXT("FurCombEdit"), TEXT("DefaultCombRadius"), Radius, GEditorPerProjectIni);
}

void UFurCombSettings::Load()
{
	GConfig->GetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombRadius"), Radius, GEditorPerProjectIni);
	Radius = (float)FMath::Clamp(Radius, RadiusMin, RadiusMax);
	GConfig->GetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombStrength"), Strength, GEditorPerProjectIni);
	GConfig->GetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombFalloffAmount"), FalloffAmount, GEditorPerProjectIni);
	GConfig->GetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombApplyHeight"), ApplyHeight, GEditorPerProjectIni);
	GConfig->GetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombApplySpread"), ApplySpread, GEditorPerProjectIni);
	GConfig->GetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombTwistCount"), TwistCount, GEditorPerProjectIni);
	GConfig->GetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombEnableFlow"), bEnableFlow, GEditorPerProjectIni);
	GConfig->GetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombMirrorX"), bMirrorX, GEditorPerProjectIni);
	GConfig->GetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombMirrorY"), bMirrorY, GEditorPerProjectIni);
	GConfig->GetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombMirrorZ"), bMirrorZ, GEditorPerProjectIni);
	GConfig->GetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombShowSplines"), bShowSplines, GEditorPerProjectIni);
}

void UFurCombSettings::DeleteFromConfig()
{
	GConfig->RemoveKey(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombRadius"), GEditorPerProjectIni);
	GConfig->RemoveKey(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombStrength"), GEditorPerProjectIni);
	GConfig->RemoveKey(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombFalloffAmount"), GEditorPerProjectIni);
	GConfig->RemoveKey(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombApplyHeight"), GEditorPerProjectIni);
	GConfig->RemoveKey(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombApplySpread"), GEditorPerProjectIni);
	GConfig->RemoveKey(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombTwistCount"), GEditorPerProjectIni);
	GConfig->RemoveKey(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombEnableFlow"), GEditorPerProjectIni);
	GConfig->RemoveKey(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombMirrorX"), GEditorPerProjectIni);
	GConfig->RemoveKey(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombMirrorY"), GEditorPerProjectIni);
	GConfig->RemoveKey(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombMirrorZ"), GEditorPerProjectIni);
	GConfig->RemoveKey(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombShowSplines"), GEditorPerProjectIni);
}

void UFurCombSettings::CopyFrom(const UFurCombSettings* other)
{
	Radius = other->Radius;
	Strength = other->Strength;
	FalloffAmount = other->FalloffAmount;
	ApplyHeight = other->ApplyHeight;
	ApplySpread = other->ApplySpread;
	TwistCount = other->TwistCount;
	bEnableFlow = other->bEnableFlow;
	bMirrorX = other->bMirrorX;
	bMirrorY = other->bMirrorY;
	bMirrorZ = other->bMirrorZ;
	bShowSplines = other->bShowSplines;

	GConfig->SetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombRadius"), Radius, GEditorPerProjectIni);
	GConfig->SetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombStrength"), Strength, GEditorPerProjectIni);
	GConfig->SetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombFalloffAmount"), FalloffAmount, GEditorPerProjectIni);
	GConfig->SetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombApplyHeight"), ApplyHeight, GEditorPerProjectIni);
	GConfig->SetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombApplySpread"), ApplySpread, GEditorPerProjectIni);
	GConfig->SetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombTwistCount"), TwistCount, GEditorPerProjectIni);
	GConfig->SetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombEnableFlow"), bEnableFlow, GEditorPerProjectIni);
	GConfig->SetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombMirrorX"), bMirrorX, GEditorPerProjectIni);
	GConfig->SetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombMirrorY"), bMirrorY, GEditorPerProjectIni);
	GConfig->SetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombMirrorZ"), bMirrorZ, GEditorPerProjectIni);
	GConfig->SetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombShowSplines"), bShowSplines, GEditorPerProjectIni);
}

bool UFurCombSettings::Equals(const UFurCombSettings* other)
{
	bool b = true;
	b &= Radius == other->Radius;
	b &= Strength == other->Strength;
	b &= FalloffAmount == other->FalloffAmount;
	b &= ApplyHeight == other->ApplyHeight;
	b &= ApplySpread == other->ApplySpread;
	b &= TwistCount == other->TwistCount;
	b &= bEnableFlow == other->bEnableFlow;
	b &= bMirrorX == other->bMirrorX;
	b &= bMirrorY == other->bMirrorY;
	b &= bMirrorZ == other->bMirrorZ;
	b &= bShowSplines == other->bShowSplines;
	return b;
}

void UFurCombSettings::RegWidget(SFurCombModeWidget* InWidget)
{
	Widget = InWidget;
}

void UFurCombSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property && PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFurCombSettings, Radius))
			GConfig->SetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombRadius"), Radius, GEditorPerProjectIni);
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFurCombSettings, Strength))
			GConfig->SetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombStrength"), Strength, GEditorPerProjectIni);
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFurCombSettings, FalloffAmount))
			GConfig->SetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombFalloffAmount"), FalloffAmount, GEditorPerProjectIni);
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFurCombSettings, ApplyHeight))
			GConfig->SetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombApplyHeight"), ApplyHeight, GEditorPerProjectIni);
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFurCombSettings, ApplySpread))
			GConfig->SetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombApplySpread"), ApplySpread, GEditorPerProjectIni);
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFurCombSettings, TwistCount))
			GConfig->SetFloat(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombTwistCount"), TwistCount, GEditorPerProjectIni);
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFurCombSettings, bEnableFlow))
			GConfig->SetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombEnableFlow"), bEnableFlow, GEditorPerProjectIni);
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFurCombSettings, bMirrorX))
			GConfig->SetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombMirrorX"), bMirrorX, GEditorPerProjectIni);
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFurCombSettings, bMirrorY))
			GConfig->SetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombMirrorY"), bMirrorY, GEditorPerProjectIni);
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFurCombSettings, bMirrorZ))
			GConfig->SetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombMirrorZ"), bMirrorZ, GEditorPerProjectIni);
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFurCombSettings, bShowSplines))
			GConfig->SetBool(TEXT("FurCombEdit"), *(ConfigPrefix + "DefaultCombShowSplines"), bShowSplines, GEditorPerProjectIni);
		if (Widget)
			Widget->UpdateSelectedPresset(this);
	}
}