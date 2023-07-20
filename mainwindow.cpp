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
//REVIEW [STRCUT][REPEATED_CODE] : Graphics Scene is already declared in the header file but it is not being used.
//                                 place inclusion properly when you need it.
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QSpinBox>
#include <QTextEdit>
#include <QJsonObject>
#include <QEvent>
#include <QPixmap>
#include <QGraphicsScene>
#include <QMediaPlayer>
#include <QSoundEffect>
#include <QAudioOutput>
#include "mainwindow.h"
#include "Graphics.h"
#include "GameManager.h"

#define MINUTE 600000

//REVIEW [STRUCT][CONVENTION] : No initialization list used for constructor. please comply with the code convention.
/*
 *
    MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
          timer(new QTimer(this)),
    {
        setCentralWidget(setupContainerVertical());
        connect(timer, SIGNAL(timeout()), this, SLOT(idle()));
        timer->start(2000*60);
    }
 *
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setCentralWidget(setupContainerVertical());
    timer = new QTimer(this);
    //REVIEW [STRUCT][CONVENTION] : Please use the new connect sintaxis when using the connect statement.
    connect(timer, SIGNAL(timeout()), this, SLOT(idle()));
    //REVIEW [STRUCT][MAGIC_NUMBER] : 2000 * 6 what does that mean? use a #define or global variable to avoid
    //                                magic numbers when you can.
    timer->start(2000*60);
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
        else gValueButton = event->key();
    }
    if (gFlee && (gState != S_WaitForArrowKeys) && (gState != S_GameOver) && (gState != S_Win) )//if not in battle move or wating for input
    {
        if (gValueButton == B_Right)
        {
            //REVIEW [STRUCT][MAGIC_NUMBER] : walk 0,1 what does that mean? use a #define or global variable to avoid
            //                                magic numbers when you can.
            mDGraphics->Hero->walk(0,1);
            mLogContent.prepend("you move right\n");
        }
        else if(gValueButton == B_Down)
        {
            mDGraphics->Hero->walk(1,0);
            mLogContent.prepend("you moved down\n");
        }
        else if(gValueButton == B_Left)
        {
            mDGraphics->Hero->walk(0,-1);
            mLogContent.prepend("you moved left\n");
        }
        else if(gValueButton == B_Up)
        {
            mDGraphics->Hero->walk(-1,0);
            mLogContent.prepend("you moved up\n");
        }
        if (gState == S_ActionToPickItem)
        {
            gState = S_Normal;
        }
        mScrollLog->setText(mLogContent);
        mDGraphics->drawPosition();
    }
    //qDebug() << "0 normal 1 key input 2 item pickup \n";
    /* handle any current position action */
    gameEventHandle();
}

void MainWindow::gameEventHandle()
{
    if(mDGraphics->Hero == nullptr) return;
    if (gState == S_GameOver)
    {
        //REVIEW [DEF][INPUT_SANITIZE] : probaly this only works on your computer.
        SoundEngine::PlaySoundByName("gameover", 1);
        QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(mGameover).scaled(PIXEL,PIXEL));
        newItem->setPos(0,0);
        mDGraphics->scene()->addItem(newItem);
        QMessageBox *myDialog = new QMessageBox(this);
        myDialog->setText("Your body lies in a pool of blood while the enemy eats your flesh and bones");
        myDialog->setToolTip("Game over");
        mDGraphics->Hero->~Character();
        Generated->~Generator();
        myDialog->exec();
    }
    else if (gState == S_Win)
    {

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
        GameManager::handleEvent(*mDGraphics->Hero,*Generated);
        mDGraphics->drawMap();
        updateInventory();
    }
}

void MainWindow::updateInventory()
{
    const int dim = 40;
    if(mDGraphics->Hero == nullptr)
        return;

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
    //gState = QJsonObject::value(json);
    gState = json["state"].toInt();
    mDefaultPath = QString(json["defaultPath"].toString());
    mLogContent = QString(json["logContent"].toString());
    gStep = json["step"].toInt();
    gHeight = json["H"].toInt();
    gWidth = json["W"].toInt();
    gFlee = json["step"].toBool();
    //lvl = Map(json["mapLvl"].toObject(lvl) ;
    //Explored = Map(json["mapExplored"].toObject(lvl);
    //mPlayer.read(json["player"].toObject());
    //Character *Hero = new Character(json["player"].toObject());
    //mDGraphics->Hero = Hero;
}

void MainWindow::write(QJsonObject &json) const
{
    //REVIEW [STRCUT][UNUSED_CODE]
    //QJsonObject::insert()
    //REVIEW [STRUCT][CONVENTION] : Use proper enum for Json tags
    json["state"] = gState;
    json["defaultPath"] = mDefaultPath;
    json["logContent"].toString() = mLogContent;
    json["step"] = gStep;
    json["H"] = gHeight;
    json["W"] = gWidth;
    json["step"] = gFlee;
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
    mLogContent += "Load a file to begin..\n";
    mScrollLog = new QTextEdit(mLogContent,this);
    mScrollLog->isReadOnly();
    mScrollLog->setFontPointSize(12);
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

    mDGraphics = new Graphics();
    //character->setBrush(QBrush(Qt::blue));
    mDGraphics->setFocus();
    graphicsScene->addItem(mDGraphics);

//  connect(graphicsScene, SIGNAL (triggered()),this, SLOT (drawFrame()));


    QGraphicsView *graphicsView = new QGraphicsView(graphicsScene, this);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    graphicsView->setScene(graphicsScene);
    graphicsView->setFixedSize(PIXEL,PIXEL);
    graphicsScene->setSceneRect(0,0,PIXEL,PIXEL);

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

    //REVIEW [STRCUT][CONVENTION] : Comply with the code convention, please use correct indentation
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
        mButtonAtack = QKeySequence::fromString(lineEditAction->text())[0];
        mButtonPotion = QKeySequence::fromString(lineEditPotion->text())[0];
        mButtonFlee = QKeySequence::fromString(lineEditFlee->text())[0];
        mButtonUp = QKeySequence::fromString(lineEditUp->text())[0];
        mButtonLeft = QKeySequence::fromString(lineEditLeft->text())[0];
        mButtonRight = QKeySequence::fromString(lineEditRight->text())[0];
        mButtonDown = QKeySequence::fromString(lineEditDown->text())[0];
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

        gStep = PIXEL/gWidth;
        gState = S_Normal;
        mLogContent = "";
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
            mDGraphics->drawPosition();
            //if(mDGraphics->Generated != nullptr) mDGraphics->drawMap();
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
