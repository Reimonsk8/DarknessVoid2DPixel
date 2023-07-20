#include <windows.h>
#include <string>
#include <iostream>
#include "Enemy.h"
#include "common.h"
#include "graphics.h"


Enemy::Enemy(std::string name, short ap, short hp, short flee)
    :mName(name),  mHP(hp), mAP(ap), mFlee(flee)
{

};
Enemy::~Enemy()
{

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
    hero.setHP(-(hero.getCurrentEnemy()->getAP()));//deal damage to player
    mLogContent.prepend(temp);
    mScrollLog->setText(mLogContent);

    if (hero.getHP() <= 0)// game over event
    {
        SoundEngine::PlaySoundByName("death", 1);
        mLogContent.prepend("YOU DIED!!! \n your body lies in a pool of blood while the enemy eat's your bones and flesh...");
        mScrollLog->setText(mLogContent);
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

    hero.getCurrentEnemy()->setHP(hero.getAP());//enemy hp - hero AP damage
    temp.append(QString::number(hero.getAP()));
    temp.append(" enemy now has ");
    temp.append(QString::number(hero.getCurrentEnemy()->getHP()));
    temp.append(" Health left \n");
    mLogContent.prepend(temp);
    mScrollLog->setText(mLogContent);

    if (hero.getCurrentEnemy()->getHP() <= 0)
    {
        SoundEngine::PlaySoundByName("win", 1);
        mLogContent.prepend( "enemy defeated, Victory achivied!!!\n");
        mScrollLog->setText(mLogContent);
		lvl.grid[hero.heroRow][hero.heroCol] = 'O';//remove enemy
        hero.getCurrentEnemy()->posX = OUT_OF_RANGE;
        hero.getCurrentEnemy()->posY = OUT_OF_RANGE;
        gState = S_Normal;
        gFlee = true;
	}
	else
        heroAtacked(hero);
};
