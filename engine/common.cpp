#include <string>
#include "common.h"

//QString gShortcut = "C:\\Users\\SRT\\Documents\\Visual Studio 2013\\Projects\\ToolsCatchup\\j.armenta\\QtCatchUp\\engine";
QString gShortcut = "D:\\GitHubProjects\\QtCatchUp\\engine";
QString mLogContent = "";
QTextEdit *mScrollLog = nullptr;
int gStep = PIXEL/gWidth;
int gState = 0;
int gValueButton=-1;
short gHeight = 30;
short gWidth = 30;
short gMaxHP = 100;
short gAP = 10;
bool gFlee = true;
Map lvl;
Map gExplored;
