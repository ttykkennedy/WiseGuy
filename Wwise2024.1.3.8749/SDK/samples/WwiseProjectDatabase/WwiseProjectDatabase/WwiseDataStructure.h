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

#include "WwiseGeneratedFiles.h"

#include "Metadata/WwiseMetadataSoundBank.h"
#include "Metadata/WwiseMetadataLanguage.h"
#include "Ref/WwiseRefAcousticTexture.h"
#include "Ref/WwiseAnyRef.h"
#include "Ref/WwiseRefAudioDevice.h"
#include "Ref/WwiseRefAuxBus.h"
#include "Ref/WwiseRefBus.h"
#include "Ref/WwiseRefCustomPlugin.h"
#include "Ref/WwiseRefDialogueArgument.h"
#include "Ref/WwiseRefExternalSource.h"
#include "Ref/WwiseRefGameParameter.h"
#include "Ref/WwiseRefLanguage.h"
#include "Ref/WwiseRefMedia.h"
#include "Ref/WwiseRefPlatform.h"
#include "Ref/WwiseRefPluginLib.h"
#include "Ref/WwiseRefPluginShareSet.h"
#include "Ref/WwiseRefSoundBank.h"
#include "Ref/WwiseRefState.h"
#include "Ref/WwiseRefSwitch.h"
#include "Ref/WwiseRefSwitchContainer.h"
#include "Ref/WwiseRefTrigger.h"
#include "AdapterTypes/WwiseDataTypesAdapter.h"

#include <inttypes.h>
#include <shared_mutex>

#include "WwiseDBSharedLanguageId.h"
#include "WwiseSharedDBPlatformId.h"

struct WwiseDBObjectInfo
{
	WwiseDBGuid WwiseGuid;

	WwiseDBShortId WwiseShortId = 0;

	WwiseDBString WwiseName;

	WwiseDBString WwiseType = "";

	WwiseDBShortId HardCodedSoundBankShortId = 0;

	WwiseDBObjectInfo()
	{}

	WwiseDBObjectInfo(const WwiseDBObjectInfo& InWwiseObjectInfo) :
		WwiseGuid(InWwiseObjectInfo.WwiseGuid),
		WwiseShortId(InWwiseObjectInfo.WwiseShortId),
		WwiseName(InWwiseObjectInfo.WwiseName),
		HardCodedSoundBankShortId(InWwiseObjectInfo.HardCodedSoundBankShortId)
	{}

	WwiseDBObjectInfo(const WwiseDBGuid& InWwiseGuid, WwiseDBShortId InWwiseShortId, const WwiseDBString& InWwiseName, const WwiseDBString& InWwiseType, WwiseDBShortId InHardCodedSoundBankShortId = 0) :
		WwiseGuid(InWwiseGuid),
		WwiseShortId(InWwiseShortId),
		WwiseName(InWwiseName),
		WwiseType(InWwiseType),
		HardCodedSoundBankShortId(InHardCodedSoundBankShortId)
	{}

	WwiseDBObjectInfo(WwiseDBShortId InWwiseShortId, const WwiseDBString& InWwiseName) :
		WwiseShortId(InWwiseShortId),
		WwiseName(InWwiseName)
	{}

	WwiseDBObjectInfo(WwiseDBShortId InWwiseShortId) :
		WwiseShortId(InWwiseShortId)
	{}

	static const WwiseDBObjectInfo DefaultInitBank;

	bool operator==(const WwiseDBObjectInfo& Rhs) const
	{
		return (!WwiseGuid.IsValid() || !Rhs.WwiseGuid.IsValid() || WwiseGuid == Rhs.WwiseGuid) &&
			(WwiseShortId == 0 || Rhs.WwiseShortId == 0 || WwiseShortId == Rhs.WwiseShortId) &&
			(WwiseName.IsEmpty() || Rhs.WwiseName.IsEmpty() || WwiseName == Rhs.WwiseName)
			&& HardCodedSoundBankShortId == Rhs.HardCodedSoundBankShortId;
	}

	bool operator!=(const WwiseDBObjectInfo& Rhs) const
	{
		return !operator==(Rhs);
	}
};

struct WwiseDBGroupValueInfo: public WwiseDBObjectInfo
{
	
	WwiseDBShortId GroupShortId = 0;
	
	WwiseDBGroupValueInfo() :
		WwiseDBObjectInfo(),
		GroupShortId(0)
	{}

	WwiseDBGroupValueInfo(const WwiseDBGroupValueInfo& InGroupValueInfo) :
		WwiseDBObjectInfo(InGroupValueInfo),
		GroupShortId(InGroupValueInfo.GroupShortId)
	{}

	WwiseDBGroupValueInfo(const WwiseDBGuid& InWwiseGuid, WwiseDBShortId InGroupShortId, WwiseDBShortId InWwiseShortId, const WwiseDBString& InWwiseName, const WwiseDBString& InWwiseType) :
		WwiseDBObjectInfo(InWwiseGuid, InWwiseShortId, InWwiseName, InWwiseType),
		GroupShortId(InGroupShortId)
	{}

	WwiseDBGroupValueInfo(WwiseDBShortId InGroupShortId, WwiseDBShortId InWwiseShortId, const WwiseDBString& InWwiseName) :
		WwiseDBObjectInfo(InWwiseShortId, InWwiseName),
		GroupShortId(InGroupShortId)
	{}

	bool operator==(const WwiseDBGroupValueInfo& Rhs) const
	{
		return (!WwiseGuid.IsValid() || !Rhs.WwiseGuid.IsValid() || WwiseGuid == Rhs.WwiseGuid) &&
			((GroupShortId == 0 && WwiseShortId == 0) || (Rhs.GroupShortId == 0 && Rhs.WwiseShortId == 0) || (GroupShortId == Rhs.GroupShortId && WwiseShortId == Rhs.WwiseShortId)) &&
			(WwiseName.IsEmpty() || Rhs.WwiseName.IsEmpty() || WwiseName == Rhs.WwiseName);
	}

	bool operator!=(const WwiseDBGroupValueInfo& Rhs) const
	{
		return !operator==(Rhs);
	}
};

inline WwiseDBShortId GetTypeHash(const WwiseDBGroupValueInfo& InValue)
{
	return WwiseDBHashCombine(WwiseDBHashCombine(WwiseDBHashCombine(
		GetTypeHash(InValue.WwiseGuid),
		GetTypeHash(InValue.GroupShortId)),
		GetTypeHash(InValue.WwiseShortId)),
		GetTypeHash(InValue.WwiseName));
}

inline WwiseDBShortId GetTypeHash(const WwiseDBObjectInfo& InValue)
{
	return WwiseDBHashCombine(WwiseDBHashCombine(WwiseDBHashCombine(
		GetTypeHash(InValue.WwiseGuid),
		GetTypeHash(InValue.WwiseShortId)),
		GetTypeHash(InValue.WwiseName)),
		GetTypeHash(InValue.HardCodedSoundBankShortId));
}

struct WwiseRootDataStructure
{
	WwiseGeneratedFiles::FGeneratedRootFiles GeneratedRootFiles;
	WwiseMetadataFileMap JsonFiles;

	WwiseLanguageNamesMap LanguageNames;
	WwiseLanguageIdsMap LanguageIds;
	WwisePlatformNamesMap PlatformNames;
	WwisePlatformGuidsMap PlatformGuids;

	WwiseDBSet<WwiseDBSharedLanguageId> Languages;
	WwiseDBSharedLanguageId DefaultLanguage;
	WwiseDBSet<WwiseDBSharedPlatformId> Platforms;

	WwiseRootDataStructure() {}
	WwiseRootDataStructure(WwiseMetadataFileMap&& JsonFiles);
	WwiseRootDataStructure& operator+=(WwiseRootDataStructure&& Rhs);

	WwiseDBShortId GetLanguageId(const WwiseDBString& Name) const
	{
		if (const auto* Language = LanguageNames.Find(Name))
		{
			return Language->LanguageId();
		}
		return WwiseDatabaseLocalizableIdKey::GENERIC_LANGUAGE;
	}
	WwiseDBString GetLanguageName(const WwiseDBShortId& InId) const
	{
		if (const auto* Language = LanguageIds.Find(InId))
		{
			return Language->LanguageName();
		}
		return {};
	}
};

struct WwisePlatformDataStructure :
	private WwiseRefAcousticTexture::FGlobalIdsMap,
	private WwiseRefAudioDevice::FGlobalIdsMap,
	private WwiseRefAuxBus::FGlobalIdsMap,
	private WwiseRefBus::FGlobalIdsMap,
	private WwiseRefCustomPlugin::FGlobalIdsMap,
	private WwiseRefDialogueArgument::FGlobalIdsMap,
	private WwiseRefDialogueEvent::FGlobalIdsMap,
	private WwiseRefEvent::FGlobalIdsMap,
	private WwiseRefExternalSource::FGlobalIdsMap,
	private WwiseRefGameParameter::FGlobalIdsMap,
	private WwiseRefMedia::FGlobalIdsMap,
	private WwiseRefPluginLib::FGlobalIdsMap,
	private WwiseRefPluginShareSet::FGlobalIdsMap,
	private WwiseRefSoundBank::FGlobalIdsMap,
	private WwiseRefState::FGlobalIdsMap,
	private WwiseRefStateGroup::FGlobalIdsMap,
	private WwiseRefSwitch::FGlobalIdsMap,
	private WwiseRefSwitchGroup::FGlobalIdsMap,
	private WwiseRefTrigger::FGlobalIdsMap
{
	WwiseDBSharedPlatformId Platform;
	WwiseRefPlatform PlatformRef;

	WwiseGeneratedFiles::FPlatformFiles GeneratedPlatformFiles;
	WwiseMetadataFileMap JsonFiles;

	WwiseAcousticTextureGlobalIdsMap& AcousticTextures;
	WwiseAudioDeviceGlobalIdsMap& AudioDevices;
	WwiseAuxBusGlobalIdsMap& AuxBusses;
	WwiseBusGlobalIdsMap& Busses;
	WwiseCustomPluginGlobalIdsMap& CustomPlugins;
	WwiseDialogueArgumentGlobalIdsMap& DialogueArguments;
	WwiseDialogueEventGlobalIdsMap& DialogueEvents;
	WwiseEventGlobalIdsMap& Events;
	WwiseExternalSourceGlobalIdsMap& ExternalSources;
	WwiseGameParameterGlobalIdsMap& GameParameters;
	WwiseMediaGlobalIdsMap& MediaFiles;
	WwisePluginLibGlobalIdsMap& PluginLibs;
	WwisePluginShareSetGlobalIdsMap& PluginShareSets;
	WwiseSoundBankGlobalIdsMap& SoundBanks;
	WwiseStateGlobalIdsMap& States;
	WwiseStateGroupGlobalIdsMap& StateGroups;
	WwiseSwitchGlobalIdsMap& Switches;
	WwiseSwitchGroupGlobalIdsMap& SwitchGroups;
	WwiseTriggerGlobalIdsMap& Triggers;

	WwisePluginLibNamesMap PluginLibNames;
	WwiseSwitchContainersByEvent SwitchContainersByEvent;
	WwiseGuidMap Guids;
	WwiseNameMap Names;

	using MediaUsageCountMap = WwiseDBMap<WwiseDatabaseMediaIdKey, int>;
	using SoundBankUsageCountMap = WwiseDBMap<WwiseDatabaseLocalizableIdKey, int>;
	MediaUsageCountMap MediaUsageCount;
	SoundBankUsageCountMap SoundBankUsageCount;

	WwisePlatformDataStructure();
	WwisePlatformDataStructure(const WwiseDBSharedPlatformId& InPlatform, WwiseRootDataStructure& InRootData, WwiseMetadataFileMap&& InJsonFiles);
	WwisePlatformDataStructure(const WwisePlatformDataStructure& Rhs);
	WwisePlatformDataStructure(WwisePlatformDataStructure&& Rhs);
	WwisePlatformDataStructure& operator+=(WwisePlatformDataStructure&& Rhs);

	template <typename RequiredRef>
	void GetRefMap(WwiseDBMap<WwiseDBSharedLanguageId, RequiredRef>& OutRefMap, const WwiseDBSet<WwiseDBSharedLanguageId>& InLanguages, const WwiseDBObjectInfo& InInfo) const;

	template <typename RequiredRef>
	void GetRefMap(WwiseDBMap<WwiseDBSharedLanguageId,  WwiseDBSet<RequiredRef>>& OutRefMap, const WwiseDBSet<WwiseDBSharedLanguageId>& InLanguages, const WwiseDBObjectInfo& InInfo) const;

	template <typename RequiredRef>
	bool GetRef(WwiseDBSet<RequiredRef>& OutRef, const WwiseDBSharedLanguageId& InLanguage, const WwiseDBObjectInfo& InInfo) const;

	template <typename RequiredRef>
	bool GetRef(RequiredRef& OutRef, const WwiseDBSharedLanguageId& InLanguage, const WwiseDBObjectInfo& InInfo) const;

	template <typename RequiredRef>
	bool GetRef(RequiredRef& OutRef, const WwiseDBSharedLanguageId& InLanguage, const WwiseDBGroupValueInfo& InInfo) const;

	template <typename RequiredRef>
	static bool GetLocalizableRef(RequiredRef& OutRef, const WwiseDBMap<WwiseDatabaseLocalizableIdKey, RequiredRef>& InGlobalMap,
		WwiseDBShortId InShortId, WwiseDBShortId InLanguageId, WwiseDBShortId InSoundBankId, const WwiseDBString InDebugName);
	
	template <typename RequiredRef>
	static bool GetLocalizableGroupRef(RequiredRef& OutRef, const WwiseDBMap<WwiseDatabaseLocalizableGroupValueKey, RequiredRef>& InGlobalMap,
		WwiseDatabaseGroupValueKey InGroupValue, WwiseDBShortId InLanguageId, WwiseDBShortId InSoundBankId, const WwiseDBString InDebugName);

	template <typename RequiredRef>
	static void GetLocalizableRefs(WwiseDBArray<RequiredRef>& OutRefs, const WwiseDBMap<WwiseDatabaseLocalizableIdKey, RequiredRef>& InGlobalMap,
		WwiseDBShortId InShortId, WwiseDBShortId InLanguageId, WwiseDBShortId InSoundBankId, const WwiseDBString InDebugName);

	template <typename RefType>
	bool GetFromId(RefType& OutRef, WwiseDBShortId InId, WwiseDBShortId InLanguageId = 0, WwiseDBShortId InSoundBankId = 0) const
	{
		return GetLocalizableRef(OutRef, RefType::FGlobalIdsMap::GlobalIdsMap, InId, InLanguageId, InSoundBankId, RefType::NAME);
	}

	template <typename RefType>
	bool GetFromId(RefType& OutRef, WwiseDatabaseGroupValueKey InId, WwiseDBShortId InLanguageId = 0, WwiseDBShortId InSoundBankId = 0) const
	{
		return GetLocalizableGroupRef(OutRef, RefType::FGlobalIdsMap::GlobalIdsMap, InId, InLanguageId, InSoundBankId, RefType::NAME);
	}
	template <typename RefType>
	bool GetFromId(WwiseDBSet<RefType>& OutRef, WwiseDBShortId InId, WwiseDBShortId InLanguageId, WwiseDBShortId InSoundBankId) const;

	bool GetFromId(WwiseRefMedia& OutRef, WwiseDBShortId InShortId, WwiseDBShortId InLanguageId, WwiseDBShortId InSoundBankId) const;

	template <typename RequiredRef>
	void AddBasicRefToMap(WwiseDBMap<WwiseDatabaseLocalizableIdKey, RequiredRef>& OutMap, const RequiredRef& InRef, const WwiseMetadataBasicReference& InObject);

	template <typename RequiredRef>
	void AddEventRefToMap(WwiseDBMap<WwiseDatabaseLocalizableIdKey, RequiredRef>& OutMap, const RequiredRef& InRef, const WwiseMetadataBasicReference& InObject);

	template <typename RequiredRef>
	void AddRefToMap(WwiseDBMap<WwiseDatabaseLocalizableIdKey, RequiredRef>& OutMap, const RequiredRef& InRef, const WwiseDBShortId& InId, const WwiseDBString* InName, const WwiseDBString* InObjectPath, const WwiseDBGuid* InGuid);

	void AddMediaRefsCount(const WwiseDBArray<WwiseMetadataMediaReference>& InMediaRefs);
	void AddSoundBankCount(const unsigned int InId, const unsigned int InLanguageId);

private:
	WwisePlatformDataStructure& operator=(const WwisePlatformDataStructure& Rhs) = delete;
};

struct WwiseDataStructure
{
	std::shared_mutex Lock;

	WwiseRootDataStructure RootData;
	WwiseDBMap<WwiseDBSharedPlatformId, WwisePlatformDataStructure> Platforms;

	WwiseDataStructure() {}
	WwiseDataStructure(const WwiseDBString& InDirectoryPath, const WwiseDBString* InPlatform = nullptr);
	~WwiseDataStructure();

	WwiseDataStructure& operator+=(WwiseDataStructure&& Rhs);
	WwiseDataStructure& operator=(WwiseDataStructure&& Rhs)
	{
		RootData = std::move(Rhs.RootData);
		Platforms = std::move(Rhs.Platforms);
		return *this;
	}

private:
	WwiseDataStructure(const WwiseDataStructure& other) = delete;
	WwiseDataStructure& operator=(const WwiseDataStructure& other) = delete;
	void LoadDataStructure(WwiseGeneratedFiles&& Directory);
};


template<typename RequiredRef>
inline void WwisePlatformDataStructure::GetRefMap(WwiseDBMap<WwiseDBSharedLanguageId, RequiredRef>& OutRefMap, const WwiseDBSet<WwiseDBSharedLanguageId>& InLanguages, const WwiseDBObjectInfo& InInfo) const
{
	OutRefMap.Empty(InLanguages.Size());
	for (const auto& Language : InLanguages)
	{
		RequiredRef Ref;
		if (GetRef(Ref, Language, InInfo))
		{
			OutRefMap.Add(Language, Ref);
		}
	}
}

template<typename RequiredRef>
inline void WwisePlatformDataStructure::GetRefMap(WwiseDBMap<WwiseDBSharedLanguageId, WwiseDBSet<RequiredRef>>& OutRefMap, const WwiseDBSet<WwiseDBSharedLanguageId>& InLanguages, const WwiseDBObjectInfo& InInfo) const
{
	OutRefMap.Empty(InLanguages.Size());
	for (const auto& Language : InLanguages)
	{
        WwiseDBSet<RequiredRef> Refs;
		if (GetRef(Refs, Language, InInfo))
		{
			if (Refs.Size() > 1)
			{
				WWISE_DB_LOG(Log, "More than one ref per language found in %s (%s %" PRIu32 "_wwise_db", *RequiredRef::NAME, *InInfo.WwiseName, InInfo.WwiseShortId);
			}
			OutRefMap.Add(Language, Refs);
		}
	}
}

template <typename RequiredRef>
bool WwisePlatformDataStructure::GetRef(WwiseDBSet<RequiredRef>& OutRef, const WwiseDBSharedLanguageId& InLanguage, const WwiseDBObjectInfo& InInfo) const
{
	const auto LanguageId = InLanguage.GetLanguageId();

	// Get from GUID
	bool bResult = false;

	if (InInfo.WwiseGuid.IsValid())
	{
		WwiseDBArray<const WwiseAnyRef*> Results;
		if (LanguageId != 0)
		{
			Guids.MultiFindPointer(WwiseDatabaseLocalizableGuidKey(InInfo.WwiseGuid, LanguageId), Results, false);
		}
		Guids.MultiFindPointer(WwiseDatabaseLocalizableGuidKey(InInfo.WwiseGuid, 0), Results, false);
		if (Results.Size() > 0) [[likely]]
		{
			for (const auto* Any : Results)
			{
				RequiredRef Result;
				if (Any->GetRef(Result)) [[likely]]
				{
					bool bAlreadyInSet = OutRef.Find(Result) != nullptr;
					if (!bAlreadyInSet) [[likely]]
					{
						if (InInfo.HardCodedSoundBankShortId == 0 || InInfo.HardCodedSoundBankShortId == Result.SoundBankId())
						{
							OutRef.Add(Result, &bAlreadyInSet);
							if (InInfo.WwiseName.IsEmpty()) [[unlikely]]
							{
								WWISE_DB_LOG(Verbose, "Name not set while retrieving Wwise Object GUID %s: Should be %s or %s.",
									*InInfo.WwiseGuid.ToString(), *Any->GetName(), *Any->GetObjectPath());
							}
							else if (InInfo.WwiseName != *Any->GetName() && InInfo.WwiseName != *Any->GetObjectPath()) [[unlikely]]
							{
								WWISE_DB_LOG(Log, "Different name while retrieving Wwise Object GUID %s (%s): Should be %s or %s.",
									*InInfo.WwiseGuid.ToString(), *InInfo.WwiseName, *Any->GetName(), *Any->GetObjectPath());
							}
							if (InInfo.WwiseShortId == 0) [[unlikely]]
							{
								WWISE_DB_LOG(Log, "Id not set while retrieving Wwise Object GUID %s: Should be %" PRIu32 ".",
									*InInfo.WwiseGuid.ToString(), Any->GetId());
							}
							else if (InInfo.WwiseShortId != Any->GetId()) [[unlikely]]
							{
								WWISE_DB_LOG(Log, "Different Id while retrieving Wwise Object GUID %s (%" PRIu32 "_wwise_db: Should be %" PRIu32 ".",
									*InInfo.WwiseGuid.ToString(), InInfo.WwiseShortId, Any->GetId());
							}
						}
					}

					bResult |= !bAlreadyInSet;
				}
			}
			return bResult;
		}
	}

	// Get from Short ID
	if (InInfo.WwiseShortId != 0)
	{
		auto OldOutRefNum = OutRef.Size();
		if (GetFromId(OutRef, InInfo.WwiseShortId, InLanguage.GetLanguageId(), InInfo.HardCodedSoundBankShortId))
		{
			return OldOutRefNum != OutRef.Size();
		}
	}

	// Get from Name. Try all found assets with such name until we get one
	if (!InInfo.WwiseName.IsEmpty())
	{
		WwiseDatabaseLocalizableNameKey LocalizableName(InInfo.WwiseName, InLanguage.GetLanguageId());
		WwiseDBArray<const WwiseAnyRef *> Results;
		if (LanguageId != 0)
		{
			Names.MultiFindPointer(WwiseDatabaseLocalizableNameKey(InInfo.WwiseName, 0), Results);
		}
		Names.MultiFindPointer(WwiseDatabaseLocalizableNameKey(InInfo.WwiseName, LanguageId), Results);
		for (const auto *Any : Results)
		{
			RequiredRef Result;
			if (Any->GetRef(Result)) [[likely]]
			{
				bool bAlreadyInSet = OutRef.Find(Result) != nullptr;
				if (!bAlreadyInSet) [[likely]]
				{
					OutRef.Add(Result, &bAlreadyInSet);
					if (InInfo.WwiseName.IsEmpty()) [[unlikely]]
					{
						WWISE_DB_LOG(Verbose, "Name not set while retrieving Wwise Object GUID %s: Should be %s or %s.",
							   *InInfo.WwiseGuid.ToString(), *Any->GetName(), *Any->GetObjectPath());
					}
					else if (InInfo.WwiseName != *Any->GetName() && InInfo.WwiseName != *Any->GetObjectPath()) [[unlikely]]
					{
						WWISE_DB_LOG(Log, "Different name while retrieving Wwise Object GUID %s (%s): Should be %s or %s.",
							   *InInfo.WwiseGuid.ToString(), *InInfo.WwiseName, *Any->GetName(), *Any->GetObjectPath());
					}
					if (InInfo.WwiseShortId == 0) [[unlikely]]
					{
						WWISE_DB_LOG(Log, "Id not set while retrieving Wwise Object GUID %s: Should be %" PRIu32 ".",
							   *InInfo.WwiseGuid.ToString(), Any->GetId());
					}
					else if (InInfo.WwiseShortId != Any->GetId()) [[unlikely]]
					{
						WWISE_DB_LOG(Log, "Different Id while retrieving Wwise Object GUID %s (%" PRIu32 "_wwise_db: Should be %" PRIu32 ".",
							   *InInfo.WwiseGuid.ToString(), InInfo.WwiseShortId, Any->GetId());
					}
				}
				bResult |= !bAlreadyInSet;
			}
			return bResult;
		}
	}
	return false;
}

template <typename RequiredRef>
inline bool WwisePlatformDataStructure::GetRef(RequiredRef& OutRef, const WwiseDBSharedLanguageId& InLanguage, const WwiseDBObjectInfo& InInfo) const
{
	const auto LanguageId = InLanguage.GetLanguageId();

	// Get from GUID
	if (InInfo.WwiseGuid.IsValid())
	{
		WwiseDatabaseLocalizableGuidKey LocalizableGuid(InInfo.WwiseGuid, LanguageId);
		WwiseDBArray<WwiseAnyRef> AssetsFromGuid;
		Guids.MultiFind(LocalizableGuid, AssetsFromGuid);
		for(auto& AssetFromGuid : AssetsFromGuid)
		{
			if(AssetFromGuid.GetRef(OutRef))
			{
				return true;
			}
		}

		if (LanguageId != 0) [[likely]]
		{
			AssetsFromGuid.Empty();
			LocalizableGuid = WwiseDatabaseLocalizableGuidKey(InInfo.WwiseGuid, 0);
			Guids.MultiFind(LocalizableGuid, AssetsFromGuid);
			for(auto& AssetFromGuid : AssetsFromGuid)
			{
				if(AssetFromGuid.GetRef(OutRef))
				{
					return true;
				}
			}
		}
		return false;
	}

	// Get from Short ID
	if (InInfo.WwiseShortId != 0)
	{
		if (GetFromId(OutRef, InInfo.WwiseShortId, LanguageId, InInfo.HardCodedSoundBankShortId))
		{
			return true;
		}
	}

	// Get from Name. Try all found assets with such name until we get one
	if (!InInfo.WwiseName.IsEmpty())
	{
		WwiseDBArray<const WwiseAnyRef*> FoundAssets;
		Names.MultiFindPointer(WwiseDatabaseLocalizableNameKey(InInfo.WwiseName, LanguageId), FoundAssets);
		if (LanguageId != 0) [[likely]]
		{
			Names.MultiFindPointer(WwiseDatabaseLocalizableNameKey(InInfo.WwiseName, 0), FoundAssets);
		}
		for (const auto* Asset : FoundAssets)
		{
			WwiseDBString startWith = "";
			if (InInfo.WwiseType == "User")
			{
				WwiseDBString language = Asset->GetLanguageName();
				if (language != "SFX")
				{
					startWith = language;
					startWith.String.append("/");
				}
				startWith.String.append(InInfo.WwiseName.String);
			}
			else
			{
				startWith = InInfo.WwiseType;
			}
			if (Asset->GetRef(OutRef) && Asset->GetPath().StartsWith(startWith))
			{
				return true;
			}
		}
	}
	return false;
}

template <typename RequiredRef>
inline bool WwisePlatformDataStructure::GetRef(RequiredRef& OutRef, const WwiseDBSharedLanguageId& InLanguage, const WwiseDBGroupValueInfo& InInfo) const
{
	const auto LanguageId = InLanguage.GetLanguageId();

	// Get from GUID
	if (InInfo.WwiseGuid.IsValid())
	{
		WwiseDatabaseLocalizableGuidKey LocalizableGuid(InInfo.WwiseGuid, LanguageId);
		WwiseDBArray< WwiseAnyRef> Array;
		Guids.MultiFind(LocalizableGuid, Array);
		if (Array.Size() != 0) [[likely]]
		{
			return Array[0].GetRef(OutRef);
		}

		if (LanguageId != 0) [[likely]]
		{
			LocalizableGuid = WwiseDatabaseLocalizableGuidKey(InInfo.WwiseGuid, 0);
			Guids.MultiFind(LocalizableGuid, Array);
			if (Array.Size() != 0) [[likely]]
			{
				return Array[0].GetRef(OutRef);
			}
		}
	}

	// Get from Short ID
	if (InInfo.WwiseShortId != 0)
	{
		if (GetFromId(OutRef, WwiseDatabaseGroupValueKey(InInfo.GroupShortId, InInfo.WwiseShortId), InLanguage.GetLanguageId(), 0))
		{
			return true;
		}
	}

	// Get from Name. Try all found assets with such name until we get one
	if (!InInfo.WwiseName.IsEmpty())
	{
		WwiseDatabaseLocalizableNameKey LocalizableName(InInfo.WwiseName, InLanguage.GetLanguageId());
		WwiseDBArray<const WwiseAnyRef *> FoundAssets;
		Names.MultiFindPointer(WwiseDatabaseLocalizableNameKey(InInfo.WwiseName, LanguageId), FoundAssets);
		if (LanguageId != 0) [[likely]]
		{
			Names.MultiFindPointer(WwiseDatabaseLocalizableNameKey(InInfo.WwiseName, 0), FoundAssets);
		}
		for (const auto *Asset : FoundAssets)
		{
			if (Asset->GetRef(OutRef))
			{
				return true;
			}
		}
	}
	return false;
}

template <typename RequiredRef>
inline bool WwisePlatformDataStructure::GetLocalizableRef(RequiredRef & OutRef, const WwiseDBMap<WwiseDatabaseLocalizableIdKey, RequiredRef> &InGlobalMap,
	WwiseDBShortId InShortId, WwiseDBShortId InLanguageId, WwiseDBShortId InSoundBankId, const WwiseDBString InDebugName)
{
	const RequiredRef* Result = nullptr;
	if (InLanguageId != 0) [[likely]]
	{
		WwiseDatabaseLocalizableIdKey LocalizableId(InShortId, InLanguageId);
		Result = InGlobalMap.Find(LocalizableId);

		if (!Result)
		{
			WwiseDatabaseLocalizableIdKey NoLanguageId(InShortId, WwiseDatabaseLocalizableIdKey::GENERIC_LANGUAGE);
			Result = InGlobalMap.Find(NoLanguageId);
		}
	}
	else
	{
		for (const auto& Elem : InGlobalMap)
		{
			WwiseDBPair<WwiseDatabaseLocalizableIdKey, RequiredRef> Pair(Elem);
			if (Pair.GetFirst().Id == InShortId)
			{
				auto Value = Pair.GetSecond();
				Result = &Value;
				break;
			}
		}
	}

	if (!Result) [[unlikely]]
	{
		return false;
	}

	if (InSoundBankId != 0) [[unlikely]]
	{
		const WwiseMetadataSoundBank* SoundBank = Result->GetSoundBank();
		if (!SoundBank)
		{
			WWISE_DB_LOG(Error, "Could not retrieve SoundBank for %s %" PRIu32 " (Lang=%" PRIu32 "; SB=%" PRIu32 "_wwise_db", *InDebugName, InShortId, InLanguageId, InSoundBankId);
			return false;
		}
		if (SoundBank->Id != InSoundBankId) [[unlikely]]
		{
			WWISE_DB_LOG(Error, "Get incorrect SoundBank %" PRIu32 " for %s %" PRIu32 " (Lang = %" PRIu32 "; SB = %" PRIu32 "_wwise_db", SoundBank->Id, *InDebugName, InShortId, InLanguageId, InSoundBankId);
			return false;
		}
	}

	OutRef = *Result;
	return true;
}

template <typename RequiredRef>
void WwisePlatformDataStructure::GetLocalizableRefs(WwiseDBArray<RequiredRef>& OutRefs,
	const WwiseDBMap<WwiseDatabaseLocalizableIdKey, RequiredRef>& InGlobalMap, WwiseDBShortId InShortId, WwiseDBShortId InLanguageId,
	WwiseDBShortId InSoundBankId, const WwiseDBString InDebugName)
{
	if (InLanguageId != 0 && InSoundBankId != 0) [[likely]]
    {
        const RequiredRef* Result = nullptr;
        WwiseDatabaseLocalizableIdKey RefId(InShortId, InLanguageId);
        Result = InGlobalMap.Find(RefId);

        if (!Result)
        {
            WwiseDatabaseLocalizableIdKey NoLanguageId(InShortId, WwiseDatabaseLocalizableIdKey::GENERIC_LANGUAGE);
            Result = InGlobalMap.Find(NoLanguageId);
        }

        if (Result)
        {
            OutRefs.Add(*Result);
        }
    }
    else
    {
        for (const auto& Elem : InGlobalMap)
        {
        	WwiseDBPair<WwiseDatabaseLocalizableIdKey, RequiredRef> Pair(Elem);
            if (Pair.GetFirst().Id == InShortId
                && (InLanguageId == 0 || Pair.GetFirst().LanguageId == InLanguageId || Pair.GetFirst().LanguageId == 0)
                && (InSoundBankId == 0 || Pair.GetFirst().SoundBankId == InSoundBankId)) [[unlikely]]
            {
                OutRefs.Add(Pair.GetSecond());
            }
        }
    }
}

template <>
inline bool WwisePlatformDataStructure::GetLocalizableRef<WwiseRefPluginLib>(WwiseRefPluginLib& OutRef, const WwiseDBMap<WwiseDatabaseLocalizableIdKey, WwiseRefPluginLib>& InGlobalMap,
	WwiseDBShortId InShortId, WwiseDBShortId InLanguageId, WwiseDBShortId InSoundBankId, const WwiseDBString InDebugName)
{
	const WwiseRefPluginLib* Result = nullptr;
	if (InLanguageId != 0) [[likely]]
	{
		WwiseDatabaseLocalizableIdKey LocalizableId(InShortId, InLanguageId);
		Result = InGlobalMap.Find(LocalizableId);

		if (!Result)
		{
			WwiseDatabaseLocalizableIdKey NoLanguageId(InShortId, WwiseDatabaseLocalizableIdKey::GENERIC_LANGUAGE);
			Result = InGlobalMap.Find(NoLanguageId);
		}
	}
	else
	{
		for (const auto& Elem : InGlobalMap)
		{
			WwiseDBPair<const WwiseDatabaseLocalizableIdKey, WwiseRefPluginLib> Pair(Elem);
			if (Pair.GetFirst().Id == InShortId)
			{
				auto Value = Pair.GetSecond();
				Result = &Value;
				break;
			}
		}
	}

	if (!Result) [[unlikely]]
	{
		return false;
	}

	OutRef = *Result;
	return true;
}

template <typename RequiredRef>
inline bool WwisePlatformDataStructure::GetLocalizableGroupRef(RequiredRef& OutRef, const WwiseDBMap<WwiseDatabaseLocalizableGroupValueKey, RequiredRef>& InGlobalMap,
	WwiseDatabaseGroupValueKey InGroupValue, WwiseDBShortId InLanguageId, WwiseDBShortId InSoundBankId, const WwiseDBString InDebugName)
{
	const RequiredRef* Result = nullptr;
	if (InLanguageId != 0) [[likely]]
	{
		WwiseDatabaseLocalizableGroupValueKey LocalizableGroupValue(InGroupValue, InLanguageId);
		Result = InGlobalMap.Find(LocalizableGroupValue);

		if (!Result)
		{
			WwiseDatabaseLocalizableGroupValueKey NoLanguageId(InGroupValue, WwiseDatabaseLocalizableIdKey::GENERIC_LANGUAGE);
			Result = InGlobalMap.Find(NoLanguageId);
		}
	}
	else
	{
		for (const auto& Elem : InGlobalMap)
		{
			WwiseDBPair<WwiseDatabaseLocalizableGroupValueKey, RequiredRef> Pair(Elem);
			if (Pair.GetFirst().GroupValue == InGroupValue)
			{
				auto Value = Pair.GetSecond();
				Result = &Value;
				break;
			}
		}
	}

	if (!Result) [[unlikely]]
	{
		return false;
	}

	if (InSoundBankId != 0)
	{
		const WwiseMetadataSoundBank* SoundBank = Result->GetSoundBank();
		if (!SoundBank) [[unlikely]]
		{
			WWISE_DB_LOG(Error, "Could not retrieve SoundBank for %s %" PRIu32 " %" PRIu32 " (Lang = %" PRIu32 "; SB = %" PRIu32 "_wwise_db", *InDebugName, InGroupValue.GroupId, InGroupValue.Id, InLanguageId, InSoundBankId);
			return false;
		}
		if (SoundBank->Id != InSoundBankId) [[unlikely]]
		{
			WWISE_DB_LOG(Error, "Get incorrect SoundBank %" PRIu32 " for %s %" PRIu32 " %" PRIu32 " (Lang = %" PRIu32 "; SB = %" PRIu32 "_wwise_db", SoundBank->Id, *InDebugName, InGroupValue.GroupId, InGroupValue.Id, InLanguageId, InSoundBankId);
			return false;
		}
	}

	OutRef = *Result;
	return true;
}


template <typename RefType>
inline	bool WwisePlatformDataStructure::GetFromId(WwiseDBSet<RefType>& OutRef, WwiseDBShortId InId, WwiseDBShortId InLanguageId, WwiseDBShortId InSoundBankId) const
{
	WwiseDBArray<RefType> Refs;
	GetLocalizableRefs(Refs, RefType::FGlobalIdsMap::GlobalIdsMap, InId, InLanguageId, InSoundBankId, RefType::NAME);
	if (Refs.Size() > 0) [[likely]]
	{
		OutRef.Append(Refs);
		return true;
	}
	return false;
}

template<typename RequiredRef>
inline void WwisePlatformDataStructure::AddBasicRefToMap(WwiseDBMap<WwiseDatabaseLocalizableIdKey, RequiredRef>& OutMap, const RequiredRef& InRef, const WwiseMetadataBasicReference& InObject)
{
	AddRefToMap(OutMap, InRef, InObject.Id, &InObject.Name, &InObject.ObjectPath, &InObject.GUID);
}

template<typename RequiredRef>
inline void WwisePlatformDataStructure::AddEventRefToMap(WwiseDBMap<WwiseDatabaseLocalizableIdKey, RequiredRef>& OutMap, const RequiredRef& InRef, const WwiseMetadataBasicReference& InObject)
{
	AddRefToMap(OutMap, InRef, InObject.Id, &InObject.Name, &InObject.ObjectPath, &InObject.GUID);
}

template<typename RequiredRef>
void WwisePlatformDataStructure::AddRefToMap(WwiseDBMap<WwiseDatabaseLocalizableIdKey, RequiredRef>& OutMap, const RequiredRef& InRef, const WwiseDBShortId& InId, const WwiseDBString* InName, const WwiseDBString* InObjectPath, const WwiseDBGuid* InGuid)
{
	const auto AnyRef = WwiseAnyRef::Create(InRef);
	if (InName && !InName->IsEmpty())
	{
		Names.Add(WwiseDatabaseLocalizableNameKey(*InName, InRef.LanguageId), AnyRef);
	}
	if (InObjectPath && !InObjectPath->IsEmpty())
	{
		Names.Add(WwiseDatabaseLocalizableNameKey(*InObjectPath, InRef.LanguageId), AnyRef);
	}
	if (InGuid && InGuid->IsValid())
	{
		Guids.Add(WwiseDatabaseLocalizableGuidKey(*InGuid, InRef.LanguageId), AnyRef);
	}
	OutMap.Add(WwiseDatabaseLocalizableIdKey(InId, InRef.LanguageId), InRef);
}

template<>
inline void WwisePlatformDataStructure::AddRefToMap<WwiseRefPluginLib>(WwiseDBMap<WwiseDatabaseLocalizableIdKey, WwiseRefPluginLib>& OutMap, const WwiseRefPluginLib& InRef, const WwiseDBShortId& InId, const WwiseDBString* InName, const WwiseDBString* InObjectPath, const WwiseDBGuid* InGuid)
{
	const auto AnyRef = WwiseAnyRef::Create(InRef);
	if (InName && !InName->IsEmpty())
	{
		Names.Add(WwiseDatabaseLocalizableNameKey(*InName, 0), AnyRef);
	}
	if (InObjectPath && !InObjectPath->IsEmpty())
	{
		Names.Add(WwiseDatabaseLocalizableNameKey(*InObjectPath, 0), AnyRef);
	}
	if (InGuid && InGuid->IsValid())
	{
		Guids.Add(WwiseDatabaseLocalizableGuidKey(*InGuid, 0), AnyRef);
	}
	OutMap.Add(WwiseDatabaseLocalizableIdKey(InId, 0), InRef);
}
