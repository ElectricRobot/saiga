#pragma once

#include <string>
#include <vector>
#include <map>

#include <saiga/sound/OpenAL.h>
#include <saiga/util/glm.h>
#include <saiga/sound/SoundSource.h>

namespace sound {
class SoundSource;
class Sound;



class SAIGA_GLOBAL SoundManager
{
private:
    SoundSource* quietSoundSource;
    std::vector<SoundSource> sources;
    int sourceIndex = 0;

    std::map<std::string,Sound*> soundMap;

    float masterVolume = 1.0f;
    float musicVolume = 1.f;
    float effectsVolume = 1.f;

    bool muted = false;
    int maxSources, fixedSources;
    int oldestSource = 0;
public:

    SoundManager (int maxSources, int fixedSources=0);
    virtual ~SoundManager ();

    void setListenerPosition(const vec3& pos);
    void setListenerVelocity(const vec3& velocity);
    void setListenerOrientation(const vec3& at, const vec3& up);
    void setListenerGain(float g);
    void setMusicVolume(float v);
    void setEffectsVolume(float v);

    void setMute(bool b);

    void setTimeScale(float scale);


    SoundSource *getSoundSource(const std::string &file, bool isMusic = false);
    SoundSource *getFixedSoundSource(const std::string &file, int id, bool isMusic = false);
    SoundSource *getFixedSoundSource(int id);

    void loadWaveSound(const std::string &file);
    void loadOpusSound(const std::string &file);





};


}

