#include "Armor.h"
#include "Equipment.h"
#include "graphics.h"
#include "common.h"
#include <string>
#include <time.h>
#include "Character.h"

Armor::Armor(std::string name, int ap, int maxhp, bool fixed) : Equipment(name, ap, maxhp, fixed)
{
	mType = T_Chest;
	if (!fixed)	//increase AP & maxHP based on type and rarity
	{
        mMaxHP = mMaxHP + ((mRandomValue * 2) + (10 * (mRandomValue / 4)));
		if (!(mRarity == "common"))
            mAP = mAP + (3 * mRandomValue / 6);
	}
}

Armor::~Armor()
{}

void Armor::pickArmor(Armor *current, Character &hero)
{
    QString temp;
    temp.append(QString::fromStdString(current->getName()));
    temp.append(" found +maxHP: ");
    temp.append(QString::number(current->getMaxHP()));
    temp.append(" replace Armor??\n");
    addStyledLogEntry(temp, false);
    if (gState == S_ActionToPickItem && gValueButton == B_Atack)
    {
        addStyledLogEntry(" Armor taken\n", false);
		hero.addToInventory(*current);
        lvl.grid[hero.heroRow][hero.heroCol] = 'O';
        gState = S_Normal;
	}
    //delete current;
}

