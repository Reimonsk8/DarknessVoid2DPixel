#include "Equipment.h"
#include "Helmet.h"
#include "common.h"
#include "graphics.h"
#include <iostream>
#include <string>
#include <time.h>


Helmet::Helmet(std::string name, int ap, int maxhp, bool fixed) : Equipment(name, ap, maxhp, fixed)
{
	mType = T_Head;
	if (!fixed)	//increase AP & maxHP based on type and rarity
	{
        mMaxHP = mMaxHP + ((mRandomValue * 1) + (5 * (mRandomValue / 4)));
		if (!(mRarity == "common"))
            mAP = mAP + (2 * mRandomValue / 6);
	}
}

Helmet::~Helmet()
{}

void Helmet::pickHelmet(Helmet *current, Character &hero)
{   
    QString temp;
    temp.append(QString::fromStdString(current->getName()));
    temp.append(" found +maxHP: ");
    temp.append(QString::number(current->getMaxHP()));
    temp.append(" replace Helmet??\n");
    mLogContent.prepend(temp);
    mScrollLog->setText(mLogContent);
    if (gState == S_ActionToPickItem && gValueButton == B_Atack)
    {
        mLogContent.prepend(temp);
        mScrollLog->setText(" Helmet taken\n");
		hero.addToInventory(*current);
		lvl.grid[hero.heroRow][hero.heroCol] = 'O';
        gState = S_Normal;
	}
}
