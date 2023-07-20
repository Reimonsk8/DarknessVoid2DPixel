#ifndef GRAPHICS_H
#define GRAPHICS
#include "common.h"
#include "Character.h"
#include <iostream>
#include <Windows.h>
//REVIEW [IMP][Karla]: Instead of declaring these functions as global, they could live in its own class/struct. 
// Or if they are utility functions, put them in a namespace to avoid global namespace pollution.

static const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void cls(bool invCLS = false);

void setConsole();

void drawScreen(Character &hero, bool inventory = false);

void printTitle();

void gameOver();

void victory();
#endif //GRAPHICS_H
