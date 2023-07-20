#ifndef CHARACTER_H
#define CHARACTER_H
#include <string>
#include <vector>
#include "SoundEngine.h"
#include "Equipment.h"
#include "common.h"

class Enemy;

class Character
{
public:
    short heroRow;
    short heroCol;
	bool enemyDamaged;

    Character(); //short gHeight, short gWidht,Map &gMap
    ~Character();

	void setName(std::string name);
	std::string getName();

    void setBaseValues(short baseAP, short baseMaxHP);

    void setHP(short hp);
    short getHP();

    void setMaxHP(short maxhp);
    short getMaxHP();

    void setAP(short ap);
    short getAP();

    short inventorySize();
    void addToInventory(Equipment item);
    Equipment selectItem(short index);
    void removeFromInventory(short index);
	
    void walk(short y, short x);
	void calculateStats();

    void setCurrentEnemy(Enemy *current);
    Enemy *getCurrentEnemy();

private:

	std::vector<Equipment> mInventory;
	std::string mName;
    short mMaxHP;
    short mHP;
    short mAP;

    short mBaseAP;
    short mBaseMaxHP;
    Enemy *mCurrentEnemy;
};

#endif //CHARACTER_H
