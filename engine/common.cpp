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

// Global scoring function implementation
void notifyEnemyDefeated(const QString& enemyName, int enemyDifficulty)
{
    qDebug() << "notifyEnemyDefeated called with enemy:" << enemyName << "difficulty:" << enemyDifficulty;
    
    if (gMainWindow) {
        qDebug() << "gMainWindow is valid, calling onEnemyDefeated";
        try {
            gMainWindow->onEnemyDefeated(enemyName, enemyDifficulty);
            qDebug() << "onEnemyDefeated completed successfully";
        } catch (const std::exception& e) {
            qDebug() << "Exception in notifyEnemyDefeated:" << e.what();
        } catch (...) {
            qDebug() << "Unknown exception in notifyEnemyDefeated";
        }
    } else {
        qDebug() << "MainWindow not available for scoring notification";
    }
}

// Global log function implementation
void addStyledLogEntry(const QString& text, bool isCurrentCommand)
{
    if (gMainWindow) {
        try {
            gMainWindow->addLogEntry(text, isCurrentCommand);
        } catch (...) {
            // MainWindow might be in destruction process, use fallback
            qDebug() << "Exception in addStyledLogEntry, using fallback";
            mLogContent.prepend(text);
            if (mScrollLog) {
                try {
                    mScrollLog->setText(mLogContent);
                } catch (...) {
                    // Even fallback failed, just log to debug
                    qDebug() << "Fallback also failed for log entry:" << text;
                }
            } else {
                // Both gMainWindow and mScrollLog are null, just log to debug
                qDebug() << "Both MainWindow and ScrollLog are null, logging to debug:" << text;
            }
        }
    } else {
        // Fallback to old system if main window not available
        mLogContent.prepend(text);
        if (mScrollLog) {
            try {
                mScrollLog->setText(mLogContent);
            } catch (...) {
                qDebug() << "Fallback system also failed for log entry:" << text;
            }
        } else {
            // Both gMainWindow and mScrollLog are null, just log to debug
            qDebug() << "Both MainWindow and ScrollLog are null, logging to debug:" << text;
        }
    }
}
