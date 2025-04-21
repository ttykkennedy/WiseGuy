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

#include "WwiseProjectDatabase.h"
#include "WwiseDataStructure.h"

WwiseDataStructure DataStructure;
WwiseDBSharedPlatformId CurrentPlatform;
WwiseDBSharedLanguageId CurrentLanguage;

/*
* Private Utilities
*/

void FillMedia(const WwiseRefMedia& InMedia, WwiseCRefMedia& OutCMedias)
{
    if(!InMedia.GetMedia())
        return;
    
    OutCMedias.name = **InMedia.MediaShortName();
    OutCMedias.shortId = InMedia.MediaId();
    OutCMedias.path = **InMedia.MediaPath();
    OutCMedias.Language = *InMedia.GetMedia()->Language;
    OutCMedias.CachePath = *InMedia.GetMedia()->CachePath;
    OutCMedias.bStreaming = InMedia.GetMedia()->bStreaming;
    OutCMedias.Location = InMedia.GetMedia()->Location;
}

void FillEvent(const WwiseRefEvent& WwiseRef, WwiseCRefEvent& OutWwiseCRef)
{
    if(!WwiseRef.GetEvent())
        return;
    
    OutWwiseCRef.guid = WwiseRef.EventGuid();
    OutWwiseCRef.shortId = WwiseRef.EventId();
    OutWwiseCRef.name = **WwiseRef.EventName();
    OutWwiseCRef.path = **WwiseRef.EventObjectPath();
    OutWwiseCRef.maxAttenuation = WwiseRef.MaxAttenuation();
    WwiseMetadataEventDurationType durationType;
    float minDuration;
    float maxDuration;
    WwiseRef.GetDuration(durationType, minDuration, maxDuration);
    OutWwiseCRef.minDuration = minDuration;
    OutWwiseCRef.maxDuration = maxDuration;


    auto Medias = WwiseRef.GetAllMedia(GetCurrentPlatformMedias());
    if (Medias.Size() > 0)
    {
        int count = 0;
        OutWwiseCRef.medias = new WwiseCRefMedia[Medias.Size()];
        for (const auto& Media : Medias)
        {
            FillMedia(Media.second, OutWwiseCRef.medias[count]);
            count++;
        }
        OutWwiseCRef.mediasCount = count;
    }
}

void FillSoundBank(const WwiseRefSoundBank& WwiseRef, WwiseCRefSoundBank& OutWwiseCRef)
{
    if(!WwiseRef.GetSoundBank())
        return;
    
    OutWwiseCRef.guid = WwiseRef.SoundBankGuid();
    OutWwiseCRef.language = **WwiseRef.SoundBankLanguage();
    OutWwiseCRef.languageId = WwiseRef.LanguageId;
    OutWwiseCRef.shortId = WwiseRef.SoundBankId();
    OutWwiseCRef.name = **WwiseRef.SoundBankShortName();
    OutWwiseCRef.path = **WwiseRef.SoundBankPath();
    OutWwiseCRef.bIsInitBank = WwiseRef.IsInitBank();
    OutWwiseCRef.bIsUserBank = WwiseRef.IsUserBank();
    OutWwiseCRef.bIsValid = true;

    auto Medias = WwiseRef.GetSoundBankMedia(GetCurrentPlatformMedias());
    if (Medias.Size() > 0)
    {
        int count = 0;
        OutWwiseCRef.medias = new WwiseCRefMedia[Medias.Size()];
        for (const auto& Media : Medias)
        {
            FillMedia(Media.second, OutWwiseCRef.medias[count]);
            count++;
        }
        OutWwiseCRef.mediasCount = count;
    }

    auto Events = WwiseRef.GetSoundBankEvents(GetCurrentPlatformEvents());
    if (Events.Size() > 0)
    {
        OutWwiseCRef.events = new WwiseCRefEvent[Events.Size()];
        int count = 0;
        for (auto& Event : Events)
        {
            FillEvent(Event.second, OutWwiseCRef.events[count]);
            count++;
        }
        OutWwiseCRef.eventsCount = count;

    }
}

void FillPlugin(const WwiseRefPluginLib& WwiseRef, WwiseCRefPlugin& OutWwiseCRef)
{
    OutWwiseCRef.id = WwiseRef.PluginLibId();
    OutWwiseCRef.name = **WwiseRef.PluginLibName();
    OutWwiseCRef.dll = **WwiseRef.PluginLibDLL();
    OutWwiseCRef.staticLib = **WwiseRef.PluginLibStaticLib();
}

WwiseCRefEvent* GetEventRef(const WwiseDBObjectInfo& Info)
{
    WwiseRefEvent Event;
    if (!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    if (DataStructure.Platforms[CurrentPlatform].GetRef(Event, CurrentLanguage, Info))
    {
        WwiseCRefEvent* CEvent = new WwiseCRefEvent();
        FillEvent(Event, *CEvent);
        return CEvent;
    }
    return nullptr;
}

WwiseCRefSoundBank* GetSoundBankRef(const WwiseDBObjectInfo& Info)
{
    WwiseRefSoundBank SoundBank;
    if (!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    if (DataStructure.Platforms[CurrentPlatform].GetRef(SoundBank, CurrentLanguage, Info))
    {
        WwiseCRefSoundBank* CSoundBank = new WwiseCRefSoundBank();
        FillSoundBank(SoundBank, *CSoundBank);
        return CSoundBank;
    }
    return nullptr;
}

WwiseDBString GetStandardPath(const char* InDirectoryPath)
{
    WwiseDBString standardFilePath = InDirectoryPath;
    standardFilePath.MakeStandardFilename();
    if(!standardFilePath.IsEmpty() && standardFilePath.String.back() != '/' && standardFilePath.String.back() != '\\')
    {
        standardFilePath.String.append("/");
    }
    return standardFilePath;
}

/*
* Exposed Utilities  
*/

void Init(const char* InDirectoryPath, const char* InDirectoryPlatformName)
{
    const auto platformAsStandardString = WwiseStandardString(InDirectoryPlatformName);
    const WwiseDBString standardFilePath = GetStandardPath(InDirectoryPath);
    DataStructure = WwiseDataStructure(standardFilePath, &platformAsStandardString);
    const WwiseRefPlatform* RootPlatformByName = DataStructure.RootData.PlatformNames.Find(InDirectoryPlatformName);
    if (RootPlatformByName)
    {
        CurrentPlatform = WwiseDBSharedPlatformId(*RootPlatformByName->PlatformGuid(), InDirectoryPlatformName);
        CurrentLanguage = DataStructure.RootData.DefaultLanguage;
    }
}

void SetCurrentPlatform(const char* InDirectoryPlatformName)
{
    WwiseRefPlatform* platform = DataStructure.RootData.PlatformNames.Find(InDirectoryPlatformName);
    if (platform)
    {
        CurrentPlatform = WwiseDBSharedPlatformId(*platform->PlatformGuid(), InDirectoryPlatformName);
    }
}

void SetCurrentLanguage(const char* InLanguageName)
{
    WwiseRefLanguage* language = DataStructure.RootData.LanguageNames.Find(InLanguageName);
    if (language)
    {
        CurrentLanguage = WwiseDBSharedLanguageId(language->LanguageId(), InLanguageName, WwiseDBLanguageRequirement::IsOptional);
    }
}

/*
    SoundBank
*/

const WwiseSoundBankGlobalIdsMap GetCurrentPlatformSoundBanks()
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].SoundBanks;
    }
    return WwiseSoundBankGlobalIdsMap();
}

const WwiseCRefSoundBank* GetAllSoundBanksRef()
{
    WwiseCRefSoundBank* allSoundBanks = new WwiseCRefSoundBank[GetCurrentPlatformSoundBanks().Size()];
    int index = 0;
    WwiseSoundBankGlobalIdsMap SoundBanks = GetCurrentPlatformSoundBanks();
    for (auto const& SoundBank : SoundBanks)
    {
        FillSoundBank(SoundBank.second, allSoundBanks[index]);
        ++index;
    }
    return allSoundBanks;
}

const unsigned int GetSoundBankCount()
{
    return GetCurrentPlatformSoundBanks().Size();
}

const WwiseCRefSoundBank* GetSoundBankRefIndex(void* InSoundBanksArrayRefPtr, int index)
{
    WwiseCRefSoundBank* result = nullptr;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBanksArrayRefPtr;
    
    if (cRef)
    {
        if ((unsigned int)index < GetCurrentPlatformSoundBanks().Size())
        {
            result = &cRef[index];
        }
    }
    return result;
}

void DeleteSoundBanksArrayRef(void* InSoundBanksArrayRefPtr)
{
    WwiseCRefSoundBank* arg1 = (WwiseCRefSoundBank*)0;
    arg1 = (WwiseCRefSoundBank*)InSoundBanksArrayRefPtr;
    {
        delete[] arg1;
    }
}


const WwiseCRefSoundBank* GetSoundBankRefString(const char* InName, const char* InType)
{
    return GetSoundBankRef(WwiseDBObjectInfo(WwiseDBGuid(), 0, InName, InType));
}

const char* GetSoundBankName(void* InSoundBankRefPtr)
{
    const char* result = nullptr;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->name;
    }
    return result;
}

const char* GetSoundBankPath(void* InSoundBankRefPtr)
{
    const char* result = nullptr;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->path;
    }
    return result;
}

const char* GetSoundBankLanguage(void* InSoundBankRefPtr)
{
    const char* result = nullptr;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->language;
    }
    return result;
}

const WwiseDBShortId GetSoundBankLanguageId(void* InSoundBankRefPtr)
{
    WwiseDBShortId result = -1;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->languageId;
    }
    return result;
}

const WwiseDBGuid* GetSoundBankGuid(void* InSoundBankRefPtr)
{
    const WwiseDBGuid* result = nullptr;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->guid;
    }
    return result;
}

const WwiseDBShortId GetSoundBankShortId(void* InSoundBankRefPtr)
{
    WwiseDBShortId result = -1;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->shortId;
    }
    return result;
}

 const bool IsUserBank(void* InSoundBankRefPtr)
{
    bool result = 0;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->bIsUserBank;
    }
    return result;
}

 const bool IsInitBank(void* InSoundBankRefPtr)
{
    bool result = 0;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->bIsInitBank;
    }
    return result;
}

 const bool IsSoundBankValid(void* InSoundBankRefPtr)
{
    bool result = 0;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->bIsValid;
    }
    return result;
}

 const WwiseCRefMedia* GetSoundBankMedia(void* InSoundBankRefPtr, int index)
{
    WwiseCRefMedia* result = nullptr;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;

    if (cRef)
    {
        if ((unsigned int)index < cRef->mediasCount)
        {
            result = &cRef->medias[index];
        }
    }
    return result;
}

 const unsigned int GetSoundBankMediasCount(void* InSoundBankRefPtr)
{
    unsigned int result = -1;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->mediasCount;
    }
    return result;
}

 const WwiseCRefEvent* GetSoundBankEvent(void* InSoundBankRefPtr, int index)
{
    WwiseCRefEvent* result = nullptr;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    
    if (cRef)
    {
        if ((unsigned int)index < cRef->eventsCount)
        {
            result = &cRef->events[index];
        }
    }
    return result;
}

 const unsigned int GetSoundBankEventsCount(void* InSoundBankRefPtr)
{
    unsigned int result = -1;
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)0;
    cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->eventsCount;
    }
    return result;
}


 void DeleteSoundBankRef(void* InSoundBankRefPtr)
{
    WwiseCRefSoundBank* arg1 = (WwiseCRefSoundBank*)0;
    arg1 = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    {
        delete arg1;
    }
}

/*
    Media
*/

 const WwiseMediaGlobalIdsMap GetCurrentPlatformMedias()
 {
     if (DataStructure.Platforms.Contains(CurrentPlatform))
     {
         return DataStructure.Platforms[CurrentPlatform].MediaFiles;
     }
     return WwiseMediaGlobalIdsMap();
 }

 const char* GetMediaName(void* InMediaRefPtr)
{
    const char* result = nullptr;
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)0;
    cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if(cRef)
    { 
        result = cRef->name;
    }

    return result;
}

 const char* GetMediaPath(void* InMediaRefPtr)
{
    const char* result = nullptr;
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)0;
    cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if (cRef)
    {
        result = cRef->path;
    }
    return result;
}

 const WwiseDBShortId GetMediaShortId(void* InMediaRefPtr)
{
    WwiseDBShortId result = -1;
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)0;
    cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if (cRef)
    {
        result = cRef->shortId;
    }
    return result;
}

 const char* GetMediaLanguage(void* InMediaRefPtr)
{
    const char* result = nullptr;
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)0;
    cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if (cRef)
    {
        result = cRef->Language;
    }
    return result;
}

 const bool GetMediaIsStreaming(void* InMediaRefPtr)
{
    bool result = false;
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)0;
    cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if (cRef)
    {
        result = cRef->bStreaming;
    }
    return result;
}

 const WwiseDBShortId GetMediaLocation(void* InMediaRefPtr)
{
    WwiseDBShortId result = -1;
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)0;
    cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if (cRef)
    {
        result = (WwiseDBShortId)cRef->Location;
    }
    return result;
}

 const char* GetMediaCachePath(void* InMediaRefPtr)
{
    const char* result = nullptr;
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)0;
    cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if (cRef)
    {
        result = cRef->CachePath;
    }
    return result;
}

 void DeleteMediaRef(void* InMediaRefPtr)
{
    WwiseCRefMedia* arg1 = (WwiseCRefMedia*)0;
    arg1 = (WwiseCRefMedia*)InMediaRefPtr;
    {
        delete arg1;
    }
}

/*
    Event
*/

 const WwiseEventGlobalIdsMap GetCurrentPlatformEvents()
 {
     if (DataStructure.Platforms.Contains(CurrentPlatform))
     {
         return DataStructure.Platforms[CurrentPlatform].Events;
     }
     return WwiseEventGlobalIdsMap();
 }

 const WwiseCRefEvent* GetEventRefString(const char* InString)
{
    return GetEventRef(WwiseDBObjectInfo(WwiseDBGuid(), 0, InString, ""));
}

 const char* GetEventName(void* InEventRefPtr)
{
    const char* result = nullptr;
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)0;
    cRef = (WwiseCRefEvent*)InEventRefPtr;
    if (cRef)
    {
        result = cRef->name;
    }
    return result;
}

 const char* GetEventPath(void* InEventRefPtr)
{
    const char* result = nullptr;
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)0;
    cRef = (WwiseCRefEvent*)InEventRefPtr;
    if (cRef)
    {
        result = cRef->path;
    }
    return result;
}

 const WwiseDBGuid* GetEventGuid(void* InEventRefPtr)
{
    const WwiseDBGuid* result = nullptr;
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)0;
    cRef = (WwiseCRefEvent*)InEventRefPtr;
    if (cRef)
    {
        result = cRef->guid;
    }
    return result;
}

 const WwiseDBShortId GetEventShortId(void* InEventRefPtr)
{
    WwiseDBShortId result = -1;
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)0;
    cRef = (WwiseCRefEvent*)InEventRefPtr;
    if (cRef)
    {
        result = cRef->shortId;
    }
    return result;
}

 const float GetEventMaxAttenuation(void* InEventRefPtr)
{
    float result = -1;
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)0;
    cRef = (WwiseCRefEvent*)InEventRefPtr;
    if (cRef)
    {
        result = cRef->maxAttenuation;
    }
    return result;
}

 const float GetEventMinDuration(void* InEventRefPtr)
 {
     float result = -1;
     WwiseCRefEvent* cRef = (WwiseCRefEvent*)0;
     cRef = (WwiseCRefEvent*)InEventRefPtr;
     if (cRef)
     {
         result = cRef->minDuration;
     }
     return result;
 }

 const float GetEventMaxDuration(void* InEventRefPtr)
 {
     float result = -1;
     WwiseCRefEvent* cRef = (WwiseCRefEvent*)0;
     cRef = (WwiseCRefEvent*)InEventRefPtr;
     if (cRef)
     {
         result = cRef->maxDuration;
     }
     return result;
 }

 const WwiseCRefMedia* GetEventMedia(void* InEventRefPtr, int index)
{
    WwiseCRefMedia* result = nullptr;
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)0;
    cRef = (WwiseCRefEvent*)InEventRefPtr;

    if (cRef)
    {
        if ((unsigned int)index < cRef->mediasCount)
        {
            result = &cRef->medias[index];
        }
    }
    return result;
}

 const unsigned int GetEventMediasCount(void* InEventRefPtr)
{
    unsigned int result = -1;
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)0;
    cRef = (WwiseCRefEvent*)InEventRefPtr;
    if (cRef)
    {
        result = cRef->mediasCount;
    }
    return result;
}

 void DeleteEventRef(void* InEventRefPtr)
{
    WwiseCRefEvent* arg1 = (WwiseCRefEvent*)0;
    arg1 = (WwiseCRefEvent*)InEventRefPtr;
    {
        delete arg1;
    }
}

const WwiseCRefPlatform* GetPlatformRef(const char* InString)
{
    auto platformRef = new WwiseCRefPlatform();
    for(auto platformName : DataStructure.RootData.PlatformNames)
    {
        if(*platformName.second.PlatformName() == InString)
        {
            platformRef->name = **platformName.second.PlatformName();
            platformRef->guid = platformName.second.PlatformGuid();
            return platformRef;
        }
    }
    return platformRef;
}

const char* GetPlatformName(void* InPlatformPtr)
{
    const char* result = nullptr;
    WwiseCRefPlatform* cRef = (WwiseCRefPlatform*)0;
    cRef = (WwiseCRefPlatform*)InPlatformPtr;
    if (cRef)
    {
        result = cRef->name;
    }
    return result;
}

const WwiseDBGuid* GetPlatformGuid(void* InPlatformPtr)
{
    const WwiseDBGuid* result = nullptr;
    WwiseCRefPlatform* cRef = (WwiseCRefPlatform*)0;
    cRef = (WwiseCRefPlatform*)InPlatformPtr;
    if (cRef)
    {
        result = cRef->guid;
    }
    return result;
}

void DeletePlatformRef(void* InPlatformPtr)
{
    WwiseCRefPlatform* arg1 = (WwiseCRefPlatform*)0;
    arg1 = (WwiseCRefPlatform*)InPlatformPtr;
    {
        delete arg1;
    }
}

const WwisePluginLibGlobalIdsMap GetCurrentPlatformPlugins()
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].PluginLibs;
    }
    return WwisePluginLibGlobalIdsMap();
}

const WwiseCRefPlugin* GetAllPluginRef()
{
    if(!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    WwiseCRefPlugin* allPlugins = new WwiseCRefPlugin[GetCurrentPlatformPlugins().Size()];
    int index = 0;
    WwisePluginLibGlobalIdsMap Plugins = GetCurrentPlatformPlugins();
    for (auto const& Plugin : Plugins)
    {
        FillPlugin(Plugin.second, allPlugins[index]);
        ++index;
    }
    return allPlugins;
}

const unsigned int GetPluginCount()
{
    return GetCurrentPlatformPlugins().Size();
}

const WwiseCRefPlugin* GetPluginRefIndex(void* InPluginArrayRefPtr, int index)
{
    WwiseCRefPlugin* result = nullptr;
    WwiseCRefPlugin* cRef = (WwiseCRefPlugin*)0;
    cRef = (WwiseCRefPlugin*)InPluginArrayRefPtr;

    if (cRef)
    {
        if ((unsigned int)index < GetCurrentPlatformPlugins().Size())
            
        {
            result = &cRef[index];
        }
    }
    return result;
}

const WwiseDBShortId GetPluginId(void* InPluginRefPtr)
{
    WwiseDBShortId result = -1;
    WwiseCRefPlugin* cRef = (WwiseCRefPlugin*)0;
    cRef = (WwiseCRefPlugin*)InPluginRefPtr;
    if (cRef)
    {
        result = cRef->id;
    }
    return result;
}

const char* GetPluginName(void* InPluginRefPtr)
{
    const char* result = nullptr;
    WwiseCRefPlugin* cRef = (WwiseCRefPlugin*)0;
    cRef = (WwiseCRefPlugin*)InPluginRefPtr;
    if (cRef)
    {
        result = cRef->name;
    }
    return result;
}

const char* GetPluginDLL(void* InPluginRefPtr)
{
    const char* result = nullptr;
    WwiseCRefPlugin* cRef = (WwiseCRefPlugin*)0;
    cRef = (WwiseCRefPlugin*)InPluginRefPtr;
    if (cRef)
    {
        result = cRef->dll;
    }
    return result;
}

const char* GetPluginStaticLib(void* InPluginRefPtr)
{
    const char* result = nullptr;
    WwiseCRefPlugin* cRef = (WwiseCRefPlugin*)0;
    cRef = (WwiseCRefPlugin*)InPluginRefPtr;
    if (cRef)
    {
        result = cRef->staticLib;
    }
    return result;
}

void DeletePluginRef(void* InPluginRefPtr)
{
    WwiseCRefPlugin* arg1 = (WwiseCRefPlugin*)0;
    arg1 = (WwiseCRefPlugin*)InPluginRefPtr;
    {
        delete arg1;
    }
}

void DeletePluginArrayRef(void* InPluginArrayRefPtr)
{
    WwiseCRefPlugin* arg1 = (WwiseCRefPlugin*)0;
    arg1 = (WwiseCRefPlugin*)InPluginArrayRefPtr;
    {
        delete[] arg1;
    }
}
