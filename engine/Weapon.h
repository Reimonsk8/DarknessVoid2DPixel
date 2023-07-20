#ifndef WEAPON_H
#define WEAPON_H
#include "Equipment.h"
#include <iostream>
class Character;//forward declaration

class Weapon : public Equipment
{

public:

	Weapon(std::string name, int ap, int maxhp, bool fixed = false);
	~Weapon();

    static void pickWeapon(Weapon *current, Character &hero);

};

#endif //WEAPON_H
