#include <time.h>
#include <vector>
#include <iostream>
#include <atomic>
#include "common.h"
#include "Generator.h"
#include <QVector>
#include <QRandomGenerator>
#include <QtDebug>

template<typename T>
void Generator::shuffleItems(std::vector<T>* items)
{
    int size = items->size();
    for (int i = 0; i < size; ++i)
    {
        int j = rand() % (size - i) + i;
        std::swap((*items)[i], (*items)[j]);
    }
}

Generator::Generator()
    :enemies(0), weapons(0), helmets(0), armors(0), potions(0)
{
	srand(time(0));

    struct EnemyConfig {
        const char* name;
        int ap;
        int maxHp;
        int flee;
        int population;
        int difficulty; // Difficulty level for scoring (1-10)
    } enemyConfigs[] = {
        {"skeleton", 10, 50, 6, 10, 2},     // Easy enemy
        {"ghost", 15, 100, 9, 10, 3},       // Easy-Medium enemy
        {"demon", 26, 200, 6, 5, 6},        // Hard enemy
        {"troll", 20, 300, 3, 4, 5},        // Medium-Hard enemy
        {"centaur", 20, 150, 8, 6, 4},      // Medium enemy
        {"bat", 7, 30, 10, 10, 1},          // Very Easy enemy
        {"minotaur", 25, 200, 4, 4, 6},     // Hard enemy
        {"gryphon", 35, 180, 9, 4, 7},      // Very Hard enemy
        {"reaper", 50, 250, 9, 2, 9},       // Extreme enemy
        {"dragon", 100, 700, 10, 1, 10}     // Boss Level enemy
    };
    int enemyConfigsSize = sizeof(enemyConfigs) / sizeof(enemyConfigs[0]);
    for (int enemyTypeIndex = 0; enemyTypeIndex < enemyConfigsSize; ++enemyTypeIndex)
    {
        for(int populationCount = 0; populationCount < enemyConfigs[enemyTypeIndex].population; ++populationCount)
        {
            listEnemies.push_back(Enemy(enemyConfigs[enemyTypeIndex].name,
                                        enemyConfigs[enemyTypeIndex].ap,
                                        enemyConfigs[enemyTypeIndex].maxHp,
                                        enemyConfigs[enemyTypeIndex].flee,
                                        enemyConfigs[enemyTypeIndex].difficulty));
        }
    }

	for (int i = 0; i < 2; i++)//generate helms
	{
        listHelmets.push_back(Helmet("hat", 0, 1));//name,AP,maxHP
        listHelmets.push_back(Helmet("thief hood", 0, 2));
        listHelmets.push_back(Helmet("soldier helm", 0, 3));
        listHelmets.push_back(Helmet("sage cap", 0, 4));
        listHelmets.push_back(Helmet("templar helmet ", 0, 5));
        listHelmets.push_back(Helmet("straight helmet", 0, 6));
        listHelmets.push_back(Helmet("dragon mask", 0, 7));
        listHelmets.push_back(Helmet("barbute helmet", 0, 8));
        listHelmets.push_back(Helmet("evil dark helm", -15, 55));

    }

	for (int i = 0; i < 2; i++)//generate armors
	{
        listArmors.push_back(Armor("cloth shirt", 0, 1));
        listArmors.push_back(Armor("stripped armor", 0, 2));
        listArmors.push_back(Armor("chainmail", 0, 3));
        listArmors.push_back(Armor("magician robe", 0, 4));
        listArmors.push_back(Armor("mithrill armor", 0, 6));
        listArmors.push_back(Armor("brigadine armor", 0, 8));
        listArmors.push_back(Armor("dragon slayer set", 0, 10));
        listArmors.push_back(Armor("evil cursed armor", 100, -100));
    }

	for (int i = 0; i < 2; i++)//generate weapons
    {
        listWeapons.push_back(Weapon("knife", 1, 0));
        listWeapons.push_back(Weapon("pickaxe", 1, 0));
        listWeapons.push_back(Weapon("sword", 2, 0));
        listWeapons.push_back(Weapon("axe", 3, 0));
        listWeapons.push_back(Weapon("trident", 4, 0));
        listWeapons.push_back(Weapon("wand", 5, 0));
        listWeapons.push_back(Weapon("longsword", 6, 0));
        listWeapons.push_back(Weapon("spear", 7, 0));
        listWeapons.push_back(Weapon("javelin", 8, 0));
        listWeapons.push_back(Weapon("claymore", 9, 0));
        listWeapons.push_back(Weapon("greatsword", 12, 0));
    }

	const int TOTAL_POTIONS = 50;
	for (int i = 0; i < TOTAL_POTIONS; i++)//generate potions
        listPotions.push_back(Potion("health potion", 0, 14));

    shuffleItems(&listEnemies);
    shuffleItems(&listHelmets);
    shuffleItems(&listArmors);
    shuffleItems(&listWeapons);
    shuffleItems(&listPotions);
};

Generator::~Generator()
{
    qDebug() << "Generator destructor called";
    
    // Clear vectors safely without calling any external functions
    // that might access Qt objects or global state
    try {
        // Clear vectors in reverse order of creation for safety
        listPotions.clear();
        listArmors.clear();
        listHelmets.clear();
        listWeapons.clear();
        listEnemies.clear();
        
        // Force memory barrier to ensure all clears are complete
        std::atomic_thread_fence(std::memory_order_seq_cst);
        
        qDebug() << "Generator vectors cleared successfully";
    } catch (const std::exception& e) {
        qDebug() << "Standard exception during Generator cleanup:" << e.what();
    } catch (...) {
        qDebug() << "Unknown exception during Generator cleanup - continuing";
    }
    
    qDebug() << "Generator destructor completed";
};

void Generator::infoPlace(int y, int x)//function to look info
{
	if (((y) < gHeight) && ((x) < gWidth) && ((y) >= 0) && ((x) >= 0))//if inside limits
	{
        for (unsigned long long  i = 0; i < listEnemies.size(); ++i)
		{
			if (y == listEnemies[i].posY && x == listEnemies[i].posX)
			{
                QString temp;
                temp.append("Enemy ahead ");
                temp.append(QString::fromStdString(listEnemies[i].getName()));
                temp.append(" health ");
                temp.append(QString::number(listEnemies[i].getHP()));
                temp.append(" power ");
                temp.append(QString::number(listEnemies[i].getAP()));
                temp.append("\n");
                addStyledLogEntry(temp, false);
				return;
			}
		}
        for (unsigned long long  i = 0; i < listHelmets.size(); ++i)
		{
			if (y == listHelmets[i].posY && x == listHelmets[i].posX)
			{
                QString temp;
                temp.append("Item ahead ");
                temp.append(QString::fromStdString(listHelmets[i].getName()));
                temp.append(" +Max health points ");
                temp.append(QString::number(listHelmets[i].getMaxHP()));
                temp.append("\n");
                addStyledLogEntry(temp, false);
				return;
			}
		}
        for (unsigned long long  i = 0; i < listArmors.size(); ++i)
		{
			if (y == listArmors[i].posY && x == listArmors[i].posX)
			{
                QString temp;
                temp.append("Item ahead ");
                temp.append(QString::fromStdString(listArmors[i].getName()));
                temp.append(" +Max health points ");
                temp.append(QString::number(listArmors[i].getMaxHP()));
                temp.append("\n");
                addStyledLogEntry(temp, false);
				return;
			}
		}
        for (unsigned long long  i = 0; i < listWeapons.size(); ++i)
		{
			if (y == listWeapons[i].posY && x == listWeapons[i].posX)
			{
                QString temp;
                temp.append("Item ahead ");
                temp.append(QString::fromStdString(listWeapons[i].getName()));
                temp.append(" +Atack power ");
                temp.append(QString::number(listWeapons[i].getAP()));
                temp.append("\n");
                addStyledLogEntry(temp, false);
				return;
			}
		}
        for (unsigned long long  i = 0; i < listPotions.size(); ++i)
		{
			if (y == listPotions[i].posY && x == listPotions[i].posX)
			{
                QString temp;
                temp.append("Item ahead ");
                temp.append(QString::fromStdString(listPotions[i].getName()));
                temp.append(" +Health points ");
                temp.append(QString::number(listPotions[i].getMaxHP()));
                temp.append("\n");
                addStyledLogEntry(temp, false);
				return;
			}
		}
	}
    addStyledLogEntry("Theres nothing to look\n", false);
	return;
}

void Generator::spawnMap()
{
	int mapValue;
	for (int row = 0; row < gHeight; ++row)//count items to generate
	{
		for (int col = 0; col < gHeight; ++col)
		{

			mapValue = lvl.grid[row][col];
			switch (mapValue)
			{
			case(L_E) :
			{
				if (enemies < listEnemies.size())
				{
					listEnemies[enemies].posY = row;
					listEnemies[enemies].posX = col;
					++enemies;
				}
				else
					std::cout << "out of enemies" << std::endl;
			}break;

			case(L_W) :
			{
				if (weapons < listWeapons.size())
				{
					listWeapons[weapons].posY = row;
					listWeapons[weapons].posX = col;
					++weapons;
				}
				else
					std::cout << "out of weapons" << std::endl;
			}break;

			case(L_A) :
			{
				if (armors < listArmors.size())
				{
					listArmors[armors].posY = row;
					listArmors[armors].posX = col;
					++armors;
				}
				else
					std::cout << "out of armors" << std::endl;
			}break;

			case(L_H) :
			{
				if (helmets < listHelmets.size())
				{
					listHelmets[helmets].posY = row;
					listHelmets[helmets].posX = col;
					++helmets;
				}
				else
					std::cout << "out of helmets" << std::endl;
			}break;

			case(L_P) :
			{
                if (potions < listPotions.size())
				{
					listPotions[potions].posY = row;
					listPotions[potions].posX = col;
					++potions;
				}
				else
					std::cout << "out of potions" << std::endl;
			}break;

			default:
				break;
			}

		}
	}
};

Enemy *Generator::returnEnemy(int heroRow, int heroCol)
{
    for (unsigned long long x = 0; x < listEnemies.size(); ++x)
	{
		if (heroRow == listEnemies[x].posY && heroCol == listEnemies[x].posX)
		{
			return &listEnemies[x];
		}
	}
	return nullptr;
}

Weapon *Generator::returnWeapon(int heroRow, int heroCol)
{
    for (unsigned long long x = 0; x < listWeapons.size(); ++x)
	{
		if (heroRow == listWeapons[x].posY && heroCol == listWeapons[x].posX)
		{
			return &listWeapons[x];
		}
	}
	return nullptr;
}

Helmet *Generator::returnHelmet(int heroRow, int heroCol)
{
    for (unsigned long long x = 0; x < listHelmets.size(); ++x)
	{
		if (heroRow == listHelmets[x].posY && heroCol == listHelmets[x].posX)
		{
			return &listHelmets[x];
		}
	}
	return nullptr;
}

Armor *Generator::returnArmor(int heroRow, int heroCol)
{
    for (unsigned long long x = 0; x < listArmors.size(); ++x)
	{
		if (heroRow == listArmors[x].posY && heroCol == listArmors[x].posX)
		{
			return &listArmors[x];
		}
	}
	return nullptr;
}

Potion *Generator::returnPotion(int heroRow, int heroCol)
{
    for (unsigned long long x = 0; x < listPotions.size(); ++x)
	{
		if (heroRow == listPotions[x].posY && heroCol == listPotions[x].posX)
		{
			return &listPotions[x];
		}
	}
	return nullptr;
}
