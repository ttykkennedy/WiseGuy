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

#include "AdapterTypes/WwiseDataTypesAdapter.h"
#include "Metadata/WwiseMetadataLoadable.h"

enum class WwiseMetadataSwitchValueGroupType : WwiseDBShortId
{
	Switch,
	State,
	Unknown = (WwiseDBShortId)-1
};

struct WwiseMetadataSwitchValueAttributes : public WwiseMetadataLoadable
{
	WwiseMetadataSwitchValueGroupType GroupType;
	WwiseDBShortId GroupId;
	WwiseDBShortId Id;
	WwiseDBGuid GUID;
	int Color;
	bool bDefault;

	WwiseMetadataSwitchValueAttributes();
	WwiseMetadataSwitchValueAttributes(WwiseMetadataLoader& Loader);

private:
	static WwiseMetadataSwitchValueGroupType GroupTypeFromString(const WwiseDBString& TypeString);
};

struct WwiseMetadataSwitchValue : public WwiseMetadataSwitchValueAttributes
{
	WwiseMetadataSwitchValue();
	WwiseMetadataSwitchValue(WwiseMetadataLoader& Loader);
};
