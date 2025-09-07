#include <windows.h>
#include <string>
#include <iostream>
#include "Enemy.h"
#include "common.h"
#include "graphics.h"
#include "../Graphics.h"
#include <QtDebug>


Enemy::Enemy(std::string name, short ap, short hp, short flee)
    :mName(name),  mHP(hp), mAP(ap), mFlee(flee)
{

};
Enemy::~Enemy()
{
    qDebug() << "Enemy destructor called for:" << QString::fromStdString(mName);
};
std::string Enemy::getName()
{
	return mName;
};

short Enemy::getHP()
{
	return mHP;
};

void Enemy::setHP(short damage)
{
	mHP = mHP - damage;
};

short Enemy::getAP()
{
	return mAP;
};

bool Enemy::tryFlee()
{
    short random_value = rand() % 13 + 1;
	if (random_value > mFlee)
		return true;
	else return false;
}

void Enemy::heroAtacked(Character &hero)
{
    QString temp;
    temp.append("Enemy ");
    temp.append(QString::fromStdString(hero.getCurrentEnemy()->getName()));
    temp.append(" atacked you dealing ");
    temp.append(QString::number(hero.getCurrentEnemy()->getAP()));
    temp.append(" damage\n");
    SoundEngine::PlaySoundByName("claw", 1);
    SoundEngine::PlaySwordSwing(); // Additional impact sound
    hero.setHP(-(hero.getCurrentEnemy()->getAP()));//deal damage to player
    
    // Trigger hero damage flash animation and screen shake
    if (Graphics::instance && Graphics::instance->scene()) {
        Graphics::instance->startHeroDamageFlash();
        Graphics::instance->startSpriteShake(8, 300); // Strong screen shake for hero taking damage
    }
    
    // Use styled log system
    addStyledLogEntry(temp, false);

    if (hero.getHP() <= 0)// game over event
    {
        SoundEngine::PlaySoundByName("death", 1);
        addStyledLogEntry("YOU DIED!!! \n your body lies in a pool of blood while the enemy eat's your bones and flesh...", true);
        gState = S_GameOver;
    }
}

void Enemy::enemyAtacked(Character &hero)
{
    hero.enemyDamaged = true;

    QString temp;
    temp.append("you atacked ");
    temp.append(QString::fromStdString(hero.getCurrentEnemy()->getName()));
    temp.append(" using your ");
    temp.append(QString::fromStdString( hero.selectItem(2).getName()));
    temp.append(" dealing ");
    temp.append(QString::number(hero.getAP()));
    temp.append(" damage ");
    temp.append(QString::fromStdString( hero.selectItem(2).getName()));
    temp.append(" dealing ");

    SoundEngine::PlaySoundByName("hit", 1);
    SoundEngine::PlayCriticalHit(); // Enhanced hit sound

    hero.getCurrentEnemy()->setHP(hero.getAP());//enemy hp - hero AP damage
    
    // Trigger enemy damage flash animation and screen shake
    qDebug() << "About to trigger enemy damage animations...";
    if (Graphics::instance && Graphics::instance->scene()) {
        qDebug() << "Graphics instance and scene are valid, calling animations...";
        qDebug() << "Calling startEnemyDamageFlash...";
        Graphics::instance->startEnemyDamageFlash(hero.heroRow, hero.heroCol);
        qDebug() << "startEnemyDamageFlash completed, calling startSpriteShake...";
        Graphics::instance->startSpriteShake(5, 200); // Medium screen shake for enemy taking damage
        qDebug() << "startSpriteShake completed";
    } else {
        qDebug() << "Graphics instance or scene is null, skipping animations";
    }
    qDebug() << "Enemy damage animations completed";
    
    temp.append(QString::number(hero.getAP()));
    temp.append(" enemy now has ");
    temp.append(QString::number(hero.getCurrentEnemy()->getHP()));
    temp.append(" Health left \n");
    
    // Use styled log system
    addStyledLogEntry(temp, false);

    if (hero.getCurrentEnemy()->getHP() <= 0)
    {
        SoundEngine::PlaySoundByName("win", 1);
        addStyledLogEntry("enemy defeated, Victory achivied!!!\n", true);
		lvl.grid[hero.heroRow][hero.heroCol] = 'O';//remove enemy
        hero.getCurrentEnemy()->posX = OUT_OF_RANGE;
        hero.getCurrentEnemy()->posY = OUT_OF_RANGE;
        gState = S_Normal;
        gFlee = true;
	}
	else
        heroAtacked(hero);
};
