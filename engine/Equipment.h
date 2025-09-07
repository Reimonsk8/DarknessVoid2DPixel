#ifndef EQUIPMENT_H
#define EQUIPMENT_H
#include <string>

class Equipment
{
public:
	
    short posY;
    short posX;

    Equipment(std::string name, short ap, short maxhp, bool fixed );
	~Equipment();

	std::string getName();
    short getAP();
    void setAP(short ap);

    short getMaxHP();
    void setMaxHP(short maxhp);

    short getType();
    std::string mRarity;
    bool legendarySoundPlayed;

protected:

	std::string mName;
    short mType;

    short mRandomValue;

    short mAP;
    short mMaxHP;
	
};

static bool greater(Equipment &a, Equipment &b){
	return a.getMaxHP() > b.getMaxHP();
}

#endif //EQUIPMENT_H
