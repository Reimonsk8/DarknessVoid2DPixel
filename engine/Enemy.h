#ifndef ENEMY_H
#define ENEMY_H
#include <string>
#include "Character.h"
#include "SoundEngine.h"


class Enemy
{
public:

    short posY;
    short posX;

    Enemy(std::string name, short ap, short hp, short flee, short difficulty = 1);//constructor sets all enemy values on creation
	~Enemy();
	
	std::string getName();

    short getHP();
    void setHP(short damage);//enemy recibes damage by this method
	
    short getAP();//enemy damages player by this method
	
    short getDifficulty();//returns enemy difficulty for scoring
	
	bool tryFlee();

    static void enemyAtacked(Character &hero);
    static void heroAtacked(Character &hero);

private:

	std::string mName;
    short mHP;
    short mAP;
    short mFlee;// to flee value should be higher than this max value should be equal to 11
    short mDifficulty;// difficulty level for scoring (1-10)
	
};

#endif //ENEMY_H
