#include <string>
#include <iostream>
#include <algorithm>
#include "Character.h"
#include "common.h"
#include "Equipment.h"

Character::Character()
    :mCurrentEnemy(nullptr)
{
    for(int row = 0; row < gHeight; ++row)
    {
        for(int col = 0; col < gWidth; ++col)
        {
            if(lvl.grid[row][col] == L_S)
            {
                heroRow = row;
                heroCol = col;
            }
        }
    }
}

Character::~Character(){}

void Character::setBaseValues(short baseAP, short baseMaxHP)
{
	mBaseAP = baseAP;
	mBaseMaxHP = baseMaxHP;
	mAP = mBaseAP;
	mMaxHP = mBaseMaxHP;
	mHP = mMaxHP;
};

void Character::setName(std::string name)
{
	mName = name;
};
std::string Character::getName()
{
	return mName;
};

void Character::setHP(short hp)
{
	mHP = mHP + hp;
};
short Character::getHP()
{
	return mHP;
};

void Character::setMaxHP(short maxhp)
{
	mMaxHP = maxhp;
};
short Character::getMaxHP()
{
	return mMaxHP;
};

void Character::setAP(short ap)
{
	mAP = ap;
};
short Character::getAP()
{
	return mAP;
};

short Character::inventorySize()
{
	return mInventory.size();
}

void Character::addToInventory(Equipment item)
{
	if (item.getType() == T_Head)
	{
		if (mInventory.empty())
			mInventory.push_back(item);
		else
			mInventory[T_Head] = item;
        QString temp;
        temp.append("item: ");
        temp.append(QString::fromStdString(item.getName()));
        temp.append(" added to inventory \n");
        addStyledLogEntry(temp, false);
        SoundEngine::PlaySoundByName("item",1);
		item.posX = OUT_OF_RANGE;
		item.posY = OUT_OF_RANGE;
	}
	else if (item.getType() == T_Chest)
	{
		if (mInventory.size() <= T_Chest)
			mInventory.push_back(item);
		else
			mInventory[T_Chest] = item;
        QString temp;
        temp.append("item: ");
        temp.append(QString::fromStdString(item.getName()));
        temp.append(" added to inventory \n");
        addStyledLogEntry(temp, false);
        SoundEngine::PlaySoundByName("item", 1);
		item.posX = OUT_OF_RANGE;
		item.posY = OUT_OF_RANGE;
	}
	else if (item.getType() == T_Weapon)
	{
		if (mInventory.size() <= T_Weapon)
			mInventory.push_back(item);
		else
			mInventory[T_Weapon] = item;
        QString temp;
        temp.append("item: ");
        temp.append(QString::fromStdString(item.getName()));
        temp.append(" added to inventory \n");
        addStyledLogEntry(temp, false);
        SoundEngine::PlaySoundByName("item", 1);
		item.posX = OUT_OF_RANGE;
		item.posY = OUT_OF_RANGE;
	}
	else if (item.getType() == T_Consumable)
	{
		if (mInventory.size() >= MAX_INVENTORY_SIZE)
			mInventory.pop_back();
        QString temp;
        temp.append("item: ");
        temp.append(QString::fromStdString(item.getName()));
        temp.append(" added to inventory \n");
        addStyledLogEntry(temp, false);
        SoundEngine::PlaySoundByName("item", 1);
		item.posX = OUT_OF_RANGE;
		item.posY = OUT_OF_RANGE;
		mInventory.push_back(item);
		if (mInventory.size() >= POTION_START_INDEX)
			std::sort(mInventory.begin() + POTION_START_INDEX, mInventory.end(), greater);
	}
};

void Character::calculateStats()
{
    short sumMaxHP=0;
    short sumAP=0;
    for (short item = 0; item <= T_Weapon; ++item)
	{
		sumMaxHP = sumMaxHP + mInventory[item].getMaxHP();
		sumAP = sumAP + mInventory[item].getAP();
	}
	mMaxHP = mBaseMaxHP + sumMaxHP;
	mAP = mBaseAP + sumAP ;

	if (mHP > mMaxHP)//if hp over maxhp
		mHP = mMaxHP;
};

Equipment Character::selectItem(short index)
{
	return mInventory[index];
};

void Character::removeFromInventory(short index)
{
	mInventory.erase(mInventory.begin() + index);
	if (mInventory.size()>4)
		std::sort(mInventory.begin() + 3, mInventory.end(), greater);
};

void Character::walk(short y, short x)
{

	if (((heroRow + y) < gHeight) && ((heroCol + x) < gWidth) && ((heroRow + y) >= 0) && ((heroCol + x) >= 0))//if inside limits
	{
        if (lvl.grid[heroRow+y][heroCol+x] != L_L){
            SoundEngine::PlayFootstep(); // Enhanced footstep sound
			heroRow = (heroRow + y);
			heroCol = (heroCol + x);
		}
	}
	/*ilumante adyacent places mark visited*/
    for (short vRow = (heroRow - 1); vRow <= (heroRow + 1); ++vRow)
	{
        for (short vCol = (heroCol - 1); vCol <= (heroCol + 1); ++vCol)
		{
			if (vRow < gHeight && vCol < gWidth && vRow >= 0 && vCol >= 0)//if inside limits
				gExplored.grid[vRow][vCol] = 1;
		}
	}
};

void Character::setCurrentEnemy(Enemy *enemy)
{
    //std::cout<< enemy->getname();
    mCurrentEnemy = enemy;

}

Enemy *Character::getCurrentEnemy()
{
    return mCurrentEnemy;
};
