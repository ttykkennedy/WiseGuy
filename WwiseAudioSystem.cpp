#include "WwiseAudioSystem.h"
#include <iostream>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkMemoryMgrModule.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/Plugin/AkRoomVerbFXFactory.h>
#include <AK/Comm/AkCommunication.h>
#include <codecvt>
#include <AkDefaultIOHookDeferred.h>

static CAkDefaultIOHookDeferred g_lowLevelIO;

bool WwiseAudioSystem::s_isInitialized = false;
static AkDeviceID s_deviceID = AK_INVALID_DEVICE_ID;

bool WwiseAudioSystem::Init()
{
    if (s_isInitialized)
        return true;

    AkMemSettings memSettings;
    AK::MemoryMgr::GetDefaultSettings(memSettings);
    if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
    {
        std::cerr << "[Wwise] Failed to initialize MemoryMgr.\n";
        return false;
    }

    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);
    if (!AK::StreamMgr::Create(stmSettings))
    {
        std::cerr << "[Wwise] Failed to create streaming manager.\n";
        return false;
    }

    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

    AKRESULT ioRes = g_lowLevelIO.Init(deviceSettings);
    if (ioRes != AK_Success)
    {
        std::cerr << "[Wwise] Failed to Init default IO hook. Error = " << ioRes << "\n";
        return false;
    }

    g_lowLevelIO.SetBasePath(L"resources/audio/");

    AkDeviceID newDevId = AK_INVALID_DEVICE_ID;
    AKRESULT res = AK::StreamMgr::CreateDevice(deviceSettings, &g_lowLevelIO, newDevId);
    if (res != AK_Success)
    {
        std::cerr << "[Wwise] Failed to create streaming device.\n";
        return false;
    }
    s_deviceID = newDevId;

    AK::StreamMgr::SetFileLocationResolver(&g_lowLevelIO);

    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

    if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
    {
        std::cerr << "[Wwise] Failed to initialize SoundEngine.\n";
        return false;
    }

    AkMusicSettings musicInit;
    AK::MusicEngine::GetDefaultInitSettings(musicInit);
    if (AK::MusicEngine::Init(&musicInit) != AK_Success)
    {
        std::cerr << "[Wwise] Failed to initialize MusicEngine.\n";
        return false;
    }

#ifdef _DEBUG
    {
        AkCommSettings commSettings;
        AK::Comm::GetDefaultInitSettings(commSettings);
        if (AK::Comm::Init(commSettings) != AK_Success)
        {
            std::cerr << "[Wwise] Failed to initialize Communication.\n";
        }
    }
#endif

    AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));

    s_isInitialized = true;
    std::cout << "[Wwise] Initialization complete.\n";
    return true;
}

void WwiseAudioSystem::Shutdown()
{
    if (!s_isInitialized)
        return;

    AK::SoundEngine::RenderAudio();

#ifdef _DEBUG
    AK::Comm::Term();
#endif
    AK::MusicEngine::Term();
    AK::SoundEngine::Term();

    g_lowLevelIO.Term();

    if (AK::IAkStreamMgr* pMgr = AK::IAkStreamMgr::Get())
    {
        if (s_deviceID != AK_INVALID_DEVICE_ID)
        {
            AK::StreamMgr::DestroyDevice(s_deviceID);
            s_deviceID = AK_INVALID_DEVICE_ID;
        }
        pMgr->Destroy();
    }

    AK::MemoryMgr::Term();

    s_isInitialized = false;
    std::cout << "[Wwise] Shutdown complete.\n";
}

void WwiseAudioSystem::Update()
{
    if (s_isInitialized)
    {
        AK::SoundEngine::RenderAudio();
    }
}

bool WwiseAudioSystem::LoadBank(const std::string& bankName)
{
    if (!s_isInitialized)
        return false;

    AkBankID bankID;
    std::string path = "banks/" + bankName;

    std::wstring widePath(path.begin(), path.end());

    AKRESULT result = AK::SoundEngine::LoadBank(widePath.c_str(), bankID);
    if (result != AK_Success)
    {
        std::cerr << "[Wwise] Failed to load bank " << path << ".\n";
        return false;
    }
    std::cout << "[Wwise] Loaded bank " << path << "\n";
    return true;
}

void WwiseAudioSystem::PostEvent(const std::string& eventName, AkGameObjectID gameObjId)
{
    if (!s_isInitialized)
        return;

    AK::SoundEngine::PostEvent(eventName.c_str(), gameObjId);
}

void WwiseAudioSystem::RegisterGameObj(AkGameObjectID gameObjId, const std::string& name)
{
    if (!s_isInitialized)
        return;

    AKRESULT res = AK::SoundEngine::RegisterGameObj(gameObjId, name.c_str());
    if (res != AK_Success)
    {
        std::cerr << "[Wwise] Warning: Cannot register game object " << name
            << " (ID: " << gameObjId << ")!\n";
    }
}

void WwiseAudioSystem::UnregisterGameObj(AkGameObjectID gameObjId)
{
    if (!s_isInitialized)
        return;

    AK::SoundEngine::UnregisterGameObj(gameObjId);
}

void WwiseAudioSystem::SetRTPCValue(const std::string& rtpcName, float value, AkGameObjectID gameObjId)
{
    if (!s_isInitialized)
        return;

    AK::SoundEngine::SetRTPCValue(rtpcName.c_str(), value, gameObjId);
}

void WwiseAudioSystem::SetSwitch(const std::string& switchGroup, const std::string& switchState, AkGameObjectID gameObjId)
{
    if (!s_isInitialized)
        return;

    AK::SoundEngine::SetSwitch(switchGroup.c_str(), switchState.c_str(), gameObjId);
}
