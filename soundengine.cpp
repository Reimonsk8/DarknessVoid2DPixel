#include "SoundEngine.h"

SoundEngine::SoundEngine()
{

}

void SoundEngine::PlaySoundByName(std::string name, float vol = 1.0)
{

    QString file = QString::fromStdString("qrc:/Sounds/" + name + ".wav");

    QMediaPlayer *Player = new QMediaPlayer();
    QAudioOutput *AudioOutput = new QAudioOutput;
    Player->setAudioOutput(AudioOutput);
    AudioOutput->setVolume(vol);

    Player->setSource(QUrl(file));
    if(Player->playbackState() == QMediaPlayer::PlayingState){
        Player->setPosition(0);
        Player->stop();
    }else if(Player->playbackState() == QMediaPlayer::StoppedState){
        Player->play();
    };
}
