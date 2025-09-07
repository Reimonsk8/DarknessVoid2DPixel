#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QToolButton>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
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
    
    // Log styling helper functions
    void addLogEntry(const QString& text, bool isCurrentCommand = false);
    void updateLogDisplay();
    
    // Auto-save system
    void enableAutoSave(bool enabled = true);
    void performAutoSave();
    void setAutoSaveInterval(int minutes);
    
    // Settings menu
    void showSettingsMenu();
    void applySettings();
    void showHelpMenu();
    
    // UI improvements
    void updateStatusBar();
    
    // Quick action hotkeys
    void toggleInventory();
    void quickSave();
    void quickLoad();
    void toggleMusic();
    
    // Game state management
    void resetGameState();
    void cleanupGameObjects();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    enum SaveFormat {
        Json, 
        Binary
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

    // Button key bindings - initialized in constructor
    int mButtonAtack;
    int mButtonPotion;
    int mButtonFlee;
    int mButtonLook;
    int mButtonUp;
    int mButtonLeft;
    int mButtonRight;
    int mButtonDown;

    QString mDefaultPath;

    // Log styling variables
    QStringList mLogEntries;
    int mCurrentCommandIndex;
    
    // Auto-save variables
    QTimer* autoSaveTimer;
    bool autoSaveEnabled;
    int autoSaveIntervalMinutes;
    
    // Settings variables
    float masterVolume;
    float musicVolume;
    float sfxVolume;
    bool screenShakeEnabled;
    bool damageFlashEnabled;
    
    // UI elements
    QStatusBar* statusBar;
    QLabel* healthLabel;
    QLabel* positionLabel;
    QLabel* autoSaveLabel;

    // Image resources
    QString mWeaponicon;
    QString mHelmeticon;
    QString mArmoricon;
    QString mPotionicon;
    QString mGameover;
    QString mWin;


};

#endif // MAINWINDOW_H

