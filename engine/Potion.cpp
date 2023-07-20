#include <windows.h>
#include "common.h"
#include "Potion.h"
#include "Equipment.h"
#include <QSoundEffect>
#include <iostream>
#include <string>
#include <time.h>

Potion::Potion(std::string name, int ap, int maxhp, bool fixed) : Equipment(name, ap, maxhp, fixed)
{
	mType = T_Consumable;
	if (!fixed)	//increase AP & maxHP based on type and rarity
	{
        mMaxHP = mMaxHP + (2 * mRandomValue)+ (5 * (mRandomValue / 3));
		if (!(mRarity == "common"))
            mAP = mAP + (1 * mRandomValue / 4);
	}
};

Potion::~Potion()
{}

void Potion::pickPotion(Potion *current, Character &hero)
{
    QString temp;
    temp.append(QString::fromStdString(current->getName()));
    temp.append(" found +HP: ");
    temp.append(QString::number(current->getMaxHP()));
    if (hero.inventorySize() >= 8) // inventory full?
        temp.append(" inventory full, replace Potion ?? \n");
	else
        temp.append(" take Potion ?? \n");
    mLogContent.prepend(temp);
    mScrollLog->setText(mLogContent);
    if (gState == S_ActionToPickItem && gValueButton == B_Atack)
	{
        mLogContent.prepend(+"Potion taken \n");
        mScrollLog->setText(mLogContent);
		hero.addToInventory(*current);
        lvl.grid[hero.heroRow][hero.heroCol] = 'O';
        gState = S_Normal;
	}
}


void Potion::usePotion(Character &hero)
{
	if (hero.inventorySize() <= 3)// if no potions
	{
        mLogContent.prepend("so dumb, you forgot that you dont have any potions left\n");
        mScrollLog->setText(mLogContent);
	}
	else
	{
		int inputNum;
        if (gValueButton >= P_1 && gValueButton <= P_5)//if accesesd by keys
            inputNum = (gValueButton - 48);
        else //input by text
        {
            inputNum = 1;
        }
        if ((inputNum + 3) <= hero.inventorySize() && (inputNum + 3) >= 4)//validate potion number
        {
            int potionHP = hero.selectItem(inputNum + 2).getMaxHP();//potion healing points
            int maxHP = hero.getMaxHP();//maxhp hero can heal
            int HP = hero.getHP();// initial hero hp
            hero.setAP(hero.getAP() + hero.selectItem(inputNum + 2).getAP());//increase ap if rare item
            /*heal 1 point until hero hp = inital hp  + potion hp or max hp reached*/
            for (int heal = HP; (hero.getHP() < (HP + potionHP)) && (hero.getHP() < maxHP); ++heal)
                hero.setHP(1);
            //QSound::play(gShortcut+"\\Sounds\\powerup.wav");
            std::cout << hero.getHP() << std::endl;
            QString temp;
            temp.append(QString::fromStdString(hero.selectItem(inputNum + 2).getName()));
            temp.append(" used!! hero hp now is ");
            temp.append(QString::number(hero.getHP()));
            mLogContent.prepend(temp);
            mScrollLog->setText(mLogContent);
            hero.removeFromInventory(inputNum + 2);
        }
        else
        {
            mLogContent.prepend("that potion slot is empty\n");
            mScrollLog->setText(mLogContent);
        }
    }
};
