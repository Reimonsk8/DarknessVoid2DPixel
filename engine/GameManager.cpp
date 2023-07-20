#include <conio.h>
#include <windows.h>
#include <fstream>
#include <QtDebug>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QFile>
#include <QTextStream>
#include <QResource>
#include <QDirIterator>
#include "GameManager.h"
#include "common.h"
#include "graphics.h"
#include "Equipment.h"


GameManager::GameManager(){}

void GameManager::readStdString(std::string filePath)
{
    std::ifstream reader(filePath);
    if(!reader)
	{
		std::cout << "While opening a file an error is encountered" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
        std::cout << "file opened sucessfully" << std::endl;
		reader >> gMaxHP;
		reader >> gAP;
		reader >> gHeight;
		reader >> gWidth;
		/* assign file info to map */
        for(int row = 0; row < gHeight; ++row)
		{
            for(int col = 0; col < gWidth; ++col)
			{
				reader >> lvl.grid[row][col];
                if(lvl.grid[row][col] == L_O)
				{
					lvl.grid[row][col] = 'X';
                }
			}
		}
	}
}
void GameManager::readQString(QString filePath)
{
    QResource::registerResource(filePath);
    QFile mapfile(filePath);
    //mapfile.open(QIODevice::ReadOnly);
    if (!mapfile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Unable to open file: " << mapfile.fileName() << " because of error " << mapfile.errorString();
        return;
    }

    QTextStream in(&mapfile);

    QString line = in.readLine();
    QStringList tuple = line.split(" ");
    gMaxHP = tuple.at(0).toShort();
    gAP = tuple.at(1).toShort();

    line = in.readLine();
    tuple = line.split(" ");
    gHeight = tuple.at(0).toShort();
    gWidth = tuple.at(1).toShort();

    /* assign file info to map */
    for(int row = 0; row < gHeight; ++row)
    {
        line = in.readLine();
        tuple = line.split(" ");
        for(int col = 0; col < gWidth; ++col)
        {
            //convert QString to char
            QString value = tuple.at(col);
            std::string valueStr = value.toStdString();
            std::vector<char> chars(valueStr.c_str(), valueStr.c_str() + valueStr.size() + 1u);

            lvl.grid[row][col] = chars[0];
            if(lvl.grid[row][col] == L_O)
            {
                lvl.grid[row][col] = 'X';
            }
        }
    }


}

bool GameManager::startMenu(Character &hero) // only one time call at the beggining//only one time call at the beggining
{
    std::string x(hero.getName());
    if((x.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) || x.length()>20)
    {
        mLogContent.prepend( "Error name not valid,\nplease input no more than 20 chars \n& no special characters only letters\n");
        mScrollLog->setText(mLogContent);
        return false;
    }
	/*beginning gear should be set to low stats*/
    QString temp;
    temp.append(" Hi ");
    temp.append(QString::fromStdString(hero.getName()));
    temp.append(" now begin your journey to safety, take this gear\n ");
    mLogContent.prepend(temp);
    mScrollLog->setText(mLogContent);
    hero.addToInventory(Helmet("hat", 0, 1, true));
    hero.addToInventory(Armor("cloth shirt", 0, 1, true));
    hero.addToInventory(Weapon("knife", 1, 0, true));
    hero.calculateStats();
    SoundEngine::PlaySoundByName("begin", 1);
    return true;
}

int GameManager::decode(int code)
{
    if(code == B_Right)
		return 0;
    else if(code == B_Down)
		return 1;
    else if(code == B_Left)
		return 2;
    else if(code == B_Up)
		return 3;
    return -1;
}

void GameManager::handleEvent(Character &hero, Generator &generated)
{
    hero.calculateStats();
	int currentChar = lvl.grid[hero.heroRow][hero.heroCol];//casting char to int 
	switch (currentChar)
	{
    case(L_X) ://look event
	{
        if(gState == S_Normal && gValueButton == B_Inspect)
        {
            SoundEngine::PlaySoundByName("ok", 0.5);
            mLogContent.prepend("enter inspect direction: (click button or press key)\n");
            mScrollLog->setText(mLogContent);
            gState = S_WaitForArrowKeys;
            return;
        }
        if((gValueButton >=3) && (gValueButton <=7) && (gState == S_WaitForArrowKeys))
        {
            if(gValueButton == B_Left)
                generated.infoPlace(hero.heroRow, hero.heroCol - 1);
            else if(gValueButton == B_Down)
                generated.infoPlace(hero.heroRow + 1, hero.heroCol);
            else if(gValueButton == B_Right)
                generated.infoPlace(hero.heroRow, hero.heroCol + 1);
            else if(gValueButton == B_Up)
                generated.infoPlace(hero.heroRow - 1, hero.heroCol);
            gState = S_Normal;
            SoundEngine::PlaySoundByName("look", 1);
            return;
        }

        else if((gState == S_Normal) && (gValueButton >= P_1) && (gValueButton <= P_5))
            Potion::usePotion(hero);
	}break;


	case(L_E) :// enemy fight event
    {
        if(gState == S_Normal)
        {
            Enemy *current = generated.returnEnemy(hero.heroRow, hero.heroCol);
            QString temp;
            temp.append("Enemy encountered  ");
            temp.append(QString::fromStdString(current->getName()));
            temp.append(" Health  ");
            temp.append(QString::number(current->getHP()));
            temp.append(" Power ");
            temp.append(QString::number(current->getAP()));
            temp.append(" Power, FIGHT begins!\n");
            temp.append("Your turn, select & enter action: (click button or press key)\n");
            mLogContent.prepend(temp);
            mScrollLog->setText(mLogContent);
            SoundEngine::PlaySoundByName("battle", 1);
            hero.setCurrentEnemy(current);// set hero current enemy
            gFlee = false; // battle on event flee set to false
            gState = S_BattleOn;
            return;
        }

        if(gState == S_BattleOn || gState == S_BattleOnPotionUsed)
        {
            if(gValueButton == B_Potion && gState != S_BattleOnPotionUsed)// potion option doesn't consume turn
            {
                Potion::usePotion(hero);
                Sleep(200);
                gState = S_BattleOnPotionUsed;
                QString temp;
                temp.append("Your turn, select & enter action: (click button or press key)\n");
                mLogContent.prepend(temp);
                mScrollLog->setText(mLogContent);
            }
            if(gValueButton == B_Atack)// atack enemy
            {
                Enemy::enemyAtacked(hero);
            }
            else if(gValueButton == B_Flee)//flee option
            {
                mLogContent.prepend("you try to flee\n");
                mScrollLog->setText(mLogContent);
                if(hero.getCurrentEnemy()->tryFlee())
                {
                    SoundEngine::PlaySoundByName("flee", 1);
                    mLogContent.prepend("you fleed sucesfully from battle !!!\n" );
                    mScrollLog->setText(mLogContent);
                    gState = S_Normal;
                    gFlee = true;
                }
                else
                    Enemy::heroAtacked(hero);

            if(gState == S_BattleOn)//turn goes on
            {
                QString temp;
                temp.append(" Your turn, select & enter action: (click button or press key)\n");
                mLogContent.prepend(temp);
                mScrollLog->setText(mLogContent);
            }
           }
        }
	}break;

	case(L_P) ://potion event
	{
        gState = S_ActionToPickItem;
		Potion *current = generated.returnPotion(hero.heroRow, hero.heroCol);
        if (current->getName().find("legendary") != std::string::npos){
           SoundEngine::PlaySoundByName("legend", 0.5);
        }
        Potion::pickPotion(current, hero);
	}break;

	case(L_W) ://weapon event
	{
        gState = S_ActionToPickItem;
		Weapon *current = generated.returnWeapon(hero.heroRow, hero.heroCol);
        if (current->getName().find("legendary") != std::string::npos){
           SoundEngine::PlaySoundByName("legend", 0.5);
        }
        Weapon::pickWeapon(current, hero);
	}break;


	case(L_A) ://armor event
	{
        gState = S_ActionToPickItem;
		Armor *current = generated.returnArmor(hero.heroRow, hero.heroCol);
        if (current->getName().find("legendary") != std::string::npos){
            SoundEngine::PlaySoundByName("legend", 0.5);
        }
        Armor::pickArmor(current, hero);
	}break;


	case(L_H) ://helmet event
    {
        gState = S_ActionToPickItem;
		Helmet *current = generated.returnHelmet(hero.heroRow, hero.heroCol);
        if (current->getName().find("legendary") != std::string::npos){
            SoundEngine::PlaySoundByName("legend", 0.5);
        }
        Helmet::pickHelmet(current, hero);
	}break;

    case(L_F) ://End game event
    {
        SoundEngine::PlaySoundByName("win", 0.5);
        gState = S_Win;
    }break;

    default:
    {}
    break;
    }

}

