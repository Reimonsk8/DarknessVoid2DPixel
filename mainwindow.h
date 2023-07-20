#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//REVIEW : [STRUCT][UNUSUED_INCLUSION] :QGraphicsScene is not being used anyware in the header file. Do not include
//         files that are not being used to avoid an increment in compilation time.
#include <QGraphicsScene>
#include <QSignalMapper>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QToolButton>
#include <QLabel>
#include "Graphics.h"
#include "Generator.h"
#include "soundengine.h"

enum EMapStart{
    E_Easy,
    E_Medium,
    E_Hard,
    E_Custom
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event);
    void inputHandle(QKeyEvent *event = nullptr ,const int direction = -1);
    void gameEventHandle();
    void updateInventory();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    //REVIEW [STRUCT][CONVENTION] : Don't mix curly brackets usage,
    /*
     * Correct way:
        enum SaveFormat
        {
            Json, Binary
        };
     *
    */
    enum SaveFormat {
        Json, Binary
    };

protected:

    bool eventFilter(QObject *obj, QEvent *ev);

private slots:

    void newFileDialog();
    void loadMapFile(int mode = 1);
    bool loadFileDialog(SaveFormat saveFormat=Json);
    bool saveFileDialog(SaveFormat saveFormat=Json) const;
    void exitDialog();
    void configControllersDialog();
    void changeFilePath();
    void changePWD();
    void idle();
    void buttonPressed(const int &buttonType);

private:

    Generator *Generated;

    QWidget *setupContainerVertical();
    QWidget *setupStatusBar();
    QWidget *setupLog();
    QWidget *setupContainerHorizontal();
    QWidget *setupMovementArrows();
    QWidget *setupMapDisplay();
    QWidget *setupInventory();

    //REVIEW [VAR][NAME] : please comply with the convention prefix for private variables.
    QTimer *timer;
    Graphics *mDGraphics;
    QSignalMapper *mButtonSignalMap;
    QLabel *mHeroName;
    QProgressBar *mHPBar;
    QLabel *mAPLabel;
    QToolButton *mWeapon;
    QToolButton *mHelmet;
    QToolButton *mArmor;
    QToolButton *mPotion1;
    QToolButton *mPotion2;
    QToolButton *mPotion3;
    QToolButton *mPotion4;
    QToolButton *mPotion5;
    QLabel *mWeaponLabel;
    QLabel *mHelmetLabel;
    QLabel *mArmorLabel;
    QLabel *mPotion1Label;
    QLabel *mPotion2Label;
    QLabel *mPotion3Label;
    QLabel *mPotion4Label;
    QLabel *mPotion5Label;

    //REVIEW [STRUCT][CONVENTION] : why are those members being initialized in the header file? please do correct use
    //                              of varibale initialization. This should be in the initialization list.
    int mButtonAtack = Qt::Key_X;
    int mButtonPotion = Qt::Key_P;
    int mButtonFlee = Qt::Key_F;
    int mButtonLook = Qt::Key_Z;
    int mButtonUp= Qt::Key_W;
    int mButtonLeft = Qt::Key_A;
    int mButtonRight= Qt::Key_D;
    int mButtonDown = Qt::Key_S;

    QString mDefaultPath = gShortcut;

    //REVIEW [DEF][INPUT_SANITIZE] : this will only work in your pc, please put them in resources
    //REVIEW [STRUCT][CONVENTION] : why are those contant being initialized in the header file? please do correct use
    //                              of varibale initialization.
    //REVIEW [IMP] : If those variables are global what is the point of declaring them private? if every function needs
    //               to access them, then make them private member variables.
    QString mWeaponicon;
    QString mHelmeticon;
    QString mArmoricon;
    QString mPotionicon;
    const QString mGameover = ":/Images/gameover.png";
    const QString mWin = ":/Images/win.png";


};

#endif // MAINWINDOW_H

