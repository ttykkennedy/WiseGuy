/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2025 Audiokinetic Inc.
*******************************************************************************/

#pragma once

#include "Metadata/WwiseMetadataBasicReference.h"

struct WwiseMetadataBusReference : public WwiseMetadataBasicReference
{
	WwiseMetadataBusReference(WwiseMetadataLoader& Loader);
};

struct WwiseMetadataBus : public WwiseMetadataBusReference
{
	WwiseMetadataPluginReferenceGroup* PluginRefs;
	WwiseDBArray<WwiseMetadataBusReference> AuxBusRefs;
	float MaxAttenuation;

	WwiseMetadataBus(WwiseMetadataLoader& Loader);
};
