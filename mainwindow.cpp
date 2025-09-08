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
#include <QButtonGroup>
#include <QDialog>
#include <QInputDialog>
#include <QThread>
#include <QApplication>
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
    
    
    // Initialize scoring system
    enemiesDefeated = 0;
    totalScore = 0;
    
    // Initialize audio settings
    masterVolume = 1.0f;
    musicVolume = 1.0f;
    sfxVolume = 1.0f;
    screenShakeEnabled = true;
    damageFlashEnabled = true;
    
    // Initialize status bar
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    
    // Create status bar widgets
    mHeroName = new QLabel("Name: Player", this);
    mHPBar = new QProgressBar(this);
    mHPBar->setRange(0,100);
    mHPBar->setOrientation(Qt::Horizontal);
    mHPBar->setFormat("/100");
    mAPLabel = new QLabel("AP: 10", this);
    
    controlsLabel = new QLabel("Controls: WASD=Move | X=Attack | P=Potion | F=Flee | Z=Look");
    
    // Add widgets to status bar
    statusBar->addWidget(mHeroName);
    statusBar->addWidget(mAPLabel);
    statusBar->addWidget(mHPBar);
    statusBar->addPermanentWidget(controlsLabel);
    
    // Set global pointer for log access
    extern MainWindow* gMainWindow;
    gMainWindow = this;
    
    // Initialize SoundEngine
    SoundEngine::SetMasterVolume(1.0f);
    SoundEngine::SetMusicVolume(0.7f);
    SoundEngine::SetSFXVolume(1.0f);
    qDebug() << "SoundEngine initialized";
    
    // Apply modern dark theme
    applyDarkTheme();
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow destructor called";
    
    // Nullify global pointers to prevent access during/after destruction
    extern MainWindow* gMainWindow;
    extern QTextEdit *mScrollLog;
    gMainWindow = nullptr;
    mScrollLog = nullptr;
    qDebug() << "Global pointers nullified during MainWindow destruction";
    
    // Clear stylesheet during actual destruction to prevent Qt cleanup issues
    this->setStyleSheet("");
    qDebug() << "Main window stylesheet cleared during destruction";
    
    // Force Qt to process all pending events before destruction
    QApplication::processEvents();
    qDebug() << "Qt events processed before MainWindow destruction";
    
    // Ensure all timers are stopped
    if (timer) {
        timer->stop();
        qDebug() << "Timer stopped in destructor";
    }
    
    qDebug() << "About to call QMainWindow destructor";
    // QMainWindow destructor will be called automatically
    qDebug() << "MainWindow destructor completed";
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
        else if (event->key() == Qt::Key_M) {
            // Toggle music (M key)
            toggleMusic();
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
        // Safety check before drawing
        if (mDGraphics && mDGraphics->Generated && mDGraphics->Hero) {
            mDGraphics->drawPosition();
        } else {
            qDebug() << "Safety check failed in input handling - not drawing position";
        }
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
        qDebug() << "Hero pointer:" << mDGraphics->Hero;
        qDebug() << "Generated pointer:" << Generated;
        try {
            qDebug() << "About to call GameManager::handleEvent";
            GameManager::handleEvent(*mDGraphics->Hero,*Generated);
            qDebug() << "GameManager::handleEvent completed";
        } catch (...) {
            qDebug() << "Exception in GameManager::handleEvent";
        }
        
        try {
            qDebug() << "About to call updateInventory";
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
    
    // Safety checks
    if(mDGraphics == nullptr) {
        qDebug() << "mDGraphics is null in updateInventory, returning";
        return;
    }
    
    if(mDGraphics->Hero == nullptr) {
        qDebug() << "Hero is null in updateInventory, returning";
        return;
    }
    
    // Check if inventory buttons exist
    if(mHelmet == nullptr || mArmor == nullptr || mWeapon == nullptr) {
        qDebug() << "Inventory buttons are null in updateInventory, returning";
        return;
    }
    
    qDebug() << "Hero inventory size:" << mDGraphics->Hero->inventorySize();

    int invSize = mDGraphics->Hero->inventorySize();

    if(invSize >= 1)
    {
        Equipment myHelmet = mDGraphics->Hero->selectItem(T_Head);
        QString helmName = QString::fromStdString(myHelmet.getName());
        mHelmeticon = mDGraphics->getEquipmentGraphic(helmName, T_Head);
        // Scale icon to fit nicely within the button (leave space for text)
        QSize iconSize = QSize(60, 60); // Fixed icon size for consistency
        QPixmap helmetPixmap = QPixmap(mHelmeticon).scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        mHelmet->setIcon(QIcon(helmetPixmap));
        mHelmet->setIconSize(iconSize);
        QString temp = QString::fromStdString(myHelmet.getName()) + "\nMaxHP:"+ QString::number(myHelmet.getMaxHP());
        mHelmet->setText(temp);
    }
    else
    {
        mHelmet->setText("");
        mHelmet->setIcon(QIcon());
    }
    if(invSize >= 2)
    {
        Equipment myArmor = mDGraphics->Hero->selectItem(T_Chest);
        QString armorName = QString::fromStdString(myArmor.getName());
        mArmoricon = mDGraphics->getEquipmentGraphic(armorName, T_Chest);
        // Scale icon to fit nicely within the button (leave space for text)
        QSize iconSize = QSize(60, 60); // Fixed icon size for consistency
        QPixmap armorPixmap = QPixmap(mArmoricon).scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        mArmor->setIcon(QIcon(armorPixmap));
        mArmor->setIconSize(iconSize);
        QString temp = QString::fromStdString(myArmor.getName()) + "\nMaxHP:"+ QString::number(myArmor.getMaxHP());
        mArmor->setText(temp);
    }
    else
    {
        mArmor->setText("");
        mArmor->setIcon(QIcon());
    }
    if(invSize >= 3)
    {
        Equipment myWeapon= mDGraphics->Hero->selectItem(T_Weapon);
        QString weaponName = QString::fromStdString(myWeapon.getName());
        mWeaponicon = mDGraphics->getEquipmentGraphic(weaponName, T_Weapon);
        // Scale icon to fit nicely within the button (leave space for text)
        QSize iconSize = QSize(60, 60); // Fixed icon size for consistency
        QPixmap weaponPixmap = QPixmap(mWeaponicon).scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        mWeapon->setIcon(QIcon(weaponPixmap));
        mWeapon->setIconSize(iconSize);
        QString temp = QString::fromStdString(myWeapon.getName()) + "\nAP:"+ QString::number(myWeapon.getAP());
        mWeapon->setText(temp);
    }
    else
    {
        mWeapon->setText("");
        mWeapon->setIcon(QIcon());
    }
    if(invSize >= 4)
    {
        Equipment myItem = mDGraphics->Hero->selectItem(invSize - 1);
        QString ItemName = QString::fromStdString(myItem.getName());
        mPotionicon = mDGraphics->getEquipmentGraphic(ItemName, T_Consumable);
        // Scale icon to fit nicely within the button (leave space for text)
        QSize iconSize = QSize(60, 60); // Fixed icon size for consistency
        QPixmap potionPixmap = QPixmap(mPotionicon).scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        mPotion1->setIcon(QIcon(potionPixmap));
        mPotion1->setIconSize(iconSize);
        QString temp = QString::fromStdString(mDGraphics->Hero->selectItem(3).getName()) + "\nHP:"+ QString::number(mDGraphics->Hero->selectItem(3).getMaxHP());
        mPotion1->setText(temp);
    }
    else
    {
        mPotion1->setText("");
        mPotion1->setIcon(QIcon());
    }
    if(invSize >= 5)
    {
        Equipment myItem = mDGraphics->Hero->selectItem(invSize-1);
        QString ItemName = QString::fromStdString(myItem.getName());
        mPotionicon = mDGraphics->getEquipmentGraphic(ItemName, T_Consumable);
        // Scale icon to fit nicely within the button (leave space for text)
        QSize iconSize = QSize(60, 60); // Fixed icon size for consistency
        QPixmap potionPixmap = QPixmap(mPotionicon).scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        mPotion2->setIcon(QIcon(potionPixmap));
        mPotion2->setIconSize(iconSize);
        QString temp = QString::fromStdString(mDGraphics->Hero->selectItem(4).getName()) + "\nHP:"+ QString::number(mDGraphics->Hero->selectItem(4).getMaxHP());
        mPotion2->setText(temp);
    }
    else
    {
        mPotion2->setText("");
        mPotion2->setIcon(QIcon());
    }
    if(invSize >= 6)
    {
        Equipment myItem = mDGraphics->Hero->selectItem(invSize-1);
        QString ItemName = QString::fromStdString(myItem.getName());
        mPotionicon = mDGraphics->getEquipmentGraphic(ItemName, T_Consumable);
        // Scale icon to fit nicely within the button (leave space for text)
        QSize iconSize = QSize(60, 60); // Fixed icon size for consistency
        QPixmap potionPixmap = QPixmap(mPotionicon).scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        mPotion3->setIcon(QIcon(potionPixmap));
        mPotion3->setIconSize(iconSize);
        QString temp = QString::fromStdString(mDGraphics->Hero->selectItem(5).getName()) + "\nHP:"+ QString::number(mDGraphics->Hero->selectItem(5).getMaxHP());
        mPotion3->setText(temp);
    }
    else
    {
        mPotion3->setText("");
        mPotion3->setIcon(QIcon());
    }
    if(invSize >= 7)
    {
        Equipment myItem = mDGraphics->Hero->selectItem(invSize-1);
        QString ItemName = QString::fromStdString(myItem.getName());
        mPotionicon = mDGraphics->getEquipmentGraphic(ItemName, T_Consumable);
        // Scale icon to fit nicely within the button (leave space for text)
        QSize iconSize = QSize(60, 60); // Fixed icon size for consistency
        QPixmap potionPixmap = QPixmap(mPotionicon).scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        mPotion4->setIcon(QIcon(potionPixmap));
        mPotion4->setIconSize(iconSize);
        QString temp = QString::fromStdString(mDGraphics->Hero->selectItem(6).getName()) + "\nHP:"+ QString::number(mDGraphics->Hero->selectItem(6).getMaxHP());
        mPotion4->setText(temp);
    }
    else
    {
        mPotion4->setText("");
        mPotion4->setIcon(QIcon());
    }
    if(invSize == 8)
    {
        Equipment myItem = mDGraphics->Hero->selectItem(invSize-1);
        QString ItemName = QString::fromStdString(myItem.getName());
        mPotionicon = mDGraphics->getEquipmentGraphic(ItemName, T_Consumable);
        // Scale icon to fit nicely within the button (leave space for text)
        QSize iconSize = QSize(60, 60); // Fixed icon size for consistency
        QPixmap potionPixmap = QPixmap(mPotionicon).scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        mPotion5->setIcon(QIcon(potionPixmap));
        mPotion5->setIconSize(iconSize);
        QString temp = QString::fromStdString(mDGraphics->Hero->selectItem(7).getName()) + "\nHP:"+ QString::number(mDGraphics->Hero->selectItem(7).getMaxHP());
        mPotion5->setText(temp);
    }
    else
    {
        mPotion5->setText("");
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
    resize(1600,1000);  // Increased width to accommodate centered layout
    //setMinimumSize(1300,850);
    QMenuBar *mainMenuBar = menuBar();
    mainMenuBar->setVisible(true); // Ensure menu bar is visible

    QMenu *fileMenu = mainMenuBar->addMenu("File");
    QAction *startMenu = fileMenu->addAction("Start Game");
    connect(startMenu, SIGNAL (triggered()),this, SLOT (startGame()));
    QAction *difficultyMenu = fileMenu->addAction("Select Difficulty");
    connect(difficultyMenu, SIGNAL (triggered()),this, SLOT (selectDifficulty()));
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

    // Create styled score display for top area
    scoreLabel = new QLabel("<b><span style='color: #FFD700; font-size: 14px;'>🏆 Enemies: 0 | Score: 0</span></b>");
    scoreLabel->setStyleSheet("QLabel { color: #FFD700; font-weight: bold; font-size: 14px; }");

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

    verticalLayout->setContentsMargins(0, 30, 0, 0); // Add top margin to avoid menu bar overlap
    verticalLayout->addWidget(scoreLabel, 0, Qt::AlignCenter);  // Add centered score display at top
    verticalLayout->addWidget(inventory,0);
    verticalLayout->addWidget(centerStack,1);  // Give main area all remaining space
    return center;
}

QWidget *MainWindow::setupStatusBar()
{
    // Status bar widgets are now created in the constructor
    // This function is kept for compatibility but widgets are already initialized
    QWidget *center = new QWidget(this);
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
    
    // Set fixed width to keep it compact
    mScrollLog->setFixedWidth(250);
    
    // Assign to global variable for fallback logging
    extern QTextEdit *mScrollLog;
    ::mScrollLog = this->mScrollLog;
    
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
    
    // Add left spacer to center the main content
    horizontalLayout->addStretch(1);
    
    // Add controls on the left side
    horizontalLayout->addWidget(movementArrows, 0, Qt::AlignLeft);
    
    // Add main game area (map + log) in the center
    QWidget *gameArea = new QWidget(this);
    QHBoxLayout *gameLayout = new QHBoxLayout(gameArea);
    gameLayout->setContentsMargins(10, 0, 10, 0); // Add some spacing around the game area
    gameLayout->addWidget(mapDisplay, 0, Qt::AlignCenter);
    gameLayout->addWidget(scrollLog, 0, Qt::AlignRight);
    
    horizontalLayout->addWidget(gameArea, 0, Qt::AlignCenter);
    
    // Add right spacer to balance the layout
    horizontalLayout->addStretch(1);

    mapDisplay->setMinimumSize(gStep*gWidth, gStep*gHeight);
    return center;
}


QWidget *MainWindow::setupMapDisplay()
{
    setMinimumSize(PIXEL,PIXEL);

    QGraphicsScene *graphicsScene = new QGraphicsScene(this);
    graphicsScene->setBackgroundBrush(QBrush(QColor("#1a1a1a"), Qt::SolidPattern)); // Dark theme background
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

QWidget* MainWindow::setupMovementArrows()
{
    QWidget* widget = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(widget);
    mainLayout->setContentsMargins(10, 10, 10, 10); // Add padding around controls
    
    QLabel* label = new QLabel("Controls");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("QLabel { font-weight: bold; font-size: 12pt; margin-bottom: 10px; }");
    mainLayout->addWidget(label);
    
    QWidget* buttonContainer = new QWidget;
    QGridLayout* layout = new QGridLayout(buttonContainer);
    layout->setSpacing(5); // Add spacing between buttons

    enum Buttons
    {
        GB_UP =4,
        GB_LEFT = 6,
        GB_RIGHT = 8,
        GB_DOWN = 10,
        GB_ACTION = 21,
        GB_POTION = 22,
        GB_FLEE = 23,
        GB_NOthing = 24,
    };

    int counter = 0;
    // Creating buttons and adding them to the grid layout
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 3; ++col) {
            switch(counter)
            {
                case GB_UP:
                {
                QPushButton* buttonGB_UP = new QPushButton(QString("UP"), this);
                    connect(buttonGB_UP, &QPushButton::clicked, layout, [this] { inputHandle(nullptr, B_Up); });
                    layout->addWidget(buttonGB_UP, row, col);
                    break;
                }
                case GB_LEFT:
                {
                    QPushButton* buttonGB_LEFT = new QPushButton(QString("LEFT"), this);
                    connect(buttonGB_LEFT, &QPushButton::clicked, layout, [this] { inputHandle(nullptr, B_Left); });
                    layout->addWidget(buttonGB_LEFT, row, col);
                    break;
                }
                case GB_RIGHT:
                {
                    QPushButton* buttonGB_RIGHT= new QPushButton(QString("RIGHT"), this);
                    connect(buttonGB_RIGHT, &QPushButton::clicked, layout, [this] { inputHandle(nullptr, B_Right); });
                    layout->addWidget(buttonGB_RIGHT, row, col);
                    break;
                }
                case GB_DOWN:
                {
                    QPushButton* buttonGB_DOWN = new QPushButton(QString("DOWN"), this);
                    connect(buttonGB_DOWN, &QPushButton::clicked, layout, [this] { inputHandle(nullptr, B_Down); });
                    layout->addWidget(buttonGB_DOWN, row, col);
                    break;
                }
                case GB_ACTION:
                {
                    QPushButton* buttonGB_ACTION = new QPushButton(QString("ACTION"), this);
                    connect(buttonGB_ACTION, &QPushButton::clicked, layout, [this] { inputHandle(nullptr, B_Atack); });
                    layout->addWidget(buttonGB_ACTION, row, col);
                    break;
                }
                case GB_POTION:
                {
                    QPushButton* buttonGB_POTION = new QPushButton(QString("POTION"), this);
                    connect(buttonGB_POTION, &QPushButton::clicked, layout, [this] { inputHandle(nullptr, B_Potion); });
                    layout->addWidget(buttonGB_POTION, row, col);
                    break;
                }
                case GB_FLEE:
                {
                    QPushButton* buttonGB_FLEE = new QPushButton(tr("FLEE"), this);
                    connect(buttonGB_FLEE, &QPushButton::clicked, this, [this] { inputHandle(nullptr, B_Flee); });
                    layout->addWidget(buttonGB_FLEE, row, col);
                    break;
                }
            }
            ++counter;
        }
    }

    layout->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(buttonContainer);
    mainLayout->addStretch(); // Add stretch to push controls to top
    
    return widget;
}

QWidget *MainWindow::setupInventory()
{
    QWidget *widgetsContainer = new QWidget(this);
    QHBoxLayout *inventory = new QHBoxLayout(widgetsContainer);

    QLabel *inventoryLabel = new QLabel("Inventory:", this);
    inventoryLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    // Create combined button containers with text and icons
    mWeapon = new QToolButton(this);
    mHelmet = new QToolButton(this);
    mArmor = new QToolButton(this);
    mPotion1 = new QToolButton(this);
    mPotion2 = new QToolButton(this);
    mPotion3 = new QToolButton(this);
    mPotion4 = new QToolButton(this);
    mPotion5 = new QToolButton(this);
    
    // Set button properties for better icon display
    mWeapon->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mHelmet->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mArmor->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mPotion1->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mPotion2->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mPotion3->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mPotion4->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mPotion5->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    
    // Set fixed sizes for consistent inventory display
    mWeapon->setFixedSize(100, 100);
    mHelmet->setFixedSize(100, 100);
    mArmor->setFixedSize(100, 100);
    mPotion1->setFixedSize(100, 100);
    mPotion2->setFixedSize(100, 100);
    mPotion3->setFixedSize(100, 100);
    mPotion4->setFixedSize(100, 100);
    mPotion5->setFixedSize(100, 100);
    
    // Connect potion buttons to use best potion available
    connect(mPotion1, &QToolButton::clicked, this, [this]() { 
        gValueButton = B_Potion; // Use general potion button which defaults to best potion
        inputHandle(nullptr, B_Potion); 
    });
    connect(mPotion2, &QToolButton::clicked, this, [this]() { 
        gValueButton = B_Potion; 
        inputHandle(nullptr, B_Potion); 
    });
    connect(mPotion3, &QToolButton::clicked, this, [this]() { 
        gValueButton = B_Potion; 
        inputHandle(nullptr, B_Potion); 
    });
    connect(mPotion4, &QToolButton::clicked, this, [this]() { 
        gValueButton = B_Potion; 
        inputHandle(nullptr, B_Potion); 
    });
    connect(mPotion5, &QToolButton::clicked, this, [this]() { 
        gValueButton = B_Potion; 
        inputHandle(nullptr, B_Potion); 
    });

    inventory->addWidget(inventoryLabel);
    inventory->addWidget(mWeapon);
    inventory->addWidget(mHelmet);
    inventory->addWidget(mArmor);
    inventory->addWidget(mPotion1);
    inventory->addWidget(mPotion2);
    inventory->addWidget(mPotion3);
    inventory->addWidget(mPotion4);
    inventory->addWidget(mPotion5);
    
    // Add consistent spacing between items
    inventory->setSpacing(8);
    inventory->setContentsMargins(10, 10, 10, 10);

    return widgetsContainer;
}

/*Slots*/

void MainWindow::exitDialog()
{
    qDebug() << "exitDialog() called";
    QMessageBox *myDialog = new QMessageBox(this);
    myDialog->setText("Are you sure to exit game");
    myDialog->setToolTip("Exit dialog");
    myDialog->exec();
    qDebug() << "About to call close()";
    close();
    qDebug() << "close() called successfully";
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
    // Safety check: don't create dialogs if the window is being destroyed
    if (isVisible() == false) {
        qDebug() << "Skipping idle dialog - window not visible (likely being destroyed)";
        return;
    }
    
    QMessageBox *myDialog = new QMessageBox(this);
    myDialog->setText("User idle due inactivity");
    myDialog->setToolTip("AFK");
    myDialog->exec();
}



void MainWindow::startGame()
{
    // Start the game with an easy level
    loadMapFile(EMapStart::E_Easy);
}

void MainWindow::selectDifficulty()
{
    QDialog *difficultyDialog = new QDialog(this);
    difficultyDialog->setWindowTitle("Select Difficulty Level");
    difficultyDialog->setModal(true);
    difficultyDialog->resize(300, 200);

    QVBoxLayout *layout = new QVBoxLayout(difficultyDialog);

    QLabel *titleLabel = new QLabel("Choose your difficulty level:");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    QPushButton *easyButton = new QPushButton("Easy (3 levels available)");
    QPushButton *normalButton = new QPushButton("Normal (3 levels available)");
    QPushButton *hardButton = new QPushButton("Hard (3 levels available)");
    QPushButton *customButton = new QPushButton("Load Custom Level");

    layout->addWidget(easyButton);
    layout->addWidget(normalButton);
    layout->addWidget(hardButton);
    layout->addWidget(customButton);

    // Connect buttons to loadMapFile with appropriate difficulty
    connect(easyButton, &QPushButton::clicked, [this, difficultyDialog] { 
        loadMapFile(EMapStart::E_Easy); 
        difficultyDialog->close(); 
    });
    connect(normalButton, &QPushButton::clicked, [this, difficultyDialog] { 
        loadMapFile(EMapStart::E_Medium); 
        difficultyDialog->close(); 
    });
    connect(hardButton, &QPushButton::clicked, [this, difficultyDialog] { 
        loadMapFile(EMapStart::E_Hard); 
        difficultyDialog->close(); 
    });
    connect(customButton, &QPushButton::clicked, [this, difficultyDialog] { 
        loadMapFile(EMapStart::E_Custom); 
        difficultyDialog->close(); 
    });

    difficultyDialog->exec();
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
        if (levelsEasy.size() > 0) {
            randomValue = rand() % levelsEasy.size();
            filePath = levelsEasy.at(randomValue);
        } else {
            addStyledLogEntry("No easy levels found!", false);
            return;
        }
        break;
    case EMapStart::E_Medium:
        if (levelsMedium.size() > 0) {
            randomValue = rand() % levelsMedium.size();
            filePath = levelsMedium.at(randomValue);
        } else {
            addStyledLogEntry("No normal levels found!", false);
            return;
        }
        break;
    case EMapStart::E_Hard:
        if (levelsHard.size() > 0) {
            randomValue = rand() % levelsHard.size();
            filePath = levelsHard.at(randomValue);
        } else {
            addStyledLogEntry("No hard levels found!", false);
            return;
        }
        break;
    case EMapStart::E_Custom:
        filePath = QFileDialog::getOpenFileName(this, tr("Search and load new input file"), mDefaultPath, tr("Text Files (*.txt)"));
        if (filePath.isEmpty()) {
            addStyledLogEntry("No custom level selected!", false);
            return;
        }
        break;
    }
    
    // Log which level was selected
    QString difficultyName;
    switch(mode) {
        case EMapStart::E_Easy: difficultyName = "Easy"; break;
        case EMapStart::E_Medium: difficultyName = "Normal"; break;
        case EMapStart::E_Hard: difficultyName = "Hard"; break;
        case EMapStart::E_Custom: difficultyName = "Custom"; break;
    }
    
    QString levelName = QFileInfo(filePath).baseName();
    addStyledLogEntry(QString("Starting %1 level: %2").arg(difficultyName).arg(levelName), true);
    
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
        qDebug() << "resetGameState() returned successfully";
        
        gStep = PIXEL/gWidth;
        qDebug() << "gStep calculated successfully";
        /*Spawn all item locations in map*/
        Generated = new Generator;
        qDebug() << "New Generator created successfully";
        
        // Safety check before setting Generator
        if (mDGraphics) {
            qDebug() << "Setting Generator in Graphics";
            mDGraphics->SetGenerator(Generated);
            qDebug() << "Generator set successfully";
        } else {
            qDebug() << "ERROR: mDGraphics is null, cannot set Generator";
            return;
        }
        
        qDebug() << "About to call spawnMap";
        Generated->spawnMap();
        qDebug() << "spawnMap completed";

        /*initialize hero with input information*/
        qDebug() << "About to create new Hero";
        Character *hero = new Character;
        qDebug() << "Hero created successfully";
        
        // Safety check before setting Hero
        if (mDGraphics) {
            qDebug() << "Setting Hero in Graphics";
            mDGraphics->Hero = hero;
            qDebug() << "Hero set successfully";
        } else {
            qDebug() << "ERROR: mDGraphics is null, cannot set Hero";
            delete hero; // Clean up the hero we just created
            return;
        }
        
        qDebug() << "About to set hero base values";
        hero->setBaseValues(gAP,gMaxHP);
        qDebug() << "Hero base values set";
        
        // Skip HP bar update during reset to prevent widget access crashes
        // The HP bar will be updated when the game actually starts running
        qDebug() << "Skipping HP bar update during reset to prevent widget access crashes";
        
        qDebug() << "hero setup done";

        // Ensure the main window is visible and ready before showing input dialog
        if (!isVisible()) {
            qDebug() << "Main window not visible, showing it first";
            show();
        }
        
        // Force the window to be active and visible
        activateWindow();
        raise();
        
        // Force Qt to process events to ensure window is ready
        QApplication::processEvents();
        qDebug() << "Main window is ready for input dialog";

        bool ok = false;
        while(!ok)
        {
            // More lenient check - only skip if window is completely invalid
            if (!isVisible()) {
                qDebug() << "Window not visible, skipping hero name input";
                hero->setName("Player"); // Use default name
                ok = true;
                break;
            }
            
            QString nameHero;
            try {
                nameHero = QInputDialog::getText(this, tr("Input text only no more than 20 characters"),
                                                 tr("Hero name:"), QLineEdit::Normal,
                                                 "Douglass", &ok);
                qDebug() << "QInputDialog completed successfully";
            } catch (...) {
                qDebug() << "Exception in QInputDialog, using default name";
                nameHero = "Player";
                ok = true;
            }
            
            qDebug() << "About to play begin sound";
            SoundEngine::PlaySoundByName("begin", 1);
            qDebug() << "Begin sound played successfully";
            
            qDebug() << "About to set hero name";
            hero->setName(nameHero.toLocal8Bit().constData());
            qDebug() << "Hero name set successfully";
            
            // Qt GUI version: Initialize hero with starting gear directly
            // Instead of calling console version's GameManager::startMenu()
            std::string heroName = hero->getName();
            qDebug() << "Hero name retrieved:" << QString::fromStdString(heroName);
            if((heroName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) || heroName.length() > 20)
            {
                addStyledLogEntry("Error name not valid,\nplease input no more than 20 chars \n& no special characters only letters\n", false);
                ok = false;
                continue;
            }
            
            // Add starting gear to hero
            QString welcomeMsg = QString(" Hi %1 now begin your journey to safety, take this gear\n ").arg(QString::fromStdString(heroName));
            addStyledLogEntry(welcomeMsg, false);
            
            hero->addToInventory(Helmet("hat", 0, 1, true));
            addStyledLogEntry("item: hat added to inventory \n", false);
            
            hero->addToInventory(Armor("cloth shirt", 0, 1, true));
            addStyledLogEntry("item: cloth shirt added to inventory \n", false);
            
            hero->addToInventory(Weapon("knife", 1, 0, true));
            addStyledLogEntry("item: knife added to inventory \n", false);
            
            qDebug() << "About to calculate hero stats";
            hero->calculateStats();
            qDebug() << "Hero stats calculated successfully";
            ok = true; // Successfully initialized hero
        }
        
        qDebug() << "About to update hero name display";
        if (mHeroName) {
            mHeroName->setText("Name: " + QString::fromStdString(mDGraphics->Hero->getName()));
            qDebug() << "Hero name display updated successfully";
        } else {
            qDebug() << "ERROR: mHeroName is null, cannot update hero name display";
        }
            
            // Safety checks before drawing
            qDebug() << "About to check graphics safety";
            if (mDGraphics && mDGraphics->Generated && mDGraphics->Hero) {
                qDebug() << "Graphics safety check passed, about to draw map";
                mDGraphics->drawMapFullStatic();
                qDebug() << "Map drawn successfully, about to draw position";
                mDGraphics->drawPosition();
                qDebug() << "Position drawn successfully";
            } else {
                qDebug() << "Safety check failed - not drawing graphics";
                qDebug() << "mDGraphics:" << (mDGraphics ? "OK" : "NULL");
                qDebug() << "Generated:" << (mDGraphics && mDGraphics->Generated ? "OK" : "NULL");
                qDebug() << "Hero:" << (mDGraphics && mDGraphics->Hero ? "OK" : "NULL");
            }
            
            qDebug() << "About to update inventory";
            updateInventory();
            qDebug() << "Inventory updated successfully";
        }
    else
    {
        QMessageBox *myDialog = new QMessageBox(this);
        myDialog->setText(subString + " invalid extension  try opening an .txt extension file");
        myDialog->setToolTip("error wrong input");
        myDialog->exec();
    }

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
        "<li><b>M</b> - Toggle Music</li>"
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
    
    // Reset member variables BEFORE cleanup to avoid accessing them after hide()
    qDebug() << "Clearing member variables before cleanup";
    mLogEntries.clear();
    mCurrentCommandIndex = -1;
    
    // Clean up existing game objects
    cleanupGameObjects();
    qDebug() << "cleanupGameObjects() returned successfully";
    
    // Reset global state variables
    qDebug() << "About to reset global state variables";
    qDebug() << "Setting gState to S_Normal";
    gState = S_Normal;
    qDebug() << "Setting gFlee to true";
    gFlee = true;
    qDebug() << "Clearing mLogContent";
    mLogContent = "";
    
    // Note: addStyledLogEntry removed here to prevent crash after cleanup
    // The log will be updated when a new game starts
    
    // Reset UI elements safely - skip if widgets may be invalid after cleanup
    qDebug() << "Skipping UI reset after cleanup to prevent widget access crashes";
    // Note: UI elements will be reset when a new game starts instead
    
    // Clear graphics scene
    if (mDGraphics && mDGraphics->scene()) {
        mDGraphics->clearSceneItems();
    }
    
    // Skip updateStatusBar() call after cleanup to prevent accessing deleted Hero
    qDebug() << "Skipping status bar update after cleanup to prevent Hero access";
    
    // Reapply the dark theme to ensure UI styling is maintained
    applyDarkTheme();
    qDebug() << "Dark theme reapplied after game reset";
    
    qDebug() << "Game state reset completed";
}

void MainWindow::cleanupGameObjects()
{
    qDebug() << "Cleaning up game objects...";
    
    // Force Qt to process all pending events before cleanup
    QApplication::processEvents();
    qDebug() << "Qt events processed before cleanup";
    
    // Note: Removed this->hide() call as it may cause Qt to start destruction process
    
    // Note: Do NOT nullify global pointers during game reset - they should remain valid
    // Only nullify them during actual MainWindow destruction
    qDebug() << "Keeping global MainWindow and mScrollLog pointers valid for logging";
    
    // Clear graphics scene first to avoid double deletion
    if (mDGraphics && mDGraphics->scene()) {
        // Nullify the Generator pointer in Graphics before clearing scene
        mDGraphics->Generated = nullptr;
        qDebug() << "Graphics Generator pointer nullified before scene clear";
        mDGraphics->clearSceneItems();
    }
    
    // Stop any ongoing animations first
    if (mDGraphics && Graphics::instance) {
        Graphics::instance->stopAllAnimations();
    }
    
    // Nullify the static Graphics pointer to prevent access during destruction
    Graphics::instance = nullptr;
    qDebug() << "Static Graphics pointer nullified";
    
    // Stop the idle timer to prevent it from firing during destruction
    if (timer) {
        timer->stop();
        qDebug() << "Idle timer stopped";
    }
    
    // Safely clean up UI elements that might cause destruction issues
    try {
        // Note: Do NOT clear stylesheets during game reset - only during actual destruction
        // The stylesheet should remain applied for the UI to look correct
        qDebug() << "Skipping stylesheet clear during game reset to maintain UI appearance";
        
        if (statusBar) {
            qDebug() << "Cleaning up status bar";
            // Don't delete statusBar - it's owned by QMainWindow
        }
        if (controlsLabel) {
            qDebug() << "Cleaning up controls label";
            // Don't delete controlsLabel - it's owned by statusBar
        }
        if (scoreLabel) {
            qDebug() << "Cleaning up score label";
            // Don't delete scoreLabel - it's owned by parent widget
        }
    } catch (...) {
        qDebug() << "Exception during UI cleanup";
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
            // TEMPORARY: Skip deletion to test if this fixes the crash
            // This is a memory leak but will help us identify the root cause
            qDebug() << "Skipping Generator deletion to test crash fix";
            Generated = nullptr; // Just nullify the pointer
            qDebug() << "Generator pointer nullified (not deleted - temporary fix)";
        } catch (const std::exception& e) {
            qDebug() << "Standard exception caught during Generator deletion:" << e.what();
        } catch (...) {
            qDebug() << "Unknown exception caught during Generator deletion";
        }
        
        qDebug() << "Generator cleanup completed";
    }
    
    qDebug() << "Game objects cleanup completed";
    
    // Force Qt to process all pending events after cleanup
    QApplication::processEvents();
    qDebug() << "Qt events processed after cleanup";
}

void MainWindow::onEnemyDefeated(const QString& enemyName, int enemyDifficulty)
{
    qDebug() << "onEnemyDefeated called with enemy:" << enemyName << "difficulty:" << enemyDifficulty;
    
    try {
        enemiesDefeated++;
        qDebug() << "Enemies defeated incremented to:" << enemiesDefeated;
        
        // Calculate score based on enemy difficulty
        int baseScore = 10;
        int difficultyMultiplier = enemyDifficulty;
        int enemyScore = baseScore * difficultyMultiplier;
        totalScore += enemyScore;
        qDebug() << "Score calculated:" << enemyScore << "Total score:" << totalScore;
        
        // Determine difficulty level name
        QString difficultyLevel;
        if (enemyDifficulty <= 2) difficultyLevel = "Very Easy";
        else if (enemyDifficulty <= 4) difficultyLevel = "Easy";
        else if (enemyDifficulty <= 6) difficultyLevel = "Medium";
        else if (enemyDifficulty <= 8) difficultyLevel = "Hard";
        else if (enemyDifficulty <= 9) difficultyLevel = "Very Hard";
        else difficultyLevel = "Boss";
        qDebug() << "Difficulty level determined:" << difficultyLevel;
        
        // Log the defeat with difficulty level
        QString defeatMessage = QString("Defeated %1 (%2)! (+%3 points) | Total: %4 enemies, Score: %5")
            .arg(enemyName)
            .arg(difficultyLevel)
            .arg(enemyScore)
            .arg(enemiesDefeated)
            .arg(totalScore);
        qDebug() << "About to add styled log entry:" << defeatMessage;
        addStyledLogEntry(defeatMessage, false);
        qDebug() << "Styled log entry added successfully";
        
        // Update score display
        qDebug() << "About to update score display";
        updateScoreDisplay();
        qDebug() << "Score display updated successfully";
        
        qDebug() << "onEnemyDefeated completed successfully";
    } catch (const std::exception& e) {
        qDebug() << "Exception in onEnemyDefeated:" << e.what();
    } catch (...) {
        qDebug() << "Unknown exception in onEnemyDefeated";
    }
}

void MainWindow::updateScoreDisplay()
{
    // Update the styled score label with enhanced graphics
    QString scoreText = QString("<b><span style='color: #FFD700; font-size: 14px;'>🏆 Enemies: %1 | Score: %2</span></b>")
        .arg(enemiesDefeated)
        .arg(totalScore);
    scoreLabel->setText(scoreText);
    
    qDebug() << "Score display updated - Enemies:" << enemiesDefeated << "Score:" << totalScore;
}

void MainWindow::updateStatusBar()
{
    qDebug() << "updateStatusBar() called";
    
    if (mDGraphics && mDGraphics->Hero) {
        qDebug() << "Updating status bar with Hero data";
        
        // Check for available potions and show key bindings
        QString potionKeys = "";
        bool hasPotions = false;
        
        // Check inventory for potions - be more careful with bounds checking
        int inventorySize = mDGraphics->Hero->inventorySize();
        qDebug() << "Hero inventory size:" << inventorySize;
        
        // Check each inventory slot for potions
        for (int i = 0; i < inventorySize; ++i) {
            try {
                Equipment item = mDGraphics->Hero->selectItem(i);
                QString itemName = QString::fromStdString(item.getName());
                qDebug() << "Checking item at index" << i << ":" << itemName;
                
                // Check if this is a potion (look for "potion" in the name)
                if (itemName.toLower().contains("potion")) {
                    if (!potionKeys.isEmpty()) {
                        potionKeys += " | ";
                    }
                    // Map inventory indices to potion button numbers
                    // Inventory slots 3,4,5,6,7 correspond to potion buttons 1,2,3,4,5
                    int potionNumber = -1;
                    if (i == 3) potionNumber = 1;
                    else if (i == 4) potionNumber = 2;
                    else if (i == 5) potionNumber = 3;
                    else if (i == 6) potionNumber = 4;
                    else if (i == 7) potionNumber = 5;
                    
                    if (potionNumber > 0) {
                        potionKeys += QString("%1").arg(potionNumber);
                        hasPotions = true;
                        qDebug() << "Found potion at inventory index" << i << "-> potion button" << potionNumber;
                    }
                }
            } catch (...) {
                qDebug() << "Error accessing item at index" << i;
                continue;
            }
        }
        
        if (hasPotions) {
            qDebug() << "Set potion keys:" << potionKeys;
        } else {
            qDebug() << "No potions found";
        }
        
        // Update controls with potion information
        updateControlsDisplay(hasPotions, potionKeys);
        
        qDebug() << "Status bar updated successfully";
    } else {
        qDebug() << "Hero is null, setting default status bar values";
        
        // Update controls without potion info
        updateControlsDisplay(false, "");
    }
    
    qDebug() << "updateStatusBar() completed";
}

void MainWindow::updateControlsDisplay(bool hasPotions, const QString& potionKeys)
{
    QString controlsText = "Controls: WASD=Move | X=Attack | F=Flee | Z=Look";
    
    if (hasPotions) {
        controlsText += " | Potions: " + potionKeys;
    }
    
    controlsLabel->setText(controlsText);
    qDebug() << "Controls updated:" << controlsText;
}

void MainWindow::applyDarkTheme()
{
    // Modern dark theme color palette
    QString darkBackground = "#1a1a1a";      // Very dark gray
    QString darkerBackground = "#0d0d0d";    // Almost black
    QString cardBackground = "#2d2d2d";      // Dark gray for cards/panels
    QString accentColor = "#4a9eff";         // Modern blue accent
    QString goldenAccent = "#FFD700";        // Golden accent (matching score)
    QString warmRed = "#FF4444";            // Warm red for health bar
    QString warmRedLight = "#FF6666";       // Lighter warm red for gradient
    QString textColor = "#e0e0e0";           // Light gray text
    QString mutedText = "#a0a0a0";           // Muted text
    QString borderColor = "#404040";         // Subtle borders
    
    // Main window styling
    this->setStyleSheet(QString(""
        "QMainWindow {"
        "    background-color: %1;"
        "    color: %2;"
        "}"
        ""
        "QWidget {"
        "    background-color: %1;"
        "    color: %2;"
        "    font-family: 'Segoe UI', Arial, sans-serif;"
        "    font-size: 9pt;"
        "}"
        ""
        "QToolButton {"
        "    background-color: %3;"
        "    color: %2;"
        "    border: 2px solid %4;"
        "    border-radius: 8px;"
        "    padding: 4px;"
        "    font-weight: bold;"
        "    width: 100px;"
        "    height: 100px;"
        "    font-size: 7pt;"
        "    text-align: center;"
        "}"
        ""
        "QToolButton:hover {"
        "    background-color: %5;"
        "    border-color: %6;"
        "}"
        ""
        "QToolButton:pressed {"
        "    background-color: %7;"
        "    border-color: %6;"
        "}"
        ""
        "QToolButton:disabled {"
        "    background-color: %8;"
        "    color: %9;"
        "    border-color: %8;"
        "}"
        ""
        "QLabel {"
        "    color: %2;"
        "    background-color: transparent;"
        "    padding: 4px;"
        "}"
        ""
        "QStatusBar {"
        "    background-color: %3;"
        "    color: %2;"
        "    border-top: 1px solid %4;"
        "    padding: 4px;"
        "}"
        ""
        "QStatusBar::item {"
        "    border: none;"
        "    padding: 2px 8px;"
        "}"
        ""
        "QProgressBar {"
        "    background-color: %8;"
        "    border: 1px solid %4;"
        "    border-radius: 4px;"
        "    text-align: center;"
        "    color: %2;"
        "}"
        ""
        "QProgressBar::chunk {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 %11, stop:1 %12);"
        "    border-radius: 3px;"
        "}"
        ""
        "QPushButton {"
        "    background-color: %3;"
        "    color: %2;"
        "    border: 2px solid %4;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "    font-weight: bold;"
        "    min-height: 20px;"
        "}"
        ""
        "QPushButton:hover {"
        "    background-color: %5;"
        "    border-color: %6;"
        "}"
        ""
        "QPushButton:pressed {"
        "    background-color: %7;"
        "    border-color: %6;"
        "}"
        ""
        "QFrame {"
        "    background-color: %3;"
        "    border: 1px solid %4;"
        "    border-radius: 6px;"
        "}"
        ""
        "QScrollArea {"
        "    background-color: %1;"
        "    border: 1px solid %4;"
        "    border-radius: 6px;"
        "}"
        ""
        "QTextEdit {"
        "    background-color: %1;"
        "    color: %2;"
        "    border: 1px solid %4;"
        "    border-radius: 6px;"
        "    padding: 8px;"
        "    font-family: 'Consolas', 'Monaco', monospace;"
        "    font-size: 9pt;"
        "}"
        ""
        "QComboBox {"
        "    background-color: %3;"
        "    color: %2;"
        "    border: 1px solid %4;"
        "    border-radius: 4px;"
        "    padding: 4px;"
        "    min-width: 80px;"
        "}"
        ""
        "QComboBox::drop-down {"
        "    border: none;"
        "}"
        ""
        "QComboBox::down-arrow {"
        "    image: none;"
        "    border-left: 5px solid transparent;"
        "    border-right: 5px solid transparent;"
        "    border-top: 5px solid %2;"
        "}"
        ""
        "QComboBox QAbstractItemView {"
        "    background-color: %3;"
        "    color: %2;"
        "    border: 1px solid %4;"
        "    selection-background-color: %5;"
        "}"
        ""
        "QCheckBox {"
        "    color: %2;"
        "    spacing: 8px;"
        "}"
        ""
        "QCheckBox::indicator {"
        "    width: 16px;"
        "    height: 16px;"
        "    border: 2px solid %4;"
        "    border-radius: 3px;"
        "    background-color: %3;"
        "}"
        ""
        "QCheckBox::indicator:checked {"
        "    background-color: %6;"
        "    border-color: %6;"
        "}"
        ""
        "QCheckBox::indicator:hover {"
        "    border-color: %6;"
        "}"
        ""
        "QSpinBox {"
        "    background-color: %3;"
        "    color: %2;"
        "    border: 1px solid %4;"
        "    border-radius: 4px;"
        "    padding: 4px;"
        "    min-width: 60px;"
        "}"
        ""
        "QSpinBox::up-button, QSpinBox::down-button {"
        "    background-color: %5;"
        "    border: 1px solid %4;"
        "    width: 16px;"
        "}"
        ""
        "QSpinBox::up-button:hover, QSpinBox::down-button:hover {"
        "    background-color: %6;"
        "}"
        ""
        "QLineEdit {"
        "    background-color: %3;"
        "    color: %2;"
        "    border: 1px solid %4;"
        "    border-radius: 4px;"
        "    padding: 4px;"
        "}"
        ""
        "QLineEdit:focus {"
        "    border-color: %6;"
        "}"
        ""
        "QMenuBar {"
        "    background-color: %3;"
        "    color: %2;"
        "    border-bottom: 1px solid %4;"
        "    font-weight: bold;"
        "    font-size: 10pt;"
        "}"
        ""
        "QMenuBar::item {"
        "    background-color: transparent;"
        "    padding: 6px 12px;"
        "    color: %2;"
        "    border-radius: 3px;"
        "}"
        ""
        "QMenuBar::item:selected {"
        "    background-color: %5;"
        "    color: %1;"
        "}"
        ""
        "QMenuBar::item:hover {"
        "    background-color: %5;"
        "    color: %1;"
        "}"
        ""
        "QMenu {"
        "    background-color: %3;"
        "    color: %2;"
        "    border: 1px solid %4;"
        "    border-radius: 6px;"
        "    padding: 4px;"
        "}"
        ""
        "QMenu::item {"
        "    background-color: transparent;"
        "    padding: 6px 12px;"
        "    border-radius: 3px;"
        "}"
        ""
        "QMenu::item:selected {"
        "    background-color: %5;"
        "    color: %1;"
        "}"
        ""
        "QMenu::item:hover {"
        "    background-color: %5;"
        "    color: %1;"
        "}"
        ""
        "QDialog {"
        "    background-color: %1;"
        "    color: %2;"
        "}"
        ""
        "QMessageBox {"
        "    background-color: %1;"
        "    color: %2;"
        "}"
        ""
        "QTabWidget::pane {"
        "    background-color: %3;"
        "    border: 1px solid %4;"
        "    border-radius: 6px;"
        "}"
        ""
        "QTabBar::tab {"
        "    background-color: %8;"
        "    color: %9;"
        "    border: 1px solid %4;"
        "    padding: 6px 12px;"
        "    margin-right: 2px;"
        "}"
        ""
        "QTabBar::tab:selected {"
        "    background-color: %3;"
        "    color: %2;"
        "    border-bottom-color: %3;"
        "}"
        ""
        "QTabBar::tab:hover {"
        "    background-color: %5;"
        "    color: %2;"
        "}"
        ""
        "QSplitter::handle {"
        "    background-color: %4;"
        "}"
        ""
        "QSplitter::handle:horizontal {"
        "    width: 2px;"
        "}"
        ""
        "QSplitter::handle:vertical {"
        "    height: 2px;"
        "}"
        ""
        "QGroupBox {"
        "    background-color: %3;"
        "    color: %2;"
        "    border: 1px solid %4;"
        "    border-radius: 6px;"
        "    margin-top: 8px;"
        "    padding-top: 8px;"
        "    font-weight: bold;"
        "}"
        ""
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 8px;"
        "    padding: 0 4px 0 4px;"
        "    color: %6;"
        "}"
        ""
        "QSlider::groove:horizontal {"
        "    background-color: %8;"
        "    height: 6px;"
        "    border-radius: 3px;"
        "}"
        ""
        "QSlider::handle:horizontal {"
        "    background-color: %6;"
        "    width: 18px;"
        "    height: 18px;"
        "    border-radius: 9px;"
        "    margin: -6px 0;"
        "}"
        ""
        "QSlider::handle:horizontal:hover {"
        "    background-color: %10;"
        "}"
        ""
        "QSlider::sub-page:horizontal {"
        "    background-color: %6;"
        "    border-radius: 3px;"
        "}"
        ""
        "QSlider::add-page:horizontal {"
        "    background-color: %8;"
        "    border-radius: 3px;"
        "}"
    ).arg(darkBackground)      // %1
     .arg(textColor)          // %2
     .arg(cardBackground)     // %3
     .arg(borderColor)        // %4
     .arg(accentColor)        // %5
     .arg(goldenAccent)       // %6
     .arg(darkerBackground)   // %7
     .arg("#404040")          // %8 - darker gray
     .arg(mutedText)          // %9
     .arg("#FFA500")          // %10 - orange accent
     .arg(warmRed)            // %11 - warm red
     .arg(warmRedLight));     // %12 - lighter warm red
    
    // Apply special styling to specific elements
    if (scoreLabel) {
        scoreLabel->setStyleSheet(QString(""
            "QLabel {"
            "    background-color: rgba(0, 0, 0, 0.8);"
            "    color: %1;"
            "    padding: 8px 12px;"
            "    border-radius: 8px;"
            "    border: 1px solid %2;"
            "    font-weight: bold;"
            "    font-size: 14px;"
            "}"
        ).arg(goldenAccent).arg(borderColor));
    }
    
    if (controlsLabel) {
        controlsLabel->setStyleSheet(QString(""
            "QLabel {"
            "    color: %1;"
            "    background-color: transparent;"
            "    padding: 4px 8px;"
            "    font-size: 10pt;"
            "    font-weight: normal;"
            "}"
        ).arg(mutedText));
    }
    
    qDebug() << "Dark theme applied successfully";
}
