#include "WwiseAudioSystem.h"
#include <iostream>
#include <codecvt>

#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkMemoryMgrModule.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/Plugin/AkRoomVerbFXFactory.h>
#include <AK/Comm/AkCommunication.h>
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
        return false;

    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);
    if (!AK::StreamMgr::Create(stmSettings))
        return false;

    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

    if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
        return false;

    g_lowLevelIO.SetBasePath(L"resources/audio/");

    AkDeviceID newDevId = AK_INVALID_DEVICE_ID;
    if (AK::StreamMgr::CreateDevice(deviceSettings, &g_lowLevelIO, newDevId) != AK_Success)
        return false;

    s_deviceID = newDevId;
    AK::StreamMgr::SetFileLocationResolver(&g_lowLevelIO);

    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

    if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
        return false;

    AkMusicSettings musicInit;
    AK::MusicEngine::GetDefaultInitSettings(musicInit);
    if (AK::MusicEngine::Init(&musicInit) != AK_Success)
        return false;

#ifdef _DEBUG
    {
        AkCommSettings commSettings;
        AK::Comm::GetDefaultInitSettings(commSettings);
        AK::Comm::Init(commSettings);
    }
#endif

    AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));

    s_isInitialized = true;
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
}

void WwiseAudioSystem::Update()
{
    if (s_isInitialized)
        AK::SoundEngine::RenderAudio();
}

bool WwiseAudioSystem::LoadBank(const std::string& bankName)
{
    if (!s_isInitialized)
        return false;

    AkBankID bankID;
    std::wstring widePath = std::wstring(L"banks/") + std::wstring(bankName.begin(), bankName.end());

    return AK::SoundEngine::LoadBank(widePath.c_str(), bankID) == AK_Success;
}

void WwiseAudioSystem::PostEvent(const std::string& eventName, AkGameObjectID gameObjId)
{
    if (s_isInitialized)
        AK::SoundEngine::PostEvent(eventName.c_str(), gameObjId);
}

void WwiseAudioSystem::RegisterGameObj(AkGameObjectID gameObjId, const std::string& name)
{
    if (s_isInitialized)
        AK::SoundEngine::RegisterGameObj(gameObjId, name.c_str());
}

void WwiseAudioSystem::UnregisterGameObj(AkGameObjectID gameObjId)
{
    if (s_isInitialized)
        AK::SoundEngine::UnregisterGameObj(gameObjId);
}

void WwiseAudioSystem::SetRTPCValue(const std::string& rtpcName, float value, AkGameObjectID gameObjId)
{
    if (s_isInitialized)
        AK::SoundEngine::SetRTPCValue(rtpcName.c_str(), value, gameObjId);
}

void WwiseAudioSystem::SetSwitch(const std::string& switchGroup, const std::string& switchState, AkGameObjectID gameObjId)
{
    if (s_isInitialized)
        AK::SoundEngine::SetSwitch(switchGroup.c_str(), switchState.c_str(), gameObjId);
}

void WwiseAudioSystem::SetState(const std::string& group, const std::string& state)
{
    if (s_isInitialized)
        AK::SoundEngine::SetState(group.c_str(), state.c_str());
}

void WwiseAudioSystem::SetGameObjectPosition(AkGameObjectID id,
    float           x,
    float           y,
    float           z)
{
    if (!s_isInitialized)
        return;

#if (AK_WWISESDK_VERSION_MAJOR >= 2022)
    AkTransform tr;
    tr.SetPosition(x, y, z);
    tr.SetOrientation(0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
    AK::SoundEngine::SetPosition(id, tr);

#else
    AkSoundPosition pos;
    pos.Position().Set(x, y, z);
    pos.OrientationFront().Set(0.f, 1.f, 0.f);
    pos.OrientationTop().Set(0.f, 0.f, 1.f);
    AK::SoundEngine::SetPosition(id, pos);
#endif
}

void WwiseAudioSystem::SetListener(AkGameObjectID id)
{
    if (!s_isInitialized)
        return;

    AK::SoundEngine::SetDefaultListeners(&id, 1);
}
