#include <QtDebug>
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QMovie>
#include <QLabel>
#include <QApplication>
#include <QGraphicsEllipseItem>
#include <QPushButton>
#include <QObject>
#include "Graphics.h"
#include "common.h"
#include "Generator.h"
#include "mainwindow.h"

// Static pointer definition
Graphics* Graphics::instance = nullptr;


Graphics::Graphics()
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    
    // Set static instance pointer
    instance = this;
    
    // Initialize equipment pointers to nullptr
    heroHelmet = nullptr;
    heroArmor = nullptr;
    heroWeapon = nullptr;
    
    // Initialize animation system
    flashTimer = nullptr; // We'll use QTimer::singleShot instead
    isFlashing = false;
    flashCount = 0;
    
    // Initialize sprite shake system (replaces screen shake)
    spriteShakeTimer = nullptr;
    spriteShakeIntensity = 0;
    spriteShakeDuration = 0;
    spriteShakeCount = 0;
    heroSpriteItem = nullptr;
    heroOriginalPos = QPointF(0, 0);
    
    // Initialize flash effects safely
    heroFlashEffect = new QGraphicsColorizeEffect();
    heroFlashEffect->setColor(Qt::red);
    heroFlashEffect->setStrength(0.8);
    
    enemyFlashEffect = new QGraphicsColorizeEffect();
    enemyFlashEffect->setColor(Qt::red);
    enemyFlashEffect->setStrength(0.8);
    
    currentEnemyItem = nullptr;
}

Graphics::~Graphics()
{
    // Clean up equipment items
    if (heroArmor) {
        delete heroArmor;
        heroArmor = nullptr;
    }
    if (heroHelmet) {
        delete heroHelmet;
        heroHelmet = nullptr;
    }
    if (heroWeapon) {
        delete heroWeapon;
        heroWeapon = nullptr;
    }
    
    // Clean up animation objects
    // flashTimer is now nullptr, no cleanup needed
    if (heroFlashEffect) {
        delete heroFlashEffect;
    }
    if (enemyFlashEffect) {
        delete enemyFlashEffect;
    }
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

void Graphics::ObjectToDraw(int row, int col)
{
    qDebug() << "ObjectToDraw() called for" << row << "," << col;
    
    // Safety checks
    if (scene() == nullptr) {
        qDebug() << "Scene is null in ObjectToDraw()";
        return;
    }
    
    if (Generated == nullptr) {
        qDebug() << "Generated is null in ObjectToDraw()";
        return;
    }

    setZValue(1);
    int mapValue = lvl.grid[row][col];
    qDebug() << "ObjectToDraw() - mapValue:" << (char)mapValue << "at" << row << "," << col;
    switch(mapValue)
    {
    case('L'):
    {
        if(gExplored.grid[row][col])
        {
            QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(mWall).scaled(gStep,gStep));
            newItem->setPos(col*gStep,row*gStep);
            newItem->setData(0, QString("tile_%1_%2").arg(row).arg(col)); // Add identifier for cleanup
            scene()->addItem(newItem);
        }
    }break;

    case('E'):
    {
        if(gExplored.grid[row][col])
        {
            QGraphicsPixmapItem *newtile=new QGraphicsPixmapItem(QPixmap(mFloor).scaled(gStep,gStep));
            newtile->setPos(col*gStep,row*gStep);
            newtile->setData(0, QString("tile_%1_%2").arg(row).arg(col));
            scene()->addItem(newtile);

            QString enemyName = QString::fromStdString(Generated->returnEnemy(row, col)->getName());
            QString enemyPath = getEnemyGraphic(enemyName);

            QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(enemyPath).scaled(gStep,gStep));
            newItem->setPos(col*gStep,row*gStep);
            newItem->setData(0, QString("enemy_%1_%2").arg(row).arg(col));
            scene()->addItem(newItem);
        }
    }break;

    case('W'):
    {
        if(gExplored.grid[row][col])
        {
            QGraphicsPixmapItem *newtile= new QGraphicsPixmapItem(QPixmap(mFloor).scaled(gStep,gStep));
            newtile->setPos(col*gStep,row*gStep);
            newtile->setData(0, QString("tile_%1_%2").arg(row).arg(col));
            scene()->addItem(newtile);
            
            //legendary glow//
            QString weaponRarity = QString::fromStdString(Generated->returnWeapon(row, col)->mRarity);
            QGraphicsEllipseItem *glowItem = RarityGlow(weaponRarity, row, col);
            glowItem->setData(0, QString("glow_%1_%2").arg(row).arg(col));
            scene()->addItem(glowItem);

            //weapon graphics spawner//
            QString weaponName = QString::fromStdString(Generated->returnWeapon(row, col)->getName());
            QString weaponPath = getEquipmentGraphic(weaponName, Type::T_Weapon);

            QGraphicsPixmapItem *newItem= new QGraphicsPixmapItem(QPixmap(weaponPath).scaled(gStep,gStep));
            newItem->setPos(col*gStep + (gStep/4) ,row*gStep - (gStep/4));
            newItem->setData(0, QString("weapon_%1_%2").arg(row).arg(col));
            scene()->addItem(newItem);
        }
    }break;

    case('H'):
    {
        if(gExplored.grid[row][col])
        {
            QGraphicsPixmapItem *newtile=new QGraphicsPixmapItem(QPixmap(mFloor).scaled(gStep,gStep));
            newtile->setPos(col*gStep,row*gStep);
            newtile->setData(0, QString("tile_%1_%2").arg(row).arg(col));
            scene()->addItem(newtile);

            //legendary glow//
            QString HelmetRarity = QString::fromStdString(Generated->returnHelmet(row, col)->mRarity);
            QGraphicsEllipseItem *glowItem = RarityGlow(HelmetRarity, row, col);
            glowItem->setData(0, QString("glow_%1_%2").arg(row).arg(col));
            scene()->addItem(glowItem);

            //weapon graphics spawner//
            QString helmetName = QString::fromStdString(Generated->returnHelmet(row, col)->getName());
            QString helmetPath = getEquipmentGraphic(helmetName, Type::T_Head);

            QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(helmetPath).scaled(gStep,gStep));
            newItem->setPos(col*gStep, row*gStep + (gStep/8));
            newItem->setData(0, QString("helmet_%1_%2").arg(row).arg(col));
            scene()->addItem(newItem);
        }
    }break;
    case('A'):
    {
        if(gExplored.grid[row][col])
        {
            QGraphicsPixmapItem *newtile=new QGraphicsPixmapItem(QPixmap(mFloor).scaled(gStep,gStep));
            newtile->setPos(col*gStep,row*gStep);
            newtile->setData(0, QString("tile_%1_%2").arg(row).arg(col));
            scene()->addItem(newtile);

            //legendary glow//
            QString ArmorRarity = QString::fromStdString(Generated->returnArmor(row, col)->mRarity);
            QGraphicsEllipseItem *glowItem = RarityGlow(ArmorRarity, row, col);
            glowItem->setData(0, QString("glow_%1_%2").arg(row).arg(col));
            scene()->addItem(glowItem);

            //weapon graphics spawner//
            QString armorName = QString::fromStdString(Generated->returnArmor(row, col)->getName());
            QString armorPath = getEquipmentGraphic(armorName, Type::T_Chest);

            QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(armorPath).scaled(gStep,gStep));
            newItem->setPos(col*gStep, row*gStep - (gStep/4));
            newItem->setData(0, QString("armor_%1_%2").arg(row).arg(col));
            scene()->addItem(newItem);
        }
    }break;
    case('P'):
    {
        if(gExplored.grid[row][col])
        {
            QGraphicsPixmapItem *newtile=new QGraphicsPixmapItem(QPixmap(mFloor).scaled(gStep,gStep));
            newtile->setPos(col*gStep,row*gStep);
            newtile->setData(0, QString("tile_%1_%2").arg(row).arg(col));
            scene()->addItem(newtile);

            //legendary glow//
            QString rarity = QString::fromStdString(Generated->returnPotion(row, col)->mRarity);
            QGraphicsEllipseItem *glowItem = RarityGlow(rarity, row, col);
            glowItem->setData(0, QString("glow_%1_%2").arg(row).arg(col));
            scene()->addItem(glowItem);

            QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(mPotion).scaled(gStep/2,gStep/2));
            newItem->setPos(col*gStep + (gStep/4) ,row*gStep + (gStep/4));
            newItem->setData(0, QString("potion_%1_%2").arg(row).arg(col));
            scene()->addItem(newItem);
        }
    }break;

    case('F'):
    {
        if(gExplored.grid[row][col])
        {
            QGraphicsRectItem *rectItem = scene()->addRect(col*gStep, row*gStep, gStep, gStep,QPen(Qt::NoPen),QBrush(Qt::magenta));
            rectItem->setData(0, QString("floor_%1_%2").arg(row).arg(col));
        }
    }break;

    default:
    {
        if(gExplored.grid[row][col])
        {
            QGraphicsPixmapItem *newItem=new QGraphicsPixmapItem(QPixmap(mFloor).scaled(gStep,gStep));
            newItem->setPos(col*gStep,row*gStep);
            newItem->setData(0, QString("tile_%1_%2").arg(row).arg(col));
            scene()->addItem(newItem);
        }
    }break;
    }
}

void Graphics::drawMapUpdateVecinity()
{
    qDebug() << "drawMapUpdateVecinity() called";
    
    // Safety checks
    if (Hero == nullptr) {
        qDebug() << "Hero is null in drawMapUpdateVecinity()";
        return;
    }
    
    if (scene() == nullptr) {
        qDebug() << "Scene is null in drawMapUpdateVecinity()";
        return;
    }

    qDebug() << "drawMapUpdateVecinity() - Hero position:" << Hero->heroRow << "," << Hero->heroCol;
    qDebug() << "drawMapUpdateVecinity() - Map dimensions:" << gWidth << "x" << gHeight;

    // Clean up existing items in the vicinity before redrawing
    for(int row=Hero->heroRow - 1; row <= Hero->heroRow + 1; ++row)
    {
        for(int col=Hero->heroCol - 1; col <= Hero->heroCol + 1; ++col)
        {
            if(row >= 0 && col >=0  && row < gWidth && col < gHeight)
            {
                // Remove existing items at this position
                QString tileId = QString("tile_%1_%2").arg(row).arg(col);
                QString enemyId = QString("enemy_%1_%2").arg(row).arg(col);
                QString weaponId = QString("weapon_%1_%2").arg(row).arg(col);
                QString helmetId = QString("helmet_%1_%2").arg(row).arg(col);
                QString armorId = QString("armor_%1_%2").arg(row).arg(col);
                QString potionId = QString("potion_%1_%2").arg(row).arg(col);
                QString glowId = QString("glow_%1_%2").arg(row).arg(col);
                QString floorId = QString("floor_%1_%2").arg(row).arg(col);
                
                QList<QGraphicsItem*> items = scene()->items();
                foreach (QGraphicsItem* item, items) {
                    QString itemId = item->data(0).toString();
                    if (itemId == tileId || itemId == enemyId || itemId == weaponId || 
                        itemId == helmetId || itemId == armorId || itemId == potionId || 
                        itemId == glowId || itemId == floorId) {
                        scene()->removeItem(item);
                        delete item;
                    }
                }
                
                // Draw new items
                qDebug() << "drawMapUpdateVecinity() - drawing at" << row << "," << col;
                ObjectToDraw(row, col);
            }
        }
    }
    setFocus();
    update();
    qDebug() << "drawMapUpdateVecinity() completed";
}

void Graphics::drawMapFullStatic()
{
    qDebug() << "drawMapFullStatic() called";
    setZValue(0);//draw in background
    QGraphicsRectItem *rectItem = scene()->addRect(0, 0, gStep*30, gStep*30,QPen(Qt::NoPen),QBrush(Qt::black));
    rectItem->setData(0, "background_rect"); // Add identifier for cleanup
    setFocus();
    update();
    qDebug() << "drawMapFullStatic() completed";
}

void Graphics::clearSceneItems()
{
    qDebug() << "Clearing scene items...";
    
    // Safety check
    if (!scene()) {
        qDebug() << "No scene available for clearing";
        return;
    }
    
    // Get all items and remove them safely
    QList<QGraphicsItem*> items = scene()->items();
    qDebug() << "Found" << items.size() << "items to clear";
    
    foreach (QGraphicsItem* item, items) {
        // Keep the hero item (this Graphics object) and UI elements
        if (item != this && item->zValue() < 10) {
            qDebug() << "Removing item:" << item->data(0).toString();
            scene()->removeItem(item);
            delete item;
        }
    }
    
    qDebug() << "Scene items cleared";
}



void Graphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Paint the hero
    if (Hero != nullptr)
    {
        painter->drawPixmap(0, 0, QPixmap(mHero).scaled(gStep, gStep));
    }
}

// Helper function to convert std::string to QString
QString toQString(const std::string& str)
{
    return QString::fromStdString(str);
}

void Graphics::updateEquipmentSprite(int type, const std::string& itemName, QGraphicsPixmapItem*& item)
{
    qDebug() << "updateEquipmentSprite() called for type:" << type << "item:" << QString::fromStdString(itemName);
    
    QString itemNameStr = QString::fromStdString(itemName);
    QString imagePath = getEquipmentGraphic(itemNameStr, type);

    if (imagePath.isEmpty()) {
        qDebug() << "Invalid image path for item:" << itemNameStr;
        return;
    }

    QPixmap pixmap(imagePath);
    if (item == nullptr) // Fix: Check if item is null
    {
        qDebug() << "Creating new equipment item";
        // Create new item if it doesn't exist
        item = new QGraphicsPixmapItem(QPixmap(imagePath).scaled(gStep, gStep));
        item->setPos(Hero->heroCol * gStep, Hero->heroRow * gStep);
        scene()->addItem(item);
        item->setZValue(3);
    }
    else
    {
        qDebug() << "Updating existing equipment item";
        // Update existing item
        item->setPixmap(pixmap.scaled(gStep, gStep));
        item->setPos(Hero->heroCol * gStep, Hero->heroRow * gStep);
    }
    qDebug() << "updateEquipmentSprite() completed";
}

void Graphics::drawPosition()
{
    qDebug() << "drawPosition() called";
    
    // Safety checks
    if (Hero == nullptr) {
        qDebug() << "Hero is null in drawPosition()";
        return;
    }
    
    if (scene() == nullptr) {
        qDebug() << "Scene is null in drawPosition()";
        return;
    }

    qDebug() << "drawPosition() - removing existing equipment items";
    // Remove existing equipment items and reset pointers
    if (heroArmor) {
        scene()->removeItem(heroArmor);
        delete heroArmor;
        heroArmor = nullptr;
    }
    if (heroHelmet) {
        scene()->removeItem(heroHelmet);
        delete heroHelmet;
        heroHelmet = nullptr;
    }
    if (heroWeapon) {
        scene()->removeItem(heroWeapon);
        delete heroWeapon;
        heroWeapon = nullptr;
    }
    
    qDebug() << "drawPosition() - updating hero position";
    // Update hero position sprite
    setPos(Hero->heroCol * gStep, Hero->heroRow * gStep);
    
    qDebug() << "drawPosition() - calling drawMapUpdateVecinity";
    drawMapUpdateVecinity();

    qDebug() << "drawPosition() - updating equipment sprites";
    // Update equipment position layers sprites
    if (Generated != nullptr && Hero != nullptr)
    {
        try {
            updateEquipmentSprite(Type::T_Chest, Hero->selectItem(T_Chest).getName(), heroArmor);
            updateEquipmentSprite(Type::T_Head, Hero->selectItem(T_Head).getName(), heroHelmet);
            updateEquipmentSprite(Type::T_Weapon, Hero->selectItem(T_Weapon).getName(), heroWeapon);
        } catch (const std::exception& e) {
            qDebug() << "Exception in updateEquipmentSprite:" << e.what();
        }
    }
    else
    {
        qDebug() << "Generated or Hero is null in drawPosition()";
    }

    setFocus();
    update();
    qDebug() << "drawPosition() completed";
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

// Simple damage flash animation (safe version)
void Graphics::startHeroDamageFlash()
{
    if (!this || !scene()) {
        return;
    }
    
    try {
        QGraphicsColorizeEffect* heroFlash = new QGraphicsColorizeEffect();
        heroFlash->setColor(Qt::red);
        heroFlash->setStrength(0.8);
        
        this->setGraphicsEffect(heroFlash);
        
        QTimer::singleShot(200, [this]() {
            if (this && scene()) {
                try {
                    this->setGraphicsEffect(nullptr);
                } catch (...) {
                    // Ignore exceptions during cleanup
                }
            }
        });
    } catch (...) {
        // Ignore exceptions during flash animation
    }
}

void Graphics::startEnemyDamageFlash(int enemyRow, int enemyCol)
{
    if (!this || !scene()) {
        return;
    }
    
    try {
        QString enemyId = QString("enemy_%1_%2").arg(enemyRow).arg(enemyCol);
        QList<QGraphicsItem*> items = scene()->items();
        
        foreach (QGraphicsItem* item, items) {
            QString itemId = item->data(0).toString();
            
            if (itemId == enemyId) {
                QGraphicsPixmapItem* enemyItem = dynamic_cast<QGraphicsPixmapItem*>(item);
                if (enemyItem) {
                    QGraphicsColorizeEffect* enemyFlash = new QGraphicsColorizeEffect();
                    enemyFlash->setColor(Qt::red);
                    enemyFlash->setStrength(0.8);
                    
                    enemyItem->setGraphicsEffect(enemyFlash);
                    
                    QTimer::singleShot(200, [enemyItem]() {
                        if (enemyItem && enemyItem->scene()) {
                            try {
                                enemyItem->setGraphicsEffect(nullptr);
                            } catch (...) {
                                // Ignore exceptions during cleanup
                            }
                        }
                    });
                    break;
                }
            }
        }
    } catch (...) {
        // Ignore exceptions during flash animation
    }
}

void Graphics::stopDamageFlash()
{
    // Function disabled - no action taken
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
    return widget;
}

void Graphics::startSpriteShake(int intensity, int duration)
{
    if (!this || !scene()) {
        return;
    }
    
    try {
        QRectF originalRect = scene()->sceneRect();
        
        int offsetX = (rand() % (intensity * 2)) - intensity;
        int offsetY = (rand() % (intensity * 2)) - intensity;
        
        scene()->setSceneRect(originalRect.x() + offsetX, originalRect.y() + offsetY, 
                            originalRect.width(), originalRect.height());
        
        QTimer::singleShot(100, [this, originalRect]() {
            if (this && scene()) {
                try {
                    scene()->setSceneRect(originalRect);
                } catch (...) {
                    // Ignore exceptions during restore
                }
            }
        });
    } catch (...) {
        // Ignore exceptions during shake animation
    }
}

void Graphics::performSpriteShakeCycle()
{
    // Function disabled
}

void Graphics::stopSpriteShake()
{
    // Function disabled
}

void Graphics::stopAllAnimations()
{
    isFlashing = false;
    flashCount = 0;
    currentEnemyItem = nullptr;
    
    if (this) {
        this->setGraphicsEffect(nullptr);
    }
    
    stopSpriteShake();
}



