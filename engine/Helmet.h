#ifndef HELMET_H
#define HELMET_H
#include "Equipment.h"
#include <iostream>
class Character;

class Helmet : public Equipment
{

public:
	Helmet(std::string name, int ap, int maxhp, bool fixed = false);
	~Helmet();

    static void pickHelmet(Helmet *current, Character &hero);
};

#endif //HELMET_H
