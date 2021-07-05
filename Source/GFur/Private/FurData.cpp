// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "FurData.h"
#include "FurComponent.h"

/** Fur Vertex Buffer */
FFurVertexBuffer::~FFurVertexBuffer()
{
	delete[]VertexData;
}

void FFurVertexBuffer::InitRHI()
{
	FRHIResourceCreateInfo CreateInfo;
	VertexBufferRHI = RHICreateVertexBuffer(Size, BUF_Static, CreateInfo);

	// Copy the vertex data into the vertex buffer.
	void* VertexBufferData = RHILockVertexBuffer(VertexBufferRHI, 0, Size, RLM_WriteOnly);
	FMemory::Memcpy(VertexBufferData, VertexData, Size);
	RHIUnlockVertexBuffer(VertexBufferRHI);

#if !WITH_EDITORONLY_DATA
	delete[]VertexData;
	VertexData = NULL;
#endif // WITH_EDITORONLY_DATA
}

void FFurVertexBuffer::Unlock()
{
	if (IsInitialized())
	{
		ENQUEUE_RENDER_COMMAND(UpdateDataCommand)([this](FRHICommandListImmediate& RHICmdList) {
			check(VertexBufferRHI.IsValid());
			if (Size != VertexBufferRHI->GetSize() || VertexBufferRHI->GetUsage() == BUF_Static)
			{
				FRHIResourceCreateInfo CreateInfo;
				VertexBufferRHI = RHICreateVertexBuffer(Size, BUF_Dynamic, CreateInfo);
			}

			// Copy the vertex data into the vertex buffer.
			void* VertexBufferData = RHILockVertexBuffer(VertexBufferRHI, 0, Size, RLM_WriteOnly);
			FMemory::Memcpy(VertexBufferData, VertexData, Size);
			RHIUnlockVertexBuffer(VertexBufferRHI);
		});
	}
	else
	{
		BeginInitResource(this);
	}
}

/** Index Buffer */
void FFurIndexBuffer::InitRHI()
{
	if (Indices.Num() == 0)
		Indices.Add(0);

	FRHIResourceCreateInfo CreateInfo;
	IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), Indices.Num() * sizeof(int32), BUF_Static, CreateInfo);
	// Write the indices to the index buffer.
	void* Buffer = RHILockIndexBuffer(IndexBufferRHI, 0, Indices.Num() * sizeof(int32), RLM_WriteOnly);
	FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(int32));
	RHIUnlockIndexBuffer(IndexBufferRHI);

#if !WITH_EDITORONLY_DATA
	Indices.SetNum(0, true);
#endif // WITH_EDITORONLY_DATA
}

TArray<int32>& FFurIndexBuffer::Lock()
{
	return Indices;
}

void FFurIndexBuffer::Unlock()
{
	if (IsInitialized())
	{
		ENQUEUE_RENDER_COMMAND(UpdateDataCommand)([this](FRHICommandListImmediate& RHICmdList) {
			check(IndexBufferRHI.IsValid());
			if (Indices.Num() == 0)
				Indices.Add(0);
			uint32 Size = Indices.Num() * sizeof(int32);
			if (Size != IndexBufferRHI->GetSize() || IndexBufferRHI->GetUsage() == BUF_Static)
			{
				FRHIResourceCreateInfo CreateInfo;
				IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), Size, BUF_Dynamic, CreateInfo);
			}

			void* Buffer = RHILockIndexBuffer(IndexBufferRHI, 0, Indices.Num() * sizeof(int32), RLM_WriteOnly);
			FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(int32));
			RHIUnlockIndexBuffer(IndexBufferRHI);
		});
	}
	else
	{
		BeginInitResource(this);
	}
}

/** Fur Data */
const int32 FFurData::MinimalFurLayerCount = 1;
const int32 FFurData::MaximalFurLayerCount = 128;
const float FFurData::MinimalFurLength = 0.001f;

FFurData::FFurData()
{
	RefCount = 1;
}

FFurData::~FFurData()
{
	if (FurSplinesGenerated)
	{
		if (FurSplinesGenerated->IsValidLowLevel())
			FurSplinesGenerated->ConditionalBeginDestroy();
	}

	VertexBuffer.ReleaseResource();
	IndexBuffer.ReleaseResource();

#if WITH_EDITORONLY_DATA
	if (FurSplinesAssigned)
		FurSplinesAssigned->RemoveFromRoot();
#endif // WITH_EDITORONLY_DATA
}

void FFurData::Set(int InFurLayerCount, int InLod, class UGFurComponent* InFurComponent)
{
	FurSplinesAssigned = InFurComponent->FurSplines;
#if WITH_EDITORONLY_DATA
	if (FurSplinesAssigned)
		FurSplinesAssigned->AddToRoot();
#endif // WITH_EDITORONLY_DATA
	Lod = InLod;
	FurLayerCount = FMath::Clamp(InFurLayerCount, MinimalFurLayerCount, MaximalFurLayerCount);
	FurLength = InFurComponent->FurLength;
	ShellBias = InFurComponent->ShellBias;
	HairLengthForceUniformity = InFurComponent->HairLengthForceUniformity;
	MinFurLength = FMath::Max(InFurComponent->MinFurLength, MinimalFurLength);
	NoiseStrength = InFurComponent->NoiseStrength;
	RemoveFacesWithoutSplines = InFurComponent->RemoveFacesWithoutSplines;

	FurSplinesUsed = FurSplinesAssigned;
	CurrentMinFurLength = InFurComponent->FurLength;
	CurrentMaxFurLength = InFurComponent->FurLength;
}

bool FFurData::Compare(int InFurLayerCount, int InLod, class UGFurComponent* InFurComponent)
{
	return FurSplinesAssigned == InFurComponent->FurSplines
		&& Lod == InLod
		&& FurLayerCount == FMath::Clamp(InFurLayerCount, MinimalFurLayerCount, MaximalFurLayerCount)
		&& FurLength == InFurComponent->FurLength
		&& ShellBias == InFurComponent->ShellBias
		&& HairLengthForceUniformity == InFurComponent->HairLengthForceUniformity
		&& MinFurLength == FMath::Max(InFurComponent->MinFurLength, MinimalFurLength)
		&& NoiseStrength == InFurComponent->NoiseStrength
		&& RemoveFacesWithoutSplines == InFurComponent->RemoveFacesWithoutSplines;
}

bool FFurData::Similar(int InLod, class UGFurComponent* InFurComponent)
{
	return Lod == InLod && FurSplinesAssigned == InFurComponent->FurSplines && RemoveFacesWithoutSplines == InFurComponent->RemoveFacesWithoutSplines;
}

void FFurData::GenerateSplineMap(const FPositionVertexBuffer& InPositions)
{
	SplineMap.Reset();
	VertexRemap.Reset();
	if (FurSplinesUsed)
	{
		uint32 SourceVertexCount = InPositions.GetNumVertices();
		int32 SplineCount = FurSplinesUsed->SplineCount();

		uint32 ValidVertexCount = 0;
		float MinLenSquared = FLT_MAX;
		float MaxLenSquared = -FLT_MAX;
		SplineMap.AddUninitialized(SourceVertexCount);

		const int32 Size = 64;
		TArray<int32> Cells;
		TArray<int32> NextIndex;
		Cells.AddUninitialized(Size * Size);
		for (int32 i = 0; i < Size * Size; i++)
			Cells[i] = -1;
		NextIndex.AddUninitialized(SplineCount);

		float MinX = FLT_MAX;
		float MinY = FLT_MAX;
		float MaxX = -FLT_MAX;
		float MaxY = -FLT_MAX;
		for (int32 i = 0; i < SplineCount; i++)
		{
			FVector p = FurSplinesUsed->GetFirstControlPoint(i);
			if (p.X < MinX)
				MinX = p.X;
			if (p.Y < MinY)
				MinY = p.Y;
			if (p.X > MaxX)
				MaxX = p.X;
			if (p.Y > MaxY)
				MaxY = p.Y;
		}
		MinX -= 1.0f;
		MinY -= 1.0f;
		MaxX += 1.0f;
		MaxY += 1.0f;
		float FactorWidth = Size / (MaxX - MinX);
		float FactorHeight = Size / (MaxY - MinY);

		for (int32 i = 0; i < SplineCount; i++)
		{
			FVector p = FurSplinesUsed->GetFirstControlPoint(i);
			uint32 X = FMath::FloorToInt((p.X - MinX) * FactorWidth);
			uint32 Y = FMath::FloorToInt((p.Y - MinY) * FactorHeight);
			check(X < Size && Y < Size);
			int32 Idx = Cells[Y * Size + X];
			if (Idx == -1)
			{
				Cells[Y * Size + X] = i;
				NextIndex[i] = -1;
			}
			else
			{
				while (NextIndex[Idx] != -1)
					Idx = NextIndex[Idx];
				NextIndex[Idx] = i;
				NextIndex[i] = -1;
			}
		}


		for (uint32 i = 0; i < SourceVertexCount; i++)
		{
			const float Epsilon = FurSplinesUsed->Threshold;
			const float EpsilonSquared = Epsilon * Epsilon;
			FVector p = InPositions.VertexPosition(i);
			int32 BeginX = FMath::Max(FMath::FloorToInt((p.X - Epsilon - MinX) * FactorWidth), 0);
			int32 BeginY = FMath::Max(FMath::FloorToInt((p.Y - Epsilon - MinY) * FactorHeight), 0);
			int32 EndX = FMath::Min(FMath::FloorToInt((p.X + Epsilon - MinX) * FactorWidth), Size - 1);
			int32 EndY = FMath::Min(FMath::FloorToInt((p.Y + Epsilon - MinY) * FactorHeight), Size - 1);
			SplineMap[i] = -1;
			float ClosestDistanceSquared = FLT_MAX;
			int32 ClosestIndex = -1;
			for (int32 Y = BeginY; Y <= EndY; Y++)
			{
				for (int32 X = BeginX; X <= EndX; X++)
				{
					if (X < Size && Y < Size)
					{
						int32 Idx = Cells[Y * Size + X];
						while (Idx != -1)
						{
							FVector s = FurSplinesUsed->GetFirstControlPoint(Idx);
							float DistanceSquared = FVector::DistSquared(s, p);
							if (DistanceSquared <= EpsilonSquared)
							{
								FVector s2 = FurSplinesUsed->GetLastControlPoint(Idx);
								if (FVector::DotProduct(s2 - s, Normals[i]) > 0.0f || MinFurLength > 0.0f)
								{
									if (DistanceSquared < ClosestDistanceSquared)
									{
										ClosestDistanceSquared = DistanceSquared;
										ClosestIndex = Idx;
									}
								}
							}
							Idx = NextIndex[Idx];
						}
					}
				}
			}
			SplineMap[i] = ClosestIndex;
			if (ClosestIndex != -1)
			{
				FVector s = FurSplinesUsed->GetFirstControlPoint(ClosestIndex);
				FVector s2 = FurSplinesUsed->GetLastControlPoint(ClosestIndex);
				float SizeSquared = (s2 - s).SizeSquared();
				if (SizeSquared < MinLenSquared)
					MinLenSquared = SizeSquared;
				if (SizeSquared > MaxLenSquared)
					MaxLenSquared = SizeSquared;
				ValidVertexCount++;
			}
		}
		CurrentMinFurLength = FMath::Sqrt(MinLenSquared) * FurLength;
		if (CurrentMinFurLength < MinFurLength)
			CurrentMinFurLength = MinFurLength;
		CurrentMaxFurLength = FMath::Sqrt(MaxLenSquared) * FurLength;

		if (RemoveFacesWithoutSplines)
		{
			VertexRemap.AddUninitialized(SourceVertexCount);
			VertexCountPerLayer = ValidVertexCount;
		}
		else
		{
			VertexCountPerLayer = SourceVertexCount;
		}
	}
	else
	{
		VertexCountPerLayer = InPositions.GetNumVertices();
	}
}

FFurData::FFurGenLayerData FFurData::CalcFurGenLayerData(int32 Layer)
{
	FFurGenLayerData Data;
	Data.LinearFactor = Layer / (float)FurLayerCount;
	float Derivative;
	if (ShellBias > 0.0f)
	{
		float invShellBias = 1.0f / ShellBias;
		Data.NonLinearFactor = Data.LinearFactor / (Data.LinearFactor + invShellBias) * (1.0f + invShellBias);
		Derivative = (invShellBias + invShellBias * invShellBias) / FMath::Square(Data.LinearFactor + invShellBias);
	}
	else
	{
		Data.NonLinearFactor = Data.LinearFactor;
		Derivative = 1.0f;
	}
	Data.LayerNoiseStrength = Derivative * NoiseStrength;
	return Data;
}

void FFurData::GenerateFurLengths(TArray<float>& FurLengths)
{
	if (FurSplinesUsed)
	{
		int32 ControlPointCount = FurSplinesUsed->ControlPointCount;
		FurLengths.AddUninitialized(FurSplinesUsed->SplineCount());
		for (int32 SplineIndex = 0, SplineCount = FurSplinesUsed->SplineCount(); SplineIndex < SplineCount; SplineIndex++)
		{
			float Length = 0.0f;
			FVector Prev = FurSplinesUsed->Vertices[SplineIndex * ControlPointCount];
			for (int32 ControlPointIndex = 1; ControlPointIndex < ControlPointCount; ControlPointIndex++)
			{
				FVector Point = FurSplinesUsed->Vertices[SplineIndex * ControlPointCount + ControlPointIndex];
				Length += FVector::Dist(Point, Prev);
				Prev = Point;
			}
			FurLengths[SplineIndex] = FMath::Max(Length * FurLength, MinFurLength);
		}
	}
}

void FFurData::GenerateFurVertex(FVector& OutFurOffset, FVector2D& OutUv1, FVector2D& OutUv2, FVector2D& OutUv3, const FVector& InTangentZ, float InFurLength, const FFurGenLayerData& InGenLayerData)
{
	OutUv1.X = InGenLayerData.NonLinearFactor * FurLength;
	float r = InGenLayerData.LayerNoiseStrength != 0 ? FMath::RandRange(-InGenLayerData.LayerNoiseStrength, InGenLayerData.LayerNoiseStrength) : 0;
	OutFurOffset = InTangentZ * (InGenLayerData.NonLinearFactor * FurLength + r);

	if (HairLengthForceUniformity > 0)
	{
		float Relative = OutUv1.X / FurLength;
		float Length = FurLength * (1.0f - HairLengthForceUniformity) + CurrentMaxFurLength * HairLengthForceUniformity;
		OutUv1.X = Relative * Length;
	}
	else
	{
		float Relative = OutUv1.X / FurLength;
		float Interpolator = -HairLengthForceUniformity;
		float Length = FurLength * (1.0f - Interpolator) + CurrentMinFurLength * Interpolator;
		OutUv1.X = Relative * Length;
	}

	OutUv1.Y = InGenLayerData.NonLinearFactor;
	OutUv2.X = InGenLayerData.LinearFactor;
	OutUv2.Y = InFurLength;
	OutUv3.X = Lod;
}

void FFurData::GenerateFurVertex(FVector& OutFurOffset, FVector2D& OutUv1, FVector2D& OutUv2, FVector2D& OutUv3, const FVector& InTangentZ, float InFurLength, const FFurGenLayerData& InGenLayerData, int32 InSplineIndex)
{
	if (InSplineIndex >= 0)
	{
		int32 Count = FurSplinesUsed->ControlPointCount;
		int32 Beginning = InSplineIndex * Count;

		float Bias = InGenLayerData.NonLinearFactor * (Count - 1);
		int Bottom = (int)Bias;
		int Top = (int)ceilf(Bias);
		float Height = Bias - Bottom;

		FVector Spline = FurSplinesUsed->Vertices[Beginning + Count - 1] - FurSplinesUsed->Vertices[Beginning];
		float SplineLength = Spline.Size() * FurLength;
		if (FVector::DotProduct(InTangentZ, Spline) <= 0.0f)
		{
			OutFurOffset = InTangentZ * (InGenLayerData.NonLinearFactor * MinFurLength);
		}
		else if (SplineLength < MinFurLength)
		{
			if (SplineLength >= 0.0001f)
			{
				float k = MinFurLength / SplineLength;
				FVector p = FurSplinesUsed->Vertices[Beginning + Bottom] * (1.0f - Height) + FurSplinesUsed->Vertices[Beginning + Top] * Height;
				OutFurOffset = (p - FurSplinesUsed->Vertices[Beginning]) * FurLength * k;
			}
			else
			{
				OutFurOffset = InTangentZ * (InGenLayerData.NonLinearFactor * MinFurLength);
			}
			SplineLength = MinFurLength;
		}
		else
		{
			OutFurOffset = FurSplinesUsed->Vertices[Beginning + Bottom] * (1.0f - Height) + FurSplinesUsed->Vertices[Beginning + Top] * Height;
			OutFurOffset = (OutFurOffset - FurSplinesUsed->Vertices[Beginning]) * FurLength;
		}
		if (InGenLayerData.LayerNoiseStrength != 0)
		{
			float r = FMath::RandRange(-InGenLayerData.LayerNoiseStrength, InGenLayerData.LayerNoiseStrength);
			OutFurOffset += InTangentZ * r;
		}

		OutUv1.X = OutFurOffset.Size();

		if (HairLengthForceUniformity > 0)
		{
			float Relative = OutUv1.X / SplineLength;
			float Length = SplineLength * (1.0f - HairLengthForceUniformity) + CurrentMaxFurLength * HairLengthForceUniformity;
			OutUv1.X = Relative * Length;
		}
		else
		{
			float Relative = OutUv1.X / SplineLength;
			float Interpolator = -HairLengthForceUniformity;
			float Length = SplineLength * (1.0f - Interpolator) + CurrentMinFurLength * Interpolator;
			OutUv1.X = Relative * Length;
		}
	}
	else
	{
		check(!RemoveFacesWithoutSplines);
		OutUv1.X = InGenLayerData.NonLinearFactor * MinFurLength;
		OutFurOffset = InTangentZ * (InGenLayerData.NonLinearFactor * MinFurLength/* + FMath::RandRange(-InNoiseStrength * Derivative, InNoiseStrength * Derivative)*/);
	}

	OutUv1.Y = InGenLayerData.NonLinearFactor;
	OutUv2.X = InGenLayerData.LinearFactor;
	OutUv2.Y = InFurLength;
	OutUv3.X = Lod;
}
