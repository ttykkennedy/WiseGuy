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

#include "WwiseProjectDatabase/AdapterTypes/WwiseWrapperTypes.h"
#include "stdafx.h"
#include "WwiseProjectDatabase/C-Ref/WwiseCRefSoundBank.h"
#include "WwiseProjectDatabase/C-Ref/WwiseCRefPlugin.h"
#include "WwiseProjectDatabase/Ref/WwiseRefCollections.h"

/*
* Exposed Utilities  
*/

/// Initialize the WwiseProjectDatabase of a given Wwise Project for a given platform.
/// This parses and populates the structures.
/// @param InDirectoryPath The Path to the Root Output Path of the Wwise Project
/// @param InDirectoryPlatformName The Platform the WwiseProjectDatabase will parse
extern "C" WWISE_PROJECT_DATABASE_API  void Init(const char* InDirectoryPath, const char* InDirectoryPlatformName);

/// Set the Platform to parse.
/// @param InDirectoryPlatformName Platform to parse
extern "C" WWISE_PROJECT_DATABASE_API  void SetCurrentPlatform(const char* InDirectoryPlatformName);

/// Set the Language to parse
/// @param InLanguageName Current Language to parse
extern "C" WWISE_PROJECT_DATABASE_API  void SetCurrentLanguage(const char* InLanguageName);

/*
    SoundBank
*/
const WwiseSoundBankGlobalIdsMap GetCurrentPlatformSoundBanks();

/// Get a reference to the start of a list containing all the SoundBanks.
/// @return The reference to the start of a list containing all the SoundBanks
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseCRefSoundBank * GetAllSoundBanksRef();

/// Get the number of SoundBanks in the Project Database.
/// @return The number of SoundBanks in the Project Database
extern "C" WWISE_PROJECT_DATABASE_API  const unsigned int GetSoundBankCount();

/// Get a reference to a SoundBank given its index in the list of all the SoundBanks.
/// @param InSoundBanksRefPtr The reference to the start of a list containing all the SoundBanks
/// @param index The index of the SoundBank to retrieve
/// @return The reference of the SoundBank
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseCRefSoundBank * GetSoundBankRefIndex(void* InSoundBanksArrayRefPtr, int index);

/// Get a reference to a SoundBank given its name
/// @param InString The SoundBank Name
/// @return The reference of the SoundBank
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseCRefSoundBank * GetSoundBankRefString(const char* InName, const char* InType);

/// Get the name of a SoundBank
/// @param InSoundBankRefPtr The SoundBank reference
/// @return The Name of the SoundBank
extern "C" WWISE_PROJECT_DATABASE_API  const char* GetSoundBankName(void* InSoundBankRefPtr);

/// Get the path of a SoundBank
/// @param InSoundBankRefPtr The SoundBank reference
/// @return The Path of the SoundBank
extern "C" WWISE_PROJECT_DATABASE_API  const char* GetSoundBankPath(void* InSoundBankRefPtr);

/// Get the Language of a SoundBank as a string
/// @param InSoundBankRefPtr The SoundBank reference
/// @return The Language of the SoundBank
extern "C" WWISE_PROJECT_DATABASE_API  const char* GetSoundBankLanguage(void* InSoundBankRefPtr);

/// Get the Language ID of a SoundBank
/// @param InSoundBankRefPtr The SoundBank reference
/// @return The Language ID of the SoundBank
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseDBShortId GetSoundBankLanguageId(void* InSoundBankRefPtr);

/// Get the GUID of a SoundBank
/// @param InSoundBankRefPtr The SoundBank reference
/// @return A reference to the GUID of the SoundBank
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseDBGuid* GetSoundBankGuid(void* InSoundBankRefPtr);

/// Get the ShortID of a SoundBank
/// @param InSoundBankRefPtr The SoundBank reference
/// @return The ShortID of the SoundBank
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseDBShortId GetSoundBankShortId(void* InSoundBankRefPtr);

/// Get if a SoundBank is a User Defined SoundBank
/// @param InSoundBankRefPtr The SoundBank reference
/// @return Whether the SoundBank is a User Defined SoundBank or not
extern "C" WWISE_PROJECT_DATABASE_API  const bool IsUserBank(void* InSoundBankRefPtr);

/// Get if a SoundBank is an Init Bank
/// @param InSoundBankRefPtr The SoundBank reference
/// @return Whether the SoundBank is an Init SoundBank or not
extern "C" WWISE_PROJECT_DATABASE_API  const bool IsInitBank(void* InSoundBankRefPtr);

/// Get if a SoundBank Reference is a valid reference
/// A valid reference is a reference with information properly filled with the metadata.
/// @param InSoundBankRefPtr The SoundBank reference
/// @return Whether the SoundBank reference is valid not
extern "C" WWISE_PROJECT_DATABASE_API  const bool IsSoundBankValid(void* InSoundBankRefPtr);

/// Get a Media in the SoundBank at a given index
/// For the index to be valid, it must be greater or equal to 0 and less than GetSoundBankMediasCount.
/// @param InSoundBankRefPtr The SoundBank reference
/// @param index The Index of the Media
/// @return The Reference of the Media or nullptr if the index is invalid
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseCRefMedia* GetSoundBankMedia(void* InSoundBankRefPtr, int index);

/// Get the number of Medias in a SoundBank
/// @param InSoundBankRefPtr The SoundBank reference
/// @return The Number of Medias in the SoundBank
extern "C" WWISE_PROJECT_DATABASE_API  const unsigned int GetSoundBankMediasCount(void* InSoundBankRefPtr);

/// Get an Event in the SoundBank at a given index
/// For the index to be valid, it must be greater or equal to 0 and less than GetSoundBankEventsCount.
/// @param InSoundBankRefPtr The SoundBank reference
/// @param index The Index of the Event
/// @return The Reference of the Event or nullptr if the index is invalid
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseCRefEvent* GetSoundBankEvent(void* InSoundBankRefPtr, int index);

/// Get the number of Events in a SoundBank
/// @param InSoundBankRefPtr The SoundBank reference
/// @return The Number of Events in the SoundBank
extern "C" WWISE_PROJECT_DATABASE_API  const unsigned int GetSoundBankEventsCount(void* InSoundBankRefPtr);

/// Delete a SoundBank Reference. This should be called upon exiting the scope of the reference.
/// @param InSoundBankRefPtr The SoundBank Reference
extern "C" WWISE_PROJECT_DATABASE_API  void DeleteSoundBankRef(void* InSoundBankRefPtr);

/// Delete an array containing all the SoundBanks. This should be called upon exiting the scope of the array.
/// @param InSoundBankRefPtr The reference to an array containing all the SoundBanks
extern "C" WWISE_PROJECT_DATABASE_API  void DeleteSoundBanksArrayRef(void* InSoundBanksArrayRefPtr);

/*
    Media
*/

const WwiseMediaGlobalIdsMap GetCurrentPlatformMedias();

/// Get the Name of a Media
/// @param InMediaRefPtr The Media Reference
/// @return The Name of the Media
extern "C" WWISE_PROJECT_DATABASE_API  const char* GetMediaName(void* InMediaRefPtr);

/// Get the Object Path of a Media
/// @param InMediaRefPtr The Media Reference
/// @return The Object Path of the Media
extern "C" WWISE_PROJECT_DATABASE_API  const char* GetMediaPath(void* InMediaRefPtr);

/// Get the ShortID of a Media
/// @param InMediaRefPtr The Media Reference
/// @return The ShortID of the Media
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseDBShortId GetMediaShortId(void* InMediaRefPtr);

/// Get the Language of a Media
/// @param InMediaRefPtr The Media Reference
/// @return The Language of the Media
extern "C" WWISE_PROJECT_DATABASE_API  const char* GetMediaLanguage(void* InMediaRefPtr);

/// Get if a Media is Streaming
/// @param InMediaRefPtr The Media Reference
/// @return Whether the Media is Streaming or not
extern "C" WWISE_PROJECT_DATABASE_API  const bool GetMediaIsStreaming(void* InMediaRefPtr);

/// Get the Media Location
/// @param InMediaRefPtr The Media Reference
/// @return The Location ID of the Media (	Memory (0), Loose (1), OtherBank (2) or Unknown (-1) )
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseDBShortId GetMediaLocation(void* InMediaRefPtr);

/// Get the Media Cache Path
/// @param InMediaRefPtr The Media Reference
/// @return The Cache Path of the SoundBank
extern "C" WWISE_PROJECT_DATABASE_API  const char* GetMediaCachePath(void* InMediaRefPtr);

/// Delete a Media Reference. This should be called upon exiting the scope of the reference.
/// @param InMediaRefPtr The Media Reference
extern "C" WWISE_PROJECT_DATABASE_API  void DeleteMediaRef(void* InMediaRefPtr);

/*
    Event
*/

const WwiseEventGlobalIdsMap GetCurrentPlatformEvents();

/// Get the reference of an Event given its name
/// @param InString The Name of the Event
/// @return The Event reference
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseCRefEvent * GetEventRefString(const char* InString);

/// Get the name of an Event
/// @param InEventRefPtr The Event reference
/// @return The Name of the Event
extern "C" WWISE_PROJECT_DATABASE_API  const char* GetEventName(void* InEventRefPtr);

/// Get the Object Path of an Event
/// @param InEventRefPtr The Event reference
/// @return The Object Path of the Event
extern "C" WWISE_PROJECT_DATABASE_API  const char* GetEventPath(void* InEventRefPtr);

/// Get the GUID of an Event
/// @param InEventRefPtr The Event reference
/// @return A reference to the GUID of the Event
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseDBGuid * GetEventGuid(void* InEventRefPtr);

/// Get the ShortID of an Event
/// @param InEventRefPtr The Event reference
/// @return The ShortID of the Event
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseDBShortId GetEventShortId(void* InEventRefPtr);

/// Get the Max Attenuation of an Event
/// @param InEventRefPtr The Event reference
/// @return The Max Attenuation of the Event
extern "C" WWISE_PROJECT_DATABASE_API  const float GetEventMaxAttenuation(void* InEventRefPtr);

/// Get the Minimum Duration of an Event
/// @param InEventRefPtr The Event reference
/// @return The Minimum Duration of the Event
extern "C" WWISE_PROJECT_DATABASE_API  const float GetEventMinDuration(void* InEventRefPtr);

/// Get the Maximum Duration of an Event
/// @param InEventRefPtr The Event reference
/// @return The Maximum Duration of the Event
extern "C" WWISE_PROJECT_DATABASE_API  const float GetEventMaxDuration(void* InEventRefPtr);

/// Get the Media of an Event at a given index
/// For the index to be valid, it must be greater or equal to 0 and less than GetEventMediasCount().
/// @param InEventRefPtr The Event reference
/// @param index The Index of the Media
/// @return The Media reference of nullptr if the index is invalid
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseCRefMedia * GetEventMedia(void* InEventRefPtr, int index);

/// Get the number of Medias in an Event
/// @param InEventRefPtr The Event reference
/// @return The Number of Medias in the Event
extern "C" WWISE_PROJECT_DATABASE_API  const unsigned int GetEventMediasCount(void* InEventRefPtr);

/// Delete an Event reference. This should be called upon exiting the scope of the reference.
/// @param InEventRefPtr The Event reference
extern "C" WWISE_PROJECT_DATABASE_API  void DeleteEventRef(void* InEventRefPtr);

/*
 * Platform
*/

/// Get a reference to a platform given its name
/// @param InPlatformName The name of the platform
/// @return The reference of the platform
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseCRefPlatform * GetPlatformRef(const char* InPlatformName);

/// Get the name of a Platform
/// @param InPlatformRefPtr The Platform reference
/// @return The Name of the Platform
extern "C" WWISE_PROJECT_DATABASE_API  const char* GetPlatformName(void* InPlatformRefPtr);

/// Get the GUID of a Platform
/// @param InPlatformRefPtr The Platform reference
/// @return A reference to the GUID of the Platform
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseDBGuid * GetPlatformGuid(void* InPlatformRefPtr);

/// Delete a Platform reference. This should be called upon exiting the scope of the reference.
/// @param InPlatformRefPtr The Platform reference
extern "C" WWISE_PROJECT_DATABASE_API  void DeletePlatformRef(void* InPlatformRefPtr);

/*
 * Plugin
*/

const WwisePluginLibGlobalIdsMap GetCurrentPlatformPlugins();

/// Get a reference to the start of a list containing all the Plugins.
/// @return The reference to the start of a list containing all the Plugins
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseCRefPlugin* GetAllPluginRef();

/// Get the number of Plugins in the Project Database.
/// @return The number of Plugins in the Project Database
extern "C" WWISE_PROJECT_DATABASE_API  const unsigned int GetPluginCount();

/// Get a reference to a Plugin given its index in the list of all the Plugins.
/// @param InPluginRefPtr The reference to the start of a list containing all the Plugins
/// @param index The index of the Plugin to retrieve
/// @return The reference of the Plugin
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseCRefPlugin * GetPluginRefIndex(void* InPluginArrayRefPtr, int index);

/// Get the id of a Plugin
/// @param InPluginRefPtr The Plugin reference
/// @return The Id of the Plugin
extern "C" WWISE_PROJECT_DATABASE_API  const WwiseDBShortId GetPluginId(void* InPluginRefPtr);

/// Get the name of a Plugin
/// @param InPluginRefPtr The Plugin reference
/// @return The Name of the Plugin
extern "C" WWISE_PROJECT_DATABASE_API  const char* GetPluginName(void* InPluginRefPtr);

/// Get the dll name of a Plugin
/// @param InPluginRefPtr The Plugin reference
/// @return The DLL name of the Plugin
extern "C" WWISE_PROJECT_DATABASE_API  const char* GetPluginDLL(void* InPluginRefPtr);

/// Get the static lib name of a Plugin
/// @param InPluginRefPtr The Plugin reference
/// @return The StaticLib of the Plugin
extern "C" WWISE_PROJECT_DATABASE_API  const char* GetPluginStaticLib(void* InPluginRefPtr);

/// Delete a Plugin Reference. This should be called upon exiting the scope of the reference.
/// @param InPluginRefPtr The Plugin Reference
extern "C" WWISE_PROJECT_DATABASE_API  void DeletePluginRef(void* InPluginRefPtr);

/// Delete an array containing all the Plugins. This should be called upon exiting the scope of the array.
/// @param InPluginRefPtr The reference to an array containing all the Plugins
extern "C" WWISE_PROJECT_DATABASE_API  void DeletePluginArrayRef(void* InPluginArrayRefPtr);

