#ifndef POTION_H
#define POTION_H
#include "Equipment.h"
#include  "Character.h"
#include <iostream>
class Character;

class Potion : public Equipment
{
public:

	Potion(std::string name, int ap, int maxhp, bool fixed = false);
	~Potion();

    static void pickPotion(Potion *current, Character &hero);
    static void usePotion(Character &hero);
};

#endif //POTION_H
