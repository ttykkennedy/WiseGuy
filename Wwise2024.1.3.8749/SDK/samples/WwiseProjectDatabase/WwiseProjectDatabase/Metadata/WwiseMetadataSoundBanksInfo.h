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

#include "Metadata/WwiseMetadataLoadable.h"
#include "Metadata/WwiseMetadataSoundBank.h"

struct WwiseMetadataSoundBanksInfoAttributes : public WwiseMetadataLoadable
{
	WwiseDBString Platform;
	WwiseDBString BasePlatform;
	WwiseDBShortId SchemaVersion;
	WwiseDBShortId SoundBankVersion;

	WwiseMetadataSoundBanksInfoAttributes(WwiseMetadataLoader& Loader);
};

struct WwiseMetadataSoundBanksInfo : public WwiseMetadataSoundBanksInfoAttributes
{
	WwiseMetadataRootPaths* RootPaths;
	WwiseDBArray<WwiseMetadataDialogueEvent> DialogueEvents;

	WwiseDBArray<WwiseMetadataSoundBank> SoundBanks;
	WwiseDBGuid FileHash;

	WwiseMetadataSoundBanksInfo(WwiseMetadataLoader& Loader);
	~WwiseMetadataSoundBanksInfo();
};
