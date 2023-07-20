#ifndef SOUNDENGINE_H
#define SOUNDENGINE_H

#include <QMediaPlayer>
#include <QAudioOutput>

class SoundEngine
{
public:
    SoundEngine();

    static void PlaySoundByName(std::string name, float vol);

private:

};

#endif // SOUNDENGINE_H
