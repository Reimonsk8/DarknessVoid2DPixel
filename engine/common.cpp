#include <string>
#include "common.h"
#include "../mainwindow.h"

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

// Global MainWindow pointer for log access
MainWindow* gMainWindow = nullptr;

// Global log function implementation
void addStyledLogEntry(const QString& text, bool isCurrentCommand)
{
    if (gMainWindow) {
        gMainWindow->addLogEntry(text, isCurrentCommand);
    } else {
        // Fallback to old system if main window not available
        mLogContent.prepend(text);
        if (mScrollLog) {
            mScrollLog->setText(mLogContent);
        }
    }
}
