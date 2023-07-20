#include <QtDebug>
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QMovie>
#include <QLabel>
#include <QApplication>
#include <QGraphicsEllipseItem>
#include <QPushButton>
#include "Graphics.h"
#include "common.h"
#include "Generator.h"
#include "mainwindow.h"


Graphics::Graphics()
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    //setFlag(QGraphicsItem::ItemIsMovable);
    //scene()->removeItem(newItem);
}

void Graphics::SetGenerator(Generator* MainGenerator)
{
    Generated = MainGenerator;
}

QRectF Graphics::boundingRect() const
{
    return QRectF(0,0,gStep,gStep);
}

QGraphicsEllipseItem* RarityGlow(QString weaponRarity, int row, int col)
{
    QGraphicsEllipseItem* circle = new QGraphicsEllipseItem(0,0, gStep, gStep);
    //QGraphicsEllipseItem(gStep/4, gStep/4, gStep/2, gStep/2);
    QRadialGradient radialGrad(QPointF(gStep/2, gStep/2), gStep);
    if(weaponRarity == "common")
    {
        radialGrad.setColorAt(0, Qt::transparent);
        radialGrad.setColorAt(0.25, Qt::transparent);
    }else if(weaponRarity.contains("rare"))
    {
        radialGrad.setColorAt(0, Qt::green);
        radialGrad.setColorAt(0.25, Qt::transparent);
    }else if(weaponRarity.contains("legendary"))
    {
        radialGrad.setColorAt(0, Qt::blue);
        radialGrad.setColorAt(0.25, Qt::yellow);
    }
    radialGrad.setColorAt(0.5, Qt::transparent);
    QPen pen(Qt::NoPen);
    circle->setPen(pen);
    //radialGrad.setColorAt(0, QColor(255, 255, 0, 150));  // Solid yellow
    //radialGrad.setColorAt(1, QColor(255, 255, 0, 0));  // Fully transpare
    circle->setBrush(radialGrad);
    circle->setPos(col*gStep,row*gStep);
    // Add the circle item to the scene
    return circle;
}


QString Graphics::getEquipmentGraphic(QString name, int itemType)
{
    switch(itemType)
    {
    case Type::T_Head:
        if(name.contains("hat")) return m_hat;
        else if (name.contains("thief hood")) return m_thiefhood;
        else if (name.contains("straight helmet")) return m_straighthelmet;
        else if (name.contains("soldier helm")) return m_soldierhelm;
        else if (name.contains("sage cap")) return m_sagecap;
        else if (name.contains("templar helmet")) return m_templarhelmet;
        else if (name.contains("dragon mask")) return m_dragon_mask;
        else if (name.contains("barbute")) return m_barbute_helmet;
        else if (name.contains("evil dark helm")) return m_evil_dark_helm;
        break;
    case Type::T_Chest:
        if(name.contains("cloth shirt")) return m_clothshirt;
        else if (name.contains("stripped armor")) return m_strippedarmor;
        else if (name.contains("chainmail")) return m_chainmail;
        else if (name.contains("magician robe")) return m_magicianrobe;
        else if (name.contains("mithrill armor")) return m_mithrillarmor;
        else if (name.contains("brigadine")) return m_brigadine;
        else if (name.contains("dragon slayer")) return m_dragonslayerset;
        else if (name.contains("evil cursed armor")) return m_evilcursedarmor;
        break;
    case Type::T_Weapon:
        if(name.contains("knife")) return m_knife;
        else if(name.contains("pickaxe")) return m_pickaxe;
        else if (name.contains("sword")) return m_sword;
        else if (name.contains("axe")) return m_axe;
        else if (name.contains("trident")) return m_trident;
        else if (name.contains("wand")) return m_wand;
        else if (name.contains("long sword")) return m_longsword;
        else if (name.contains("spear")) return m_spear;
        else if (name.contains("javelin")) return m_javelin;
        else if (name.contains("claymore")) return m_claymore;
        else if (name.contains("great sword")) return m_greatsword;
        break;
    case Type::T_Consumable:
        if(name.contains("potion")) return mPotion;
        else if (name.contains("")) return mPotion;
        break;
    }

    qDebug() << "no graphic for: " << name << itemType;
    return m_None;
}

QString Graphics::getEnemyGraphic(QString name)
{
    if(name.contains("skeleton")) return m_skeleton;
    else if(name.contains("ghost")) return m_ghost;
    else if (name.contains("demon")) return m_demon;
    else if (name.contains("troll")) return m_troll;
    else if (name.contains("bat")) return m_bat;
    else if (name.contains("minotaur")) return m_minotaur;
    else if (name.contains("gryphon")) return m_gryphon;
    else if (name.contains("centaur")) return m_centaur;
    else if (name.contains("reaper")) return m_reaper;
    else if (name.contains("dragon")) return m_dragon;
    qDebug() << "no graphic for: " << name;
    return m_None;
}

void Graphics::drawMap()
{
    int mapValue;
    setZValue(0);//draw in background
    scene()->addRect(0, 0, gStep*30, gStep*30,QPen(Qt::NoPen),QBrush(Qt::black));
    //scene()->addRect(0, 0, gStep*gWidth, gStep*gHeight,QPen(Qt::gray),QBrush(Qt::gray));
    setZValue(1);//draw over background

    for(int row=0; row < gHeight; ++row)
    {
      for(int col=0; col < gWidth; ++col)
      {
          mapValue = lvl.grid[row][col];

          //REVIEW : [STRUCT][CONVENTION] : You're comparing a int vs a char, please fix this, use proper enum also
          switch(mapValue)
          {
              case('L'):
              {
                if(gExplored.grid[row][col])
                {
                    QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(mWall).scaled(gStep,gStep));
                    newItem->setPos(col*gStep,row*gStep);
                    scene()->addItem(newItem);
                 }
              }break;

              case('E'):
              {
                  if(gExplored.grid[row][col])
                  {
                    QGraphicsPixmapItem *newtile=new QGraphicsPixmapItem(QPixmap(mFloor).scaled(gStep,gStep));
                    newtile->setPos(col*gStep,row*gStep);
                    scene()->addItem(newtile);

                    QString enemyName = QString::fromStdString(Generated->returnEnemy(row, col)->getName());
                    QString enemyPath = getEnemyGraphic(enemyName);

                    QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(enemyPath).scaled(gStep,gStep));
                    newItem->setPos(col*gStep,row*gStep);
                    scene()->addItem(newItem);

                    /*
                     * GIF TEST
                     *
                    QGraphicsPixmapItem newItem;
                    QMovie enemymov2(":/Images/ghost.gif");
                    //QGraphicsPixmapItem *newItem = new QGraphicsPixmapItem(QPixmap(&enemymov).scaled(gStep,gStep));
                    newItem.setPixmap(enemymov2.currentPixmap());
                    newItem.setScale(gStep);
                    newItem.setPos(col*gStep,row*gStep);
                    scene()->addItem(&newItem);
                    enemymov2.start();
                    */
                  }
              }break;

              case('W'):
              {
                  if(gExplored.grid[row][col])
                  {
                    QGraphicsPixmapItem *newtile= new QGraphicsPixmapItem(QPixmap(mFloor).scaled(gStep,gStep));
                    newtile->setPos(col*gStep,row*gStep);
                    scene()->addItem(newtile);
                    //legendary glow//
                    QString weaponRarity = QString::fromStdString(Generated->returnWeapon(row, col)->mRarity);
                    scene()->addItem(RarityGlow(weaponRarity, row, col));

                    //weapon graphics spawner//
                    QString weaponName = QString::fromStdString(Generated->returnWeapon(row, col)->getName());
                    QString weaponPath = getEquipmentGraphic(weaponName, Type::T_Weapon);

                    QGraphicsPixmapItem *newItem= new QGraphicsPixmapItem(QPixmap(weaponPath).scaled(gStep,gStep));
                    newItem->setPos(col*gStep + (gStep/4) ,row*gStep - (gStep/4));
                    scene()->addItem(newItem);
                  }
              }break;

              case('H'):
              {
                  if(gExplored.grid[row][col])
                  {
                    QGraphicsPixmapItem *newtile=new QGraphicsPixmapItem(QPixmap(mFloor).scaled(gStep,gStep));
                    newtile->setPos(col*gStep,row*gStep);
                    scene()->addItem(newtile);

                    //legendary glow//
                    QString HelmetRarity = QString::fromStdString(Generated->returnHelmet(row, col)->mRarity);
                    scene()->addItem(RarityGlow(HelmetRarity, row, col));

                    //weapon graphics spawner//
                    QString helmetName = QString::fromStdString(Generated->returnHelmet(row, col)->getName());
                    QString helmetPath = getEquipmentGraphic(helmetName, Type::T_Head);

                    QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(helmetPath).scaled(gStep,gStep));
                    newItem->setPos(col*gStep, row*gStep + (gStep/8));
                    scene()->addItem(newItem);
                  }
              }break;
              case('A'):
              {
                  if(gExplored.grid[row][col])
                  {
                    QGraphicsPixmapItem *newtile=new QGraphicsPixmapItem(QPixmap(mFloor).scaled(gStep,gStep));
                    newtile->setPos(col*gStep,row*gStep);
                    scene()->addItem(newtile);

                    //legendary glow//
                    QString ArmorRarity = QString::fromStdString(Generated->returnArmor(row, col)->mRarity);
                    scene()->addItem(RarityGlow(ArmorRarity, row, col));


                    //weapon graphics spawner//
                    QString armorName = QString::fromStdString(Generated->returnArmor(row, col)->getName());
                    QString armorPath = getEquipmentGraphic(armorName, Type::T_Chest);

                    QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(armorPath).scaled(gStep,gStep));
                    newItem->setPos(col*gStep, row*gStep - (gStep/4));
                    scene()->addItem(newItem);
                  }
              }break;
              case('P'):
              {
                if(gExplored.grid[row][col])
                {
                  QGraphicsPixmapItem *newtile=new QGraphicsPixmapItem(QPixmap(mFloor).scaled(gStep,gStep));
                  newtile->setPos(col*gStep,row*gStep);
                  scene()->addItem(newtile);

                  //legendary glow//
                  QString rarity = QString::fromStdString(Generated->returnPotion(row, col)->mRarity);
                  scene()->addItem(RarityGlow(rarity, row, col));

                  QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(mPotion).scaled(gStep/2,gStep/2));
                  newItem->setPos(col*gStep + (gStep/4) ,row*gStep + (gStep/4));
                  scene()->addItem(newItem);
                }
              }break;

              case('F'):
              {
                if(gExplored.grid[row][col])
                    scene()->addRect(col*gStep, row*gStep, gStep, gStep,QPen(Qt::NoPen),QBrush(Qt::magenta));
              }break;

                default:
                {
                    if(gExplored.grid[row][col])
                        {
                        //QGraphicsPixmapItem(QPixmap(mWall).scaled(gStep,gStep)).setPos(col*gStep,row*gStep);
                        QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(mFloor).scaled(gStep,gStep));
                        newItem->setPos(col*gStep,row*gStep);
                        scene()->addItem(newItem);
                    }
                }break;
            }
        }
     }
    update();
}

void Graphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
      //REVIEW [IMP] : paint event is called tons of time, what happens it pressed is true or in the else case?
      //               QGraphicsItem::paint should be called in those cases.
      //REVIEW [STRUCT][UNUSED_CODE] : remove unnecessary code
      if(pressed)
      {

      }
      else
      {

      }
      if(Hero != nullptr)
      {
        painter->drawPixmap(0,0,QPixmap(mHero).scaled(gStep,gStep));
      }
}

void Graphics::drawPosition()
{
    //update hero position sprite
    setPos(Hero->heroCol*gStep,Hero->heroRow*gStep);
    //update equipment position layers sprites
    if(Generated != nullptr && Hero != nullptr)
    {
        // armor chest
        QString chestName = QString::fromStdString(Hero->selectItem(T_Chest).getName());
        QString chestPath = getEquipmentGraphic(chestName, Type::T_Chest);
        if(heroArmor == nullptr)
        {
            heroArmor = new QGraphicsPixmapItem(QPixmap(chestPath).scaled(gStep,gStep));
            heroArmor->setPos(Hero->heroCol*gStep, Hero->heroRow*gStep);
            scene()->addItem(heroArmor);
            heroArmor->setZValue(7);
        }else{
            heroArmor->setPixmap(QPixmap(chestPath).scaled(gStep,gStep));
            heroArmor->setPos(Hero->heroCol*gStep, Hero->heroRow*gStep);
        }
        // helmet head
        QString helmetName = QString::fromStdString(Hero->selectItem(T_Head).getName());
        QString helmetPath = getEquipmentGraphic(helmetName, Type::T_Head);
        if(heroHelmet== nullptr)
        {
            heroHelmet = new QGraphicsPixmapItem(QPixmap(helmetPath).scaled(gStep,gStep));
            heroHelmet->setPos(Hero->heroCol*gStep, Hero->heroRow*gStep);
            scene()->addItem(heroHelmet);
            heroHelmet->setZValue(7);
        }else{
            heroHelmet->setPixmap(QPixmap(helmetPath).scaled(gStep,gStep));
            heroHelmet->setPos(Hero->heroCol*gStep, Hero->heroRow*gStep);
        }
        // weapon
        QString weaponName = QString::fromStdString(Hero->selectItem(T_Weapon).getName());
        QString weaponPath = getEquipmentGraphic(weaponName, Type::T_Weapon);
        if(heroWeapon== nullptr)
        {
            heroWeapon = new QGraphicsPixmapItem(QPixmap(weaponPath).scaled(gStep,gStep));
            heroWeapon->setPos(Hero->heroCol*gStep, Hero->heroRow*gStep);
            scene()->addItem(heroWeapon);
            heroWeapon->setZValue(7);
        }else{
            heroWeapon->setPixmap(QPixmap(weaponPath).scaled(gStep,gStep));
            heroWeapon->setPos(Hero->heroCol*gStep, Hero->heroRow*gStep);
        }
    }else
        qDebug() << "Generated Error";
    /*
    QGraphicsScene scene;
    QLabel *gif_anim = new QLabel();
    QMovie *movie = new QMovie(":/Images/herodefault_walking.gif");
    gif_anim->setMovie(movie);
    movie->start();
    QGraphicsProxyWidget *proxy = scene.addWidget(gif_anim);
    setZValue(7);
    */
    setFocus();
    //setZValue(2);
    update();
}

void Graphics::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = false;
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void Graphics::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = true;
    update();
    QGraphicsItem::mousePressEvent(event);
}


QWidget* MainWindow::setupMovementArrows()
{
    QWidget* widget = new QWidget;
    QGridLayout* layout = new QGridLayout(widget);
    QLabel* label = new QLabel("Controls");
    layout->addWidget(label);

    enum Buttons
    {
        GB_UP =4,
        GB_LEFT = 6,
        GB_RIGHT = 8,
        GB_DOWN = 10,
        GB_ACTION = 21,
        GB_POTION = 22,
        GB_FLEE = 23,
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
                    QPushButton* buttonGB_FLEE = new QPushButton(QString("FLEE").arg(row * 3 + col + 1), this);
                    connect(buttonGB_FLEE, &QPushButton::clicked, layout, [this] { inputHandle(nullptr, B_Flee); });
                    layout->addWidget(buttonGB_FLEE, row, col);
                    break;
                }
                deafult:
                {
                    QLabel* labelfill = new QLabel("x");
                    layout->addWidget(labelfill, row, col);
                }
            }
            ++counter;
        }
    }

    layout->setAlignment(Qt::AlignCenter);
    return widget;
}

