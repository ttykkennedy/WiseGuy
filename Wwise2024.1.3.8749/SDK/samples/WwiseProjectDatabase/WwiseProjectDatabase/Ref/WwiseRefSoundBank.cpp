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

#include "Ref/WwiseRefSoundBank.h"

#include "Ref/WwiseRefAcousticTexture.h"
#include "Ref/WwiseRefAudioDevice.h"
#include "Ref/WwiseRefAuxBus.h"
#include "Ref/WwiseRefBus.h"
#include "Ref/WwiseRefCustomPlugin.h"
#include "Ref/WwiseRefDialogueArgument.h"
#include "Ref/WwiseRefEvent.h"
#include "Ref/WwiseRefExternalSource.h"
#include "Ref/WwiseRefGameParameter.h"
#include "Ref/WwiseRefMedia.h"
#include "Ref/WwiseRefPluginShareSet.h"
#include "Ref/WwiseRefState.h"
#include "Ref/WwiseRefStateGroup.h"
#include "Ref/WwiseRefSwitch.h"
#include "Ref/WwiseRefSwitchGroup.h"
#include "Ref/WwiseRefTrigger.h"

#include "Metadata/WwiseMetadataPlugin.h"
#include "Metadata/WwiseMetadataPluginGroup.h"
#include "Metadata/WwiseMetadataSoundBank.h"
#include "Metadata/WwiseMetadataSoundBanksInfo.h"


const WwiseDBString WwiseRefSoundBank::NAME = "SoundBank"_wwise_db;

const WwiseMetadataSoundBank* WwiseRefSoundBank::GetSoundBank() const
{
	const auto* SoundBanksInfo = GetSoundBanksInfo();
	if (!SoundBanksInfo) [[unlikely]]
	{
		return nullptr;
	}
	const auto& SoundBanks = SoundBanksInfo->SoundBanks;
	if (SoundBanks.IsValidIndex(SoundBankIndex))
	{
		return &SoundBanks[SoundBankIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get SoundBank index #%zu", SoundBankIndex);
		return nullptr;
	}
}

WwiseMediaIdsMap WwiseRefSoundBank::GetSoundBankMedia(const WwiseMediaGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto& Media = SoundBank->Media;

	WwiseMediaIdsMap Result;
	Result.Empty(Media.Size());
	for (const auto& Elem : Media)
	{
		WwiseDatabaseMediaIdKey Id(Elem.Id, SoundBank->Id);

		const WwiseRefMedia* MediaInGlobalMap = GlobalMap.Find(Id);
		if (MediaInGlobalMap)
		{
			Result.Add(Elem.Id, *MediaInGlobalMap);
		}
	}
	return Result;
}

WwiseCustomPluginIdsMap WwiseRefSoundBank::GetSoundBankCustomPlugins(const WwiseCustomPluginGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank || !SoundBank->Plugins) [[unlikely]]
	{
		return {};
	}
	const auto& CustomPlugins = SoundBank->Plugins->Custom;

	WwiseCustomPluginIdsMap Result;
	Result.Empty(CustomPlugins.Size());
	for (const auto& Elem : CustomPlugins)
	{
		WwiseDatabaseLocalizableIdKey Id(Elem.Id, LanguageId);

		const WwiseRefCustomPlugin* InGlobalMap = GlobalMap.Find(Id);
		if (InGlobalMap)
		{
			Result.Add(Elem.Id, *InGlobalMap);
		}
	}
	return Result;
}

WwisePluginShareSetIdsMap WwiseRefSoundBank::GetSoundBankPluginShareSets(const WwisePluginShareSetGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank || !SoundBank->Plugins) [[unlikely]]
	{
		return {};
	}
	const auto& PluginShareSets = SoundBank->Plugins->ShareSets;

	WwisePluginShareSetIdsMap Result;
	Result.Empty(PluginShareSets.Size());
	for (const auto& Elem : PluginShareSets)
	{
		WwiseDatabaseLocalizableIdKey Id(Elem.Id, LanguageId);

		const WwiseRefPluginShareSet* InGlobalMap = GlobalMap.Find(Id);
		if (InGlobalMap)
		{
			Result.Add(Elem.Id, *InGlobalMap);
		}
	}
	return Result;
}

WwiseAudioDeviceIdsMap WwiseRefSoundBank::GetSoundBankAudioDevices(const WwiseAudioDeviceGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank || !SoundBank->Plugins) [[unlikely]]
	{
		return {};
	}
	const auto& AudioDevices = SoundBank->Plugins->AudioDevices;

	WwiseAudioDeviceIdsMap Result;
	Result.Empty(AudioDevices.Size());
	for (const auto& Elem : AudioDevices)
	{
		WwiseDatabaseLocalizableIdKey Id(Elem.Id, LanguageId);

		const WwiseRefAudioDevice* InGlobalMap = GlobalMap.Find(Id);
		if (InGlobalMap)
		{
			Result.Add(Elem.Id, *InGlobalMap);
		}
	}
	return Result;
}

WwiseEventIdsMap WwiseRefSoundBank::GetSoundBankEvents(const WwiseEventGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto& Events = SoundBank->Events;

	WwiseEventIdsMap Result;
	Result.Empty(Events.Size());
	for (const auto& Elem : Events)
	{
		WwiseDatabaseLocalizableIdKey Id(Elem.Id, LanguageId);

		const WwiseRefEvent* InGlobalMap = GlobalMap.Find(Id);
		if (InGlobalMap)
		{
			Result.Add(Elem.Id, *InGlobalMap);
		}
	}
	return Result;
}

WwiseDialogueEventIdsMap WwiseRefSoundBank::GetSoundBankDialogueEvents(const WwiseDialogueEventGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto& DialogueEvents = SoundBank->DialogueEvents;
	WwiseDialogueEventIdsMap Result;
	Result.Empty(DialogueEvents.Size());
	for (const auto& Elem : DialogueEvents)
	{
		const WwiseRefDialogueEvent* GlobalRef = GlobalMap.Find(WwiseDatabaseLocalizableIdKey(Elem.Id, LanguageId));
		if (GlobalRef)
		{
			Result.Add(Elem.Id, *GlobalRef);
		}
	}

	return Result;
}

WwiseDialogueArgumentIdsMap WwiseRefSoundBank::GetAllSoundBankDialogueArguments(const WwiseDialogueArgumentGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto DialogueArguments = SoundBank->GetAllDialogueArguments();
	WwiseDialogueArgumentIdsMap Result;
	Result.Empty(DialogueArguments.Size());
	for (const auto& Elem : DialogueArguments)
	{
		const WwiseRefDialogueArgument* GlobalRef = GlobalMap.Find(WwiseDatabaseLocalizableIdKey(Elem.Id, LanguageId));
		if (GlobalRef)
		{
			Result.Add(Elem.Id, *GlobalRef);
		}
	}

	return Result;
}

WwiseBusIdsMap WwiseRefSoundBank::GetSoundBankBusses(const WwiseBusGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto& Busses = SoundBank->Busses;
	WwiseBusIdsMap Result;
	Result.Empty(Busses.Size());
	for (const auto& Elem : Busses)
	{
		const WwiseRefBus* GlobalRef = GlobalMap.Find(WwiseDatabaseLocalizableIdKey(Elem.Id, LanguageId));
		if (GlobalRef)
		{
			Result.Add(Elem.Id, *GlobalRef);
		}
	}

	return Result;
}

WwiseAuxBusIdsMap WwiseRefSoundBank::GetSoundBankAuxBusses(const WwiseAuxBusGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto& AuxBusses = SoundBank->AuxBusses;
	WwiseAuxBusIdsMap Result;
	Result.Empty(AuxBusses.Size());
	for (const auto& Elem : AuxBusses)
	{
		const WwiseRefAuxBus* GlobalRef = GlobalMap.Find(WwiseDatabaseLocalizableIdKey(Elem.Id, LanguageId));
		if (GlobalRef)
		{
			Result.Add(Elem.Id, *GlobalRef);
		}
	}

	return Result;
}

WwiseGameParameterIdsMap WwiseRefSoundBank::GetSoundBankGameParameters(const WwiseGameParameterGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto& GameParameters = SoundBank->GameParameters;
	WwiseGameParameterIdsMap Result;
	Result.Empty(GameParameters.Size());
	for (const auto& Elem : GameParameters)
	{
		const WwiseRefGameParameter* GlobalRef = GlobalMap.Find(WwiseDatabaseLocalizableIdKey(Elem.Id, LanguageId));
		if (GlobalRef)
		{
			Result.Add(Elem.Id, *GlobalRef);
		}
	}

	return Result;
}

WwiseStateGroupIdsMap WwiseRefSoundBank::GetSoundBankStateGroups(const WwiseStateGroupGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto& StateGroups = SoundBank->StateGroups;
	WwiseStateGroupIdsMap Result;
	Result.Empty(StateGroups.Size());
	for (const auto& Elem : StateGroups)
	{
		const WwiseRefStateGroup* GlobalRef = GlobalMap.Find(WwiseDatabaseLocalizableIdKey(Elem.Id, LanguageId));
		if (GlobalRef)
		{
			Result.Add(Elem.Id, *GlobalRef);
		}
	}

	return Result;
}

WwiseStateIdsMap WwiseRefSoundBank::GetAllSoundBankStates(const WwiseStateGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto States = SoundBank->GetAllStates();
	WwiseStateIdsMap Result;
	Result.Empty(States.Size());
	for (const auto& Elem : States)
	{
		const WwiseRefState* GlobalRef = GlobalMap.Find(WwiseDatabaseLocalizableGroupValueKey(Elem.GetFirst().Id, Elem.GetSecond().Id, LanguageId));
		if (GlobalRef)
		{
			Result.Add(WwiseDatabaseGroupValueKey(Elem.GetFirst().Id, Elem.GetSecond().Id), *GlobalRef);
		}
	}

	return Result;
}

WwiseSwitchGroupIdsMap WwiseRefSoundBank::GetSoundBankSwitchGroups(const WwiseSwitchGroupGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto& SwitchGroups = SoundBank->SwitchGroups;
	WwiseSwitchGroupIdsMap Result;
	Result.Empty(SwitchGroups.Size());
	for (const auto& Elem : SwitchGroups)
	{
		const WwiseRefSwitchGroup* GlobalRef = GlobalMap.Find(WwiseDatabaseLocalizableIdKey(Elem.Id, LanguageId));
		if (GlobalRef)
		{
			Result.Add(Elem.Id, *GlobalRef);
		}
	}

	return Result;
}

WwiseSwitchIdsMap WwiseRefSoundBank::GetAllSoundBankSwitches(const WwiseSwitchGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto Switches = SoundBank->GetAllSwitches();
	WwiseSwitchIdsMap Result;
	Result.Empty(Switches.Size());
	for (const auto& Elem : Switches)
	{
		const WwiseRefSwitch* GlobalRef = GlobalMap.Find(WwiseDatabaseLocalizableGroupValueKey(Elem.GetFirst().Id, Elem.GetSecond().Id, LanguageId));
		if (GlobalRef)
		{
			Result.Add(WwiseDatabaseGroupValueKey(Elem.GetFirst().Id, Elem.GetSecond().Id), *GlobalRef);
		}
	}

	return Result;
}

WwiseTriggerIdsMap WwiseRefSoundBank::GetSoundBankTriggers(const WwiseTriggerGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto& Triggers = SoundBank->Triggers;
	WwiseTriggerIdsMap Result;
	Result.Empty(Triggers.Size());
	for (const auto& Elem : Triggers)
	{
		const WwiseRefTrigger* GlobalRef = GlobalMap.Find(WwiseDatabaseLocalizableIdKey(Elem.Id, LanguageId));
		if (GlobalRef)
		{
			Result.Add(Elem.Id, *GlobalRef);
		}
	}

	return Result;
}

WwiseExternalSourceIdsMap WwiseRefSoundBank::GetSoundBankExternalSources(const WwiseExternalSourceGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto& ExternalSources = SoundBank->ExternalSources;
	WwiseExternalSourceIdsMap Result;
	Result.Empty(ExternalSources.Size());
	for (const auto& Elem : ExternalSources)
	{
		const WwiseRefExternalSource* GlobalRef = GlobalMap.Find(WwiseDatabaseLocalizableIdKey(Elem.Cookie, LanguageId));
		if (GlobalRef)
		{
			Result.Add(Elem.Cookie, *GlobalRef);
		}
	}

	return Result;
}

WwiseAcousticTextureIdsMap WwiseRefSoundBank::GetSoundBankAcousticTextures(const WwiseAcousticTextureGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto& AcousticTextures = SoundBank->AcousticTextures;
	WwiseAcousticTextureIdsMap Result;
	Result.Empty(AcousticTextures.Size());
	for (const auto& Elem : AcousticTextures)
	{
		const WwiseRefAcousticTexture* GlobalRef = GlobalMap.Find(WwiseDatabaseLocalizableIdKey(Elem.Id, LanguageId));
		if (GlobalRef)
		{
			Result.Add(Elem.Id, *GlobalRef);
		}
	}

	return Result;
}

bool WwiseRefSoundBank::IsUserBank() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return false;
	}
	return SoundBank->Type == EMetadataSoundBankType::User;
}

bool WwiseRefSoundBank::IsInitBank() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return false;
	}
	return SoundBank->IsInitBank();
}


WwiseDBShortId WwiseRefSoundBank::SoundBankId() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return 0;
	}
	return SoundBank->Id;
}

const WwiseDBGuid* WwiseRefSoundBank::SoundBankGuid() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	return &SoundBank->GUID;
}

const WwiseDBString* WwiseRefSoundBank::SoundBankLanguage() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	return &SoundBank->Language;
}

const WwiseDBString* WwiseRefSoundBank::SoundBankShortName() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}	
	return &SoundBank->ShortName;
}

const WwiseDBString* WwiseRefSoundBank::SoundBankObjectPath() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &SoundBank->ObjectPath;
}

const WwiseDBString* WwiseRefSoundBank::SoundBankPath() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return {};
	}
	return &SoundBank->Path;
}

WwiseDBShortId WwiseRefSoundBank::Hash() const
{
	auto Result = WwiseRefSoundBanksInfo::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(SoundBankIndex));
	return Result;
}
