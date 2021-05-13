// Copyright 2019 GiM s.r.o. All Rights Reserved.

#include "FurCombEdMode.h"
#include "FurComb.h"
#include "FurCombModeToolKit.h"
#include "FurCombSettings.h"

#include "EdMode.h"
#include "EditorModeManager.h"
#include "EditorViewportClient.h"
#include "AssetRegistryModule.h"
#include "EditorReimportHandler.h"
#include "Editor/UnrealEd/Classes/Editor/EditorEngine.h"
#include "Editor/ViewportInteraction/Public/ViewportWorldInteraction.h"
#include "Editor/ViewportInteraction/Public/ViewportInteraction.h"
#include "EditorWorldExtension.h"
#include "Editor/UnrealEd/Public/Toolkits/BaseToolkit.h"
#include "Editor/UnrealEd/Public/Toolkits/ToolkitManager.h"
#include "VREditorMode.h"
#include "VREditorInteractor.h"
#include "Runtime/Engine/Classes/Engine/Selection.h"

#define LOCTEXT_NAMESPACE "FurCombEdMode"

const FEditorModeID FEdModeFurComb::EM_FurComb(TEXT("EM_FurComb"));

/** Constructor */
FEdModeFurComb::FEdModeFurComb()
	: FEdMode(),
	PaintingWithInteractorInVR(nullptr)
{
}

FEdModeFurComb::~FEdModeFurComb()
{
}

void FEdModeFurComb::Initialize()
{
	FurComb = FFurComb::Get();
}

TSharedPtr<class FModeToolkit> FEdModeFurComb::GetToolkit()
{
	return MakeShareable(new FFurCombModeToolKit(this));
}

/** FGCObject interface */
void FEdModeFurComb::AddReferencedObjects(FReferenceCollector& Collector)
{
	// Call parent implementation
	FEdMode::AddReferencedObjects(Collector);
	FurComb->AddReferencedObjects(Collector);
}

void FEdModeFurComb::Enter()
{
	// Call parent implementation
	FEdMode::Enter();

	checkf(FurComb != nullptr, TEXT("Fur Comb was not initialized"));

	// The user can manipulate the editor selection lock flag in paint mode so we save off the value here so it can be restored later
	bWasSelectionLockedOnStart = GEdSelectionLock;

	// Catch when objects are replaced when a construction script is rerun
	GEditor->OnObjectsReplaced().AddSP(this, &FEdModeFurComb::OnObjectsReplaced);

	// Hook into pre/post world save, so that the original collision volumes can be temporarily reinstated
	FEditorDelegates::PreSaveWorld.AddSP(this, &FEdModeFurComb::OnPreSaveWorld);
	FEditorDelegates::PostSaveWorld.AddSP(this, &FEdModeFurComb::OnPostSaveWorld);

	// Catch assets if they are about to be (re)imported
	GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPostImport.AddSP(this, &FEdModeFurComb::OnPostImportAsset);
//	FEditorDelegates::OnAssetPostImport.AddSP(this, &FEdModeFurComb::OnPostImportAsset);
	FReimportManager::Instance()->OnPostReimport().AddSP(this, &FEdModeFurComb::OnPostReimportAsset);

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().OnAssetRemoved().AddSP(this, &FEdModeFurComb::OnAssetRemoved);

	if (UsesToolkits() && !Toolkit.IsValid())
	{
		Toolkit = GetToolkit();
		Toolkit->Init(Owner->GetToolkitHost());
	}

	// Change the engine to draw selected objects without a color boost, but unselected objects will
	// be darkened slightly.  This just makes it easier to paint on selected objects without the
	// highlight effect distorting the appearance.
	GEngine->OverrideSelectedMaterialColor(FLinearColor::Black);

	UEditorWorldExtensionCollection* ExtensionCollection = GEditor->GetEditorWorldExtensionsManager()->GetEditorWorldExtensions(GetWorld());
	if (ExtensionCollection != nullptr)
	{
		// Register to find out about VR input events
		UViewportWorldInteraction* ViewportWorldInteraction = Cast<UViewportWorldInteraction>(ExtensionCollection->FindExtension(UViewportWorldInteraction::StaticClass()));
		if (ViewportWorldInteraction != nullptr)
		{
			ViewportWorldInteraction->OnViewportInteractionInputAction().RemoveAll(this);
			ViewportWorldInteraction->OnViewportInteractionInputAction().AddRaw(this, &FEdModeFurComb::OnVRAction);

			// Hide the VR transform gizmo while we're in mesh paint mode.  It sort of gets in the way of painting.
			ViewportWorldInteraction->SetTransformGizmoVisible(false);
		}
	}

	if (UsesToolkits())
	{
		FurComb->RegisterCommands(Toolkit->GetToolkitCommands());
	}

	USelection* SelectedActors = GEditor->GetSelectedActors();
	TArray<AActor*> Actors;
	SelectedActors->GetSelectedObjects(Actors);
	for (AActor* Actor : Actors)
		FurComb->ActorSelected(Actor);
}

void FEdModeFurComb::Exit()
{
	/** Finish up painting if we still are */
	if (FurComb->IsCombing())
	{
		FurComb->FinishCombing();
	}

	/** Reset paint state and unregister commands */
	if (UsesToolkits())
	{
		FurComb->UnregisterCommands(Toolkit->GetToolkitCommands());
	}

	UEditorWorldExtensionCollection* ExtensionCollection = GEditor->GetEditorWorldExtensionsManager()->GetEditorWorldExtensions(GetWorld());
	if (ExtensionCollection != nullptr)
	{
		UViewportWorldInteraction* ViewportWorldInteraction = Cast<UViewportWorldInteraction>(ExtensionCollection->FindExtension(UViewportWorldInteraction::StaticClass()));
		if (ViewportWorldInteraction != nullptr)
		{
			// Restore the transform gizmo visibility
			ViewportWorldInteraction->SetTransformGizmoVisible(true);

			// Unregister from event handlers
			ViewportWorldInteraction->OnViewportInteractionInputAction().RemoveAll(this);
		}
	}

	// The user can manipulate the editor selection lock flag in paint mode so we make sure to restore it here
	GEdSelectionLock = bWasSelectionLockedOnStart;

	// Restore selection color
	GEngine->RestoreSelectedMaterialColor();

	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Unbind delegates
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().OnAssetRemoved().RemoveAll(this);
	FReimportManager::Instance()->OnPostReimport().RemoveAll(this);
	GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPostImport.RemoveAll(this);
//	FEditorDelegates::OnAssetPostImport.RemoveAll(this);
	FEditorDelegates::PreSaveWorld.RemoveAll(this);
	FEditorDelegates::PostSaveWorld.RemoveAll(this);
	GEditor->OnObjectsReplaced().RemoveAll(this);
	USelection::SelectionChangedEvent.Remove(SelectionChangedHandle);

	// Call parent implementation
	FEdMode::Exit();
}

bool FEdModeFurComb::CapturedMouseMove(FEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InMouseX, int32 InMouseY)
{
	// We only care about perspective viewports
	if (BlockMouse)
		return true;

	bool bCombApplied = false;
	if (InViewportClient->IsPerspective())
	{
		if (FurComb->IsCombing())
		{
			// Compute a world space ray from the screen space mouse coordinates
			FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
				InViewportClient->Viewport,
				InViewportClient->GetScene(),
				InViewportClient->EngineShowFlags)
				.SetRealtimeUpdate(InViewportClient->IsRealtime()));
			FSceneView* View = InViewportClient->CalcSceneView(&ViewFamily);
			FViewportCursorLocation MouseViewportRay(View, (FEditorViewportClient*)InViewport->GetClient(), InMouseX, InMouseY);

			bCombApplied = FurComb->Comb(InViewport, View->ViewMatrices.GetViewOrigin(), MouseViewportRay.GetOrigin(), MouseViewportRay.GetDirection());
			if (bCombApplied)
				BlockMouse = true;
		}
	}

	return bCombApplied;
}

/** FEdMode: Called when the a mouse button is released */
bool FEdModeFurComb::EndTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport)
{
	InViewportClient->bLockFlightCamera = false;
	FurComb->FinishCombing();
	PaintingWithInteractorInVR = nullptr;
	return true;
}

/** FEdMode: Called when a key is pressed */
bool FEdModeFurComb::InputKey(FEditorViewportClient* InViewportClient, FViewport* InViewport, FKey InKey, EInputEvent InEvent)
{
	bool bHandled = FurComb->InputKey(InViewportClient, InViewport, InKey, InEvent);

	if (bHandled)
	{
		return bHandled;
	}
	const bool bIsLeftButtonDown = (InKey == EKeys::LeftMouseButton && InEvent != IE_Released) || InViewport->KeyState(EKeys::LeftMouseButton);
	const bool bIsRightButtonDown = (InKey == EKeys::RightMouseButton && InEvent != IE_Released) || InViewport->KeyState(EKeys::RightMouseButton);
	const bool bIsCtrlDown = ((InKey == EKeys::LeftControl || InKey == EKeys::RightControl) && InEvent != IE_Released) || InViewport->KeyState(EKeys::LeftControl) || InViewport->KeyState(EKeys::RightControl);
	const bool bIsShiftDown = ((InKey == EKeys::LeftShift || InKey == EKeys::RightShift) && InEvent != IE_Released) || InViewport->KeyState(EKeys::LeftShift) || InViewport->KeyState(EKeys::RightShift);
	const bool bIsAltDown = ((InKey == EKeys::LeftAlt || InKey == EKeys::RightAlt) && InEvent != IE_Released) || InViewport->KeyState(EKeys::LeftAlt) || InViewport->KeyState(EKeys::RightAlt);

	// When painting we only care about perspective viewports
	if (!bIsAltDown && InViewportClient->IsPerspective())
	{
		// Does the user want to paint right now?
		const bool bUserWantsPaint = bIsLeftButtonDown && !bIsRightButtonDown && !bIsAltDown;
		bool bCombApplied = false;

		// Stop current tracking if the user is no longer painting
		if (FurComb->IsCombing() && !bUserWantsPaint &&
			(InKey == EKeys::LeftMouseButton || InKey == EKeys::RightMouseButton || InKey == EKeys::LeftAlt || InKey == EKeys::RightAlt))
		{
			bHandled = true;
			FurComb->FinishCombing();
			PaintingWithInteractorInVR = nullptr;
		}
		else if (!FurComb->IsCombing() && bUserWantsPaint && !InViewportClient->IsMovingCamera())
		{
			bHandled = true;

			// Compute a world space ray from the screen space mouse coordinates
			FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
				InViewportClient->Viewport,
				InViewportClient->GetScene(),
				InViewportClient->EngineShowFlags)
				.SetRealtimeUpdate(InViewportClient->IsRealtime()));

			FSceneView* View = InViewportClient->CalcSceneView(&ViewFamily);
			const FViewportCursorLocation MouseViewportRay(View, (FEditorViewportClient*)InViewport->GetClient(), InViewport->GetMouseX(), InViewport->GetMouseY());

			// Paint!
			bCombApplied = FurComb->Comb(InViewport, View->ViewMatrices.GetViewOrigin(), MouseViewportRay.GetOrigin(), MouseViewportRay.GetDirection());

		}
		else if (FurComb->IsCombing() && bUserWantsPaint)
		{
			bHandled = true;
		}

		if (!bCombApplied && !FurComb->IsCombing())
		{
			bHandled = false;
		}
		else
		{
			InViewportClient->bLockFlightCamera = true;
		}

		// Also absorb other mouse buttons, and Ctrl/Alt/Shift events that occur while we're painting as these would cause
		// the editor viewport to start panning/dollying the camera
		{
			const bool bIsOtherMouseButtonEvent = (InKey == EKeys::MiddleMouseButton || InKey == EKeys::RightMouseButton);
			const bool bCtrlButtonEvent = (InKey == EKeys::LeftControl || InKey == EKeys::RightControl);
			const bool bShiftButtonEvent = (InKey == EKeys::LeftShift || InKey == EKeys::RightShift);
			const bool bAltButtonEvent = (InKey == EKeys::LeftAlt || InKey == EKeys::RightAlt);
			if (FurComb->IsCombing() && (bIsOtherMouseButtonEvent || bShiftButtonEvent || bAltButtonEvent))
			{
				bHandled = true;
			}

			if (bCtrlButtonEvent && !FurComb->IsCombing())
			{
				bHandled = false;
			}
			else if (bIsCtrlDown)
			{
				//default to assuming this is a paint command
				bHandled = true;

				// Allow Ctrl+B to pass through so we can support the finding of a selected static mesh in the content browser.
				if (!(bShiftButtonEvent || bAltButtonEvent || bIsOtherMouseButtonEvent) && ((InKey == EKeys::B) && (InEvent == IE_Pressed)))
				{
					bHandled = false;
				}

				// If we are not painting, we will let the CTRL-Z and CTRL-Y key presses through to support undo/redo.
				if (!FurComb->IsCombing() && (InKey == EKeys::Z || InKey == EKeys::Y))
				{
					bHandled = false;
				}
			}
		}
	}

	return bHandled;
}

void FEdModeFurComb::OnPreSaveWorld(uint32 SaveFlags, UWorld* World)
{
}

void FEdModeFurComb::OnPostSaveWorld(uint32 SaveFlags, UWorld* World, bool bSuccess)
{
}

void FEdModeFurComb::OnPostImportAsset(UFactory* Factory, UObject* Object)
{
}

void FEdModeFurComb::OnPostReimportAsset(UObject* Object, bool bSuccess)
{
}

void FEdModeFurComb::OnAssetRemoved(const FAssetData& AssetData)
{
}

void FEdModeFurComb::OnObjectsReplaced(const TMap<UObject*, UObject*>& OldToNewInstanceMap)
{
}

/** FEdMode: Called after an Undo operation */
void FEdModeFurComb::PostUndo()
{
	FEdMode::PostUndo();
	FurComb->PostUndo();
}

/** FEdMode: Render the mesh paint tool */
void FEdModeFurComb::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	/** Call parent implementation */
	FEdMode::Render(View, Viewport, PDI);
	FurComb->Render(View, Viewport, PDI);

	BlockMouse = false;

	// Flow painting
	if (FurComb->IsCombing())
	{
		/** If we are currently painting with a VR interactor, apply paint for the current vr interactor state/position */
		if (PaintingWithInteractorInVR != nullptr)
		{
			UVREditorInteractor* VRInteractor = Cast<UVREditorInteractor>(PaintingWithInteractorInVR);
			FVector LaserPointerStart, LaserPointerEnd;
			if (VRInteractor->GetLaserPointer( /* Out */ LaserPointerStart, /* Out */ LaserPointerEnd))
			{
				const FVector LaserPointerDirection = (LaserPointerEnd - LaserPointerStart).GetSafeNormal();
				UVREditorMode* VREditorMode = Cast<UVREditorMode>(GEditor->GetEditorWorldExtensionsManager()->GetEditorWorldExtensions(GetWorld())->FindExtension(UVREditorMode::StaticClass()));
				FurComb->CombVR(Viewport, VREditorMode->GetHeadTransform().GetLocation(), LaserPointerStart, LaserPointerDirection, VRInteractor);
			}
		}
		else if (FurComb->GetCurrentFurCombSettings()->bEnableFlow)
		{
			// Make sure the cursor is visible OR we're flood filling.  No point drawing a paint cue when there's no cursor.
			if (Viewport->IsCursorVisible())
			{
				// Grab the mouse cursor position
				FIntPoint MousePosition;
				Viewport->GetMousePos(MousePosition);

				// Is the mouse currently over the viewport? or flood filling
				if ((MousePosition.X >= 0 && MousePosition.Y >= 0 && MousePosition.X < (int32)Viewport->GetSizeXY().X && MousePosition.Y < (int32)Viewport->GetSizeXY().Y))
				{
					// Compute a world space ray from the screen space mouse coordinates
					FViewportCursorLocation MouseViewportRay(View, static_cast<FEditorViewportClient*>(Viewport->GetClient()), MousePosition.X, MousePosition.Y);
					FurComb->Comb(Viewport, View->ViewMatrices.GetViewOrigin(), MouseViewportRay.GetOrigin(), MouseViewportRay.GetDirection());
				}
			}
		}
	}
}

/** FEdMode: Handling SelectActor */
bool FEdModeFurComb::Select(AActor* InActor, bool bInSelected)
{
	if (bInSelected)
	{
		FurComb->ActorSelected(InActor);
	}
	else
	{
		FurComb->ActorDeselected(InActor);
	}

	return false;
}

/** FEdMode: Called when the currently selected actor has changed */
void FEdModeFurComb::ActorSelectionChangeNotify()
{
}

/** IMeshPaintEdMode: Called once per frame */
void FEdModeFurComb::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
	FEdMode::Tick(ViewportClient, DeltaTime);
	FurComb->Tick(ViewportClient, DeltaTime);
}

FFurComb* FEdModeFurComb::GetFurComb()
{
	checkf(FurComb != nullptr, TEXT("Invalid Fur comb ptr"));
	return FurComb;
}

bool FEdModeFurComb::ProcessEditDelete()
{
	return false;
}

void FEdModeFurComb::OnVRAction(class FEditorViewportClient& ViewportClient, UViewportInteractor* Interactor,
	const FViewportActionKeyInput& Action, bool& bOutIsInputCaptured, bool& bWasHandled)
{
	UVREditorMode* VREditorMode = Cast<UVREditorMode>(GEditor->GetEditorWorldExtensionsManager()->GetEditorWorldExtensions(GetWorld())->FindExtension(UVREditorMode::StaticClass()));
	UVREditorInteractor* VRInteractor = Cast<UVREditorInteractor>(Interactor);
	if (VREditorMode != nullptr && VREditorMode->IsActive() && VRInteractor != nullptr && VRInteractor->GetDraggingMode() == EViewportInteractionDraggingMode::Nothing)
	{
		if (Action.ActionType == ViewportWorldActionTypes::SelectAndMove)
		{
			if (!FurComb->IsCombing() && Action.Event == IE_Pressed && !VRInteractor->IsHoveringOverPriorityType())
			{
				// Check to see that we're clicking on a selected object.  You can only paint on selected things.  Otherwise,
				// we'll fall through to the normal interaction code which might cause the object to become selected.
				bool bIsClickingOnSelectedObject = false;
				{
					FHitResult HitResult = VRInteractor->GetHitResultFromLaserPointer();
					if (HitResult.Actor.IsValid())
					{
						UViewportWorldInteraction& WorldInteraction = VREditorMode->GetWorldInteraction();

						if (WorldInteraction.IsInteractableComponent(HitResult.GetComponent()))
						{
							AActor* Actor = HitResult.Actor.Get();

							// Make sure we're not hovering over some other viewport interactable, such as a dockable window selection bar or close button
							IViewportInteractableInterface* ActorInteractable = Cast<IViewportInteractableInterface>(Actor);
							if (ActorInteractable == nullptr)
							{
								if (Actor != WorldInteraction.GetTransformGizmoActor())  // Don't change any actor selection state if the user clicked on a gizmo
								{
									if (Actor->IsSelected())
									{
										bIsClickingOnSelectedObject = true;
									}
								}
							}
						}
					}
				}

				if (bIsClickingOnSelectedObject)
				{
					bWasHandled = true;
					bOutIsInputCaptured = true;

					// Go ahead and paint immediately
					FVector LaserPointerStart, LaserPointerEnd;
					if (Interactor->GetLaserPointer( /* Out */ LaserPointerStart, /* Out */ LaserPointerEnd))
					{
						const FVector LaserPointerDirection = (LaserPointerEnd - LaserPointerStart).GetSafeNormal();

						/** Apply painting using the current state/position of the VR interactor */
						const bool bAnyPaintableActorsUnderCursor = FurComb->CombVR(ViewportClient.Viewport, VREditorMode->GetHeadTransform().GetLocation(), LaserPointerStart, LaserPointerDirection, VRInteractor);
						if (bAnyPaintableActorsUnderCursor)
						{
							PaintingWithInteractorInVR = VRInteractor;
							ViewportClient.bLockFlightCamera = true;
						}
					}
				}
			}
			// Stop current tracking if the user is no longer painting
			else if (FurComb->IsCombing() && Action.Event == IE_Released && PaintingWithInteractorInVR && PaintingWithInteractorInVR == Interactor)
			{
				FurComb->FinishCombing();
				ViewportClient.bLockFlightCamera = false;
				PaintingWithInteractorInVR = nullptr;

				bWasHandled = true;
				bOutIsInputCaptured = false;
			}

			else if (FurComb->IsCombing())
			{
				// A different hand might be painting, so absorb the input
				bOutIsInputCaptured = bWasHandled = (Action.Event == IE_Released ? false : true);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE // "FurCombEdMode"
