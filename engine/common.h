#ifndef COMMON_H
#define COMMON_H
#include <QString>
#include <QTextEdit>

// Game enums and constants
enum Letter {L_A = 65, L_E = 69, L_F, L_H = 72, L_L = 76, L_O = 79, L_P, L_S = 83, L_W = 87, L_X, L__ = 95};
enum Type {T_Head, T_Chest, T_Weapon, T_Consumable};
enum PotionCode { P_1 = 49, P_2, P_3, P_4, P_5 };
enum Buttons {B_Atack, B_Potion, B_Flee, B_Right, B_Down, B_Left, B_Up, B_Inspect};
enum States {S_Normal, S_WaitForArrowKeys, S_ActionToPickItem, S_BattleOn, S_BattleOnPotionUsed, S_GameOver, S_Win};

static const short OUT_OF_RANGE = -1;
static const short MAX_INVENTORY_SIZE = 8;
static const short POTION_START_INDEX = 3;
static const short PIXEL = 750;

// Global game state variables
extern int gStep;
extern int gState;
extern int gValueButton;
extern QString gShortcut;
extern QString mLogContent;
extern QTextEdit *mScrollLog;
extern short gHeight;
extern short gWidth;
extern short gMaxHP;
extern short gAP;
extern bool gFlee;

// Map structure for game world
struct Map
{
	char grid[30][30];
};

extern Map lvl;
extern Map gExplored;

// Forward declaration
class MainWindow;

// Global log function for styled logging
void addStyledLogEntry(const QString& text, bool isCurrentCommand = false);

// Global scoring function for enemy defeats
void notifyEnemyDefeated(const QString& enemyName, int enemyDifficulty);

#endif //COMMON_H
