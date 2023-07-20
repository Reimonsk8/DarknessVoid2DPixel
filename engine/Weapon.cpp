#include "Weapon.h"
#include "Equipment.h"
#include "Common.h"
#include "graphics.h"
#include <string>
#include <time.h>

Weapon::Weapon(std::string name, int ap, int maxhp, bool fixed) : Equipment(name, ap, maxhp, fixed)
{
	mType = T_Weapon;
	if (!fixed)	//increase AP & maxHP based on type and rarity
	{
        mAP = mAP + (5 * (mRandomValue / 4));
		if (!(mRarity == "common"))
            mMaxHP = mMaxHP + (5 * mRandomValue / 6);
	}
}

Weapon::~Weapon()
{}

void Weapon::pickWeapon(Weapon *current, Character &hero)
{
    QString temp;
    temp.append(QString::fromStdString(current->getName()));
    temp.append(" found +maxAP: ");
    temp.append(QString::number(current->getAP()));
    temp.append(" replace Weapon??\n");
    mLogContent.prepend(temp);
    mScrollLog->setText(mLogContent);

    if (gState == S_ActionToPickItem && gValueButton == B_Atack)
	{
        mLogContent.prepend("Weapon taken\n");
        mScrollLog->setText(mLogContent);
		hero.addToInventory(*current);
        lvl.grid[hero.heroRow][hero.heroCol] = 'O';
        gState = S_Normal;
	}
}

