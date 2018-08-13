// Copyright 2018 GiM s.r.o. All Rights Reserved.

#pragma once

#include "FurSplines.generated.h"

UCLASS()
class GFUR_API UFurSplines : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY()
	TArray<FVector> Vertices;

	UPROPERTY()
	TArray<int> Index;

	UPROPERTY()
	TArray<int> Count;

	UPROPERTY()
	FString ImportFilename;

	void Refresh();
};
