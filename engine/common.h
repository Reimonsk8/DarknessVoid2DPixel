#ifndef COMMON_H
#define COMMON_H
#include <QString>
#include <QTextEdit>

//REVIEW [STRUCT][CONVENTION] : Please comply with the code convention
/*
 *
     enum Letter
     {
        L_A = 65,
        L_E = 69,
        L_F,
        L_H = 72,
        L_L = 76,
        L_O = 79,
        L_P,
        L_S = 83,
        L_W = 87,
        L_X,
        L__ = 95
     };
 *
 */
enum Letter {L_A = 65, L_E = 69, L_F, L_H = 72, L_L = 76, L_O = 79, L_P, L_S = 83, L_W = 87, L_X, L__ = 95};
enum Type {T_Head, T_Chest, T_Weapon, T_Consumable};
enum PotionCode { P_1 = 49, P_2, P_3, P_4, P_5 };
enum Buttons {B_Atack, B_Potion, B_Flee, B_Right, B_Down, B_Left, B_Up, B_Inspect};
enum States {S_Normal, S_WaitForArrowKeys, S_ActionToPickItem, S_BattleOn, S_BattleOnPotionUsed, S_GameOver, S_Win}; // progress trough event handler

static const short OUT_OF_RANGE = -1;
static const short MAX_INVENTORY_SIZE = 8;
static const short POTION_START_INDEX = 3;
static const short PIXEL = 750;



//REVIEW [VAR][ENCAPSULATION][Karla]: Instead of using non-const variables as global, these could be
// abstracted and be placed in its respective class/struct.

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

//REVIEW [IMP]: what is the point of encapsulating a char[][]? Structs should be used for data arrangement
//              here is only serving as a mere container/wrapper
struct Map
{
	char grid[30][30];
};

//REVIEW [VAR][NAME] : Use proper naming for variables, lvl? what does that mean even.
extern Map lvl;
extern Map gExplored;

#endif //COMMON_H
