#ifndef WWISEAUDIOSYSTEM_H
#define WWISEAUDIOSYSTEM_H

#include <string>
#include <AK/SoundEngine/Common/AkTypes.h>

class WwiseAudioSystem
{
public:
    static bool Init();
    static void Shutdown();
    static void Update();

    static bool LoadBank(const std::string& bankName);

    static void PostEvent(const std::string& eventName, AkGameObjectID gameObjId);

    static void RegisterGameObj(AkGameObjectID gameObjId, const std::string& name);
    static void UnregisterGameObj(AkGameObjectID gameObjId);

    static void SetRTPCValue(const std::string& rtpcName, float value, AkGameObjectID gameObjId);
    static void SetSwitch(const std::string& switchGroup, const std::string& switchState, AkGameObjectID gameObjId);

    static void SetState(const std::string& group, const std::string& state);
    static void SetGameObjectPosition(AkGameObjectID id, float x, float y, float z = 0.0f);
    static void SetListener(AkGameObjectID id);

private:
    static bool s_isInitialized;
};

#endif
