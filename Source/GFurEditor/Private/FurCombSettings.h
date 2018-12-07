// Copyright 2019 GiM s.r.o. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "FurCombSettings.generated.h"

UCLASS()
class GFUREDITOR_API UFurCombSettings : public UObject
{
	GENERATED_UCLASS_BODY()
public:

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	~UFurCombSettings();

	float GetRadius() const { return Radius; }
	void SetRadius(float InRadius);

protected:
	/** Radius of the Brush used for Painting */
	UPROPERTY(EditAnywhere, Category = Comb, meta = (DisplayName = "Radius", UIMin = "0.01", UIMax = "50.0", ClampMin = "0.01", ClampMax = "1000000.0"))
	float Radius;

public:
	/** Min and Max brush radius retrieved from config */
	float RadiusMin;
	float RadiusMax;

	/** Strength of the brush (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, Category = Comb, meta = (DisplayName = "Strength", UIMin = "0.0", UIMax = "1.0", ClampMin= "0.0", ClampMax = "1000.0"))
	float Strength; 

	/** Amount of falloff to apply (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, Category = Comb, meta = (DisplayName = "Falloff", UIMin = "0.0", UIMax = "1.0", ClampMin = "0.0", ClampMax = "1.0"))
	float FalloffAmount;

	UPROPERTY(EditAnywhere, Category = Comb, meta = (DisplayName = "Apply Height", UIMin = "0.0", UIMax = "1.0", ClampMin = "0.0", ClampMax = "1.0"))
	float ApplyHeight;

	UPROPERTY(EditAnywhere, Category = Comb, meta = (DisplayName = "Apply Spread", UIMin = "-1.0", UIMax = "1.0", ClampMin = "-1.0", ClampMax = "1.0"))
	float ApplySpread;

	UPROPERTY(EditAnywhere, Category = Comb)
	bool bEnableFlow;

	UPROPERTY(EditAnywhere, Category = Comb)
	bool bMirrorX;
	UPROPERTY(EditAnywhere, Category = Comb)
	bool bMirrorY;
	UPROPERTY(EditAnywhere, Category = Comb)
	bool bMirrorZ;

	UPROPERTY(EditAnywhere, Category = Comb)
	bool bShowSplines;
};
