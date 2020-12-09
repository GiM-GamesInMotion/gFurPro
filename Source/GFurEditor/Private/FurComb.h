// Copyright 2019 GiM s.r.o. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Engine/EngineBaseTypes.h"
#include "InputCoreTypes.h"

class UFurCombSettings;
class UMeshComponent;
class UViewportInteractor;
class UVREditorInteractor;
class UGFurComponent;
class UFurSplines;
class FScopedTransaction;
class FUICommandList;
class FViewport;
class FPrimitiveDrawInterface;
class FEditorViewportClient;
class FSceneView;
class AActor;
class SFurCombModeWidget;

enum class ECombAction
{
	Normal,
	Inverse,
};

enum class EFurCombMode
{
	Length,
	AverageLength,
	Bend,
	Clump,
	Twist,
	Noise,
	Curl,
	Relax,
	AddRemove,

	Count
};

/** Class for combing fur */
class GFUREDITOR_API FFurComb
{
public:
	FFurComb();
	~FFurComb();

	void Init();

	static FFurComb* Get();
	TSharedPtr<FUICommandList> GetUICommandList();

	/** Renders ray widgets for the active viewport interactors */
	void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI);

	/** Base ticking functionality, keeps track of time and caches VR interactors */
	void Tick(FEditorViewportClient* ViewportClient, float DeltaTime);

	/** Returns whether or not painting (of any sorts) is in progress*/
	bool IsCombing() { return bCombing; }

	/** Registers a base set of commands for altering UFurCombSettings */
	void RegisterCommands(TSharedRef<FUICommandList> CommandList);

	/** Unregisters a base set of commands */
	void UnregisterCommands(TSharedRef<FUICommandList> CommandList);

	/** 'Normal' painting functionality, called when the user tries to paint on a mesh using the mouse */
	bool Comb(FViewport* Viewport, const FVector& InCameraOrigin, const FVector& InRayOrigin, const FVector& InRayDirection);

	/** VR painting functionality, called when the user tries to paint on a mesh using a VR controller */
	bool CombVR(FViewport* Viewport, const FVector& InCameraOrigin, const FVector& InRayOrigin, const FVector& InRayDirection, UVREditorInteractor* VRInteractor);

	/** Allows painter to act on specific key actions */
	bool InputKey(FEditorViewportClient* InViewportClient, FViewport* InViewport, FKey InKey, EInputEvent InEvent);

	/** If painting is in progress will propagate changes and cleanup the painting state */
	void FinishCombing();

	/** Is called by the owning EdMode when an actor is de-selected in the viewport */
	void ActorSelected(AActor* Actor);
	/** Is called by the owning EdMode when an actor is selected in the viewport */
	void ActorDeselected(AActor* Actor);

	/** Tries to retrieves a valid mesh adapter for the given component (derived painters can cache these hence no base implementation) */
//	TSharedPtr<IMeshPaintGeometryAdapter> GetMeshAdapterForComponent(const UMeshComponent* Component);//TODO

	/** Functionality to collect painter referenced objects to prevent GC */
	void AddReferencedObjects(FReferenceCollector& Collector);

	/** Returns an instance of UPaintBrushSettings (base or derived) which contains the basic settings used for painting */
	UFurCombSettings* GetCurrentFurCombSettings();

	/** Returns an instance of UMeshPainterSettings used for specific types of painting */
//	UMeshPaintSettings* GetPainterSettings();//TODO

	/** Returns a SWidget which represents the painters state and allows for changes / extra functionality */
	TSharedPtr<class SWidget> GetWidget();

	/** Returns a HitResult structure given a origin and direction to trace a ray for */
	const FHitResult GetHitResult(const FVector& Origin, const FVector& Direction);

	void PostUndo();

	void SetMode(EFurCombMode InMode) { Mode = InMode; }
	EFurCombMode GetMode() const { return Mode; }

protected:
	/** Internal painting functionality, is called from Paint and PaintVR and implemented in the derived painters */
	bool CombInternal(const FVector& InCameraOrigin, const FVector& InRayOrigin, const FVector& InRayDirection, ECombAction CombAction, float StrengthScale);

	/** Renders viewport interactor widget */
	void RenderInteractors(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI, ESceneDepthPriorityGroup DepthGroup = SDPG_World);

	/** Functionality for rendering the interactor widget given its positional information and paint action */
	void RenderInteractorWidget(const FVector& InRayOrigin, const FVector& InRayDirection, FPrimitiveDrawInterface* PDI, ESceneDepthPriorityGroup DepthGroup = SDPG_World);

	void RenderSplines(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI, ESceneDepthPriorityGroup DepthGroup = SDPG_World);
protected:
	/** When painting in VR, this is viewport interactor we are using */
	class UViewportInteractor* CurrentViewportInteractor;
protected:
	/** Flag for whether or not we are currently combing */
	bool bCombing;
	/** Time kept since the user has started painting */
	float TimeSinceStartedPainting;
	/** Overall time value kept for drawing effects */
	float Time;

	float LastDeltaTime;

	EFurCombMode Mode;
	FVector OldLocation;

protected:
	/** Begins and keeps track of an Editor transaction with the given Description */
	void BeginTransaction(const FText Description);
	/** Ends the current transaction */
	void EndTransaction();
	/** Painting transaction instance which is currently active */
	FScopedTransaction* CombTransaction;

private:
	struct CombParams
	{
		FVector Location;
		FVector OldLocation;
		FVector Normal;
		float CombRadius;
		float Strength;
		float Falloff;
		float ApplyHeight;
		float ApplySpread;
		float TwistCount;
		bool MirrorX;
		bool MirrorY;
		bool MirrorZ;
	};

	struct FPerSplineData
	{
		const FVector& BaseVertex;
		const FVector& Normal;
		int32 ControlPointCount;
	};

	struct FPerSegmentData
	{
		const FVector& Dir;
		const FVector& Normal;
		float Strength;
		float Height;
	};

	static const float MinLayerDist;

	/** Widget representing the state and settings for the painter */
	TSharedPtr<SFurCombModeWidget> Widget;

	/** Fur Comb settings */
	UFurCombSettings* FurCombSettings[(int)EFurCombMode::Count];

	TArray<UGFurComponent*> FurComponents;

	TArray<uint32> VertexSet;
	TSet<int32> SplineSet;
	TArray<FVector> SplineNormals;

	/** UI command list object */
	TSharedPtr<FUICommandList> UICommandList;

	bool LineTraceComponent(struct FHitResult& OutHit, const FVector Start, const FVector End, const struct FCollisionQueryParams& Params, UGFurComponent* FurComponent) const;

	static FVector MirrorVector(const FVector& Vec, const FVector& Vertex, const CombParams& Params);
	static FVector BendFur(const FVector& Dir, const FVector& Normal, const FVector& Offset);

	template<bool UseStrengthHeight, typename F, typename G>
	void Comb(UFurSplines* FurSplines, const CombParams& Params, const F& FuncPerSpline, const G& FuncPerSegment);

	void CombLength(UFurSplines* FurSplines, const CombParams& Params);
	void CombAverageLength(UFurSplines* FurSplines, const CombParams& Params);
	void CombBend(UFurSplines* FurSplines, const CombParams& Params);
	void CombClump(UFurSplines* FurSplines, const CombParams& Params);
	void CombTwist(UFurSplines* FurSplines, const CombParams& Params);
	void CombNoise(UFurSplines* FurSplines, const CombParams& Params);
	void CombCurl(UFurSplines* FurSplines, const CombParams& Params);
	void CombRelax(UFurSplines* FurSplines, const CombParams& Params);

	void CombAdd(UFurSplines* FurSplines, const class FPositionVertexBuffer& Positions, const TArray<FVector>& Normals);
	void CombRemove(UFurSplines* FurSplines);
};
