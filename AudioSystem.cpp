#include "AudioSystem.h"
#include "AudioHelper.h"
#include <iostream>
#include <filesystem>

std::unordered_map<std::string, Mix_Chunk*> AudioSystem::s_clips;

Mix_Chunk* AudioSystem::LoadClip(const std::string& clipName)
{
    auto it = s_clips.find(clipName);
    if (it != s_clips.end())
    {
        return it->second;
    }

    std::string wavPath = "resources/audio/" + clipName + ".wav";
    std::string oggPath = "resources/audio/" + clipName + ".ogg";

    std::string path;
    if (std::filesystem::exists(wavPath))
    {
        path = wavPath;
    }
    else if (std::filesystem::exists(oggPath))
    {
        path = oggPath;
    }
    else
    {
        std::cout << "AudioSystem: missing file "
            << wavPath << " or " << oggPath;
        return nullptr;
    }

    Mix_Chunk* c = AudioHelper::Mix_LoadWAV(path.c_str());
    if (!c)
    {
        std::cout << "AudioSystem: error loading " << path;
        return nullptr;
    }

    s_clips[clipName] = c;
    return c;
}

void AudioSystem::Play(int channel, const std::string& clipName, bool doesLoop)
{
    Mix_Chunk* c = LoadClip(clipName);
    if (!c) return;

    int loops = doesLoop ? -1 : 0;
    AudioHelper::Mix_PlayChannel(channel, c, loops);
}

void AudioSystem::Halt(int channel)
{
    AudioHelper::Mix_HaltChannel(channel);
}

void AudioSystem::SetVolume(int channel, float volume)
{
    int volInt = static_cast<int>(volume);
    if (volInt < 0) volInt = 0;
    if (volInt > 128) volInt = 128;
    AudioHelper::Mix_Volume(channel, volInt);
}
