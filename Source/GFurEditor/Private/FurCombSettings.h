// Copyright 2019 GiM s.r.o. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "FurCombSettings.generated.h"

class SFurCombModeWidget;

UCLASS()
class GFUREDITOR_API UFurCombSettings : public UObject
{
	GENERATED_UCLASS_BODY()
public:

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	~UFurCombSettings();

	float GetRadius() const { return Radius; }
	void SetRadius(float InRadius);

	void SetConfigPrefix(const FString& InConfigPrefix) { ConfigPrefix = InConfigPrefix; }

	void Load();
	void DeleteFromConfig();

	void CopyFrom(const UFurCombSettings* other);
	bool Equals(const UFurCombSettings* other);

	void RegWidget(SFurCombModeWidget* InWidget);

protected:
	/** Radius of the Brush used for Painting */
	UPROPERTY(EditAnywhere, Category = Comb, meta = (DisplayName = "Radius", UIMin = "0.01", UIMax = "50.0", ClampMin = "0.01", ClampMax = "1000000.0"))
	float Radius;

public:
	/** Min and Max brush radius retrieved from config */
	float RadiusMin;
	float RadiusMax;

	/** Strength of the brush (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, Category = Comb, meta = (DisplayName = "Strength", UIMin = "0.0", UIMax = "5.0", ClampMin= "0.0", ClampMax = "1000.0"))
	float Strength; 

	/** Amount of falloff to apply (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, Category = Comb, meta = (DisplayName = "Falloff", UIMin = "0.0", UIMax = "1.0", ClampMin = "0.0", ClampMax = "1.0"))
	float FalloffAmount;

	/** Height of fur spline at which the brush effect is applied. 0=root, 1=tip */
	UPROPERTY(EditAnywhere, Category = Comb, meta = (DisplayName = "Effect Height", UIMin = "0.0", UIMax = "1.0", ClampMin = "0.0", ClampMax = "1.0"))
	float ApplyHeight;

	/** Affects the distance at which the effect is applied or faded out in both directions from the "Effect Height" point. -1 = extremely short, mostly at exact location defined by "Effect Height", 0 smooth distribution which fades out in both directions from "Effect Height", 1 affects the whole spline same way, achieving sort of linear effect.*/
	UPROPERTY(EditAnywhere, Category = Comb, meta = (DisplayName = "Effect Distribution", UIMin = "-1.0", UIMax = "1.0", ClampMin = "-1.0", ClampMax = "1.0"))
	float ApplySpread;

	/** Affects the distance at which the effect is applied or faded out in both directions from the "Effect Height" point. -1 = extremely short, mostly at exact location defined by "Effect Height", 0 smooth distribution which fades out in both directions from "Effect Height", 1 affects the whole spline same way, achieving sort of linear effect.*/
	UPROPERTY(EditAnywhere, Category = Comb, meta = (DisplayName = "Twist Count", UIMin = "0.0", UIMax = "1000.0", ClampMin = "0.0", ClampMax = "10.0"))
	float TwistCount;

	/** If the brush should affect the fur even if it's not moved.*/
	UPROPERTY(EditAnywhere, Category = Comb)
	bool bEnableFlow;

	/** Mirrors the grooming along the X axis of the mesh.*/
	UPROPERTY(EditAnywhere, Category = Comb)
	bool bMirrorX;
	/** Mirrors the grooming along the Y axis of the mesh.*/
	UPROPERTY(EditAnywhere, Category = Comb)
	bool bMirrorY;
	/** Mirrors the grooming along the Z axis of the mesh.*/
	UPROPERTY(EditAnywhere, Category = Comb)
	bool bMirrorZ;

	/** Toggles visualization of the spline guides.*/
	UPROPERTY(EditAnywhere, Category = Comb)
	bool bShowSplines;

private:
	FString ConfigPrefix;
	SFurCombModeWidget* Widget = nullptr;
};
