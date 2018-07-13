// Copyright 2017 GiM s.r.o. All Rights Reserved.

#include "FurSplines.h"
#include "GFur.h"
#include "FurComponent.h"

UFurSplines::UFurSplines(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UFurSplines::Refresh()
{
	UGFurComponent::reloadFurSplines(this);
}
