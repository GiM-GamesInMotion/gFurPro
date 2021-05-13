// Copyright 2019 GiM s.r.o. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "EdMode.h"

class FEditorViewportClient;
class FPrimitiveDrawInterface;
class FSceneView;
class FViewport;
class UFactory;
class UViewportInteractor;
class FFurComb;

/**
 * Fur comb editor mode
 */
class GFUREDITOR_API FEdModeFurComb : public FEdMode
{
public:
	static const FEditorModeID EM_FurComb;

public:
	/** Constructor */
	FEdModeFurComb();

	/** Destructor */
	virtual ~FEdModeFurComb();

	virtual void Initialize() override;
	TSharedPtr<class FModeToolkit> GetToolkit();

	/** FGCObject interface */
	virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;

	// FEdMode interface
	virtual bool UsesToolkits() const override { return true; }
	virtual void Enter() override;
	virtual void Exit() override;
	virtual bool CapturedMouseMove( FEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InMouseX, int32 InMouseY ) override;
	virtual bool StartTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport) override { return true; }
	virtual bool EndTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport) override;
	virtual bool InputKey( FEditorViewportClient* InViewportClient, FViewport* InViewport, FKey InKey, EInputEvent InEvent ) override;
	virtual void PostUndo() override;
	virtual void Render( const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI ) override;
	virtual bool Select( AActor* InActor, bool bInSelected ) override;
	virtual void ActorSelectionChangeNotify() override;
	virtual bool AllowWidgetMove() override { return false; }
	virtual bool ShouldDrawWidget() const override { return false; }
	virtual bool UsesTransformWidget() const override { return false; }
	virtual void Tick( FEditorViewportClient* ViewportClient, float DeltaTime ) override;
	virtual bool ProcessEditDelete() override;
	// End of FEdMode interface

	/** Returns the fur comb for this mode */
	FFurComb* GetFurComb();
private:
	/** Called prior to saving a level */
	void OnPreSaveWorld(uint32 SaveFlags, UWorld* World);

	/** Called after saving a level */
	void OnPostSaveWorld(uint32 SaveFlags, UWorld* World, bool bSuccess);

	/** Called when an asset has just been imported */
	void OnPostImportAsset(UFactory* Factory, UObject* Object);

	/** Called when an asset has just been reimported */
	void OnPostReimportAsset(UObject* Object, bool bSuccess);

	/** Called when an asset is deleted */
	void OnAssetRemoved(const FAssetData& AssetData);

	/** Called when the user presses a button on their motion controller device */
	void OnVRAction( FEditorViewportClient& ViewportClient, class UViewportInteractor* Interactor,
		const struct FViewportActionKeyInput& Action, bool& bOutIsInputCaptured, bool& bWasHandled );

	/** Called when rerunning a construction script causes objects to be replaced */
	void OnObjectsReplaced(const TMap<UObject*, UObject*>& OldToNewInstanceMap);

private:	
	/** When painting in VR, this is the hand index that we're painting with.  Otherwise INDEX_NONE. */
	UViewportInteractor* PaintingWithInteractorInVR;
	
	/** Will store the state of selection locks on start of paint mode so that it can be restored on close */
	bool bWasSelectionLockedOnStart;
	/** Delegate handle for registered selection change lambda */
	FDelegateHandle SelectionChangedHandle;
protected:
	/** Painter used by this edit mode for applying paint actions */
	FFurComb* FurComb;
	bool BlockMouse = false;
};