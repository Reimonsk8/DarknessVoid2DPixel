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
    std::string mRarity; //common, rare or legendary

protected:

	std::string mName;
    short mType; //enum weapon, wearable or consumable

    short mRandomValue;

    short mAP;//atack points gained by equipment
    short mMaxHP;//max health gained by equipment
	
};

static bool greater(Equipment &a, Equipment &b){
	return a.getMaxHP() > b.getMaxHP();
}

#endif //EQUIPMENT_H
