#include "stdafx.h"
#include <time.h>
#include "Generator.h"
#include "GameManager.h"

int main(int argc, _TCHAR* argv[])
{
	srand(time(NULL));
	GameManager::readFiles();

	/*initialize hero & generate items and enemies*/
	Character hero;
	Generator generated;
	hero.setBaseValues(gAP,gMaxHP);

	GameManager::startMenu(hero);
	generated.SpawnInMap();

	/*maing game loop*/
	while (hero.getHP()>0)
	{
		GameManager::handleInput(hero, generated);
	}
	//gameOver();

	return 0;
}

