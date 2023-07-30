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

	// Old - don't use
	UPROPERTY()
	TArray<int32> Index;
	UPROPERTY()
	TArray<int32> Count;

	UPROPERTY()
	int32 ControlPointCount;

	UPROPERTY()
	FString ImportFilename;

	UPROPERTY()
	int32 Version;

	UPROPERTY()
	int32 ImportTransformation;

	UPROPERTY()
	float Threshold;

	int32 SplineCount() const { return Vertices.Num() / ControlPointCount; }
	FVector GetFirstControlPoint(int32 SplineIndex) const { return Vertices[SplineIndex * ControlPointCount]; }
	FVector GetLastControlPoint(int32 SplineIndex) const { return Vertices[SplineIndex * ControlPointCount + ControlPointCount - 1]; }

	void PostLoad() override;

	void UpdateSplines();

#if WITH_EDITOR
	/** Notification when anything changed */
	DECLARE_MULTICAST_DELEGATE(FOnSplinesChanged);
	FOnSplinesChanged OnSplinesChanged;
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSplinesCombed, const TArray<uint32>&);
	FOnSplinesCombed OnSplinesCombed;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditUndo() override;
#endif

private:
	void ConvertToUniformControlPointCount(int32 NumControlPoints);
};
