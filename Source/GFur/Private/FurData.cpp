// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "FurData.h"

void FFurIndexBuffer::InitRHI()
{
	FRHIResourceCreateInfo CreateInfo;
	IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), Indices.Num() * sizeof(int32), BUF_Static, CreateInfo);
	// Write the indices to the index buffer.
	void* Buffer = RHILockIndexBuffer(IndexBufferRHI, 0, Indices.Num() * sizeof(int32), RLM_WriteOnly);
	FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(int32));
	RHIUnlockIndexBuffer(IndexBufferRHI);
}
