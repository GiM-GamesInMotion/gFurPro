// Copyright 2019 GiM s.r.o. All Rights Reserved.

#include "FurComb.h"
#include "FurCombCommands.h"
#include "FurCombSettings.h"
#include "FurComponent.h"
#include "FurSplines.h"

#include "EditorViewportClient.h"
#include "Runtime/Slate/Public/Framework/Commands/UICommandList.h"
#include "ScopedTransaction.h"
#include "SFurCombModeWidget.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Public/Rendering/SkeletalMeshRenderData.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Editor/MeshPaint/Public/MeshPaintHelpers.h"
#include "VREditorInteractor.h"

#define LOCTEXT_NAMESPACE "FurComb"

const float FFurComb::MinLayerDist = 0.01f;

FFurComb::FFurComb()
	: CurrentViewportInteractor(nullptr),
	TimeSinceStartedPainting(0.0f),
	Time(0.0f),
	Mode(EFurCombMode::Length),
	bCombing(false),
	CombTransaction(nullptr)
{}

FFurComb::~FFurComb()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
/*	ComponentToAdapterMap.Empty();
	ComponentToTexturePaintSettingsMap.Empty();*/
}

void FFurComb::Init()
{
	/** Setup necessary data */
	FurCombSettings = DuplicateObject<UFurCombSettings>(GetMutableDefault<UFurCombSettings>(), GetTransientPackage());
	FurCombSettings->AddToRoot();
//	PaintSettings = UPaintModeSettings::Get();
	FFurCombCommands::Register();
	UICommandList = TSharedPtr<FUICommandList>(new FUICommandList());
/*	RegisterVertexPaintCommands();
	RegisterTexturePaintCommands();*/
	Widget = SNew(SFurCombModeWidget, this);
/*	CachedLODIndex = PaintSettings->VertexPaintSettings.LODIndex;
	bCachedForceLOD = PaintSettings->VertexPaintSettings.bPaintOnSpecificLOD;
	FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FPaintModePainter::UpdatePaintTargets);*/
}

FFurComb* FFurComb::Get()
{
	static FFurComb* Comb = nullptr;
	static bool bInit = false;
	if (!bInit)
	{
		bInit = true;
		Comb = new FFurComb();
		Comb->Init();
	}
	return Comb;
}

TSharedPtr<FUICommandList> FFurComb::GetUICommandList()
{
	return UICommandList;
}

void FFurComb::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	/** Render viewport interactors */
	RenderInteractors(View, Viewport, PDI, true/*PaintSettings->PaintMode == EPaintMode::Vertices*/);//TODO
}

void FFurComb::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
/*	if (bCombing)
	{
		TimeSinceStartedPainting += DeltaTime;
	}

	if (ViewportClient->IsPerspective())
	{
		// Make sure perspective viewports are still set to real-time
		MeshPaintHelpers::SetRealtimeViewport(true);

		// Set viewport show flags		
//		MeshPaintHelpers::SetViewportColorMode(GetBrushSettings()->ColorViewMode, ViewportClient);TODO
	}

	Time += DeltaTime;


	if (bRefreshCachedData)
	{
		bRefreshCachedData = false;
		CacheSelectionData();
		CacheTexturePaintData();

		bDoRestoreRenTargets = true;
	}

	// Will set the texture override up for the selected texture, important for the drop down combo-list and selecting between material instances.
	if (PaintSettings->PaintMode == EPaintMode::Textures && PaintableComponents.Num() == 1 && PaintSettings->TexturePaintSettings.PaintTexture)
	{
		for (UMeshComponent* MeshComponent : PaintableComponents)
		{
			TSharedPtr<IMeshPaintGeometryAdapter>* MeshAdapter = ComponentToAdapterMap.Find(MeshComponent);
			if (MeshAdapter)
			{
				SetSpecificTextureOverrideForMesh(*MeshAdapter->Get(), PaintSettings->TexturePaintSettings.PaintTexture);
			}
		}
	}

	// If this is true probably someone force deleted a texture out from under us
	bool bBadAssetFound = false;

	if (bDoRestoreRenTargets && PaintSettings->PaintMode == EPaintMode::Textures)
	{
		if (PaintingTexture2D == nullptr)
		{
			for (TMap< UTexture2D*, FPaintTexture2DData >::TIterator It(PaintTargetData); It; ++It)
			{

				if (!It.Key())
				{
					bBadAssetFound = true;
					break;
				}

				FPaintTexture2DData* TextureData = &It.Value();
				if (TextureData->PaintRenderTargetTexture != nullptr)
				{

					bool bIsSourceTextureStreamedIn = TextureData->PaintingTexture2D->IsFullyStreamedIn();

					if (!bIsSourceTextureStreamedIn)
					{
						//   Make sure it is fully streamed in before we try to do anything with it.
						TextureData->PaintingTexture2D->SetForceMipLevelsToBeResident(30.0f);
						TextureData->PaintingTexture2D->WaitForStreaming();
					}

					//Use the duplicate texture here because as we modify the texture and do undo's, it will be different over the original.
					TexturePaintHelpers::SetupInitialRenderTargetData(TextureData->PaintingTexture2D, TextureData->PaintRenderTargetTexture);

				}
			}
		}
		// We attempted a restore of the rendertargets so go ahead and clear the flag
		bDoRestoreRenTargets = false;
	}

	if (bBadAssetFound)
	{
		PaintTargetData.Empty();
	}*/
}

void FFurComb::RegisterCommands(TSharedRef<FUICommandList> CommandList)
{
	const FFurCombCommands& Commands = FFurCombCommands::Get();

	// Lambda used to alter the fur comb size
	auto CombLambda = [this](float Multiplier)
	{
		const float CombChangeValue = 0.05f;
		float CombRadius = GetFurCombSettings()->GetRadius();
		CombRadius *= (1.f + (CombChangeValue * Multiplier));
		GetFurCombSettings()->SetRadius(CombRadius);
	};
	CommandList->MapAction(Commands.IncreaseCombSize, FExecuteAction::CreateLambda(CombLambda, 1.0f), FCanExecuteAction(), EUIActionRepeatMode::RepeatEnabled);
	CommandList->MapAction(Commands.DecreaseCombSize, FExecuteAction::CreateLambda(CombLambda, -1.0f), FCanExecuteAction(), EUIActionRepeatMode::RepeatEnabled);


	/** Lambda used to cycle through available textures to paint on */
/*	auto TextureCycleLambda = [this](int32 Direction) TODO
	{
		UTexture2D*& SelectedTexture = PaintSettings->TexturePaintSettings.PaintTexture;

		const int32 TextureIndex = (SelectedTexture != nullptr) ? PaintableTextures.IndexOfByKey(SelectedTexture) : 0;
		if (TextureIndex != INDEX_NONE)
		{
			int32 NewTextureIndex = TextureIndex + Direction;
			if (NewTextureIndex < 0)
			{
				NewTextureIndex += PaintableTextures.Num();
			}
			NewTextureIndex %= PaintableTextures.Num();

			if (PaintableTextures.IsValidIndex(NewTextureIndex))
			{
				SelectedTexture = (UTexture2D*)PaintableTextures[NewTextureIndex].Texture;
			}
		}
	};*/

	/** Map next and previous texture commands to lambda */
/*	auto TexturePaintModeLambda = [this]() -> bool { return PaintSettings->PaintMode == EPaintMode::Textures;  };
	CommandList->MapAction(Commands.NextTexture, FExecuteAction::CreateLambda(TextureCycleLambda, 1), FCanExecuteAction::CreateLambda(TexturePaintModeLambda));
	CommandList->MapAction(Commands.PreviousTexture, FExecuteAction::CreateLambda(TextureCycleLambda, -1), FCanExecuteAction::CreateLambda(TexturePaintModeLambda));

	CommandList->MapAction(Commands.SwitchForeAndBackgroundColor, FExecuteAction::CreateLambda([this]()
	{
		if (PaintSettings->PaintMode == EPaintMode::Vertices)
		{
			const FLinearColor Temp = PaintSettings->VertexPaintSettings.PaintColor;
			PaintSettings->VertexPaintSettings.PaintColor = PaintSettings->VertexPaintSettings.EraseColor;
			PaintSettings->VertexPaintSettings.EraseColor = Temp;
		}
	}));
	
	CommandList->MapAction(Commands.CycleToNextLOD, FExecuteAction::CreateRaw(this, &FPaintModePainter::CycleMeshLODs, 1));
	CommandList->MapAction(Commands.CycleToPreviousLOD, FExecuteAction::CreateRaw(this, &FPaintModePainter::CycleMeshLODs, -1));*/
	
	/** Map commit texture painting to commiting all the outstanding paint changes */
//	auto CanCommitLambda = [this]() -> bool { return GetNumberOfPendingPaintChanges() > 0; };
//	CommandList->MapAction(Commands.CommitTexturePainting, FExecuteAction::CreateLambda([this]() { CommitAllPaintedTextures(); }), FCanExecuteAction::CreateLambda([this]() -> bool { return GetNumberOfPendingPaintChanges() > 0; }));
}

void FFurComb::UnregisterCommands(TSharedRef<FUICommandList> CommandList)
{
	const FFurCombCommands& Commands = FFurCombCommands::Get();
	for (const TSharedPtr<const FUICommandInfo> Action : Commands.Commands)
	{
		CommandList->UnmapAction(Action);
	}
}

bool FFurComb::Comb(FViewport* Viewport, const FVector& InCameraOrigin, const FVector& InRayOrigin, const FVector& InRayDirection)
{
	// Determine paint action according to whether or not shift is held down
	const ECombAction CombAction = (Viewport->KeyState(EKeys::LeftShift) || Viewport->KeyState(EKeys::RightShift)) ? ECombAction::Inverse : ECombAction::Normal;

	const float Strength = Viewport->IsPenActive() ? Viewport->GetTabletPressure() : 1.f;
	// Handle internal painting functionality
	return CombInternal(InCameraOrigin, InRayOrigin, InRayDirection, CombAction, Strength);
}

bool FFurComb::CombVR(FViewport* Viewport, const FVector& InCameraOrigin, const FVector& InRayOrigin, const FVector& InRayDirection, UVREditorInteractor* VRInteractor)
{
	bool bCombApplied = false;
	// When painting using VR, allow the modifier button to activate Erase mode
	if (VRInteractor)
	{
		// Determine custom paint strength from trigger
		const float StrengthScale = VRInteractor->GetSelectAndMoveTriggerValue();
		ECombAction CombAction;
		const bool bIsModifierPressed = VRInteractor->IsModifierPressed();
		// Determine paint action according to whether or not modifier button is pressed
		CombAction = bIsModifierPressed ? ECombAction::Inverse : ECombAction::Normal;

		// Handle internal painting functionality
		bCombApplied = CombInternal(InCameraOrigin, InRayOrigin, InRayDirection, CombAction, StrengthScale);

		if (bCombApplied)
		{
			CurrentViewportInteractor = VRInteractor;
		}
	}

	return bCombApplied;
}

bool FFurComb::InputKey(FEditorViewportClient* InViewportClient, FViewport* InViewport, FKey InKey, EInputEvent InEvent)
{
	bool bHandled = false;

	// Lambda used to alter the paint brush size
	auto CombLambda = [this](float Multiplier)
	{
		const float CombChangeValue = 0.05f;
		float CombRadius = GetFurCombSettings()->GetRadius();
		CombRadius *= (1.f + (CombChangeValue * Multiplier));
		GetFurCombSettings()->SetRadius(CombRadius);
	};

	if (InKey == EKeys::LeftBracket)
	{
		CombLambda(-1.0f);
		bHandled = true;
	}
	else if (InKey == EKeys::RightBracket)
	{
		CombLambda(1.0f);
		bHandled = true;
	}

	return bHandled;
}

void FFurComb::FinishCombing()
{
	if (bCombing)
	{
		bCombing = false;
		EndTransaction();
	}

	CurrentViewportInteractor = nullptr;
}

void FFurComb::ActorSelected(AActor* Actor)
{
	Actor->GetComponents<UGFurComponent>(FurComponents);
/*	
	TInlineComponentArray<UMeshComponent*> MeshComponents;
	Actor->GetComponents<UMeshComponent>(MeshComponents);

	for (UMeshComponent* MeshComponent : MeshComponents)
	{
		FInstanceTexturePaintSettings& Settings = AddOrRetrieveInstanceTexturePaintSettings(MeshComponent);
		PaintSettings->TexturePaintSettings.PaintTexture = Settings.SelectedTexture;
		PaintSettings->TexturePaintSettings.UVChannel = Settings.SelectedUVChannel;
	}
*/
	Refresh();
}

void FFurComb::ActorDeselected(AActor* Actor)
{
	FurComponents.Reset();

	Refresh();
}

void FFurComb::AddReferencedObjects(FReferenceCollector& Collector)
{
/*	Collector.AddReferencedObject(TexturePaintingCurrentMeshComponent);
	Collector.AddReferencedObject(PaintingTexture2D);
	Collector.AddReferencedObject(BrushRenderTargetTexture);
	Collector.AddReferencedObject(BrushMaskRenderTargetTexture);
	Collector.AddReferencedObject(SeamMaskRenderTargetTexture);
	for (TMap< UTexture2D*, FPaintTexture2DData >::TIterator It(PaintTargetData); It; ++It)
	{
		Collector.AddReferencedObject(It.Key());
		It.Value().AddReferencedObjects(Collector);
	}

	for (TMap< UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>>::TIterator It(ComponentToAdapterMap); It; ++It)
	{
		Collector.AddReferencedObject(It.Key());
		It.Value()->AddReferencedObjects(Collector);
	}*/
}

UFurCombSettings* FFurComb::GetFurCombSettings()
{
	return FurCombSettings;
}

TSharedPtr<class SWidget> FFurComb::GetWidget()
{
	return Widget;
}

const FHitResult FFurComb::GetHitResult(const FVector& Origin, const FVector& Direction)
{
	TArray<UGFurComponent*> HoveredComponents;
	HoveredComponents.Empty(FurComponents.Num());

	// Fire out a ray to see if there is a *selected* component under the mouse cursor that can be painted.
	// NOTE: We can't use a GWorld line check for this as that would ignore components that have collision disabled
	FHitResult BestTraceResult;
	{
		const FVector TraceStart(Origin);
		const FVector TraceEnd(Origin + Direction * HALF_WORLD_MAX);

		for (UGFurComponent* FurComponent : FurComponents)
		{
			// Ray trace
			FHitResult TraceHitResult(1.0f);

			if (LineTraceComponent(TraceHitResult, TraceStart, TraceEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true), FurComponent))
			{
				// Find the closest impact
				if ((BestTraceResult.GetComponent() == nullptr) || (TraceHitResult.Time < BestTraceResult.Time))
				{
					BestTraceResult = TraceHitResult;
				}
			}
		}
	}

	return BestTraceResult;
}

void FFurComb::Refresh()
{
	// Ensure that we call OnRemoved while adapter/components are still valid
/*	PaintableComponents.Empty();
	for (auto MeshAdapterPair : ComponentToAdapterMap)
	{
		MeshAdapterPair.Value->OnRemoved();
	}
	ComponentToAdapterMap.Empty();

	bRefreshCachedData = true;*/
}

void FFurComb::Reset()
{
	//If we're painting vertex colors then propagate the painting done on LOD0 to all lower LODs. 
	//Then stop forcing the LOD level of the mesh to LOD0.
/*	ApplyForcedLODIndex(-1);

	// If the user has pending changes and the editor is not exiting, we want to do the commit for all the modified textures.
	if ((GetNumberOfPendingPaintChanges() > 0) && !GIsRequestingExit)
	{
		CommitAllPaintedTextures();
	}
	else
	{
		ClearAllTextureOverrides();
	}

	PaintTargetData.Empty();

	// Remove any existing texture targets
	TexturePaintTargetList.Empty();

	// Cleanup all cached 
	for (auto MeshAdapterPair : ComponentToAdapterMap)
	{
		MeshAdapterPair.Value->OnRemoved();
	}
	ComponentToAdapterMap.Empty();*/
}

void FFurComb::PostUndo()
{
	for (UGFurComponent* FurComponent : FurComponents)
	{
		if (FurComponent->FurSplines)
			FurComponent->FurSplines->Refresh();
	}
}

bool FFurComb::CombInternal(const FVector& InCameraOrigin, const FVector& InRayOrigin, const FVector& InRayDirection, ECombAction CombAction, float StrengthScale)
{
	const float CombRadius = FurCombSettings->GetRadius();
	const float CombFalloff = FurCombSettings->FalloffAmount;
	// Fire out a ray to see if there is a *selected* component under the mouse cursor that can be painted.
	// NOTE: We can't use a GWorld line check for this as that would ignore components that have collision disabled

	TArray<UGFurComponent*> HoveredComponents;
	FHitResult BestTraceResult;
	{
		const FVector TraceStart(InRayOrigin);
		const FVector TraceEnd(InRayOrigin + InRayDirection * HALF_WORLD_MAX);

		for (UGFurComponent* FurComponent : FurComponents)
		{
/*			TSharedPtr<IMeshPaintGeometryAdapter>* MeshAdapterPtr = ComponentToAdapterMap.Find(MeshComponent);
			if (!MeshAdapterPtr)
			{
				continue;
			}
			TSharedPtr<IMeshPaintGeometryAdapter> MeshAdapter = *MeshAdapterPtr;*/

			// Ray trace
			FHitResult TraceHitResult(1.0f);

			if (LineTraceComponent(TraceHitResult, TraceStart, TraceEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true), FurComponent))
			{
				// Find the closest impact
				if ((BestTraceResult.GetComponent() == nullptr) || (TraceHitResult.Time < BestTraceResult.Time))
				{
					BestTraceResult = TraceHitResult;
				}
			}
		}

		if (BestTraceResult.GetComponent() != nullptr)
		{
			FBox BrushBounds = FBox::BuildAABB(BestTraceResult.Location, FVector(CombRadius * 1.25f, CombRadius * 1.25f, CombRadius * 1.25f));

			// Vertex paint mode, so we want all valid components overlapping the brush hit location
			for (auto TestComponent : FurComponents)
			{
				const FBox ComponentBounds = TestComponent->Bounds.GetBox();

				if (ComponentBounds.Intersect(BrushBounds))
				{
					// OK, this mesh potentially overlaps the brush!
					HoveredComponents.AddUnique(TestComponent);
				}
			}
		}
	}

	bool bCombApplied = false;

	if (HoveredComponents.Num() > 0)
	{
		if (bCombing == false)
		{
			BeginTransaction(LOCTEXT("FurCombMode_TransactionComb", "Fur Comb"));//TODO
			bCombing = true;
			OldLocation = BestTraceResult.Location;
			TimeSinceStartedPainting = 0.0f;
		}

		FVector CombXAxis, CombYAxis;
		BestTraceResult.Normal.FindBestAxisVectors(CombXAxis, CombYAxis);
		// Display settings
		const float VisualBiasDistance = 0.15f;
		const FVector CombVisualPosition = BestTraceResult.Location + BestTraceResult.Normal * VisualBiasDistance;

//		const FLinearColor PaintColor = (PaintSettings->PaintMode == EPaintMode::Vertices) ? (PaintSettings->VertexPaintSettings.PaintColor) : PaintSettings->TexturePaintSettings.PaintColor;
//		const FLinearColor EraseColor = (PaintSettings->PaintMode == EPaintMode::Vertices) ? (PaintSettings->VertexPaintSettings.EraseColor) : PaintSettings->TexturePaintSettings.EraseColor;

		// NOTE: We square the comb strength to maximize slider precision in the low range
		float CombStrength = FurCombSettings->Strength * FurCombSettings->Strength * StrengthScale;
		if (CombAction == ECombAction::Inverse)
			CombStrength = -CombStrength;

		// Iterate over the selected meshes under the cursor and paint them!
		for (UGFurComponent* FurComponent : HoveredComponents)
		{
			UFurSplines* FurSplines = FurComponent->FurSplines;
			if (FurSplines == nullptr)
				continue;

			TArray<FVector4> SplineVertexNormals;
			TSet<int32> SplineSet;
			CombParams Params;
			{
				const float MinFurLength = FurComponent->MinFurLength;

				const FTransform& ComponentTransform = FurComponent->GetComponentTransform();

				const FTransform InverseComponentTransform = ComponentTransform.Inverse();
				const FVector ComponentSpaceLocation = InverseComponentTransform.TransformPosition(BestTraceResult.Location);
				const float ComponentSpaceRadius = InverseComponentTransform.TransformVector(FVector(CombRadius, 0.0f, 0.0f)).Size();
				const float ComponentSpaceRadiusSquared = ComponentSpaceRadius * ComponentSpaceRadius;

				SplineSet.Reserve(FurSplines->Index.Num());
				SplineVertexNormals.AddUninitialized(FurSplines->Index.Num());

				const FStaticMeshVertexBuffers* VertexBuffers;
				if (FurComponent->SkeletalGrowMesh)
				{
					USkeletalMesh* Mesh = FurComponent->SkeletalGrowMesh;
					VertexBuffers = &Mesh->GetResourceForRendering()->LODRenderData[0].StaticVertexBuffers;
				}
				else if (FurComponent->StaticGrowMesh)
				{
					UStaticMesh* Mesh = FurComponent->StaticGrowMesh;
					VertexBuffers = &Mesh->RenderData->LODResources[0].VertexBuffers;
				}
				else
				{
					continue;
				}

				const FPositionVertexBuffer& Positions = VertexBuffers->PositionVertexBuffer;
				const FStaticMeshVertexBuffer& Vertices = VertexBuffers->StaticMeshVertexBuffer;
				int32 VertexCount = Positions.GetNumVertices();
				for (int32 VertexIndex = 0; VertexIndex < VertexCount; VertexIndex++)
				{
					const FVector& Position = Positions.VertexPosition(VertexIndex);
					if (FVector::DistSquared(Position, ComponentSpaceLocation) > ComponentSpaceRadiusSquared)
						continue;

					int ClosestSplineIndex = -1;
					float ClosestDist = FLT_MAX;
					for (int si = 0; si < FurSplines->Index.Num(); si++)
					{
						FVector p = FurSplines->Vertices[FurSplines->Index[si]];
						p.Y = -p.Y;
						float d = FVector::DistSquared(p, Position);
						if (d < ClosestDist)
						{
							ClosestDist = d;
							ClosestSplineIndex = si;
						}
					}
					if (ClosestDist < 0.01f)
					{
						uint32 idx = FurSplines->Index[ClosestSplineIndex];
						FVector p1 = FurSplines->Vertices[idx];
						FVector p2 = FurSplines->Vertices[idx + FurSplines->Count[ClosestSplineIndex] - 1];
						FVector normal = Vertices.VertexTangentZ(VertexIndex);
						normal.Y = -normal.Y;
						if (FVector::DotProduct(p2 - p1, normal) > 0.0f || MinFurLength > 0.0f)
						{
							bool IsAlreadyInSet;
							SplineSet.Add(ClosestSplineIndex, &IsAlreadyInSet);
							FVector4 Norm(normal.X, normal.Y, normal.Z, 1.0f);
							if (IsAlreadyInSet)
								SplineVertexNormals[ClosestSplineIndex] += Norm;
							else
								SplineVertexNormals[ClosestSplineIndex] = Norm;
						}
					}
				}

				Params.Location = ComponentSpaceLocation;
				Params.Location.Y = -Params.Location.Y;
				Params.OldLocation = InverseComponentTransform.TransformPosition(OldLocation);
				Params.OldLocation.Y = -Params.OldLocation.Y;
				Params.Normal = InverseComponentTransform.TransformVector(BestTraceResult.Normal);
				Params.Normal.Y = -Params.Normal.Y;
				Params.CombRadius = ComponentSpaceRadius;
				Params.Strength = CombStrength;
				Params.Falloff = CombFalloff;
			}


			if (SplineSet.Num())
			{
				FurSplines->Modify();
				switch (Mode)
				{
				case EFurCombMode::Length:
					CombLength(FurSplines, SplineSet, SplineVertexNormals, Params);
					break;
				case EFurCombMode::Bend:
					CombBend(FurSplines, SplineSet, SplineVertexNormals, Params);
					break;
				case EFurCombMode::Clump:
					CombClump(FurSplines, SplineSet, SplineVertexNormals, Params);
					break;
				case EFurCombMode::Twist:
					CombTwist(FurSplines, SplineSet, SplineVertexNormals, Params);
					break;
				case EFurCombMode::Noise:
					CombNoise(FurSplines, SplineSet, SplineVertexNormals, Params);
					break;
				}
				bCombApplied = true;
				FurSplines->Refresh();
			}
		}
	}

	return bCombApplied;
}












void FFurComb::RenderInteractors(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI, bool bRenderVertices, ESceneDepthPriorityGroup DepthGroup/* = SDPG_World*/)
{
	TArray<MeshPaintHelpers::FPaintRay> PaintRays;
	MeshPaintHelpers::RetrieveViewportPaintRays(View, Viewport, PDI, PaintRays);

	// Apply paint pressure and start painting (or if not currently painting, draw a preview of where paint will be applied)
	for (const MeshPaintHelpers::FPaintRay& PaintRay : PaintRays)
	{
		const UVREditorInteractor* VRInteractor = Cast<UVREditorInteractor>(PaintRay.ViewportInteractor);
		ECombAction CombAction = VRInteractor ? (VRInteractor->IsModifierPressed() ? ECombAction::Inverse : ECombAction::Normal)
			: (Viewport->KeyState(EKeys::LeftControl) || Viewport->KeyState(EKeys::RightControl)) ? ECombAction::Inverse : ECombAction::Normal;

		RenderInteractorWidget(PaintRay.CameraLocation, PaintRay.RayStart, PaintRay.RayDirection, PDI, CombAction, bRenderVertices, DepthGroup);
	}
}

void FFurComb::RenderInteractorWidget(const FVector& InCameraOrigin, const FVector& InRayOrigin, const FVector& InRayDirection, FPrimitiveDrawInterface* PDI, ECombAction CombAction, bool bRenderVertices, ESceneDepthPriorityGroup DepthGroup /*= SDPG_World*/)
{
	const UFurCombSettings* CombSettings = GetFurCombSettings();
	const float CombRadius = CombSettings->GetRadius();

	const FHitResult& HitResult = GetHitResult(InRayOrigin, InRayDirection);

	if (HitResult.Component != nullptr)
	{
		// Brush properties
		const float CombFalloffAmount = CombSettings->FalloffAmount;

		// Display settings
		const float VisualBiasDistance = 0.15f;
		const FLinearColor CombCueColor = (bCombing ? FLinearColor(1.0f, 1.0f, 0.3f) : FLinearColor(0.3f, 1.0f, 0.3f));

		FVector BrushXAxis, BrushYAxis;
		HitResult.Normal.FindBestAxisVectors(BrushXAxis, BrushYAxis);
		const FVector CombVisualPosition = HitResult.Location + HitResult.Normal * VisualBiasDistance;


		if (PDI != NULL)
		{
			const int32 NumSides = 128;
			DrawWireSphere(PDI, FTransform(BrushXAxis, BrushYAxis, HitResult.Normal, CombVisualPosition), CombCueColor, CombRadius, NumSides, DepthGroup);

			// Also draw the inner brush radius
			const float InnerCombRadius = (CombRadius * CombFalloffAmount);
			DrawWireSphere(PDI, FTransform(BrushXAxis, BrushYAxis, HitResult.Normal, CombVisualPosition), CombCueColor, InnerCombRadius, NumSides, DepthGroup);

			// Draw trace surface normal
			const FVector NormalLineEnd(CombVisualPosition + HitResult.Normal * CombRadius);
			PDI->DrawLine(CombVisualPosition, NormalLineEnd, CombCueColor, DepthGroup);

/*			UGFurComponent* FurComponent = Cast<UGFurComponent>(HitResult.Component.Get());
			if (FurComponent && bRenderVertices)
			{
				const FMatrix ComponentToWorldMatrix = FurComponent->GetComponentToWorld().ToMatrixWithScale();
				const FVector ComponentSpaceCameraPosition(ComponentToWorldMatrix.InverseTransformPosition(InCameraOrigin));
				const FVector ComponentSpaceBrushPosition(ComponentToWorldMatrix.InverseTransformPosition(HitResult.Location));

				// @todo MeshPaint: Input vector doesn't work well with non-uniform scale
				const float ComponentSpaceBrushRadius = ComponentToWorldMatrix.InverseTransformVector(FVector(BrushRadius, 0.0f, 0.0f)).Size();
				const float ComponentSpaceSquaredBrushRadius = ComponentSpaceBrushRadius * ComponentSpaceBrushRadius;

//				const TArray<FVector>& InRangeVertices = MeshAdapter->SphereIntersectVertices(ComponentSpaceSquaredBrushRadius, ComponentSpaceBrushPosition, ComponentSpaceCameraPosition, GetBrushSettings()->bOnlyFrontFacingTriangles);
				TArray<FVector> InRangeVertices;


				//TODO
				if (FurComponent->SkeletalGrowMesh)
				{
					USkeletalMesh* Mesh = FurComponent->SkeletalGrowMesh;
					FSkeletalMeshRenderData* RenderData = Mesh->GetResourceForRendering();
					auto* IndexBuffer = RenderData->LODRenderData[0].MultiSizeIndexContainer.GetIndexBuffer();
					auto& VertexBuffer = RenderData->LODRenderData[0].StaticVertexBuffers.PositionVertexBuffer;
					const int32 NumTriangles = IndexBuffer->Num() / 3;

					for (int32 TriangleIndex = 0; TriangleIndex < NumTriangles; ++TriangleIndex)
					{
						// Compute the normal of the triangle
						const FVector& P0 = VertexBuffer.VertexPosition(IndexBuffer->Get((TriangleIndex * 3) + 0));
						const FVector& P1 = VertexBuffer.VertexPosition(IndexBuffer->Get((TriangleIndex * 3) + 1));
						const FVector& P2 = VertexBuffer.VertexPosition(IndexBuffer->Get((TriangleIndex * 3) + 2));
						InRangeVertices.Add(P0);
						InRangeVertices.Add(P1);
						InRangeVertices.Add(P2);
					}
				}



				for (const FVector& Vertex : InRangeVertices)
				{
					const FVector WorldPositionVertex = ComponentToWorldMatrix.TransformPosition(Vertex);
					if ((HitResult.Location - WorldPositionVertex).Size() <= BrushRadius)
					{
						const FVector VertexVisualPosition = WorldPositionVertex + HitResult.Normal * VisualBiasDistance;
						PDI->DrawPoint(VertexVisualPosition, HoverVertexPointColor, PointDrawSize, DepthGroup);
					}
				}
			}*/
		}
	}
}

void FFurComb::BeginTransaction(const FText Description)
{
	// In paint mode we only allow the BeginTransaction to be called with the EndTransaction pair. We should never be
	// in a state where a second transaction was started before the first was ended.
	checkf(CombTransaction == NULL, TEXT("Cannot create Transaction while another one is still active"));
	if (CombTransaction == NULL)
	{
		CombTransaction = new FScopedTransaction(Description);
	}
}

void FFurComb::EndTransaction()
{
	checkf(CombTransaction != NULL, TEXT("Cannot end Transaction since there isn't one Active"));
	delete CombTransaction;
	CombTransaction = NULL;
}










bool FFurComb::LineTraceComponent(struct FHitResult& OutHit, const FVector Start, const FVector End, const struct FCollisionQueryParams& Params, UGFurComponent* FurComponent) const
{
	const bool bHitBounds = FMath::LineSphereIntersection(Start, End.GetSafeNormal(), (End - Start).SizeSquared(), FurComponent->Bounds.Origin, FurComponent->Bounds.SphereRadius);
	const float SqrRadius = FMath::Square(FurComponent->Bounds.SphereRadius);
	const bool bInsideBounds = (FurComponent->Bounds.ComputeSquaredDistanceFromBoxToPoint(Start) <= SqrRadius) || (FurComponent->Bounds.ComputeSquaredDistanceFromBoxToPoint(End) <= SqrRadius);

	bool bHitTriangle = false;
	if (bHitBounds || bInsideBounds)
	{
		float MinDistance = FLT_MAX;
		FVector Intersect;
		FVector Normal;

		const FTransform& ComponentTransform = FurComponent->GetComponentTransform();
		const FTransform InverseComponentTransform = ComponentTransform.Inverse();
		const FVector LocalStart = InverseComponentTransform.TransformPosition(Start);
		const FVector LocalEnd = InverseComponentTransform.TransformPosition(End);

		if (FurComponent->SkeletalGrowMesh)
		{
			USkeletalMesh* Mesh = FurComponent->SkeletalGrowMesh;
			FSkeletalMeshRenderData* RenderData = Mesh->GetResourceForRendering();
			auto* IndexBuffer = RenderData->LODRenderData[0].MultiSizeIndexContainer.GetIndexBuffer();
			auto& VertexBuffer = RenderData->LODRenderData[0].StaticVertexBuffers.PositionVertexBuffer;
			const int32 NumTriangles = IndexBuffer->Num() / 3;

			for (int32 TriangleIndex = 0; TriangleIndex < NumTriangles; ++TriangleIndex)
			{
				// Compute the normal of the triangle
				const FVector& P0 = VertexBuffer.VertexPosition(IndexBuffer->Get((TriangleIndex * 3) + 0));
				const FVector& P1 = VertexBuffer.VertexPosition(IndexBuffer->Get((TriangleIndex * 3) + 1));
				const FVector& P2 = VertexBuffer.VertexPosition(IndexBuffer->Get((TriangleIndex * 3) + 2));

				const FVector TriNorm = (P1 - P0) ^ (P2 - P0);

				//check collinearity of A,B,C
				if (TriNorm.SizeSquared() > SMALL_NUMBER)
				{
					FVector IntersectPoint;
					FVector HitNormal;
					bool bHit = FMath::SegmentTriangleIntersection(LocalStart, LocalEnd, P0, P1, P2, IntersectPoint, HitNormal);

					if (bHit)
					{
						const float Distance = (LocalStart - IntersectPoint).SizeSquared();
						if (Distance < MinDistance)
						{
							MinDistance = Distance;
							Intersect = IntersectPoint;
							Normal = HitNormal;
						}
					}
				}
			}
		}
		else
		{
			UStaticMesh* Mesh = FurComponent->StaticGrowMesh;
			auto& IndexBuffer = Mesh->RenderData->LODResources[0].IndexBuffer;
			auto& VertexBuffer = Mesh->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;

			const int32 NumTriangles = IndexBuffer.GetNumIndices() / 3;

			for (int32 TriangleIndex = 0; TriangleIndex < NumTriangles; ++TriangleIndex)
			{
				// Compute the normal of the triangle
				const FVector& P0 = VertexBuffer.VertexPosition(IndexBuffer.GetIndex((TriangleIndex * 3) + 0));
				const FVector& P1 = VertexBuffer.VertexPosition(IndexBuffer.GetIndex((TriangleIndex * 3) + 1));
				const FVector& P2 = VertexBuffer.VertexPosition(IndexBuffer.GetIndex((TriangleIndex * 3) + 2));

				const FVector TriNorm = (P1 - P0) ^ (P2 - P0);

				//check collinearity of A,B,C
				if (TriNorm.SizeSquared() > SMALL_NUMBER)
				{
					FVector IntersectPoint;
					FVector HitNormal;
					bool bHit = FMath::SegmentTriangleIntersection(LocalStart, LocalEnd, P0, P1, P2, IntersectPoint, HitNormal);

					if (bHit)
					{
						const float Distance = (LocalStart - IntersectPoint).SizeSquared();
						if (Distance < MinDistance)
						{
							MinDistance = Distance;
							Intersect = IntersectPoint;
							Normal = HitNormal;
						}
					}
				}
			}
		}

		if (MinDistance != FLT_MAX)
		{
			OutHit.Component = FurComponent;
			OutHit.Normal = Normal.GetSafeNormal();
			OutHit.Location = ComponentTransform.TransformPosition(Intersect);
			OutHit.bBlockingHit = true;
			bHitTriangle = true;
		}
	}

	return bHitTriangle;
}


/*
TArray<uint32> FFurComb::SphereIntersectTriangles(const float ComponentSpaceSquaredBrushRadius, const FVector& ComponentSpaceBrushPosition, const FVector& ComponentSpaceCameraPosition, const bool bOnlyFrontFacing) const
{
	TArray<uint32> OutTriangles;

	// Use a bit of distance bias to make sure that we get all of the overlapping triangles.  We
	// definitely don't want our brush to be cut off by a hard triangle edge
	const float SquaredRadiusBias = ComponentSpaceSquaredBrushRadius * 0.025f;

	for (FMeshPaintTriangleOctree::TConstElementBoxIterator<> TriIt(*MeshTriOctree, FBoxCenterAndExtent(ComponentSpaceBrushPosition, FVector(FMath::Sqrt(ComponentSpaceSquaredBrushRadius + SquaredRadiusBias)))); TriIt.HasPendingElements(); TriIt.Advance())
	{
		// Check to see if the triangle is front facing
		const FMeshPaintTriangle & CurrentTri = TriIt.GetCurrentElement();
		const float SignedPlaneDist = FVector::PointPlaneDist(ComponentSpaceCameraPosition, CurrentTri.Vertices[0], CurrentTri.Normal);
		if (!bOnlyFrontFacing || SignedPlaneDist < 0.0f)
		{
			OutTriangles.Add(CurrentTri.Index);
		}
	}

	return OutTriangles;
}

TArray<FVector> FFurComb::SphereIntersectVertices(const float ComponentSpaceSquaredBrushRadius, const FVector& ComponentSpaceBrushPosition, const FVector& ComponentSpaceCameraPosition, const bool bOnlyFrontFacing) const
{
	// Get list of intersecting triangles with given sphere data
	const TArray<uint32> IntersectedTriangles = SphereIntersectTriangles(ComponentSpaceSquaredBrushRadius, ComponentSpaceBrushPosition, ComponentSpaceCameraPosition, bOnlyFrontFacing);
	// Get a list of unique vertices indexed by the influenced triangles
	TSet<int32> InfluencedVertices;
	InfluencedVertices.Reserve(IntersectedTriangles.Num());
	for (int32 IntersectedTriangle : IntersectedTriangles)
	{
		InfluencedVertices.Add(MeshIndices[IntersectedTriangle * 3 + 0]);
		InfluencedVertices.Add(MeshIndices[IntersectedTriangle * 3 + 1]);
		InfluencedVertices.Add(MeshIndices[IntersectedTriangle * 3 + 2]);
	}

	TArray<FVector> InRangeVertices;
	InRangeVertices.Empty(MeshVertices.Num());
	for (int32 VertexIndex : InfluencedVertices)
	{
		const FVector& Vertex = MeshVertices[VertexIndex];
		if (FVector::DistSquared(ComponentSpaceBrushPosition, Vertex) <= ComponentSpaceSquaredBrushRadius)
		{
			InRangeVertices.Add(Vertex);
		}
	}

	return InRangeVertices;
}
*/

#if 0
bool FFurComb::ApplyPerVertexPaintAction(FPerVertexPaintActionArgs& InArgs, FPerVertexPaintAction Action)
{
	// Retrieve components world matrix
	const FMatrix& ComponentToWorldMatrix = InArgs.Adapter->GetComponentToWorldMatrix();

	// Compute the camera position in actor space.  We need this later to check for back facing triangles.
	const FVector ComponentSpaceCameraPosition(ComponentToWorldMatrix.InverseTransformPosition(InArgs.CameraPosition));
	const FVector ComponentSpaceBrushPosition(ComponentToWorldMatrix.InverseTransformPosition(InArgs.HitResult.Location));

	// @todo MeshPaint: Input vector doesn't work well with non-uniform scale
	const float BrushRadius = InArgs.BrushSettings->GetBrushRadius();
	const float ComponentSpaceBrushRadius = ComponentToWorldMatrix.InverseTransformVector(FVector(BrushRadius, 0.0f, 0.0f)).Size();
	const float ComponentSpaceSquaredBrushRadius = ComponentSpaceBrushRadius * ComponentSpaceBrushRadius;

	// Get a list of unique vertices indexed by the influenced triangles
	TSet<int32> InfluencedVertices;
	InArgs.Adapter->GetInfluencedVertexIndices(ComponentSpaceSquaredBrushRadius, ComponentSpaceBrushPosition, ComponentSpaceCameraPosition, InArgs.BrushSettings->bOnlyFrontFacingTriangles, InfluencedVertices);


	const int32 NumParallelFors = 4;
	const int32 NumPerFor = FMath::CeilToInt((float)InfluencedVertices.Num() / NumParallelFors);

	// Parallel applying
	/*ParallelFor(NumParallelFors, [=](int32 Index)
	{
		const int32 Start = Index * NumPerFor;
		const int32 End = FMath::Min(Start + NumPerFor, InfluencedVertices.Num());

		for (int32 VertexIndex = Start; VertexIndex < End; ++VertexIndex)
		{
			Action.ExecuteIfBound(Adapter, VertexIndex);
		}
	});*/
	if (InfluencedVertices.Num())
	{
		InArgs.Adapter->PreEdit();
		for (const int32 VertexIndex : InfluencedVertices)
		{
			// Apply the action!			
			Action.ExecuteIfBound(InArgs, VertexIndex);
		}
		InArgs.Adapter->PostEdit();
	}

	return (InfluencedVertices.Num() > 0);
}










void FFurComb::ApplyVertexColor(FPerVertexPaintActionArgs& InArgs, int32 VertexIndex, FMeshPaintParameters Parameters)
{
	/** Retrieve vertex position and color for applying vertex painting */
	FColor PaintColor;
	FVector Position;
	InArgs.Adapter->GetVertexPosition(VertexIndex, Position);
	Position = InArgs.Adapter->GetComponentToWorldMatrix().TransformPosition(Position);
	InArgs.Adapter->GetVertexColor(VertexIndex, PaintColor, true);
	MeshPaintHelpers::PaintVertex(Position, Parameters, PaintColor);
	InArgs.Adapter->SetVertexColor(VertexIndex, PaintColor, true);
}

#endif // 0











void FFurComb::CombLength(UFurSplines* FurSplines, const TSet<int32>& SplineSet, const TArray<FVector4>& SplineVertexNormals, const CombParams& Params)
{
	float RcpCombRadius = 1.0f / Params.CombRadius;
	float RcpInvFalloff = 1.0f / (1.0f - Params.Falloff);
	float Strength = Params.Strength;
	for (int32 Index : SplineSet)
	{
		int32 Idx = FurSplines->Index[Index];
		int32 Cnt = FurSplines->Count[Index];
		FVector PrevVertexOld = FurSplines->Vertices[Idx];
		FVector PrevVertexNew = PrevVertexOld;
		float Dist = FVector::Distance(PrevVertexOld, Params.Location);
		float D = Dist * RcpCombRadius - Params.Falloff;
		if (D > 0.0f)
			Strength *= 1.0f - D * RcpInvFalloff;
		for (int32 i = Idx + 1, End = Idx + Cnt; i < End; i++)
		{
			FVector& Vertex = FurSplines->Vertices[i];
			FVector TempVertex = Vertex;
			FVector Dir = Vertex - PrevVertexOld;
			PrevVertexOld = Vertex;
			float Size = Dir.Size();
			float NewSize = FMath::Max(Size * (1.0f + Strength), 0.001f);
			Vertex = PrevVertexNew + Dir * (NewSize / Size);
			PrevVertexNew = Vertex;
		}
	}
}

void FFurComb::CombBend(UFurSplines* FurSplines, const TSet<int32>& SplineSet, const TArray<FVector4>& SplineVertexNormals, const CombParams& Params)
{
	float RcpCombRadius = 1.0f / Params.CombRadius;
	float RcpInvFalloff = 1.0f / (1.0f - Params.Falloff);
	float Strength = Params.Strength;
	FVector BendDir = Params.Location - Params.OldLocation;
	if (BendDir.X == 0.0f && BendDir.Y == 0.0f && BendDir.Z == 0.0f)
		return;
	for (int32 Index : SplineSet)
	{
		int32 Idx = FurSplines->Index[Index];
		int32 Cnt = FurSplines->Count[Index];
		FVector PrevVertexOld = FurSplines->Vertices[Idx];
		FVector PrevVertexNew = PrevVertexOld;
		float PrevDist = 0.0f;
		float Dist = FVector::Distance(PrevVertexOld, Params.Location);
		float D = Dist * RcpCombRadius - Params.Falloff;
		if (D > 0.0f)
			Strength *= 1.0f - D * RcpInvFalloff;

		FVector Normal = SplineVertexNormals[Index];
		for (int32 i = Idx + 1, End = Idx + Cnt; i < End; i++)
		{
			FPlane Plane(PrevVertexOld, Normal);

			FVector& Vertex = FurSplines->Vertices[i];
			FVector TempVertex = Vertex;
			FVector Dir = Vertex - PrevVertexOld;
			PrevVertexOld = Vertex;
			float Size = Dir.Size();
			FVector NewDir = Dir + BendDir * Strength;
			float NewSize = NewDir.Size();
			Vertex = PrevVertexNew + NewDir * (Size / NewSize);
			Dist = Plane.PlaneDot(Vertex);
			if (Dist < MinLayerDist)
				Vertex += Normal * (-Dist + MinLayerDist);
			PrevVertexNew = Vertex;
		}
	}
}

void FFurComb::CombClump(UFurSplines* FurSplines, const TSet<int32>& SplineSet, const TArray<FVector4>& SplineVertexNormals, const CombParams& Params)
{
	float RcpCombRadius = 1.0f / Params.CombRadius;
	float RcpInvFalloff = 1.0f / (1.0f - Params.Falloff);
	float Strength = Params.Strength;
	for (int32 Index : SplineSet)
	{
		int32 Idx = FurSplines->Index[Index];
		int32 Cnt = FurSplines->Count[Index];
		FVector PrevVertexOld = FurSplines->Vertices[Idx];
		FVector PrevVertexNew = PrevVertexOld;
		float Dist = FVector::Distance(PrevVertexOld, Params.Location);
		float D = Dist * RcpCombRadius - Params.Falloff;
		if (D > 0.0f)
			Strength *= 1.0f - D * RcpInvFalloff;
		FVector BendDir = Params.Location - PrevVertexOld;

		FVector Normal = SplineVertexNormals[Index];
		for (int32 i = Idx + 1, End = Idx + Cnt; i < End; i++)
		{
			FPlane Plane(PrevVertexOld, Normal);

			FVector& Vertex = FurSplines->Vertices[i];
			FVector TempVertex = Vertex;
			FVector Dir = Vertex - PrevVertexOld;
			PrevVertexOld = Vertex;
			float Size = Dir.Size();
			FVector NewDir = Dir + BendDir * Strength;
			float NewSize = NewDir.Size();
			Vertex = PrevVertexNew + NewDir * (Size / NewSize);
			Dist = Plane.PlaneDot(Vertex);
			if (Dist < MinLayerDist)
				Vertex += Normal * (-Dist + MinLayerDist);
			PrevVertexNew = Vertex;
		}
	}
}

void FFurComb::CombTwist(UFurSplines* FurSplines, const TSet<int32>& SplineSet, const TArray<FVector4>& SplineVertexNormals, const CombParams& Params)
{
	float RcpCombRadius = 1.0f / Params.CombRadius;
	float RcpInvFalloff = 1.0f / (1.0f - Params.Falloff);
	float Strength = Params.Strength;
	for (int32 Index : SplineSet)
	{
		int32 Idx = FurSplines->Index[Index];
		int32 Cnt = FurSplines->Count[Index];
		FVector PrevVertexOld = FurSplines->Vertices[Idx];
		FVector PrevVertexNew = PrevVertexOld;
		float Dist = FVector::Distance(PrevVertexOld, Params.Location);
		float D = Dist * RcpCombRadius - Params.Falloff;
		if (D > 0.0f)
			Strength *= 1.0f - D * RcpInvFalloff;
		FVector BendDir = FVector::CrossProduct(Params.Location - PrevVertexOld, Params.Normal);

		FVector Normal = SplineVertexNormals[Index];
		for (int32 i = Idx + 1, End = Idx + Cnt; i < End; i++)
		{
			FPlane Plane(PrevVertexOld, Normal);

			FVector& Vertex = FurSplines->Vertices[i];
			FVector TempVertex = Vertex;
			FVector Dir = Vertex - PrevVertexOld;
			PrevVertexOld = Vertex;
			float Size = Dir.Size();
			FVector NewDir = Dir + BendDir * Strength;
			float NewSize = NewDir.Size();
			Vertex = PrevVertexNew + NewDir * (Size / NewSize);
			Dist = Plane.PlaneDot(Vertex);
			if (Dist < MinLayerDist)
				Vertex += Normal * (-Dist + MinLayerDist);
			PrevVertexNew = Vertex;
		}
	}
}

void FFurComb::CombNoise(UFurSplines* FurSplines, const TSet<int32>& SplineSet, const TArray<FVector4>& SplineVertexNormals, const CombParams& Params)
{
	float RcpCombRadius = 1.0f / Params.CombRadius;
	float RcpInvFalloff = 1.0f / (1.0f - Params.Falloff);
	float Strength = Params.Strength;
	for (int32 Index : SplineSet)
	{
		int32 Idx = FurSplines->Index[Index];
		int32 Cnt = FurSplines->Count[Index];
		FVector PrevVertexOld = FurSplines->Vertices[Idx];
		FVector PrevVertexNew = PrevVertexOld;
		float Dist = FVector::Distance(PrevVertexOld, Params.Location);
		float D = Dist * RcpCombRadius - Params.Falloff;
		if (D > 0.0f)
			Strength *= 1.0f - D * RcpInvFalloff;
		FVector BendDir;
		BendDir.RotateAngleAxis(FMath::FRandRange(0.0f, 2.0f * PI), Params.Normal);

		FVector Normal = SplineVertexNormals[Index];
		for (int32 i = Idx + 1, End = Idx + Cnt; i < End; i++)
		{
			FPlane Plane(PrevVertexOld, Normal);

			FVector& Vertex = FurSplines->Vertices[i];
			FVector TempVertex = Vertex;
			FVector Dir = Vertex - PrevVertexOld;
			PrevVertexOld = Vertex;
			float Size = Dir.Size();
			FVector NewDir = Dir + BendDir * Strength;
			float NewSize = NewDir.Size();
			Vertex = PrevVertexNew + NewDir * (Size / NewSize);
			Dist = Plane.PlaneDot(Vertex);
			if (Dist < MinLayerDist)
				Vertex += Normal * (-Dist + MinLayerDist);
			PrevVertexNew = Vertex;
		}
	}
}



#undef LOCTEXT_NAMESPACE // "FurComb"
