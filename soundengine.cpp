#include "SoundEngine.h"
#include <QDebug>

// Static member initialization
SoundEngine* SoundEngine::instance = nullptr;
QMediaPlayer* SoundEngine::musicPlayer = nullptr;
QMap<QString, QMediaPlayer*> SoundEngine::soundPlayers;
float SoundEngine::masterVolume = 1.0f;
float SoundEngine::musicVolume = 0.7f;
float SoundEngine::sfxVolume = 1.0f;

SoundEngine::SoundEngine()
{
    if (!instance) {
        instance = this;
        initializeSoundPlayers();
    }
}

SoundEngine::~SoundEngine()
{
    cleanupSoundPlayers();
}

void SoundEngine::initializeSoundPlayers()
{
    // Initialize music player
    musicPlayer = new QMediaPlayer();
    QAudioOutput* musicOutput = new QAudioOutput();
    musicPlayer->setAudioOutput(musicOutput);
    musicOutput->setVolume(musicVolume * masterVolume);
    
    // Pre-load common sound effects
    QStringList soundFiles = {
        "step", "hit", "claw", "battle", "victory", "gameover", 
        "item", "powerup", "death", "flee", "ok", "cancel"
    };
    
    for (const QString& soundFile : soundFiles) {
        QMediaPlayer* player = new QMediaPlayer();
        QAudioOutput* output = new QAudioOutput();
        player->setAudioOutput(output);
        output->setVolume(sfxVolume * masterVolume);
        
        QString file = QString("qrc:/Sounds/%1.wav").arg(soundFile);
        player->setSource(QUrl(file));
        
        soundPlayers[soundFile] = player;
    }
}

void SoundEngine::cleanupSoundPlayers()
{
    // Clean up music player
    if (musicPlayer) {
        musicPlayer->stop();
        delete musicPlayer->audioOutput();
        delete musicPlayer;
        musicPlayer = nullptr;
    }
    
    // Clean up sound players
    for (auto it = soundPlayers.begin(); it != soundPlayers.end(); ++it) {
        QMediaPlayer* player = it.value();
        if (player) {
            player->stop();
            delete player->audioOutput();
            delete player;
        }
    }
    soundPlayers.clear();
}

void SoundEngine::PlaySoundByName(std::string name, float vol)
{
    try {
        QString soundName = QString::fromStdString(name);
        
        if (soundPlayers.contains(soundName)) {
            QMediaPlayer* player = soundPlayers[soundName];
            if (player) {
                player->setPosition(0);
                player->play();
            }
        } else {
            // Fallback to old method for sounds not in pre-loaded list
            QString file = QString::fromStdString("qrc:/Sounds/" + name + ".wav");
            QMediaPlayer *Player = new QMediaPlayer();
            QAudioOutput *AudioOutput = new QAudioOutput;
            Player->setAudioOutput(AudioOutput);
            AudioOutput->setVolume(vol * sfxVolume * masterVolume);
            Player->setSource(QUrl(file));
            Player->play();
            
            // Auto-cleanup after playing
            QTimer::singleShot(5000, [Player, AudioOutput]() {
                try {
                    Player->stop();
                    delete AudioOutput;
                    delete Player;
                } catch (...) {
                    qDebug() << "Exception during sound cleanup";
                }
            });
        }
    } catch (...) {
        qDebug() << "Exception in PlaySoundByName:" << QString::fromStdString(name);
    }
}

void SoundEngine::PlayMusicByName(std::string name, float vol)
{
    try {
        if (!musicPlayer) return;
        
        QString file = QString::fromStdString("qrc:/Sounds/" + name + ".wav");
        musicPlayer->setSource(QUrl(file));
        musicPlayer->audioOutput()->setVolume(vol * musicVolume * masterVolume);
        musicPlayer->play();
    } catch (...) {
        qDebug() << "Exception in PlayMusicByName:" << QString::fromStdString(name);
    }
}

void SoundEngine::StopMusic()
{
    if (musicPlayer) {
        musicPlayer->stop();
    }
}

void SoundEngine::SetMasterVolume(float volume)
{
    masterVolume = qBound(0.0f, volume, 1.0f);
    
    // Update music volume
    if (musicPlayer && musicPlayer->audioOutput()) {
        musicPlayer->audioOutput()->setVolume(musicVolume * masterVolume);
    }
    
    // Update all sound effect volumes
    for (auto it = soundPlayers.begin(); it != soundPlayers.end(); ++it) {
        QMediaPlayer* player = it.value();
        if (player && player->audioOutput()) {
            player->audioOutput()->setVolume(sfxVolume * masterVolume);
        }
    }
}

void SoundEngine::SetMusicVolume(float volume)
{
    musicVolume = qBound(0.0f, volume, 1.0f);
    if (musicPlayer && musicPlayer->audioOutput()) {
        musicPlayer->audioOutput()->setVolume(musicVolume * masterVolume);
    }
}

void SoundEngine::SetSFXVolume(float volume)
{
    sfxVolume = qBound(0.0f, volume, 1.0f);
    for (auto it = soundPlayers.begin(); it != soundPlayers.end(); ++it) {
        QMediaPlayer* player = it.value();
        if (player && player->audioOutput()) {
            player->audioOutput()->setVolume(sfxVolume * masterVolume);
        }
    }
}

// Enhanced sound effects
void SoundEngine::PlayFootstep()
{
    PlaySoundByName("step", 0.8f);
}

void SoundEngine::PlaySwordSwing()
{
    PlaySoundByName("hit", 1.2f);
}

void SoundEngine::PlayMagicCast()
{
    PlaySoundByName("powerup", 1.0f);
}

void SoundEngine::PlayItemPickup()
{
    PlaySoundByName("item", 1.0f);
}

void SoundEngine::PlayLevelUp()
{
    PlaySoundByName("powerup", 1.5f);
}

void SoundEngine::PlayCriticalHit()
{
    PlaySoundByName("hit", 1.5f);
}

void SoundEngine::PlayDodgeSound()
{
    PlaySoundByName("flee", 0.7f);
}
