#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include <QKeyEvent>
#include <QGraphicsPixmapItem>
#include <QFile>
#include <QTextStream>
#include "SoundEngine.h"
#include "Generator.h"
#include "Character.h"

class GameManager
{
public:

    GameManager();

    static void readStdString(std::string filePath);

    static void readQString(QString filePath);

    static bool startMenu(Character &hero);

	static int decode(int code);

    static void handleInput(Character &hero, Generator &generated);

    static void handleEvent(Character &hero, Generator &generated);

private:


};
#endif //GAMEMANAGER_H
