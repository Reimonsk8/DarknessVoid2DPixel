#ifndef SOUNDENGINE_H
#define SOUNDENGINE_H

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMap>
#include <QTimer>

class SoundEngine
{
public:
    SoundEngine();
    ~SoundEngine();

    static void PlaySoundByName(std::string name, float vol = 1.0);
    static void PlayMusicByName(std::string name, float vol = 0.7);
    static void StopMusic();
    static void SetMasterVolume(float volume);
    static void SetMusicVolume(float volume);
    static void SetSFXVolume(float volume);
    
    // Enhanced sound effects
    static void PlayFootstep();
    static void PlaySwordSwing();
    static void PlayMagicCast();
    static void PlayItemPickup();
    static void PlayLevelUp();
    static void PlayCriticalHit();
    static void PlayDodgeSound();

private:
    static SoundEngine* instance;
    static QMediaPlayer* musicPlayer;
    static QMap<QString, QMediaPlayer*> soundPlayers;
    static float masterVolume;
    static float musicVolume;
    static float sfxVolume;
    
    static void initializeSoundPlayers();
    static void cleanupSoundPlayers();
};

#endif // SOUNDENGINE_H
