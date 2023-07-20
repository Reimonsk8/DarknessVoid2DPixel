#ifndef ARMOR_H
#define ARMOR_H
#include "Equipment.h"
#include <iostream>

class Character;

class Armor : public Equipment
{

public:

	Armor(std::string name, int ap, int maxhp, bool fixed = false);
	~Armor();

    static void pickArmor(Armor *current, Character &hero);
};

#endif //ARMOR_H
