#include <QLabel>
#include <QSplitter>
#include <QTabWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QFrame>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QComboBox>
#include <QToolButton>
#include <QJsonDocument>
#include <QButtonGroup>
#include <QDialog>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTimer>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QDirIterator>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QSpinBox>
#include <QTextEdit>
#include <QJsonObject>
#include <QJsonArray>
#include <QEvent>
#include <QPixmap>
#include <QGraphicsScene>
#include <QMediaPlayer>
#include <QSoundEffect>
#include <QAudioOutput>
#include "SoundEngine.h"
#include "mainwindow.h"
#include "Graphics.h"
#include "GameManager.h"

#define MINUTE 600000 // 10 minutes in milliseconds
#define DEFAULT_MAP_SIZE 100


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), timer(new QTimer(this))
{
    setCentralWidget(setupContainerVertical());
    connect(timer, &QTimer::timeout, this, &MainWindow::idle);
    timer->start(MINUTE/10);
    
    // Initialize button key bindings
    mButtonAtack = Qt::Key_X;
    mButtonPotion = Qt::Key_P;
    mButtonFlee = Qt::Key_F;
    mButtonLook = Qt::Key_Z;
    mButtonUp = Qt::Key_W;
    mButtonLeft = Qt::Key_A;
    mButtonRight = Qt::Key_D;
    mButtonDown = Qt::Key_S;
    
    mDefaultPath = gShortcut;
    
    // Initialize const image resources
    mGameover = ":/Images/gameover.png";
    mWin = ":/Images/win.png";
    
    // Initialize log styling variables
    mLogEntries.clear();
    mCurrentCommandIndex = -1;
    
    // Initialize auto-save system
    autoSaveTimer = new QTimer(this);
    autoSaveEnabled = true;
    autoSaveIntervalMinutes = 2; // Auto-save every 2 minutes
    connect(autoSaveTimer, &QTimer::timeout, this, &MainWindow::performAutoSave);
    autoSaveTimer->start(autoSaveIntervalMinutes * 60 * 1000); // Convert minutes to milliseconds
    
    // Initialize settings
    masterVolume = 1.0f;
    musicVolume = 0.7f;
    sfxVolume = 1.0f;
    screenShakeEnabled = true;
    damageFlashEnabled = true;
    
    // Initialize status bar
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    
    healthLabel = new QLabel("Health: --/--");
    positionLabel = new QLabel("Position: --, --");
    autoSaveLabel = new QLabel("Auto-save: ON");
    
    statusBar->addWidget(healthLabel);
    statusBar->addWidget(positionLabel);
    statusBar->addPermanentWidget(autoSaveLabel);
    
    // Set global pointer for log access
    extern MainWindow* gMainWindow;
    gMainWindow = this;
}

MainWindow::~MainWindow()
{
}

/*Member functions*/
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    inputHandle(event);
}

void MainWindow::inputHandle(QKeyEvent *event, const int input)
{
    timer->start(2*MINUTE);
    if (mDGraphics->Hero == nullptr) return;
    gValueButton = input;
    /*reassign value if key pressed*/
    if (event != nullptr)
    {
        if (event->key() == mButtonRight) gValueButton = B_Right;
        else if(event->key() == mButtonDown) gValueButton = B_Down;
        else if (event->key() == mButtonLeft) gValueButton = B_Left;
        else if (event->key() == mButtonUp) gValueButton = B_Up;
        else if(event->key() == mButtonAtack) gValueButton = B_Atack;
        else if (event->key() == mButtonPotion) gValueButton = B_Potion;
        else if (event->key() == mButtonFlee) gValueButton = B_Flee;
        // Additional hotkeys for quick actions
        else if (event->key() == Qt::Key_Space) gValueButton = B_Atack; // Space for attack
        else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) gValueButton = B_Potion; // Enter for potion
        else if (event->key() == Qt::Key_Escape) gValueButton = B_Flee; // Escape for flee
        else if (event->key() == Qt::Key_I) {
            // Quick inventory toggle (I key)
            toggleInventory();
            return;
        }
        else if (event->key() == Qt::Key_S) {
            // Quick save (S key)
            quickSave();
            return;
        }
        else if (event->key() == Qt::Key_L) {
            // Quick load (L key)
            quickLoad();
            return;
        }
        else if (event->key() == Qt::Key_M) {
            // Toggle music (M key)
            toggleMusic();
            return;
        }
        else if (event->key() == Qt::Key_A) {
            // Toggle auto-save (A key)
            enableAutoSave(!autoSaveEnabled);
            return;
        }
        else if (event->key() == Qt::Key_F1) {
            // Show settings menu (F1 key)
            showSettingsMenu();
            return;
        }
        else if (event->key() == Qt::Key_F2) {
            // Show help menu (F2 key)
            showHelpMenu();
            return;
        }
        else gValueButton = event->key();
    }
    if (gFlee && (gState != S_WaitForArrowKeys) && (gState != S_GameOver) && (gState != S_Win) )//if not in battle move or wating for input
    {
        if (gValueButton == B_Right)
        {
            // Move right: walk(0,1) means no vertical change, +1 horizontal
            mDGraphics->Hero->walk(0,1);
            addStyledLogEntry("you move right\n", false);
        }
        else if(gValueButton == B_Down)
        {
            mDGraphics->Hero->walk(1,0);
            addStyledLogEntry("you moved down\n", false);
        }
        else if(gValueButton == B_Left)
        {
            mDGraphics->Hero->walk(0,-1);
            addStyledLogEntry("you moved left\n", false);
        }
        else if(gValueButton == B_Up)
        {
            mDGraphics->Hero->walk(-1,0);
            addStyledLogEntry("you moved up\n", false);
        }
        if (gState == S_ActionToPickItem)
        {
            gState = S_Normal;
        }
        mDGraphics->drawPosition();
        updateStatusBar(); // Update status bar after movement
    }
    //qDebug() << "0 normal 1 key input 2 item pickup \n";
    /* handle any current position action */
    try {
        gameEventHandle();
        qDebug() << "gameEventHandle completed in inputHandle";
    } catch (...) {
        qDebug() << "Exception in gameEventHandle from inputHandle";
    }
    
    qDebug() << "inputHandle() completed";
}

void MainWindow::gameEventHandle()
{
    qDebug() << "gameEventHandle() called";
    
    if(mDGraphics->Hero == nullptr) {
        qDebug() << "Hero is null, returning from gameEventHandle";
        return;
    }
    
    qDebug() << "Current game state:" << gState;
    
    if (gState == S_GameOver)
    {
        qDebug() << "Handling game over state";
        SoundEngine::PlaySoundByName("gameover", 1);
        QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(mGameover).scaled(PIXEL,PIXEL));
        newItem->setPos(0,0);
        mDGraphics->scene()->addItem(newItem);
        QMessageBox *myDialog = new QMessageBox(this);
        myDialog->setText("Your body lies in a pool of blood while the enemy eats your flesh and bones");
        myDialog->setToolTip("Game over");
        
        // Proper cleanup - delete objects instead of calling destructors directly
        if (mDGraphics->Hero) {
            delete mDGraphics->Hero;
            mDGraphics->Hero = nullptr;
        }
        if (Generated) {
            // First, nullify the Graphics pointer to Generated to prevent dangling reference
            if (mDGraphics) {
                mDGraphics->Generated = nullptr;
            }
            // Skip deletion to avoid crash - will investigate
            Generated = nullptr;
        }
        
        myDialog->exec();
    }
    else if (gState == S_Win)
    {
        qDebug() << "Handling win state";
        QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(mWin).scaled(PIXEL,PIXEL));
        newItem->setPos(0,0);
        mDGraphics->scene()->addItem(newItem);
        QMessageBox *myDialog = new QMessageBox(this);
        myDialog->setText("You made its safe to the forest and now u can rest at the bonfire, you live another Day!!");
        myDialog->setToolTip("Victory!!");
        myDialog->exec();
    }
    else
    {
        qDebug() << "Handling normal game state";
        try {
            GameManager::handleEvent(*mDGraphics->Hero,*Generated);
            qDebug() << "GameManager::handleEvent completed";
        } catch (...) {
            qDebug() << "Exception in GameManager::handleEvent";
        }
        
        try {
            updateInventory();
            qDebug() << "updateInventory completed";
        } catch (...) {
            qDebug() << "Exception in updateInventory";
        }
        
        try {
            updateStatusBar(); // Update status bar after game events
            qDebug() << "updateStatusBar completed";
        } catch (...) {
            qDebug() << "Exception in updateStatusBar";
        }
    }
    
    qDebug() << "gameEventHandle() completed";
}

void MainWindow::updateInventory()
{
    qDebug() << "updateInventory() called";
    
    const int dim = 40;
    if(mDGraphics->Hero == nullptr) {
        qDebug() << "Hero is null in updateInventory, returning";
        return;
    }
    
    qDebug() << "Hero inventory size:" << mDGraphics->Hero->inventorySize();

    int invSize = mDGraphics->Hero->inventorySize();

    if(invSize >= 1)
    {
        Equipment myHelmet = mDGraphics->Hero->selectItem(T_Head);
        QString helmName = QString::fromStdString(myHelmet.getName());
        mHelmeticon = mDGraphics->getEquipmentGraphic(helmName, T_Head);
        mHelmet->setIcon(QIcon(QPixmap(mHelmeticon).scaled(dim,dim)));
        QString temp = " MaxHP:"+ QString::number(myHelmet.getMaxHP());
        mHelmetLabel->setText(QString::fromStdString(myHelmet.getName())+temp);
    }
    else
    {
        mHelmetLabel->setText("");
        mHelmet->setIcon(QIcon());
    }
    if(invSize >= 2)
    {
        Equipment myArmor = mDGraphics->Hero->selectItem(T_Chest);
        QString armorName = QString::fromStdString(myArmor.getName());
        mArmoricon = mDGraphics->getEquipmentGraphic(armorName, T_Chest);
        mArmor->setIcon(QIcon(QPixmap(mArmoricon).scaled(dim,dim)));
        QString temp = " MaxHP:"+ QString::number(myArmor.getMaxHP());
        mArmorLabel->setText(QString::fromStdString(myArmor.getName())+temp);
    }
    else
    {
        mArmorLabel->setText("");
        mArmor->setIcon(QIcon());
    }
    if(invSize >= 3)
    {
        Equipment myWeapon= mDGraphics->Hero->selectItem(T_Weapon);
        QString weaponName = QString::fromStdString(myWeapon.getName());
        mWeaponicon = mDGraphics->getEquipmentGraphic(weaponName, T_Weapon);
        mWeapon->setIcon(QIcon(QPixmap(mWeaponicon).scaled(dim,dim)));
        QString temp = " AP:"+ QString::number(myWeapon.getAP());
        mWeaponLabel->setText(QString::fromStdString(myWeapon.getName())+temp);
    }
    else
    {
        mWeaponLabel->setText("");
        mWeapon->setIcon(QIcon());
    }
    if(invSize >= 4)
    {
        Equipment myItem = mDGraphics->Hero->selectItem(invSize - 1);
        QString ItemName = QString::fromStdString(myItem.getName());
        mPotionicon = mDGraphics->getEquipmentGraphic(ItemName, T_Consumable);
        mPotion1->setIcon(QIcon(QPixmap(mPotionicon).scaled(dim,dim)));
        QString temp = " HP:"+ QString::number(mDGraphics->Hero->selectItem(3).getMaxHP());
        mPotion1Label->setText(QString::fromStdString(mDGraphics->Hero->selectItem(3).getName())+temp);
    }
    else
    {
        mPotion1Label->setText("");
        mPotion1->setIcon(QIcon());
    }
    if(invSize >= 5)
    {
        Equipment myItem = mDGraphics->Hero->selectItem(invSize-1);
        QString ItemName = QString::fromStdString(myItem.getName());
        mPotionicon = mDGraphics->getEquipmentGraphic(ItemName, T_Consumable);
        mPotion2->setIcon(QIcon(QPixmap(mPotionicon).scaled(dim,dim)));
        QString temp = " HP:"+ QString::number(mDGraphics->Hero->selectItem(4).getMaxHP());
        mPotion2Label->setText(QString::fromStdString(mDGraphics->Hero->selectItem(4).getName())+temp);
    }
    else
    {
        mPotion2Label->setText("");
        mPotion2->setIcon(QIcon());
    }
    if(invSize >= 6)
    {
        Equipment myItem = mDGraphics->Hero->selectItem(invSize-1);
        QString ItemName = QString::fromStdString(myItem.getName());
        mPotionicon = mDGraphics->getEquipmentGraphic(ItemName, T_Consumable);
        mPotion3->setIcon(QIcon(QPixmap(mPotionicon).scaled(dim,dim)));
        QString temp = " HP:"+ QString::number(mDGraphics->Hero->selectItem(5).getMaxHP());
        mPotion3Label->setText(QString::fromStdString(mDGraphics->Hero->selectItem(5).getName())+temp);
    }
    else
    {
        Equipment myItem = mDGraphics->Hero->selectItem(invSize-1);
        QString ItemName = QString::fromStdString(myItem.getName());
        mPotionicon = mDGraphics->getEquipmentGraphic(ItemName, T_Consumable);
        mPotion3Label->setText("");
        mPotion3->setIcon(QIcon());
    }
    if(invSize >= 7)
    {
        Equipment myItem = mDGraphics->Hero->selectItem(invSize-1);
        QString ItemName = QString::fromStdString(myItem.getName());
        mPotionicon = mDGraphics->getEquipmentGraphic(ItemName, T_Consumable);
        mPotion4->setIcon(QIcon(QPixmap(mPotionicon).scaled(dim,dim)));
        QString temp = " HP:"+ QString::number(mDGraphics->Hero->selectItem(6).getMaxHP());
        mPotion4Label->setText(QString::fromStdString(mDGraphics->Hero->selectItem(6).getName())+temp);
    }
    else
    {
        mPotion4Label->setText("");
        mPotion4->setIcon(QIcon());
    }
    if(invSize == 8)
    {
        Equipment myItem = mDGraphics->Hero->selectItem(invSize-1);
        QString ItemName = QString::fromStdString(myItem.getName());
        mPotionicon = mDGraphics->getEquipmentGraphic(ItemName, T_Consumable);
        mPotion5->setIcon(QIcon(QPixmap(mPotionicon).scaled(dim,dim)));
        QString temp = " HP:"+ QString::number(mDGraphics->Hero->selectItem(7).getMaxHP());
        mPotion5Label->setText(QString::fromStdString(mDGraphics->Hero->selectItem(7).getName())+temp);
    }
    else
    {
        mPotion5Label->setText("");
        mPotion5->setIcon(QIcon());
    }

    /*udpate healthbar*/
    mHPBar->setRange(0,mDGraphics->Hero->getMaxHP());
    if (mDGraphics->Hero->getHP()<0)
            mHPBar->setValue(0);
    else
        mHPBar->setValue(mDGraphics->Hero->getHP());
    QString temp;
    temp.append("HP: ");
    temp.append(QString::number(mDGraphics->Hero->getHP()));
    temp.append(" / MaxHP: ");
    temp.append(QString::number(mDGraphics->Hero->getMaxHP()));
    mHPBar->setFormat(temp);
    temp = "";
    temp.append("AP: ");
    temp.append(QString::number(mDGraphics->Hero->getAP()));
    mAPLabel->setText(temp);
}

void MainWindow::read(const QJsonObject &json)
{
    // Load basic game state
    gState = json["state"].toInt();
    mDefaultPath = json["defaultPath"].toString();
    mLogContent = json["logContent"].toString();
    gStep = json["step"].toInt();
    gHeight = json["H"].toInt();
    gWidth = json["W"].toInt();
    gFlee = json["flee"].toBool();
    
    // Load Hero data if exists
    if (json.contains("hero") && json["hero"].isObject()) {
        QJsonObject heroData = json["hero"].toObject();
        
        // Clean up existing hero
        if (mDGraphics && mDGraphics->Hero) {
            delete mDGraphics->Hero;
            mDGraphics->Hero = nullptr;
        }
        
        // Create new hero
        Character *hero = new Character();
        hero->setName(heroData["name"].toString().toStdString());
        hero->setHP(heroData["hp"].toInt());
        hero->setMaxHP(heroData["maxHp"].toInt());
        hero->setAP(heroData["ap"].toInt());
        hero->heroRow = heroData["row"].toInt();
        hero->heroCol = heroData["col"].toInt();
        
        // Load inventory
        if (heroData.contains("inventory") && heroData["inventory"].isArray()) {
            QJsonArray inventory = heroData["inventory"].toArray();
            for (int i = 0; i < inventory.size(); ++i) {
                QJsonObject itemData = inventory[i].toObject();
                // Note: This is simplified - you'd need proper Equipment constructor
                // Equipment item(itemData["name"].toString().toStdString(), 
                //               itemData["ap"].toInt(), 
                //               itemData["maxHp"].toInt(), 
                //               true);
                // hero->addToInventory(item);
            }
        }
        
        mDGraphics->Hero = hero;
        mHeroName->setText("Name: " + QString::fromStdString(hero->getName()));
        mHPBar->setValue(hero->getHP());
    }
    
    // Load log entries
    if (json.contains("logEntries") && json["logEntries"].isArray()) {
        QJsonArray logEntries = json["logEntries"].toArray();
        mLogEntries.clear();
        for (int i = 0; i < logEntries.size(); ++i) {
            mLogEntries.append(logEntries[i].toString());
        }
        mCurrentCommandIndex = json["currentCommandIndex"].toInt();
        updateLogDisplay();
    }
    
    // Update UI
    updateInventory();
    updateStatusBar();
    
    // Redraw graphics
    if (mDGraphics) {
        mDGraphics->drawMapFullStatic();
        mDGraphics->drawPosition();
    }
}

void MainWindow::write(QJsonObject &json) const
{
    // Basic game state
    json["state"] = gState;
    json["defaultPath"] = mDefaultPath;
    json["logContent"] = mLogContent;
    json["step"] = gStep;
    json["H"] = gHeight;
    json["W"] = gWidth;
    json["flee"] = gFlee;
    
    // Save Hero data if exists
    if (mDGraphics && mDGraphics->Hero) {
        QJsonObject heroData;
        heroData["name"] = QString::fromStdString(mDGraphics->Hero->getName());
        heroData["hp"] = mDGraphics->Hero->getHP();
        heroData["maxHp"] = mDGraphics->Hero->getMaxHP();
        heroData["ap"] = mDGraphics->Hero->getAP();
        heroData["row"] = mDGraphics->Hero->heroRow;
        heroData["col"] = mDGraphics->Hero->heroCol;
        
        // Save inventory
        QJsonArray inventory;
        for (int i = 0; i < mDGraphics->Hero->inventorySize(); ++i) {
            Equipment item = mDGraphics->Hero->selectItem(i);
            QJsonObject itemData;
            itemData["name"] = QString::fromStdString(item.getName());
            itemData["ap"] = item.getAP();
            itemData["maxHp"] = item.getMaxHP();
            itemData["type"] = i; // Use index as type for now
            inventory.append(itemData);
        }
        heroData["inventory"] = inventory;
        
        json["hero"] = heroData;
    }
    
    // Save Generator/Map data if exists
    if (Generated) {
        QJsonObject generatorData;
        // Add generator-specific data here when available
        json["generator"] = generatorData;
    }
    
    // Save log entries
    QJsonArray logEntries;
    for (const QString& entry : mLogEntries) {
        logEntries.append(entry);
    }
    json["logEntries"] = logEntries;
    json["currentCommandIndex"] = mCurrentCommandIndex;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
      if(ev->type() == QEvent::KeyPress || ev->type() == QEvent::MouseMove)
      {
            timer->start(1000);
       }
   return QObject::eventFilter(obj, ev);
}

/*Widgets*/
QWidget *MainWindow::setupContainerVertical()
{
    resize(1200,1000);
    //setMinimumSize(1300,850);
    QMenuBar *mainMenuBar = menuBar();

    QMenu *fileMenu = mainMenuBar->addMenu("File");
    QAction *newMenu= fileMenu->addAction("New game");
    connect(newMenu, SIGNAL (triggered()),this, SLOT (newFileDialog()));
    QAction *saveMenu= fileMenu->addAction("Save game");
    connect(saveMenu, SIGNAL (triggered()),this, SLOT (saveFileDialog()));
    QAction *loadMenu= fileMenu->addAction("Load game");
    connect(loadMenu, SIGNAL (triggered()),this, SLOT (loadFileDialog()));
    QAction *exitMenu= fileMenu->addAction("Exit game");
    connect(exitMenu, SIGNAL (triggered()),this, SLOT (exitDialog()));
    QMenu *settingsMenu = mainMenuBar->addMenu("Settings");
    QAction *controllersMenu = settingsMenu->addAction("Key configuration");
    connect(controllersMenu, SIGNAL (triggered()),this, SLOT (configControllersDialog()));
    QAction *savePathMenu = settingsMenu->addAction("Save path");
    connect(savePathMenu, SIGNAL (triggered()),this, SLOT (changeFilePath()));
    QAction *setPWDMenu = settingsMenu->addAction("Set PWD");
    connect(setPWDMenu, SIGNAL (triggered()),this, SLOT (changePWD()));
    QWidget *center = new QWidget(this);

    QWidget *statusBar = setupStatusBar();
    QWidget *inventory = setupInventory();
    QWidget *centerStack = setupContainerHorizontal();

    /*Using the layout*/
    QVBoxLayout *verticalLayout = new QVBoxLayout(center);
    /**************/
    /*            */
    /**************/
    /*            */
    /**************/
    /*            */
    /**************/

    verticalLayout->addWidget(inventory,0);
    verticalLayout->addWidget(statusBar, 0);
    verticalLayout->addWidget(centerStack,1);
    return center;
}

QWidget *MainWindow::setupStatusBar()
{
    QWidget *center = new QWidget(this);

    QLabel *statusBarLabel = new QLabel("Status:", this);
    statusBarLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    mHeroName = new QLabel(this);
    mHPBar = new QProgressBar(this);
    mHPBar->setRange(0,100);
    mHPBar->text()+"HP:";
    mHPBar->setOrientation(Qt::Horizontal);
    mHPBar->setFormat("/100");
    mAPLabel = new QLabel("AP:", this);

    QHBoxLayout *horizontalLayout = new QHBoxLayout(center);
    horizontalLayout->addWidget(mHeroName);
    horizontalLayout->addWidget(statusBarLabel);
    horizontalLayout->addWidget(mAPLabel);
    horizontalLayout->addWidget(mHPBar);


    return center;

}

QWidget *MainWindow::setupLog()
{
    // Create the QTextEdit widget
    mScrollLog = new QTextEdit(this);
    mScrollLog->setReadOnly(true);
    mScrollLog->setFontPointSize(12);
    
    // Set black background for the log
    mScrollLog->setStyleSheet("QTextEdit { background-color: black; color: white; }");
    
    // Initialize the log with the first entry
    addLogEntry("Load a file to begin..", true); // This will be the current command initially
    
    return mScrollLog;
}

QWidget* MainWindow::setupContainerHorizontal()
{
    QWidget *center = new QWidget(this);
    QWidget* movementArrows = setupMovementArrows();
    QWidget *mapDisplay = setupMapDisplay();
    QWidget *scrollLog = setupLog();

    QHBoxLayout *horizontalLayout = new QHBoxLayout(center);
    /****************/
    /*    /    /    */
    /*    /    /    */
    /*    /    /    */
    /****************/
    horizontalLayout->addWidget(movementArrows,0);
    horizontalLayout->addWidget(mapDisplay,2);
    horizontalLayout->addWidget(scrollLog,1);

    mapDisplay->setMinimumSize(gStep*gWidth, gStep*gHeight);
    return center;
}


QWidget *MainWindow::setupMapDisplay()
{
    setMinimumSize(PIXEL,PIXEL);

    QGraphicsScene *graphicsScene = new QGraphicsScene(this);
    graphicsScene->setBackgroundBrush(QBrush(Qt::gray, Qt::SolidPattern));
    graphicsScene->setSceneRect(0,0,PIXEL,PIXEL);

    mDGraphics = new Graphics();
    mDGraphics->setFocus();
    graphicsScene->addItem(mDGraphics);


    QGraphicsView* graphicsView = new QGraphicsView(graphicsScene, this);
    graphicsView->setRenderHint(QPainter::Antialiasing);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setScene(graphicsScene);
    graphicsView->setFixedSize(PIXEL, PIXEL);

    return graphicsView;
}

QWidget *MainWindow::setupInventory()
{
    QWidget *widgetsContainer = new QWidget(this);
    QHBoxLayout *inventory = new QHBoxLayout(widgetsContainer);

    QLabel *inventoryLabel = new QLabel("Inventory:", this);
    inventoryLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    mWeapon = new QToolButton(this);
    mHelmet = new QToolButton(this);
    mArmor = new QToolButton(this);
    mPotion1 = new QToolButton(this);
    mPotion2 = new QToolButton(this);
    mPotion3 = new QToolButton(this);
    mPotion4 = new QToolButton(this);
    mPotion5 = new QToolButton(this);

    mWeaponLabel = new QLabel(this);
    mHelmetLabel = new QLabel(this);
    mArmorLabel = new QLabel(this);
    mPotion1Label = new QLabel(this);
    mPotion2Label= new QLabel(this);
    mPotion3Label  = new QLabel(this);
    mPotion4Label = new QLabel(this);
    mPotion5Label = new QLabel(this);

    inventory->addWidget(inventoryLabel);
    inventory->addWidget(mWeapon);
    inventory->addWidget(mWeaponLabel);
    inventory->addWidget(mHelmet);
    inventory->addWidget(mHelmetLabel);
    inventory->addWidget(mArmor);
    inventory->addWidget(mArmorLabel);
    inventory->addWidget(mPotion1);
    inventory->addWidget(mPotion1Label);
    inventory->addWidget(mPotion2);
    inventory->addWidget(mPotion2Label);
    inventory->addWidget(mPotion3);
    inventory->addWidget(mPotion3Label);
    inventory->addWidget(mPotion4);
    inventory->addWidget(mPotion4Label);
    inventory->addWidget(mPotion5);
    inventory->addWidget(mPotion5Label);

    return widgetsContainer;
}

/*Slots*/

void MainWindow::exitDialog()
{
    QMessageBox *myDialog = new QMessageBox(this);
    myDialog->setText("Are you sure to exit game");
    myDialog->setToolTip("Exit dialog");
    myDialog->exec();
    close();
}

void MainWindow::configControllersDialog()
{
    QDialog * d = new QDialog();
    d->setToolTip("Enter your controller custom configuration");

    QVBoxLayout * vbox = new QVBoxLayout();
    QLabel * labelAction = new QLabel("Action / Atack key");
    QLineEdit * lineEditAction = new QLineEdit(QString::number(mButtonAtack));
    QLabel * labelPotion = new QLabel("Potion key");
    QLineEdit * lineEditPotion= new QLineEdit(QString::number(mButtonPotion));
    QLabel * labelFlee = new QLabel("Flee key");


    QLineEdit * lineEditFlee = new QLineEdit(QString::number(mButtonFlee));
    QLabel * labelUp = new QLabel("Move up key");
    QLineEdit * lineEditUp = new QLineEdit(QString::number(mButtonUp));
    QLabel * labelLeft = new QLabel("Move left key");
    QLineEdit * lineEditLeft = new QLineEdit(QString::number(mButtonLeft));
    QLabel * labelRight = new QLabel("Move right key");
    QLineEdit * lineEditRight = new QLineEdit(QString::number(mButtonRight));
    QLabel * labelDown = new QLabel("Move down key");
    QLineEdit * lineEditDown = new QLineEdit(QString::number(mButtonDown));

    //QKeySequence()

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                        | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), d, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), d, SLOT(reject()));

    vbox->addWidget(labelAction);
    vbox->addWidget(lineEditAction);
    vbox->addWidget(labelPotion);
    vbox->addWidget(lineEditPotion);
    vbox->addWidget(labelFlee);
    vbox->addWidget(lineEditFlee);
    vbox->addWidget(labelUp);
    vbox->addWidget(lineEditUp);
    vbox->addWidget(labelLeft);
    vbox->addWidget(lineEditLeft);
    vbox->addWidget(labelRight);
    vbox->addWidget(lineEditRight);
    vbox->addWidget(labelDown);
    vbox->addWidget(lineEditDown);
    vbox->addWidget(buttonBox);

    d->setLayout(vbox);

    int result = d->exec();
    if(result == QDialog::Accepted)
    {
        std::map<std::string, QLineEdit*> keyBindings = {
            {"Attack", lineEditAction},
            {"Potion", lineEditPotion},
            {"Flee", lineEditFlee},
            {"Up", lineEditUp},
            {"Left", lineEditLeft},
            {"Right", lineEditRight},
            {"Down", lineEditDown}
        };

        for(const auto& [action, edit] : keyBindings)
        {
            QString text = edit->text();
            QKeySequence sequence = QKeySequence::fromString(text);

            if(sequence.isEmpty())
            {
                QMessageBox::warning(this, "Invalid Key Binding",
                                     QString("Invalid key binding for %1. Please enter a valid key.").arg(QString::fromStdString(action)));
                continue;
            }

            // Assuming mButtonAttack, mButtonPotion, etc., are member variables
            if(action == "Attack")     mButtonAtack = sequence[0];
            else if(action == "Potion") mButtonPotion = sequence[0];
            else if(action == "Flee")   mButtonFlee = sequence[0];
            else if(action == "Up")     mButtonUp = sequence[0];
            else if(action == "Left")   mButtonLeft = sequence[0];
            else if(action == "Right")  mButtonRight = sequence[0];
            else if(action == "Down")   mButtonDown = sequence[0];
        }
    }
}

void MainWindow::changeFilePath()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Change default file path location"),
                                         tr("New Path: "), QLineEdit::Normal,
                                         mDefaultPath, &ok);
    if (ok && !text.isEmpty())
        mDefaultPath = text;
}

void MainWindow::changePWD()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Change current directory where image and sound files are located"),
                                         tr("Enter current PWD : "), QLineEdit::Normal,
                                         gShortcut, &ok);
    if (ok && !text.isEmpty())
        gShortcut = text;
}

void MainWindow::idle()
{
    QMessageBox *myDialog = new QMessageBox(this);
    myDialog->setText("User idle due inactivity");
    myDialog->setToolTip("AFK");
    myDialog->exec();
}

void MainWindow::newFileDialog()
{
    QDialog *myDialog = new QDialog(this);
    //myDialog->setMinimumHeight(640);
    //myDialog->setMinimumWidth(480);
    myDialog->setWindowTitle("Choose Game Difficulty");

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->setParent(myDialog);
    buttonBox->setContentsMargins(15,15,15,15);

    QPushButton *buttonEasy = new QPushButton("Easy",this);
    connect(buttonEasy, &QPushButton::clicked, myDialog, [this] { loadMapFile(EMapStart::E_Easy); });
    connect(buttonEasy, &QPushButton::clicked, myDialog, &QDialog::close);

    QPushButton *buttonMedium = new QPushButton("Medium", this);
    connect(buttonMedium, &QPushButton::clicked, myDialog, [this] { loadMapFile(EMapStart::E_Medium); });
    connect(buttonMedium, &QPushButton::clicked, myDialog, &QDialog::close);

    QPushButton *buttonHard = new QPushButton("Hard", this);
    connect(buttonHard, &QPushButton::clicked, myDialog, [this] { loadMapFile(EMapStart::E_Hard); });
    connect(buttonHard, &QPushButton::clicked, myDialog, &QDialog::close);

    QPushButton *buttonCustomLoad = new QPushButton("Load Custom", this);
    connect(buttonCustomLoad, &QPushButton::clicked, myDialog, [this] { loadMapFile(EMapStart::E_Custom); });
    connect(buttonCustomLoad, &QPushButton::clicked, myDialog, [this, myDialog] { myDialog->close(); });

    //test->setMapping(loadMapFile());
    //buttonPotion->setIcon(QPixmap(":/icons/yuv/v"));
    //->setText("Potion");

    buttonBox->addButton(buttonEasy, QDialogButtonBox::ActionRole);
    buttonBox->addButton(buttonMedium, QDialogButtonBox::ActionRole);
    buttonBox->addButton(buttonHard, QDialogButtonBox::ActionRole);
    buttonBox->addButton(buttonCustomLoad, QDialogButtonBox::ActionRole);


    //buttonBox->setGeometry(2100,40,200,50);
    buttonBox->show();
    myDialog->exec();
}


void MainWindow::loadMapFile(int mode)
{
    QVector<QString> levelsEasy;
    QVector<QString> levelsMedium;
    QVector<QString> levelsHard;
    QDirIterator it(":Levels/", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString leveltext = it.next();
        if(leveltext.contains("easy")) levelsEasy.push_back(leveltext);
        else if(leveltext.contains("normal")) levelsMedium.push_back(leveltext);
        else if(leveltext.contains("hard")) levelsHard.push_back(leveltext);
    }

    QString filePath;
    int randomValue = 0;
    switch(mode)
    {
    case EMapStart::E_Easy:
        filePath = levelsEasy.at(randomValue);//"qrc:/Levels/easy_0.txt";
        break;
    case EMapStart::E_Medium:
        filePath = levelsMedium.at(randomValue);
        break;
    case EMapStart::E_Hard:
        filePath = levelsHard.at(randomValue);
        break;
    case EMapStart::E_Custom:
        filePath = QFileDialog::getOpenFileName(this, "Search and load new input file", mDefaultPath);
        break;
    }
    QString ext = ".txt";
    QString subString = filePath.mid(filePath.size()-4, 4);
    //Verifying that the path is not empty and the directory exists
    if(ext == subString)
    {
        if(mode == EMapStart::E_Custom)
            GameManager::readStdString(filePath.toLocal8Bit().constData());
        else
            GameManager::readQString(filePath);

        // Reset game state before starting new game
        resetGameState();
        
        gStep = PIXEL/gWidth;
        /*Spawn all item locations in map*/
        Generated = new Generator;
        mDGraphics->SetGenerator(Generated);
        Generated->spawnMap();

        /*initialize hero with input information*/
        Character *hero = new Character;
        mDGraphics->Hero = hero;
        hero->setBaseValues(gAP,gMaxHP);
        mHPBar->setValue(hero->getHP());
        qDebug() << "hero setup done";

        bool ok = false;
        while(!ok)
        {
            QString nameHero = QInputDialog::getText(this, tr("Input text only no more than 20 characters"),
                                                     tr("Hero name:"), QLineEdit::Normal,
                                                     "Douglass", &ok);
            SoundEngine::PlaySoundByName("begin", 1);
            hero->setName(nameHero.toLocal8Bit().constData());
            ok = false;
            ok = GameManager::startMenu(*hero);
            mHeroName->setText("Name: " + QString::fromStdString(mDGraphics->Hero->getName()));
            mDGraphics->drawMapFullStatic();
            mDGraphics->drawPosition();
            updateInventory();
        }
    }
    else
    {
        QMessageBox *myDialog = new QMessageBox(this);
        myDialog->setText(subString + " invalid extension  try opening an .txt extension file");
        myDialog->setToolTip("error wrong input");
        myDialog->exec();
    }

}

bool MainWindow::saveFileDialog(MainWindow::SaveFormat saveFormat) const
{
    QFile saveFile(saveFormat == Json
        ? QStringLiteral("save.json")
        : QStringLiteral("save.dat"));

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't create save file.");
        QMessageBox *myDialog = new QMessageBox();
        myDialog->setToolTip("Saving your file in JSON format");
        myDialog->setText("Couldn't create save file.");
        myDialog->exec();
        return false;
    }

    QJsonObject gameObject;// fix this <-------- to make it work
    write(gameObject);
    QJsonDocument saveDoc(gameObject);
    saveFile.write(saveDoc.toJson());
    /*
    saveFile.write(saveFormat == Json
        ? saveDoc.toJson()
        : saveDoc.toVariant());
    */

    QMessageBox *myDialog = new QMessageBox();
    myDialog->setToolTip("Saving your file in JSON format");
    myDialog->setText("File Saved!!");
    myDialog->exec();

    return true;
}

bool MainWindow::loadFileDialog(MainWindow::SaveFormat saveFormat)
{
    QFile loadFile(saveFormat == Json
        ? QStringLiteral("save.json")
        : QStringLiteral("save.dat"));

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        QMessageBox *myDialog = new QMessageBox();
        myDialog->setToolTip("Loading your file in JSON format");
        myDialog->setText("Couldn't open save file.");
        myDialog->exec();
        return false;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(saveFormat == Json
        ? QJsonDocument::fromJson(saveData)
                              : QJsonDocument::fromVariant(saveData));

    read(loadDoc.object());

    QMessageBox *myDialog = new QMessageBox();
    myDialog->setToolTip("Loading your file in JSON format");
    myDialog->setText("File Loaded Sucessfully!!");
    myDialog->exec();

    return true;
}

void MainWindow::buttonPressed(const int &buttonType)
{
    inputHandle(nullptr, buttonType);
}

// Log styling helper functions
void MainWindow::addLogEntry(const QString& text, bool isCurrentCommand)
{
    // Add the new entry to our list
    mLogEntries.prepend(text);
    
    // If this is a current command, mark it
    if (isCurrentCommand) {
        mCurrentCommandIndex = 0; // Most recent entry
    } else {
        // Shift the current command index if we're adding a new entry
        if (mCurrentCommandIndex >= 0) {
            mCurrentCommandIndex++;
        }
    }
    
    // Limit the number of entries to prevent memory issues
    if (mLogEntries.size() > 50) {
        mLogEntries.removeLast();
        if (mCurrentCommandIndex >= mLogEntries.size()) {
            mCurrentCommandIndex = -1; // No current command
        }
    }
    
    updateLogDisplay();
}

void MainWindow::updateLogDisplay()
{
    QString htmlContent = "<html><body style='background-color: black; color: white; font-size: 12px;'>";
    
    for (int i = 0; i < mLogEntries.size(); ++i) {
        QString entry = mLogEntries[i];
        // Escape HTML special characters
        entry.replace("&", "&amp;");
        entry.replace("<", "&lt;");
        entry.replace(">", "&gt;");
        
        if (i == 0) {
            // Most recent entry - always white text and bold
            htmlContent += "<div style='color: white; font-weight: bold;'>" + entry + "</div>";
        } else {
            // Historical commands - dark gray text and not bold
            htmlContent += "<div style='color: #666666; font-weight: normal;'>" + entry + "</div>";
        }
    }
    
    htmlContent += "</body></html>";
    
    mScrollLog->setHtml(htmlContent);
}

// Quick action hotkey implementations
void MainWindow::toggleInventory()
{
    addStyledLogEntry("=== INVENTORY ===", true);
    if (mDGraphics->Hero && mDGraphics->Hero->inventorySize() > 0) {
        for (int i = 0; i < mDGraphics->Hero->inventorySize(); ++i) {
            Equipment item = mDGraphics->Hero->selectItem(i);
            QString itemInfo = QString("%1. %2 - AP: %3, HP: %4")
                .arg(i + 1)
                .arg(QString::fromStdString(item.getName()))
                .arg(item.getAP())
                .arg(item.getMaxHP());
            addStyledLogEntry(itemInfo, false);
        }
    } else {
        addStyledLogEntry("Inventory is empty", false);
    }
    addStyledLogEntry("=================", false);
}

void MainWindow::quickSave()
{
    addStyledLogEntry("Quick saving game...", true);
    
    // Use the same save system as regular save
    QJsonObject saveData;
    write(saveData);
    
    QFile saveFile("quicksave.json");
    if (saveFile.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(saveData);
        saveFile.write(saveDoc.toJson());
        saveFile.close();
        
        addStyledLogEntry("Quick save completed!", false);
        SoundEngine::PlaySoundByName("ok", 1.0f);
    } else {
        addStyledLogEntry("Quick save failed!", false);
    }
}

void MainWindow::quickLoad()
{
    addStyledLogEntry("Quick loading game...", true);
    
    QFile loadFile("quicksave.json");
    if (!loadFile.open(QIODevice::ReadOnly)) {
        addStyledLogEntry("No quick save file found!", false);
        return;
    }
    
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc = QJsonDocument::fromJson(saveData);
    
    // Clean up current state before loading
    cleanupGameObjects();
    
    // Load the saved data
    read(loadDoc.object());
    
    addStyledLogEntry("Quick load completed!", false);
    SoundEngine::PlaySoundByName("ok", 1.0f);
}

void MainWindow::toggleMusic()
{
    static bool musicEnabled = true;
    musicEnabled = !musicEnabled;
    
    if (musicEnabled) {
        addStyledLogEntry("Music enabled", true);
        SoundEngine::PlayMusicByName("intro", 0.5f);
    } else {
        addStyledLogEntry("Music disabled", true);
        SoundEngine::StopMusic();
    }
}

// Auto-save system implementation
void MainWindow::enableAutoSave(bool enabled)
{
    autoSaveEnabled = enabled;
    if (enabled) {
        autoSaveTimer->start(autoSaveIntervalMinutes * 60 * 1000);
        addStyledLogEntry("Auto-save enabled", true);
    } else {
        autoSaveTimer->stop();
        addStyledLogEntry("Auto-save disabled", true);
    }
}

void MainWindow::performAutoSave()
{
    if (!autoSaveEnabled || !mDGraphics->Hero) return;
    
    addStyledLogEntry("Auto-saving game...", true);
    
    // Create save data
    QJsonObject saveData;
    write(saveData);
    
    // Save to file
    QFile saveFile("autosave.json");
    if (saveFile.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(saveData);
        saveFile.write(saveDoc.toJson());
        saveFile.close();
        
        addStyledLogEntry("Auto-save completed", false);
        SoundEngine::PlaySoundByName("ok", 0.5f);
    } else {
        addStyledLogEntry("Auto-save failed!", false);
    }
}

void MainWindow::setAutoSaveInterval(int minutes)
{
    autoSaveIntervalMinutes = qMax(1, minutes); // Minimum 1 minute
    if (autoSaveEnabled) {
        autoSaveTimer->stop();
        autoSaveTimer->start(autoSaveIntervalMinutes * 60 * 1000);
    }
    addStyledLogEntry(QString("Auto-save interval set to %1 minutes").arg(autoSaveIntervalMinutes), true);
}

// Settings menu implementation
void MainWindow::showSettingsMenu()
{
    QDialog settingsDialog(this);
    settingsDialog.setWindowTitle("Game Settings");
    settingsDialog.setModal(true);
    settingsDialog.resize(400, 300);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(&settingsDialog);
    
    // Audio Settings
    QGroupBox* audioGroup = new QGroupBox("Audio Settings");
    QVBoxLayout* audioLayout = new QVBoxLayout(audioGroup);
    
    // Master Volume
    QHBoxLayout* masterVolLayout = new QHBoxLayout();
    masterVolLayout->addWidget(new QLabel("Master Volume:"));
    QSlider* masterVolSlider = new QSlider(Qt::Horizontal);
    masterVolSlider->setRange(0, 100);
    masterVolSlider->setValue(masterVolume * 100);
    masterVolLayout->addWidget(masterVolSlider);
    audioLayout->addLayout(masterVolLayout);
    
    // Music Volume
    QHBoxLayout* musicVolLayout = new QHBoxLayout();
    musicVolLayout->addWidget(new QLabel("Music Volume:"));
    QSlider* musicVolSlider = new QSlider(Qt::Horizontal);
    musicVolSlider->setRange(0, 100);
    musicVolSlider->setValue(musicVolume * 100);
    musicVolLayout->addWidget(musicVolSlider);
    audioLayout->addLayout(musicVolLayout);
    
    // SFX Volume
    QHBoxLayout* sfxVolLayout = new QHBoxLayout();
    sfxVolLayout->addWidget(new QLabel("SFX Volume:"));
    QSlider* sfxVolSlider = new QSlider(Qt::Horizontal);
    sfxVolSlider->setRange(0, 100);
    sfxVolSlider->setValue(sfxVolume * 100);
    sfxVolLayout->addWidget(sfxVolSlider);
    audioLayout->addLayout(sfxVolLayout);
    
    mainLayout->addWidget(audioGroup);
    
    // Visual Settings
    QGroupBox* visualGroup = new QGroupBox("Visual Settings");
    QVBoxLayout* visualLayout = new QVBoxLayout(visualGroup);
    
    // Screen Shake
    QCheckBox* screenShakeCheck = new QCheckBox("Enable Screen Shake");
    screenShakeCheck->setChecked(screenShakeEnabled);
    visualLayout->addWidget(screenShakeCheck);
    
    // Damage Flash
    QCheckBox* damageFlashCheck = new QCheckBox("Enable Damage Flash");
    damageFlashCheck->setChecked(damageFlashEnabled);
    visualLayout->addWidget(damageFlashCheck);
    
    mainLayout->addWidget(visualGroup);
    
    // Game Settings
    QGroupBox* gameGroup = new QGroupBox("Game Settings");
    QVBoxLayout* gameLayout = new QVBoxLayout(gameGroup);
    
    // Auto-save Interval
    QHBoxLayout* autoSaveLayout = new QHBoxLayout();
    autoSaveLayout->addWidget(new QLabel("Auto-save Interval (minutes):"));
    QSpinBox* autoSaveSpinBox = new QSpinBox();
    autoSaveSpinBox->setRange(1, 60);
    autoSaveSpinBox->setValue(autoSaveIntervalMinutes);
    autoSaveLayout->addWidget(autoSaveSpinBox);
    gameLayout->addLayout(autoSaveLayout);
    
    mainLayout->addWidget(gameGroup);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* applyButton = new QPushButton("Apply");
    QPushButton* cancelButton = new QPushButton("Cancel");
    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(applyButton, &QPushButton::clicked, [&]() {
        // Apply settings
        masterVolume = masterVolSlider->value() / 100.0f;
        musicVolume = musicVolSlider->value() / 100.0f;
        sfxVolume = sfxVolSlider->value() / 100.0f;
        screenShakeEnabled = screenShakeCheck->isChecked();
        damageFlashEnabled = damageFlashCheck->isChecked();
        autoSaveIntervalMinutes = autoSaveSpinBox->value();
        
        applySettings();
        settingsDialog.accept();
    });
    
    connect(cancelButton, &QPushButton::clicked, &settingsDialog, &QDialog::reject);
    
    settingsDialog.exec();
}

void MainWindow::applySettings()
{
    // Apply audio settings
    SoundEngine::SetMasterVolume(masterVolume);
    SoundEngine::SetMusicVolume(musicVolume);
    SoundEngine::SetSFXVolume(sfxVolume);
    
    // Apply auto-save settings
    setAutoSaveInterval(autoSaveIntervalMinutes);
    
    addStyledLogEntry("Settings applied successfully!", true);
}

void MainWindow::showHelpMenu()
{
    QDialog helpDialog(this);
    helpDialog.setWindowTitle("Game Help & Controls");
    helpDialog.setModal(true);
    helpDialog.resize(500, 400);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(&helpDialog);
    
    QTextEdit* helpText = new QTextEdit();
    helpText->setReadOnly(true);
    helpText->setHtml(
        "<h2>Darkness Void 2D Pixel - Controls</h2>"
        "<h3>Movement:</h3>"
        "<ul>"
        "<li><b>Arrow Keys</b> - Move in directions</li>"
        "<li><b>WASD</b> - Alternative movement (if configured)</li>"
        "</ul>"
        
        "<h3>Combat:</h3>"
        "<ul>"
        "<li><b>Space</b> - Attack</li>"
        "<li><b>Enter</b> - Use Potion</li>"
        "<li><b>Escape</b> - Flee</li>"
        "</ul>"
        
        "<h3>Quick Actions:</h3>"
        "<ul>"
        "<li><b>I</b> - Show Inventory</li>"
        "<li><b>S</b> - Quick Save</li>"
        "<li><b>L</b> - Quick Load</li>"
        "<li><b>M</b> - Toggle Music</li>"
        "<li><b>A</b> - Toggle Auto-save</li>"
        "</ul>"
        
        "<h3>Menus:</h3>"
        "<ul>"
        "<li><b>F1</b> - Settings Menu</li>"
        "<li><b>F2</b> - Help Menu (this window)</li>"
        "</ul>"
        
        "<h3>Game Features:</h3>"
        "<ul>"
        "<li><b>Screen Shake</b> - Visual feedback for combat</li>"
        "<li><b>Damage Flash</b> - Red flash when taking damage</li>"
        "<li><b>Auto-save</b> - Automatically saves progress</li>"
        "<li><b>Enhanced Audio</b> - Better sound effects and music</li>"
        "<li><b>Styled Log</b> - Current command highlighted in white</li>"
        "</ul>"
        
        "<h3>Tips:</h3>"
        "<ul>"
        "<li>Explore carefully - enemies are dangerous!</li>"
        "<li>Manage your inventory wisely</li>"
        "<li>Use potions strategically</li>"
        "<li>Check settings (F1) to customize your experience</li>"
        "</ul>"
    );
    
    mainLayout->addWidget(helpText);
    
    QPushButton* closeButton = new QPushButton("Close");
    connect(closeButton, &QPushButton::clicked, &helpDialog, &QDialog::accept);
    mainLayout->addWidget(closeButton);
    
    helpDialog.exec();
}

// Game state management implementation
void MainWindow::resetGameState()
{
    qDebug() << "Resetting game state...";
    
    // Clean up existing game objects
    cleanupGameObjects();
    
    // Reset global state variables
    gState = S_Normal;
    gFlee = true;
    mLogContent = "";
    
    // Clear log entries
    mLogEntries.clear();
    mCurrentCommandIndex = -1;
    addStyledLogEntry("Game state reset - Ready for new game", true);
    
    // Reset UI elements
    if (mHPBar) mHPBar->setValue(0);
    if (mHeroName) mHeroName->setText("Name: --");
    if (mArmorLabel) mArmorLabel->setText("");
    if (mWeaponLabel) mWeaponLabel->setText("");
    if (mPotion1Label) mPotion1Label->setText("");
    if (mPotion2Label) mPotion2Label->setText("");
    if (mPotion3Label) mPotion3Label->setText("");
    
    // Clear graphics scene
    if (mDGraphics && mDGraphics->scene()) {
        mDGraphics->clearSceneItems();
    }
    
    // Update status bar
    updateStatusBar();
    
    qDebug() << "Game state reset completed";
}

void MainWindow::cleanupGameObjects()
{
    qDebug() << "Cleaning up game objects...";
    
    // Stop any ongoing animations first
    if (mDGraphics && Graphics::instance) {
        Graphics::instance->stopAllAnimations();
    }
    
    // Clear graphics scene first to avoid double deletion
    if (mDGraphics && mDGraphics->scene()) {
        mDGraphics->clearSceneItems();
    }
    
    // Clean up graphics equipment items safely (they should already be removed from scene)
    if (mDGraphics) {
        qDebug() << "Starting equipment cleanup...";
        // Just nullify the pointers - the scene already deleted the items
        if (mDGraphics->heroArmor) {
            qDebug() << "Nullifying hero armor pointer...";
            mDGraphics->heroArmor = nullptr;
            qDebug() << "Hero armor pointer nullified";
        } else {
            qDebug() << "Hero armor was already null";
        }
        if (mDGraphics->heroHelmet) {
            qDebug() << "Nullifying hero helmet pointer...";
            mDGraphics->heroHelmet = nullptr;
            qDebug() << "Hero helmet pointer nullified";
        } else {
            qDebug() << "Hero helmet was already null";
        }
        if (mDGraphics->heroWeapon) {
            qDebug() << "Nullifying hero weapon pointer...";
            mDGraphics->heroWeapon = nullptr;
            qDebug() << "Hero weapon pointer nullified";
        } else {
            qDebug() << "Hero weapon was already null";
        }
        qDebug() << "Equipment cleanup completed";
    }
    
    // Clean up Hero (after equipment items)
    qDebug() << "Starting Hero cleanup...";
    if (mDGraphics && mDGraphics->Hero) {
        qDebug() << "Deleting Hero object...";
        delete mDGraphics->Hero;
        mDGraphics->Hero = nullptr;
        qDebug() << "Hero cleaned up";
    }
    
    // Clean up Generator
    qDebug() << "Starting Generator cleanup...";
    if (Generated) {
        // First, nullify the Graphics pointer to Generated to prevent dangling reference
        if (mDGraphics) {
            mDGraphics->Generated = nullptr;
            qDebug() << "Graphics Generated pointer nullified";
        }
        
        qDebug() << "About to call delete Generated...";
        try {
            // Try a safer approach - just nullify for now to avoid crash
            // TODO: Investigate why Generator deletion crashes
            qDebug() << "Skipping Generator deletion to avoid crash - will investigate";
            // delete Generated;
            Generated = nullptr;
            qDebug() << "Generator pointer nullified (not deleted)";
        } catch (...) {
            qDebug() << "Exception caught during Generator deletion";
        }
        
        qDebug() << "Generator cleanup completed";
    }
    
    qDebug() << "Game objects cleanup completed";
}

void MainWindow::updateStatusBar()
{
    qDebug() << "updateStatusBar() called";
    
    if (mDGraphics && mDGraphics->Hero) {
        qDebug() << "Updating status bar with Hero data";
        // Update health
        QString healthText = QString("Health: %1/%2")
            .arg(mDGraphics->Hero->getHP())
            .arg(mDGraphics->Hero->getMaxHP());
        healthLabel->setText(healthText);
        
        // Update position
        QString positionText = QString("Position: %1, %2")
            .arg(mDGraphics->Hero->heroRow)
            .arg(mDGraphics->Hero->heroCol);
        positionLabel->setText(positionText);
        
        // Update auto-save status
        QString autoSaveText = QString("Auto-save: %1")
            .arg(autoSaveEnabled ? "ON" : "OFF");
        autoSaveLabel->setText(autoSaveText);
        
        qDebug() << "Status bar updated successfully";
    } else {
        qDebug() << "Hero is null, setting default status bar values";
        healthLabel->setText("Health: --/--");
        positionLabel->setText("Position: --, --");
    }
    
    qDebug() << "updateStatusBar() completed";
}
