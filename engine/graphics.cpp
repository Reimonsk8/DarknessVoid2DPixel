#include "stdafx.h"
#include <Windows.h>
#include <fstream>
#include "graphics.h"
#include "Character.h"
#include "common.h"

//REVIEW [VAR][ENCAPSULATION][Karla]: Abstract variables in order for them to live in its own scope
// and avoid global namespace pollution.
static const enum BackgroundColors {B_Hero =26 ,B_Enemy = 44, B_Floor = 120, B_Potion = 116, B_Gear = 126, B_Walls = 135};
static const int notExplored = C_Black;//should be black
static const int defaultTextColor = C_White;

void cls(bool invCLS)
{
	SetConsoleActiveScreenBuffer(hConsole);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	std::cout.flush();
	COORD topLeft = { 0, 0 };
	COORD inventory;
	if (gInstructions)
		inventory = { gWidth, gHeight + 3 };
	else
		inventory = { gWidth, gHeight };
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
		abort();
	DWORD length = csbi.dwSize.X * csbi.dwSize.Y;
	DWORD written;
	if (!gFlee)
	{
		FillConsoleOutputCharacter(hConsole, TEXT(' '), length, topLeft, &written);
		FillConsoleOutputAttribute(hConsole, csbi.wAttributes, length, topLeft, &written);
	}
	else if (invCLS)
		FillConsoleOutputCharacter(hConsole, TEXT(' '), length / 2, inventory, &written);
	
	if (gInstructions)
	{
		FillConsoleOutputCharacter(hConsole, TEXT(' '), 90 * 5, topLeft, &written);
		FillConsoleOutputAttribute(hConsole, csbi.wAttributes, 90 * 5, topLeft, &written);
	}
	SetConsoleCursorPosition(hConsole, topLeft);
}

void redraw(int y, int x)
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (!GetConsoleScreenBufferInfo(hConsole, &cbsi))
		abort();
	COORD coord;
	coord.X = cbsi.dwSize.X-x;
	coord.Y = cbsi.dwSize.Y-y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	std::cout << "                                                                                     " << std::endl;
}



void drawScreen(Character &hero, bool inventory)
{
	
	cls(inventory);
	if (gInstructions)
	{
		std::cout << "############################## Instructions: ##############################" << std::endl;
		std::cout << "## arrow keys to move # enter key to interact # Toogle I Key instructions #" << std::endl;
		std::cout << "###########################################################################" << std::endl;
	}
	/*draw map*/
	if (gFlee)//if not in battle print map
	{
		for (int row = 0; row < gHeight; ++row)
		{
			std::cout << std::endl;
			for (int col = 0; col < gWidth; ++col)
			{
				SetConsoleTextAttribute(hConsole, defaultTextColor);
				if (row == hero.heroRow && col == hero.heroCol)//draw Hero position
				{
					SetConsoleTextAttribute(hConsole, B_Hero);
					std::cout << "[C]";
					SetConsoleTextAttribute(hConsole, defaultTextColor);
				}
				else
				{
					switch (lvl.grid[row][col])
					{
					case(L_L) ://Walls
					{
                        // REVIEW [STRUCT][REPEATED_CODE][Karla]: The following code could be placed in a function, except for the 
                        // unexplored case, to reduce repeated code.
						if (gExplored.grid[row][col] == 1)
							SetConsoleTextAttribute(hConsole, B_Walls);
						else
							SetConsoleTextAttribute(hConsole, notExplored);
						std::cout << "[" << lvl.grid[row][col] << "]";
						SetConsoleTextAttribute(hConsole, defaultTextColor);
					}break;

					case(L_E) ://enemys
					{
						if (gExplored.grid[row][col] == 1)
							SetConsoleTextAttribute(hConsole, B_Enemy);
						else
							SetConsoleTextAttribute(hConsole, notExplored);
						std::cout << "[" << lvl.grid[row][col] << "]";
						SetConsoleTextAttribute(hConsole, defaultTextColor);
					}break;

					case(L_W) ://item
					{
						if (gExplored.grid[row][col] == 1)
							SetConsoleTextAttribute(hConsole, 126);
						else
							SetConsoleTextAttribute(hConsole, notExplored);
						std::cout << " " << lvl.grid[row][col] << " ";
						SetConsoleTextAttribute(hConsole, defaultTextColor);
					}break;

					case(L_H) ://helmet
					{
						if (gExplored.grid[row][col] == 1)
							SetConsoleTextAttribute(hConsole, B_Gear);
						else
							SetConsoleTextAttribute(hConsole, notExplored);
						std::cout << " " << lvl.grid[row][col] << " ";
						SetConsoleTextAttribute(hConsole, defaultTextColor);
					}break;

					case(L_A) ://armor
					{
						if (gExplored.grid[row][col] == 1)
							SetConsoleTextAttribute(hConsole, B_Gear);
						else
							SetConsoleTextAttribute(hConsole, notExplored);
						std::cout << " " << lvl.grid[row][col] << " ";
						SetConsoleTextAttribute(hConsole, defaultTextColor);
					}break;

					case(L_P) ://potions
					{
						if (gExplored.grid[row][col] == 1)
							SetConsoleTextAttribute(hConsole, B_Potion);
						else
							SetConsoleTextAttribute(hConsole, notExplored);
						std::cout << " " << lvl.grid[row][col] << " ";
						SetConsoleTextAttribute(hConsole, defaultTextColor);
					}break;

					case(L_X) ://unexplored
					{
						if (gExplored.grid[row][col] == 1)
						{
							lvl.grid[row][col] = 'O';
							SetConsoleTextAttribute(hConsole, B_Floor);
						}
						else
							SetConsoleTextAttribute(hConsole, notExplored);
						std::cout << " " << lvl.grid[row][col] << " ";
						SetConsoleTextAttribute(hConsole, notExplored);
					}break;

					case(L_O) ://explored
					{
						if (gExplored.grid[row][col] == 1)
							SetConsoleTextAttribute(hConsole, B_Floor);
						else
							SetConsoleTextAttribute(hConsole, notExplored);
						std::cout << " " << lvl.grid[row][col] << " ";
						SetConsoleTextAttribute(hConsole, defaultTextColor);
					}break;

					case(L_S) ://start point
					{
						if (gExplored.grid[row][col] == 1)
							SetConsoleTextAttribute(hConsole, 112);
						else
							SetConsoleTextAttribute(hConsole, notExplored);
						std::cout << " " << lvl.grid[row][col] << " ";
						SetConsoleTextAttribute(hConsole, defaultTextColor);
					}break;

					case(L_F) ://finish point
					{
						if (gExplored.grid[row][col] == 1)
							SetConsoleTextAttribute(hConsole, 125);
						else
							SetConsoleTextAttribute(hConsole, notExplored);
						std::cout << " " << lvl.grid[row][col] << " ";
						SetConsoleTextAttribute(hConsole, defaultTextColor);
					}break;

					default:
					{
						if (gExplored.grid[row][col] == 1)
							SetConsoleTextAttribute(hConsole, defaultTextColor);
						else
							SetConsoleTextAttribute(hConsole, notExplored);
						std::cout << "[" << lvl.grid[row][col] << "]";
						SetConsoleTextAttribute(hConsole, defaultTextColor);
					}break;
					}

				}

			}
			SetConsoleTextAttribute(hConsole, defaultTextColor);
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}
	else if (!gFlee)// if battle event going on print current enemy 
	{
		cls(inventory);
		std::string fileName = "./Graphics/"+hero.getCurrentEnemy()+".txt";
		std::ifstream file(fileName);
		std::string str;
		if (hero.enemyDamaged)
			SetConsoleTextAttribute(hConsole, C_Red);
		while (std::getline(file, str))
		{
			std::cout << str << std::endl;
		}
		file.close();
		SetConsoleTextAttribute(hConsole, C_White);
	}

	/*draw menu*/
	std::cout << "############################### Status ####################################" << std::endl;
	SetConsoleTextAttribute(hConsole, C_Blue);
	std::cout << "N:" << hero.getName();
	SetConsoleTextAttribute(hConsole, C_BGreen);
	std::cout << " Hp: " << hero.getHP()<<"/"<<hero.getMaxHP();
	SetConsoleTextAttribute(hConsole, C_Red);
	std::cout << " Ap: " << hero.getAP() << std::endl;
	SetConsoleTextAttribute(hConsole, C_White);
	std::cout << "############################## Iventory ##################################" << std::endl;
	if (hero.inventorySize()>0)
	{
		for (int item = 0; item < hero.inventorySize(); ++item)
		{
            // REVIEW [CTRLS][Karla]: Maybe evaluate to "== T_Consumable" and change "else-if" for an "else" statement
			if (hero.selectItem(item).getType() != T_Consumable)//color weapons yellow and print info
			{
				SetConsoleTextAttribute(hConsole, C_Yellow);
				std::cout << hero.selectItem(item).getName() << " ";
				if (hero.selectItem(item).getAP()>0)
				{
					SetConsoleTextAttribute(hConsole, C_Red);
					std::cout << "+AP: " << hero.selectItem(item).getAP() << " ";
				}
				if (hero.selectItem(item).getMaxHP() > 0)
				{
					SetConsoleTextAttribute(hConsole, C_Green);
					std::cout << "+maxHP: " << hero.selectItem(item).getMaxHP() << " ";
				}
				SetConsoleTextAttribute(hConsole, C_White);
			}
			else if (hero.selectItem(item).getType() == T_Consumable)//color potions and print info
			{
				SetConsoleTextAttribute(hConsole, C_Pink);
				std::cout <<"(" << item - 2<<") "<< hero.selectItem(item).getName() << " ";
				if (hero.selectItem(item).getAP()>0)
				{
					SetConsoleTextAttribute(hConsole, C_Red);
					std::cout << "+AP: " << hero.selectItem(item).getAP() << " ";
				}
				if (hero.selectItem(item).getMaxHP() > 0)
				{
					SetConsoleTextAttribute(hConsole, C_BGreen);
					std::cout << "+HP: " << hero.selectItem(item).getMaxHP() << " ";
				}
				SetConsoleTextAttribute(hConsole, C_White);
			}
		
			std::cout << std::endl;
		}
	}
	else
		std::cout << "###                                                                    ###" << std::endl;
	std::cout << "##########################################################################" << std::endl;



};

void printTitle()
{
	gFlee = false;
	int times = 20;
	bool end = false;
	PlaySound(TEXT("./Sounds/intro.wav"), NULL, SND_ASYNC);
	//SetConsoleTextAttribute(hConsole, C_Yellow);
	while (!end)
	{
		cls();

        // REVIEW[STRUCT][CONVENTION][Karla]: All loops (in fact, all scopes) must have its body enclosed by curly braces.
		for (int x = times; x >= 0; --x)
			std::cout << std::endl;
		std::cout << "_____  _____   _____  __      __    _____ _____  ___   ____   _____  __  __   __  " << std::endl;
		std::cout << " ||   ((   )) ((   )) ||     ((     ||  ) ||_// ||=|| (( ___ ((   )) ||\\\\||  ((   " << std::endl;
		std::cout << " ||    \\\\_//   \\\\_//  ||__| \\_))    ||_// || \\\\ || ||  \\\\_||  \\\\_//  || \\|| \\_))  " << std::endl;
		Sleep(20);
		--times;
		if (times <=0)
			end = true;
	}
	gFlee = true;

}

void gameOver()//animation of game over
{
	gFlee = false;
	int times = 0;
	bool end = false;
	SetConsoleTextAttribute(hConsole, C_Red);
	PlaySound(TEXT("./Sounds/gameover.wav"), NULL, SND_SYNC);
	while (!end)
	{
		cls();
		std::cout << "YOU DIED!!! \n your body lies in a pool of blood while the enemy eat's your bones and flesh...";
		for (int x = 0; x <= times;++x)
			std::cout << std::endl;
		std::ifstream file("./Graphics/gameover.txt");
		std::string str;
		while (std::getline(file, str))
			std::cout << str << std::endl;
		file.close();
		Sleep(500);
		++times;
		if (times >= 6)
			end = true;
	}
	exit(EXIT_SUCCESS);
	gFlee = true;
}

void victory()//animation of game over
{
	gFlee = false;
	int times = 0;
	bool end = false;
	SetConsoleTextAttribute(hConsole, C_Yellow);
	PlaySound(TEXT("./Sounds/victory.wav"), NULL, SND_ASYNC);
	while (!end)
	{
		cls();
		std::cout << "You made it safe back home,\n victory achieved!!\n now u can rest...";
		for (int x = 0; x <= times; ++x)
			std::cout << std::endl;
		std::ifstream file("./Graphics/victory.txt");
		std::string str;
		while (std::getline(file, str))
			std::cout << str << std::endl;
		file.close();
		Sleep(500);
		++times;
		if (times >= 6)
			end = true;
	}
	exit(EXIT_SUCCESS);
	gFlee = true;
}
