// Copyright 2019 GiM s.r.o. All Rights Reserved.

#include "FurCombSettings.h"
#include "UObject/UnrealType.h"

#include "Misc/ConfigCacheIni.h"

UFurCombSettings::UFurCombSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	Radius(16.0f),
	Strength(0.5f),
	FalloffAmount(0.5f),
	ApplyHeight(1.0f),
	ApplySpread(0.0f),
	bEnableFlow(true),
	bMirrorX(false),
	bMirrorY(false),
	bMirrorZ(false),
	bShowSplines(false)
{
	RadiusMin = 0.01f, RadiusMax = 250.0f;

	GConfig->GetFloat(TEXT("FurCombEdit"), TEXT("DefaultCombRadius"), Radius, GEditorPerProjectIni);
	Radius = (float)FMath::Clamp(Radius, RadiusMin, RadiusMax);
}

UFurCombSettings::~UFurCombSettings()
{
}

void UFurCombSettings::SetRadius(float InRadius)
{
	Radius = (float)FMath::Clamp(InRadius, RadiusMin, RadiusMax);
	GConfig->SetFloat(TEXT("FurCombEdit"), TEXT("DefaultCombRadius"), Radius, GEditorPerProjectIni);
}

void UFurCombSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFurCombSettings, Radius) && PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		GConfig->SetFloat(TEXT("FurCombEdit"), TEXT("DefaultCombRadius"), Radius, GEditorPerProjectIni);
	}
}