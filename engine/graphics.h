#ifndef GRAPHICS_H
#define GRAPHICS
#include "common.h"
#include "Character.h"
#include <iostream>
#include <Windows.h>

// Console utility functions
static const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void cls(bool invCLS = false);
void setConsole();
void drawScreen(Character &hero, bool inventory = false);
void printTitle();

void gameOver();

void victory();
#endif //GRAPHICS_H
