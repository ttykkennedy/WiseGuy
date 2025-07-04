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

#include "Ref/WwiseRefSoundBanksInfo.h"

class WwiseRefSoundBank : public WwiseRefSoundBanksInfo
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::SoundBank;
	struct FGlobalIdsMap;

	WwiseRefIndexType SoundBankIndex;
	WwiseDBShortId LanguageId;

	WwiseRefSoundBank() {}
	WwiseRefSoundBank(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId) :
		WwiseRefSoundBanksInfo(InRootMediaRef, InJsonFilePath),
		SoundBankIndex(InSoundBankIndex),
		LanguageId(InLanguageId)
	{}
	const WwiseMetadataSoundBank* GetSoundBank() const;
	WwiseMediaIdsMap GetSoundBankMedia(const WwiseMediaGlobalIdsMap& GlobalMap) const;
	WwiseCustomPluginIdsMap GetSoundBankCustomPlugins(const WwiseCustomPluginGlobalIdsMap& GlobalMap) const;
	WwisePluginShareSetIdsMap GetSoundBankPluginShareSets(const WwisePluginShareSetGlobalIdsMap& GlobalMap) const;
	WwiseAudioDeviceIdsMap GetSoundBankAudioDevices(const WwiseAudioDeviceGlobalIdsMap& GlobalMap) const;
	WwiseEventIdsMap GetSoundBankEvents(const WwiseEventGlobalIdsMap& GlobalMap) const;
	WwiseDialogueEventIdsMap GetSoundBankDialogueEvents(const WwiseDialogueEventGlobalIdsMap& GlobalMap) const;
	WwiseDialogueArgumentIdsMap GetAllSoundBankDialogueArguments(const WwiseDialogueArgumentGlobalIdsMap& GlobalMap) const;
	WwiseBusIdsMap GetSoundBankBusses(const WwiseBusGlobalIdsMap& GlobalMap) const;
	WwiseAuxBusIdsMap GetSoundBankAuxBusses(const WwiseAuxBusGlobalIdsMap& GlobalMap) const;
	WwiseGameParameterIdsMap GetSoundBankGameParameters(const WwiseGameParameterGlobalIdsMap& GlobalMap) const;
	WwiseStateGroupIdsMap GetSoundBankStateGroups(const WwiseStateGroupGlobalIdsMap& GlobalMap) const;
	WwiseStateIdsMap GetAllSoundBankStates(const WwiseStateGlobalIdsMap& GlobalMap) const;
	WwiseSwitchGroupIdsMap GetSoundBankSwitchGroups(const WwiseSwitchGroupGlobalIdsMap& GlobalMap) const;
	WwiseSwitchIdsMap GetAllSoundBankSwitches(const WwiseSwitchGlobalIdsMap& GlobalMap) const;
	WwiseTriggerIdsMap GetSoundBankTriggers(const WwiseTriggerGlobalIdsMap& GlobalMap) const;
	WwiseExternalSourceIdsMap GetSoundBankExternalSources(const WwiseExternalSourceGlobalIdsMap& GlobalMap) const;
	WwiseAcousticTextureIdsMap GetSoundBankAcousticTextures(const WwiseAcousticTextureGlobalIdsMap& GlobalMap) const;
	bool IsUserBank() const;
	bool IsInitBank() const;

	WwiseDBShortId SoundBankId() const;
	const WwiseDBGuid* SoundBankGuid() const;
	const WwiseDBString* SoundBankLanguage() const;
	const WwiseDBString* SoundBankShortName() const;
	const WwiseDBString* SoundBankObjectPath() const;
	const WwiseDBString* SoundBankPath() const;
	
	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefSoundBank& Rhs) const
	{
		return WwiseRefSoundBanksInfo::operator ==(Rhs)
			&& SoundBankIndex == Rhs.SoundBankIndex;
	}
	bool operator!=(const WwiseRefSoundBank& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefSoundBank::FGlobalIdsMap
{
	WwiseSoundBankGlobalIdsMap GlobalIdsMap;
};
