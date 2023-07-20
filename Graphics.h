#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QGraphicsPixmapItem>
#include <QMovie>
#include "Generator.h"
#include "Character.h"

class Graphics : public QGraphicsItem
{

public:

    Graphics();
    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void drawMap();
    void drawPosition();

    Character *Hero = nullptr;
    QGraphicsPixmapItem *heroHelmet;
    QGraphicsPixmapItem *heroArmor;
    QGraphicsPixmapItem *heroWeapon;
    QString getEnemyGraphic(QString name);
    QString getEquipmentGraphic(QString name, int itemType);

    void SetGenerator(Generator *MainGenerator);
    Generator* Generated;

    bool pressed;

protected: 
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

private:
    const QString mWall = ":/Images/wall.png";
    const QString mFloor = ":/Images/floor.png";

    //weapons
    const QString m_knife = ":/Images/knife.png";
    const QString m_pickaxe = ":/Images/pickaxe.png";
    const QString m_sword = ":/Images/sword.png";
    const QString m_axe = ":/Images/axe.png";
    const QString m_trident = ":/Images/trident.png";
    const QString m_wand = ":/Images/sword.png";
    const QString m_longsword = ":/Images/sword.png";
    const QString m_spear = ":/Images/sword.png";
    const QString m_javelin = ":/Images/sword.png";
    const QString m_claymore= ":/Images/sword.png";
    const QString m_greatsword = ":/Images/sword.png";

    //Helmet
    const QString m_hat = ":/Images/Transparent.png";
    const QString m_thiefhood = ":/Images/soldierhelm.png";
    const QString m_soldierhelm= ":/Images/soldierhelm.png";
    const QString m_sagecap = ":/Images/sagecap.png";
    const QString m_straighthelmet = ":/Images/soldierhelm.png";
    const QString m_templarhelmet  = ":/Images/templarhelmet.png";
    //Utilities
    const QString mHero = ":/Images/herodefault.png";
    const QString m_None = ":/Images/None.png";
    const QString m_Transparent = ":/Images/Transparent.png";


    //Weapon

    const QString m_dragon_mask = ":/Images/templarhelmet.png";
    const QString m_barbute_helmet = ":/Images/templarhelmet.png";
    const QString m_evil_dark_helm = ":/Images/templarhelmet.png";

    //Armor
    const QString m_clothshirt= ":/Images/clothshirt.png";
    const QString m_strippedarmor = ":/Images/strippedarmor.png";
    const QString m_chainmail = ":/Images/chainmail.png";
    const QString m_magicianrobe = ":/Images/magicianrobe.png";
    const QString m_mithrillarmor = ":/Images/mithrillarmor.png";
    const QString m_brigadine= ":/Images/brigadine.png";
    const QString m_dragonslayerset = ":/Images/brigadine.png";
    const QString m_evilcursedarmor = ":/Images/brigadine.png";

    //Potions
    const QString mPotion = ":/Images/potion.png";
    //Enemy
    const QString m_skeleton = ":/Images/skeleton.png";
    const QString m_ghost = ":/Images/ghost.png";
    const QString m_demon = ":/Images/demon.png";
    const QString m_troll = ":/Images/troll.png";
    const QString m_bat = ":/Images/bat.png";
    const QString m_minotaur = ":/Images/minotaur.png";
    const QString m_centaur = ":/Images/centaur.png";
    const QString m_gryphon = ":/Images/gryphon.png";
    const QString m_reaper = ":/Images/reaper.png";
    const QString m_dragon = ":/Images/dragon.png";
    QMovie enemymov = QMovie(":/Images/skeleton.gif");

};

#endif // GRAPHICS_H
