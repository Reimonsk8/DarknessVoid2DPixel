#include "Equipment.h"
#include <iostream>
#include <string>
#include <time.h>

Equipment::Equipment(std::string name, short ap, short maxhp, bool fixed)
	:mAP(ap), mMaxHP(maxhp), legendarySoundPlayed(false)
{
	if (fixed)
	{
        mName = name;
		mRarity = "common";
        mRandomValue = 1;
	}
	else
	{
        mRandomValue = rand() % 13 + 1;
        if (mRandomValue >= 12) {
            mRarity = "legendary ";
        } else if (mRandomValue >= 8) {
            mRarity = "rare ";
        } else {
            mRarity = "common";
        }

        if (!(mRarity == "common")) {
            mName = mRarity + name;
        } else {
            mName = name;
        }
	}
}

Equipment::~Equipment()
{}

short Equipment::getType()
{
	return mType;
}

std::string Equipment::getName()
{
	return mName;
}
void Equipment::setAP(short ap)
{
	mAP = ap;
}

short Equipment::getAP()
{
	return mAP;
}

void Equipment::setMaxHP(short maxhp)
{
	mMaxHP = maxhp;
}

short Equipment::getMaxHP()
{
	return mMaxHP;
}

