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

const float FFurComb::MinLayerDist = 0.001f;

FFurComb::FFurComb()
	: CurrentViewportInteractor(nullptr),
	bCombing(false),
	TimeSinceStartedPainting(0.0f),
	Time(0.0f),
	LastDeltaTime(0.0f),
	Mode(EFurCombMode::Length),
	CombTransaction(nullptr)
{}

FFurComb::~FFurComb()
{
}

void FFurComb::Init()
{
	static FString ConfigPrefixes[] = {
		"Length_",
		"AverageLength_",
		"Bend_",
		"Clump_",
		"Twist_",
		"Noise_",
		"Curl_",
		"Relax_",
		"AddRemove_",
	};

	/** Setup necessary data */
	for (int i = 0; i < (int)EFurCombMode::Count; i++)
	{
//		FurCombSettings[i] = DuplicateObject<UFurCombSettings>(GetMutableDefault<UFurCombSettings>(), GetTransientPackage());
		FurCombSettings[i] = NewObject<UFurCombSettings>();
		FurCombSettings[i]->AddToRoot();
		FurCombSettings[i]->SetConfigPrefix(ConfigPrefixes[i]);
		FurCombSettings[i]->Load();
	}

	FFurCombCommands::Register();
	UICommandList = TSharedPtr<FUICommandList>(new FUICommandList());
	Widget = SNew(SFurCombModeWidget, this);
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
	if (GetCurrentFurCombSettings()->bShowSplines)
		RenderSplines(View, Viewport, PDI);
	RenderInteractors(View, Viewport, PDI);
}

void FFurComb::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
	if (bCombing)
	{
		TimeSinceStartedPainting += DeltaTime;
	}

	if (ViewportClient->IsPerspective())
	{
		// Make sure perspective viewports are still set to real-time
		MeshPaintHelpers::SetRealtimeViewport(true);
	}

	Time += DeltaTime;
	LastDeltaTime = DeltaTime;
}

void FFurComb::RegisterCommands(TSharedRef<FUICommandList> CommandList)
{
	const FFurCombCommands& Commands = FFurCombCommands::Get();

	// Lambda used to alter the fur comb size
	auto CombLambda = [this](float Multiplier)
	{
		const float CombChangeValue = 0.05f;
		float CombRadius = GetCurrentFurCombSettings()->GetRadius();
		CombRadius *= (1.f + (CombChangeValue * Multiplier));
		GetCurrentFurCombSettings()->SetRadius(CombRadius);
	};
	CommandList->MapAction(Commands.IncreaseCombSize, FExecuteAction::CreateLambda(CombLambda, 1.0f), FCanExecuteAction(), EUIActionRepeatMode::RepeatEnabled);
	CommandList->MapAction(Commands.DecreaseCombSize, FExecuteAction::CreateLambda(CombLambda, -1.0f), FCanExecuteAction(), EUIActionRepeatMode::RepeatEnabled);
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
	return CombInternal(InCameraOrigin, InRayOrigin, InRayDirection, CombAction, Strength * LastDeltaTime);
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
		float CombRadius = GetCurrentFurCombSettings()->GetRadius();
		CombRadius *= (1.f + (CombChangeValue * Multiplier));
		GetCurrentFurCombSettings()->SetRadius(CombRadius);
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
}

void FFurComb::ActorDeselected(AActor* Actor)
{
	FurComponents.Reset();
}

void FFurComb::AddReferencedObjects(FReferenceCollector& Collector)
{

}

UFurCombSettings* FFurComb::GetCurrentFurCombSettings()
{
	return FurCombSettings[(int)Mode];
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

void FFurComb::PostUndo()
{
}

bool FFurComb::CombInternal(const FVector& InCameraOrigin, const FVector& InRayOrigin, const FVector& InRayDirection, ECombAction CombAction, float StrengthScale)
{
	UFurCombSettings* Settings = FurCombSettings[(int)Mode];
	const float CombRadius = Settings->GetRadius();
	// Fire out a ray to see if there is a *selected* component under the mouse cursor that can be painted.
	// NOTE: We can't use a GWorld line check for this as that would ignore components that have collision disabled

	TArray<UGFurComponent*> HoveredComponents;
	FHitResult BestTraceResult;
	{
		const FVector TraceStart(InRayOrigin);
		const FVector TraceEnd(InRayOrigin + InRayDirection * HALF_WORLD_MAX);

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
			BeginTransaction(LOCTEXT("FurCombMode_TransactionComb", "Fur Comb"));
			bCombing = true;
			OldLocation = BestTraceResult.Location;
			TimeSinceStartedPainting = 0.0f;
		}

		FVector CombXAxis, CombYAxis;
		BestTraceResult.Normal.FindBestAxisVectors(CombXAxis, CombYAxis);
		// Display settings
		const float VisualBiasDistance = 0.15f;
		const FVector CombVisualPosition = BestTraceResult.Location + BestTraceResult.Normal * VisualBiasDistance;

		// NOTE: We square the comb strength to maximize slider precision in the low range
		float CombStrength = Settings->Strength * Settings->Strength * StrengthScale;
		if (CombAction == ECombAction::Inverse)
			CombStrength = -CombStrength;

		// Iterate over the selected meshes under the cursor and paint them!
		for (UGFurComponent* FurComponent : HoveredComponents)
		{
			UFurSplines* FurSplines = FurComponent->FurSplines;
			if (FurSplines == nullptr)
				continue;

			int32 SplineCount = FurSplines->SplineCount();
			const auto& SplineMap = FurComponent->GetFurSplineMap();
			const auto& VertexNormals = FurComponent->GetVertexNormals();

			const FStaticMeshVertexBuffers* VertexBuffers;
			if (FurComponent->SkeletalGrowMesh)
			{
				USkeletalMesh* Mesh = FurComponent->SkeletalGrowMesh;
				VertexBuffers = &Mesh->GetResourceForRendering()->LODRenderData[0].StaticVertexBuffers;
			}
			else if (FurComponent->StaticGrowMesh)
			{
				UStaticMesh* Mesh = FurComponent->StaticGrowMesh;
				VertexBuffers = &Mesh->GetRenderData()->LODResources[0].VertexBuffers;
			}
			else
			{
				continue;
			}

			const FPositionVertexBuffer& Positions = VertexBuffers->PositionVertexBuffer;

			const FTransform& ComponentTransform = FurComponent->GetComponentTransform();

			const FTransform InverseComponentTransform = ComponentTransform.Inverse();
			const FVector ComponentSpaceLocation = InverseComponentTransform.TransformPosition(BestTraceResult.Location);
			const float ComponentSpaceRadius = InverseComponentTransform.TransformVector(FVector(CombRadius, 0.0f, 0.0f)).Size();
			const float ComponentSpaceRadiusSquared = ComponentSpaceRadius * ComponentSpaceRadius;

			const FStaticMeshVertexBuffer& Vertices = VertexBuffers->StaticMeshVertexBuffer;
			int32 VertexCount = Positions.GetNumVertices();

			VertexSet.Reset();
			SplineSet.Reset();
			if (SplineNormals.Num() < SplineCount)
				SplineNormals.AddUninitialized(SplineCount - SplineNormals.Num());

			CombParams Params;
			Params.Location = ComponentSpaceLocation;
			Params.OldLocation = InverseComponentTransform.TransformPosition(OldLocation);
			Params.Normal = InverseComponentTransform.TransformVector(BestTraceResult.Normal);
			Params.CombRadius = ComponentSpaceRadius;
			Params.Strength = CombStrength;
			Params.Falloff = Settings->FalloffAmount;
			Params.ApplyHeight = Settings->ApplyHeight;
			Params.ApplySpread = Settings->ApplySpread;
			Params.TwistCount = Settings->TwistCount;
			Params.MirrorX = Settings->bMirrorX;
			Params.MirrorY = Settings->bMirrorY;
			Params.MirrorZ = Settings->bMirrorZ;


			if (Mode == EFurCombMode::AddRemove && Params.Strength > 0)
			{
				for (int32 VertexIndex = 0; VertexIndex < VertexCount; VertexIndex++)
				{
					const FVector& Position = Positions.VertexPosition(VertexIndex);
					FVector Location = MirrorVector(ComponentSpaceLocation, Position, Params);
					if (FVector::DistSquared(Position, Location) > ComponentSpaceRadiusSquared)
						continue;

					int32 SplineIndex = SplineMap[VertexIndex];
					if (SplineIndex == -1)
						VertexSet.Add(VertexIndex);
				}

				if (VertexSet.Num())
				{
					CombAdd(FurSplines, Positions, VertexNormals);
					FurSplines->Modify();
					bCombApplied = true;
					FurSplines->OnSplinesChanged.Broadcast();
				}
			}
			else
			{
				for (int32 VertexIndex = 0; VertexIndex < VertexCount; VertexIndex++)
				{
					const FVector& Position = Positions.VertexPosition(VertexIndex);
					FVector Location = MirrorVector(ComponentSpaceLocation, Position, Params);
					if (FVector::DistSquared(Position, Location) > ComponentSpaceRadiusSquared)
						continue;

					int32 SplineIndex = SplineMap[VertexIndex];
					if (SplineIndex != -1)
					{
						VertexSet.Add(VertexIndex);
						bool IsAlreadyInSet;
						SplineSet.Add(SplineIndex, &IsAlreadyInSet);
						const FVector& Norm = VertexNormals[VertexIndex];
						if (IsAlreadyInSet)
							SplineNormals[SplineIndex] += Norm;
						else
							SplineNormals[SplineIndex] = Norm;
					}
				}

				if (SplineSet.Num())
				{
					bool fullBuild = false;
					FurSplines->Modify();
					switch (Mode)
					{
					case EFurCombMode::Length:
						CombLength(FurSplines, Params);
						break;
					case EFurCombMode::AverageLength:
						CombAverageLength(FurSplines, Params);
						break;
					case EFurCombMode::Bend:
						CombBend(FurSplines, Params);
						break;
					case EFurCombMode::Clump:
						CombClump(FurSplines, Params);
						break;
					case EFurCombMode::Twist:
						CombTwist(FurSplines, Params);
						break;
					case EFurCombMode::Noise:
						CombNoise(FurSplines, Params);
						break;
					case EFurCombMode::Curl:
						CombCurl(FurSplines, Params);
						break;
					case EFurCombMode::Relax:
						CombRelax(FurSplines, Params);
						break;
					case EFurCombMode::AddRemove:
						CombRemove(FurSplines);
						fullBuild = true;
						break;
					}
					bCombApplied = true;
					if (fullBuild)
						FurSplines->OnSplinesChanged.Broadcast();
					else
						FurSplines->OnSplinesCombed.Broadcast(VertexSet);
				}
			}
		}
		OldLocation = BestTraceResult.Location;
	}

	return bCombApplied;
}

void FFurComb::RenderInteractors(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI, ESceneDepthPriorityGroup DepthGroup/* = SDPG_World*/)
{
	TArray<MeshPaintHelpers::FPaintRay> PaintRays;
	MeshPaintHelpers::RetrieveViewportPaintRays(View, Viewport, PDI, PaintRays);

	// Apply paint pressure and start painting (or if not currently painting, draw a preview of where paint will be applied)
	for (const MeshPaintHelpers::FPaintRay& PaintRay : PaintRays)
	{
		const UVREditorInteractor* VRInteractor = Cast<UVREditorInteractor>(PaintRay.ViewportInteractor);
		ECombAction CombAction = VRInteractor ? (VRInteractor->IsModifierPressed() ? ECombAction::Inverse : ECombAction::Normal)
			: (Viewport->KeyState(EKeys::LeftControl) || Viewport->KeyState(EKeys::RightControl)) ? ECombAction::Inverse : ECombAction::Normal;

		RenderInteractorWidget(PaintRay.RayStart, PaintRay.RayDirection, PDI, DepthGroup);
	}
}

void FFurComb::RenderInteractorWidget(const FVector& InRayOrigin, const FVector& InRayDirection, FPrimitiveDrawInterface* PDI, ESceneDepthPriorityGroup DepthGroup /*= SDPG_World*/)
{
	const UFurCombSettings* CombSettings = GetCurrentFurCombSettings();
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
		}
	}
}

void FFurComb::RenderSplines(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI, ESceneDepthPriorityGroup DepthGroup)
{
	const FColor Color = FColor::Red;

	for (UGFurComponent* FurComponent : FurComponents)
	{
		auto* Splines = FurComponent->FurSplines;
		if (Splines)
		{
			int32 Count = Splines->ControlPointCount;
			const auto& Transform = FurComponent->GetComponentTransform();
			for (int i = 0, c = Splines->SplineCount(); i < c; i++)
			{
				int32 Index = i * Count;
				FVector Prev = Transform.TransformPosition(Splines->Vertices[Index]);
				for (int j = Index + 1, e = Index + Count; j < e; j++)
				{
					FVector Curr = Transform.TransformPosition(Splines->Vertices[j]);
					PDI->DrawLine(Prev, Curr, Color, DepthGroup);
					Prev = Curr;
				}
			}
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
			auto& IndexBuffer = Mesh->GetRenderData()->LODResources[0].IndexBuffer;
			auto& VertexBuffer = Mesh->GetRenderData()->LODResources[0].VertexBuffers.PositionVertexBuffer;

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

FVector FFurComb::MirrorVector(const FVector& Vec, const FVector& Vertex, const CombParams& Params)
{
	FVector Result;
	Result.X = Params.MirrorX && ((Vertex.X >= 0.0f) != (Vec.X >= 0.0f)) ? -Vec.X : Vec.X;
	Result.Y = Params.MirrorY && ((Vertex.Y >= 0.0f) != (Vec.Y >= 0.0f)) ? -Vec.Y : Vec.Y;
	Result.Z = Params.MirrorZ && ((Vertex.Z >= 0.0f) != (Vec.Z >= 0.0f)) ? -Vec.Z : Vec.Z;
	return Result;
}

FVector FFurComb::BendFur(const FVector& Dir, const FVector& Normal, const FVector& Offset)
{
	float OldSize = Dir.Size();
	FVector NewDir = Dir + Offset;
	float NewSize = NewDir.Size();
	NewDir *= (OldSize / NewSize);
	float Distance = FVector::DotProduct(Normal, NewDir);
	if (Distance >= MinLayerDist)
		return NewDir;

	NewDir += Normal * (-Distance + MinLayerDist);
	FVector v = NewDir - Normal * MinLayerDist;
	float s = OldSize > MinLayerDist ? FMath::Sqrt(OldSize * OldSize - MinLayerDist * MinLayerDist) : 0.0f;
	v *= s / v.Size();
	return Normal * MinLayerDist + v;
}

template<bool UseStrengthHeight, typename F, typename G>
void FFurComb::Comb(UFurSplines* FurSplines, const CombParams& Params, const F& FuncPerSpline, const G& FuncPerSegment)
{
	const float RcpCombRadius = 1.0f / Params.CombRadius;
	const float RcpInvFalloff = 1.0f / (1.0f - Params.Falloff);
	for (int32 Index : SplineSet)
	{
		float Strength = Params.Strength;
		int32 Cnt = FurSplines->ControlPointCount;
		int32 Idx = Index * Cnt;
		FVector PrevVertexOld = FurSplines->Vertices[Idx];
		FVector PrevVertexNew = PrevVertexOld;
		FVector Location = MirrorVector(Params.Location, PrevVertexOld, Params);
		float Dist = FVector::Distance(PrevVertexOld, Location);
		float D = Dist * RcpCombRadius - Params.Falloff;
		if (D > 0.0f)
			Strength *= 1.0f - D * RcpInvFalloff;
		const FVector& Normal = SplineNormals[Index];
		FuncPerSpline(FPerSplineData{ PrevVertexOld, Normal, Cnt });
		for (int32 i = Idx + 1, End = Idx + Cnt; i < End; i++)
		{
			float StrengtHeight = 1.0f;
			float Height = (i - Idx) / (float)(Cnt - 1);
			if (UseStrengthHeight)
			{
				float Exp;
				if (Params.ApplySpread >= 0)
					Exp = 1.0f - Params.ApplySpread;
				else
					Exp = 1.0f / (1.1f + Params.ApplySpread);
				StrengtHeight = powf(1.0f - FMath::Abs(Height - Params.ApplyHeight), Exp);
			}

			FVector& Vertex = FurSplines->Vertices[i];
			FVector Dir = Vertex - PrevVertexOld;
			PrevVertexOld = Vertex;

			Vertex = PrevVertexNew + FuncPerSegment(FPerSegmentData{ Dir, Normal, Strength * StrengtHeight, Height });
			PrevVertexNew = Vertex;
		}
	}
}

void FFurComb::CombLength(UFurSplines* FurSplines, const CombParams& Params)
{
	float TotalSizeDiff = Params.Strength * 10.0f;
	float SegSizeDiff;
	Comb<false>(FurSplines, Params, [TotalSizeDiff, &SegSizeDiff](const FPerSplineData& Data) {
		SegSizeDiff = TotalSizeDiff / (Data.ControlPointCount - 1);
	}, [&SegSizeDiff](const FPerSegmentData& Data) {
		float Size = Data.Dir.Size();
		float NewSize = FMath::Max(Size + SegSizeDiff, 0.001f);
		return Data.Dir * (NewSize / Size);
	});
}

void FFurComb::CombAverageLength(UFurSplines* FurSplines, const CombParams& Params)
{
	float RcpCombRadius = 1.0f / Params.CombRadius;
	float RcpInvFalloff = 1.0f / (1.0f - Params.Falloff);
	float Strength = Params.Strength;
	float LengthSum = 0.0f;
	float LengthCnt = 0;
	for (int32 Index : SplineSet)
	{
		int32 Cnt = FurSplines->ControlPointCount;
		int32 Idx = Index * Cnt;
		FVector PrevVertex = FurSplines->Vertices[Idx];
		for (int32 i = Idx + 1, End = Idx + Cnt; i < End; i++)
		{
			FVector& Vertex = FurSplines->Vertices[i];
			LengthSum += (Vertex - PrevVertex).Size();
			PrevVertex = Vertex;
		}
		LengthCnt += 1.0f;
	}

	if (LengthCnt == 0)
		return;

	float TargetLength = LengthSum / LengthCnt;
	float SegmentTargetLength;
	Comb<false>(FurSplines, Params, [&SegmentTargetLength, TargetLength](const FPerSplineData& Data) {
		SegmentTargetLength = TargetLength / (Data.ControlPointCount - 1);
	}, [&SegmentTargetLength](const FPerSegmentData& Data) {
		float Size = Data.Dir.Size();
		float Diff = SegmentTargetLength - Size;
		float NewSize = FMath::Max(Size + Diff * Data.Strength, 0.001f);
		return Data.Dir * (NewSize / Size);
	});
}

void FFurComb::CombBend(UFurSplines* FurSplines, const CombParams& Params)
{
	FVector OrigBendDir = Params.Location - Params.OldLocation;
	if (OrigBendDir.X == 0.0f && OrigBendDir.Y == 0.0f && OrigBendDir.Z == 0.0f)
		return;

	FVector BendDir;
	Comb<true>(FurSplines, Params, [&OrigBendDir, &BendDir, &Params](const FPerSplineData& Data) {
		BendDir = MirrorVector(OrigBendDir, Data.BaseVertex, Params);
	}, [&BendDir](const FPerSegmentData& Data) {
		return BendFur(Data.Dir, Data.Normal, BendDir * Data.Strength * 5.0f);
	});
}

void FFurComb::CombClump(UFurSplines* FurSplines, const CombParams& Params)
{
	FVector BendDir;
	Comb<true>(FurSplines, Params, [&Params, &BendDir](const FPerSplineData& Data) {
		BendDir = MirrorVector(Params.Location, Data.BaseVertex, Params) - Data.BaseVertex;
	}, [&BendDir](const FPerSegmentData& Data) {
		return BendFur(Data.Dir, Data.Normal, BendDir * Data.Strength * 0.1f);
	});
}

void FFurComb::CombTwist(UFurSplines* FurSplines, const CombParams& Params)
{
	FVector BendDir;
	Comb<true>(FurSplines, Params, [&Params, &BendDir](const FPerSplineData& Data) {
		BendDir = FVector::CrossProduct(MirrorVector(Params.Location, Data.BaseVertex, Params) - Data.BaseVertex, Params.Normal);
		BendDir = MirrorVector(BendDir, Data.BaseVertex, Params);
	}, [&BendDir](const FPerSegmentData& Data) {
		return BendFur(Data.Dir, Data.Normal, BendDir * Data.Strength * 0.1f);
	});
}

void FFurComb::CombNoise(UFurSplines* FurSplines, const CombParams& Params)
{
	FVector BendDir;
	Comb<true>(FurSplines, Params, [&Params, &BendDir](const FPerSplineData& Data) {
		BendDir.X = FMath::Sin(Data.BaseVertex.X * 50.0f) + FMath::Sin(Data.BaseVertex.Y * 35.0f) + FMath::Sin(Data.BaseVertex.Z * 96.0f);
		BendDir.X += FMath::Sin(Data.Normal.X * 122.0f) + FMath::Sin(Data.Normal.Y * 67.0f) + FMath::Sin(Data.Normal.Z * 16.0f);

		BendDir.Y = FMath::Sin(Data.BaseVertex.X * 99.0f) + FMath::Sin(Data.BaseVertex.Y * 75.0f) + FMath::Sin(Data.BaseVertex.Z * 15.0f);
		BendDir.Y += FMath::Sin(Data.Normal.X * 30.0f) + FMath::Sin(Data.Normal.Y * 164.0f) + FMath::Sin(Data.Normal.Z * 79.0f);

		BendDir.Z = FMath::Sin(Data.BaseVertex.X * 88.0f) + FMath::Sin(Data.BaseVertex.Y * 47.0f) + FMath::Sin(Data.BaseVertex.Z * 94.0f);
		BendDir.Z += FMath::Sin(Data.Normal.X * 42.0f) + FMath::Sin(Data.Normal.Y * 21.0f) + FMath::Sin(Data.Normal.Z * 74.0f);
	}, [&BendDir](const FPerSegmentData& Data) {
		return BendFur(Data.Dir, Data.Normal, BendDir * Data.Strength * 0.2f);
	});
}

void FFurComb::CombCurl(UFurSplines* FurSplines, const CombParams& Params)
{
	Comb<true>(FurSplines, Params, [](const FPerSplineData& Data) {
	}, [&Params](const FPerSegmentData& Data) {
		FVector v = FMath::Abs(Data.Normal.X) < 0.707f ? FVector(1, 0, 0) : FVector(0, 1, 0);
		FVector u = FVector::CrossProduct(v, Data.Normal);
		u.Normalize();
		v = FVector::CrossProduct(Data.Normal, u);
		float s, c;
		FMath::SinCos(&s, &c, Data.Height * Params.TwistCount * 2.0f * PI);
		FVector BendDir = u * s + v * c;
		return BendFur(Data.Dir, Data.Normal, BendDir * Data.Strength * 0.1f);
	});
}

void FFurComb::CombRelax(UFurSplines* FurSplines, const CombParams& Params)
{
	Comb<true>(FurSplines, Params, [](const FPerSplineData& Data) {
	}, [](const FPerSegmentData& Data) {
		float Size = Data.Dir.Size();
		FVector Dest = Data.Normal * Size;
		FVector BendDir = Dest - Data.Dir;
		return BendFur(Data.Dir, Data.Normal, BendDir * FMath::Min(1.0f, Data.Strength * 2.0f));
	});
}

void FFurComb::CombAdd(UFurSplines* FurSplines, const FPositionVertexBuffer& Positions, const TArray<FVector>& Normals)
{
	auto& Vertices = FurSplines->Vertices;
	int32 Count = FurSplines->ControlPointCount;
	const float Length = 1.0f;
	int32 OldSplineCount = FurSplines->SplineCount();
	int32 NewSplineCount = OldSplineCount;
	for (uint32 VertexIndex : VertexSet)
	{
		FVector v = Positions.VertexPosition(VertexIndex);
		bool found = false;
		for (int32 i = OldSplineCount; i < NewSplineCount; i++)
		{
			if (FVector::DistSquared(v, Vertices[i * Count]) <= 0.01f)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			FVector n = Normals[VertexIndex];

			int32 s = Vertices.AddUninitialized(Count);
			for (int32 i = 0; i < Count; i++)
			{
				float t = i / (float)(Count - 1);
				Vertices[s + i] = v + n * t * Length;
			}
		}
	}
}

void FFurComb::CombRemove(UFurSplines* FurSplines)
{
	auto SplineIterator = SplineSet.CreateConstIterator();

	auto& Vertices = FurSplines->Vertices;
	uint32 Count = FurSplines->ControlPointCount;
	uint32 Dst = *SplineIterator * Count;
	for (int32 i = *SplineIterator, SplineCount = FurSplines->SplineCount(); i < SplineCount; i++)
	{
		if (i != *SplineIterator)
		{
			int32 Offset = i * Count;
			for (int32 j = Offset, e = Offset + Count; j < e; j++)
				Vertices[Dst++] = Vertices[j];
		}
		else
		{
			if (!(++SplineIterator))
			{
				int32 Offset = (i + 1) * Count;
				for (int32 j = Offset, e = Vertices.Num(); j < e; j++)
					Vertices[Dst++] = Vertices[j];
				break;
			}
		}
	}
	Vertices.SetNum(Dst);
}

#undef LOCTEXT_NAMESPACE // "FurComb"
