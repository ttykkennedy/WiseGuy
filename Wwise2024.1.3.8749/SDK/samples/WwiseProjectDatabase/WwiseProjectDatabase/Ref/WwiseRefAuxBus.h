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

#include "Ref/WwiseRefSoundBank.h"

class WwiseRefAuxBus : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::AuxBus;
	struct FGlobalIdsMap;

	WwiseRefIndexType AuxBusIndex;

	WwiseRefAuxBus() {}
	WwiseRefAuxBus(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InAuxBusIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		AuxBusIndex(InAuxBusIndex)
	{}
	const WwiseMetadataBus* GetAuxBus() const;
	void GetAllAuxBusRefs(WwiseDBSet<const WwiseRefAuxBus*>& OutAuxBusRefs, const WwiseAuxBusGlobalIdsMap& InGlobalMap) const;
	WwiseCustomPluginIdsMap GetAuxBusCustomPlugins(const WwiseCustomPluginGlobalIdsMap& GlobalMap) const;
	WwisePluginShareSetIdsMap GetAuxBusPluginShareSets(const WwisePluginShareSetGlobalIdsMap& GlobalMap) const;
	WwiseAudioDeviceIdsMap GetAuxBusAudioDevices(const WwiseAudioDeviceGlobalIdsMap& GlobalMap) const;

	WwiseDBShortId AuxBusId() const;
	WwiseDBGuid AuxBusGuid() const;
	const WwiseDBString* AuxBusName() const;
	const WwiseDBString* AuxBusObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefAuxBus& Rhs) const
	{
		return WwiseRefSoundBank::operator ==(Rhs)
			&& AuxBusIndex == Rhs.AuxBusIndex;
	}
	bool operator!=(const WwiseRefAuxBus& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefAuxBus::FGlobalIdsMap
{
	WwiseAuxBusGlobalIdsMap GlobalIdsMap;
};
