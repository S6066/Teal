// Copyright (C) 2016 Samy Bensaid
// This file is part of the Teal game.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef COMMON_HPP
#define COMMON_HPP

#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QGraphicsSimpleTextItem>
#include <QAbstractGraphicsShapeItem>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QGraphicsObject>
#include <QGraphicsItem>

#include <QGraphicsScene>
#include <anax/World.hpp>
#include <anax/Component.hpp>
#include <anax/Entity.hpp>

#include <array>
#include <vector>
#include <tuple>
#include <utility>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <micropather.h>
#include "global.hpp"
#include "util.hpp"

namespace Items
{

struct Item;
struct Equippable;
struct Edible;
struct Resource;

struct AttackBonus;
struct AttackResistance;

}

namespace Components
{

struct Name : public anax::Component
{
    std::string name{};
};

struct Level : public anax::Component
{
    unsigned level{};
};

///
/// \brief The Inventory class
///
/// Contains groups of items
/// Items are entities
///
/// \note This class doesn't manage the entities
///       It only store the IDs
///

class Inventory : public anax::Component
{
    using int_type = anax::Entity::Id::int_type;

    ///
    /// \brief The EntityCache typedef
    ///
    /// It is an unordered_map which contains the id of the entity
    /// and a bool to determine if the entity is activated
    ///
    /// \example If the entity is activated, the bool is equal to true
    ///          If the entity is deactivated, the bool is equal to false
    ///          If the entity is killed, it is erased from the cache
    ///

    using EntityCache = std::unordered_map<int_type, bool>;

public:
    class Group
    {
    public:
        Group() = default;
        Group(const std::string& name) : m_name{name} {}
        ~Group() = default;

        const std::string& name() const
        {
            return m_name;
        }
        const EntityCache& entities() const
        {
            return m_entities;
        }

        void add(int_type id, bool activated = true)
        {
            m_entities[id] = activated;
        }
        void remove(int_type id)
        {
            auto it = m_entities.find(id);

            if (it != m_entities.end())
                m_entities.erase(it);
        }

        void deactivate(int_type id)
        {
            setValue(id, false);
        }
        void activate(int_type id)
        {
            setValue(id, true);
        }

        EntityCache::iterator find(int_type id, bool activated)
        {
            auto it = m_entities.find(id);

            if (it == m_entities.end())
                return it;

            if (it->second == activated)
                return it;

            return m_entities.end();
        }

    private:
        std::string m_name{"undefined"}; // ID of the group
        EntityCache m_entities; // entities of the group

        void setValue(int_type id, bool value)
        {
            auto it = m_entities.find(id);

            if (it != m_entities.end())
                it->second = value;
        }
    };

    Inventory(anax::World& world) : m_world(world), m_groups()
    {
        reset();
    }
    ~Inventory() = default;

    void add(const anax::Entity::Id& id);
    void remove(const anax::Entity::Id& id);
    bool has(const anax::Entity::Id& id);

    void deactivate(const anax::Entity::Id& id);
    void activate(const anax::Entity::Id& id);

    const EntityCache& getAll();
    const Group& group(const std::string& name)
    {
        return m_groups[name];
    }

private:
    anax::World& m_world;
    std::unordered_map<std::string, Group> m_groups;

    ///
    /// \brief reset
    ///
    /// Clears the inventory
    /// Also used to init the inventory with empty groups
    ///

    void reset();

    void assertItem(const anax::Entity& entity) const
    {
        assert(entity.isValid() && "Entity isn't valid !");
        assert(entity.hasComponent<Items::Item>() && "Entity isn't an item !");
    }
};

struct CDirection : public anax::Component
{
    CDirection(const Orientation& o = Orientation::Down) : dir{o} {}
    CDirection(const CDirection&) = default;
    Orientation dir{};
};

struct Animation : public anax::Component
{
    Animation(Orientation& d, const QSize& s, const QPixmap& tex, unsigned mf = 0)
        : dir{d}, frame{0}, size{s}, maxframe{mf}, texture{tex} {}

    Orientation& dir; // dir * size of img = img (x)
    unsigned frame{0}; // frame * size of img = img (y)
    QSize size{};
    unsigned maxframe{}; // 0 if no animation (only direction change)
    QPixmap texture{};
    bool animated{false};
};

struct Position : public anax::Component
{
    Position(unsigned x_ = 0, unsigned y_ = 0) : x{x_}, y{y_} {}

    unsigned x{}; //The actual position
    unsigned y{}; //(absolute)

    //num:
    //X: +32px == +1
    //Y: +16px == +1

    int inX{}; //Position in tile from x/y.
    int inY{}; //difference.

    bool moving{false};
};

struct MoveTo : public anax::Component
{
    int diffX{}; //The position we want to go
    int diffY{}; //Adds it to the Position's XY to get the tile
    //0 == nowhere
};

struct Path : public anax::Component
{
    std::queue<std::pair<Direction::Dir, bool>> path;
    //To add: path.push(...);
    //To read: path.front(); && path.pop();
};

struct Fight : public anax::Component
{
    bool fight{false}; //isFighting would be a better name, but too long for me
};

class Life : public anax::Component
{
public:
    Life(unsigned maxhp) : m_hp{maxhp}, m_maxhp{maxhp} {}
    ~Life() = default;

    bool alive() const noexcept
    {
        return m_alive;
    }
    unsigned HP() const noexcept
    {
        return m_hp;
    }
    void setHP(unsigned hp)
    {
        m_hp = hp;
        verifyInfos();
    }

    unsigned maxHP() const noexcept
    {
        return m_maxhp;
    }
    void setMaxHP(unsigned newHP)
    {
        m_maxhp = newHP;
        verifyInfos();
    }

private:
    unsigned m_hp{100};
    unsigned m_maxhp{100};
    bool m_alive{true};

    void verifyInfos()
    {
        m_hp = (m_hp > m_maxhp) ? m_maxhp : m_hp;
        m_alive = (m_hp == 0);
    }
};

///
/// \brief The Map class
///
/// Map of the game.
/// Must be rendered first.
/// Only one instance of it may exist.
///

class Map : public anax::Component, public micropather::Graph
{
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

public:
    Map() = default;
    Map(const TILEARRAY& _map,
        const TILEARRAY& _obs)
        : map(_map), obs(_obs) {}

    Map(Map&&) = default;
    Map& operator=(Map&&) = default;

    ~Map() = default;

    TILEARRAY map;
    TILEARRAY obs;
    //0 = can pass, 1 = can't pass but can view through (in fight), 2 = can't pass and can't view through


    //Utility
    static void NodeToXY(void* node, unsigned& x, unsigned& y)
    {
        int index = (int) node;
        y = index / Def::MAPX;
        x = index - y * Def::MAPX;
    }
    static inline void* XYToNode(const unsigned& x, const unsigned& y)
    {
        return (void*) ( y*Def::MAPX + x );
    }
    static inline void XYToArray(const unsigned& /*x*/, unsigned& y)
    {
        y /= 2;
    }

private:
    bool passable(unsigned sX, unsigned sY, unsigned eX, unsigned eY);

    //Micropather
    virtual float LeastCostEstimate( void* nodeStart, void* nodeEnd ) override;
    virtual void AdjacentCost( void* node, std::vector< micropather::StateCost > *neighbors ) override;
    virtual void PrintStateInfo( void* /*node*/ ) override {}
};

///
/// \brief The GraphicsItem class
///
/// Base class for all Qt graphics Item with anax.
///

class GraphicsItem : public anax::Component
{
    GraphicsItem() = delete;
    GraphicsItem(GraphicsItem&&) = delete;
    GraphicsItem(const GraphicsItem&) = delete;

    GraphicsItem& operator=(const GraphicsItem&) = delete;
    GraphicsItem& operator=(GraphicsItem&&) = delete;

public:
    GraphicsItem(QGraphicsItem* p_item, const Vector2i& defPos)
        : m_item(p_item), m_defPos{defPos} { item()->setZValue(1); }

    ~GraphicsItem() {}
    const Vector2i& defaultPos() const
    {
        return m_defPos;
    }
    void gotoDefaultPos() //should go to 0,0 logic position
    {
        m_item->setPos(m_defPos.first(), m_defPos.second());
    }
    bool operator==(const GraphicsItem& other) const
    {
        return (m_item == other.m_item)
            && (m_visible == other.m_visible);
    }

    inline void hide()
    {
        m_visible = false;
    }
    inline void show()
    {
        m_visible = true;
    }
    inline bool isVisible()
    {
        return m_visible;
    }
    inline QGraphicsItem* item() const
    {
        return m_item.get();
    }

    QGraphicsPixmapItem* pixmap() const
    {
        return dynamic_cast<QGraphicsPixmapItem*>(item());
    }
    QGraphicsLineItem* line() const
    {
        return dynamic_cast<QGraphicsLineItem*>(item());
    }

    QAbstractGraphicsShapeItem* shape() const
    {
        return dynamic_cast<QAbstractGraphicsShapeItem*>(item());
    }
    QGraphicsRectItem* rect() const
    {
        return dynamic_cast<QGraphicsRectItem*>(shape());
    }
    QGraphicsPolygonItem* polygon() const
    {
        return dynamic_cast<QGraphicsPolygonItem*>(shape());
    }

    QGraphicsObject* object() const
    {
        return dynamic_cast<QGraphicsObject*>(item());
    }
    QGraphicsTextItem* text() const
    {
        return dynamic_cast<QGraphicsTextItem*>(object());
    }
    QGraphicsSimpleTextItem* sText() const
    {
        return dynamic_cast<QGraphicsSimpleTextItem*>(shape());
    }
    QGraphicsWidget* widget() const
    {
        return dynamic_cast<QGraphicsWidget*>(object());
    }
    QGraphicsProxyWidget* proxy() const
    {
        return dynamic_cast<QGraphicsProxyWidget*>(widget());
    }

private:
    non_nullptr<QGraphicsItem> m_item;
    bool m_visible{false};
    Vector2i m_defPos{};
};

}

#endif // COMMON_HPP