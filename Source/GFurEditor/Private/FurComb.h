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
	Bend,
	Clump,
	Twist,
	Noise,
	Relax,
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
	UFurCombSettings* GetFurCombSettings();

	/** Returns an instance of UMeshPainterSettings used for specific types of painting */
//	UMeshPaintSettings* GetPainterSettings();//TODO

	/** Returns a SWidget which represents the painters state and allows for changes / extra functionality */
	TSharedPtr<class SWidget> GetWidget();

	/** Returns a HitResult structure given a origin and direction to trace a ray for */
	const FHitResult GetHitResult(const FVector& Origin, const FVector& Direction);

	/** Refreshes the underlying data */
	void Refresh();

	/** Resets the state of the painter */
	void Reset();

	void PostUndo();

	void SetMode(EFurCombMode InMode) { Mode = InMode; }
	EFurCombMode GetMode() const { return Mode; }

protected:
	/** Internal painting functionality, is called from Paint and PaintVR and implemented in the derived painters */
	bool CombInternal(const FVector& InCameraOrigin, const FVector& InRayOrigin, const FVector& InRayDirection, ECombAction CombAction, float StrengthScale);

	/** Renders viewport interactor widget */
	void RenderInteractors(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI, bool bRenderVertices, ESceneDepthPriorityGroup DepthGroup = SDPG_World);

	/** Functionality for rendering the interactor widget given its positional information and paint action */
	void RenderInteractorWidget(const FVector& InCameraOrigin, const FVector& InRayOrigin, const FVector& InRayDirection, FPrimitiveDrawInterface* PDI, ECombAction CombAction, bool bRenderVertices, ESceneDepthPriorityGroup DepthGroup = SDPG_World);//TODO
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
		bool MirrorX;
		bool MirrorY;
		bool MirrorZ;
	};

	static const float MinLayerDist;

	/** Widget representing the state and settings for the painter */
	TSharedPtr<SFurCombModeWidget> Widget;

	/** Fur Comb settings */
	UFurCombSettings* FurCombSettings;

	TArray<UGFurComponent*> FurComponents;

#if 0
	/** Forces a specific LOD level to be rendered for the selected mesh components */
	void ApplyForcedLODIndex(int32 ForcedLODIndex);

	/** Updates the paint targets based on property changes on actors in the scene */
	void UpdatePaintTargets(UObject* InObject, struct FPropertyChangedEvent& InPropertyChangedEvent);

	/** Vertex color action callbacks */
	void FillWithVertexColor();
	void PropagateVertexColorsToAsset();
	void ImportVertexColors();
	void SavePaintedAssets();
	void SaveModifiedTextures();
protected:
	/** Texture paint state */
	/** Textures eligible for painting retrieved from the current selection */
	TArray<FPaintableTexture> PaintableTextures;
	/** Cached / stored instance texture paint settings for selected components */
	TMap<UMeshComponent*, FInstanceTexturePaintSettings> ComponentToTexturePaintSettingsMap;

	/** Temporary render target used to draw incremental paint to */
	UTextureRenderTarget2D* BrushRenderTargetTexture;

	/** Temporary render target used to store a mask of the affected paint region, updated every time we add incremental texture paint */
	UTextureRenderTarget2D* BrushMaskRenderTargetTexture;

	/** Temporary render target used to store generated mask for texture seams, we create this by projecting object triangles into texture space using the selected UV channel */
	UTextureRenderTarget2D* SeamMaskRenderTargetTexture;

	/** Stores data associated with our paint target textures */
	TMap< UTexture2D*, FPaintTexture2DData > PaintTargetData;

	/** Texture paint: Will hold a list of texture items that we can paint on */
	TArray<FTextureTargetListInfo> TexturePaintTargetList;

	/** Texture paint: The mesh components that we're currently painting */
	UMeshComponent* TexturePaintingCurrentMeshComponent;

	/** The original texture that we're painting */
	UTexture2D* PaintingTexture2D;

	/** True if we need to generate a texture seam mask used for texture dilation */
	bool bGenerateSeamMask;

	/** Used to store a flag that will tell the tick function to restore data to our rendertargets after they have been invalidated by a viewport resize. */
	bool bDoRestoreRenTargets;

	/** A map of the currently selected actors against info required for painting (selected material index etc)*/
	TMap<TWeakObjectPtr<AActor>, FMeshSelectedMaterialInfo> CurrentlySelectedActorsMaterialInfo;

	/** The currently selected actor, used to refer into the Map of Selected actor info */
	TWeakObjectPtr<AActor> ActorBeingEdited;
	// End texture paint state

	// Painter state
	/** Flag for updating cached data */
	bool bRefreshCachedData;
	/** Map of geometry adapters for each selected mesh component */
	TMap<UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>> ComponentToAdapterMap;
	// End painter state

	// Vertex paint state
	/** Current LOD index used for painting / forcing */
	int32 CachedLODIndex;
	/** Whether or not a specific LOD level should be forced */
	bool bCachedForceLOD;
	/** Flag whether or not the current selection contains per-LOD specific vertex colors */
	bool bSelectionContainsPerLODColors;
	/** Mesh components within the current selection which are eligible for painting */
	TArray<UMeshComponent*> PaintableComponents;
	/** Contains copied vertex color data */
	TArray<FPerComponentVertexColorData> CopiedColorsByComponent;
	// End vertex paint state

#endif // 0
	/** UI command list object */
	TSharedPtr<FUICommandList> UICommandList;

	bool LineTraceComponent(struct FHitResult& OutHit, const FVector Start, const FVector End, const struct FCollisionQueryParams& Params, UGFurComponent* FurComponent) const;

	static FVector MirrorVector(const FVector& Vec, const FVector& Vertex, const CombParams& Params);
	static FVector BendFur(const FVector& Dir, const FVector& Normal, const FVector& Offset);

	template<bool UseStrengthHeight, typename F, typename G>
	void Comb(UFurSplines* FurSplines, const TSet<int32>& SplineSet, const TArray<FVector4>& SplineVertexNormals, const CombParams& Params, const F& FuncPerSpline,
		const G& FuncPerSegment);

	void CombLength(UFurSplines* FurSplines, const TSet<int32>& SplineSet, const TArray<FVector4>& SplineVertexNormals, const CombParams& Params);
	void CombBend(UFurSplines* FurSplines, const TSet<int32>& SplineSet, const TArray<FVector4>& SplineVertexNormals, const CombParams& Params);
	void CombClump(UFurSplines* FurSplines, const TSet<int32>& SplineSet, const TArray<FVector4>& SplineVertexNormals, const CombParams& Params);
	void CombTwist(UFurSplines* FurSplines, const TSet<int32>& SplineSet, const TArray<FVector4>& SplineVertexNormals, const CombParams& Params);
	void CombNoise(UFurSplines* FurSplines, const TSet<int32>& SplineSet, const TArray<FVector4>& SplineVertexNormals, const CombParams& Params);
	void CombRelax(UFurSplines* FurSplines, const TSet<int32>& SplineSet, const TArray<FVector4>& SplineVertexNormals, const CombParams& Params);
};
