#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <string>
#include <unordered_map>
#include "SDL_mixer/SDL_mixer.h"

class AudioSystem
{
public:
    static Mix_Chunk* LoadClip(const std::string& clipName);

    static void Play(int channel, const std::string& clipName, bool doesLoop);

    static void Halt(int channel);

    static void SetVolume(int channel, float volume);

private:
    static std::unordered_map<std::string, Mix_Chunk*> s_clips;
};

#endif
