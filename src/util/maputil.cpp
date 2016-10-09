// Copyright (C) 2016 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

#include "util/maputil.hpp"

namespace
{

MapCore* m_maps {};
std::shared_ptr<MapInstance> m_map {};
micropather::MicroPather* m_pather {};

}

std::pair<bool, Direction::Dir> canChangeMap(const Ndk::EntityHandle& p)
{
    NazaraAssert(isMapUtilityInited(), "Map Utility hasn't been inited !");
    NazaraAssert(hasComponentsToChangeMap(p), "Entity hasn't right components to change map !");

    auto& mapPos = p->GetComponent<MapPositionComponent>();
    auto& pos = p->GetComponent<PositionComponent>();

    // Where is the entity in the map ? Right, left, down, or up ?

    bool validPos { false }; // If the entity is at one extremity
    Direction::Dir entExt { Direction::Up }; // Entity Extremity

    if (pos.x == 0u) // Left
    {
        if (m_maps->get({ mapPos.x - 1, mapPos.y }))
        {
            validPos = true;
            entExt = Direction::Left;
        }
    }

    else if (pos.x == Def::MAPX) // Right
    {
        if (m_maps->get({ mapPos.x + 1, mapPos.y }))
        {
            validPos = true;
            entExt = Direction::Right;
        }
    }

    else if (pos.y == 0u) // Up
    {
        if (m_maps->get({ mapPos.x, mapPos.y - 1 }))
        {
            validPos = true;
            entExt = Direction::Up;
        }
    }

    else if (pos.y == Def::MAPY) // Down
    {
        if (m_maps->get({ mapPos.x, mapPos.y + 1 }))
        {
            validPos = true;
            entExt = Direction::Down;
        }
    }

    if (!validPos)
        return std::make_pair(false, entExt); // Entity isn't even at an extremity

    // Okay, now, let's check if the position where the entity
    // will move to is valid (no obstacle)

    MapData map; // Map the entity will move to
    unsigned x {}, y {}; // New position of the entity after changing map

    switch (entExt)
    {
    case Direction::Left:
        map = *m_maps->get({ mapPos.x - 1, mapPos.y });

        x = Def::MAPX;
        y = pos.y;
        
        break;

    case Direction::Right:
        map = *m_maps->get({ mapPos.x + 1, mapPos.y });

        x = 0u;
        y = pos.y;

        break;

    case Direction::Up:
        map = *m_maps->get({ mapPos.x, mapPos.y - 1 });

        x = pos.x;
        y = Def::MAPY;

        break;

    case Direction::Down:
        map = *m_maps->get({ mapPos.x, mapPos.y + 1 });

        x = pos.x;
        y = 0u;

        break;

    default: // Todo: Make a fake BSoD to scare that kind of bugs
        NazaraAssert(false, "I am an error message. Just look the code to see the reason of me.");

        return std::make_pair(false, entExt);
        break;
    }

    MapInstance::XYToArray(x, y);

    if (map.obs[x + y * Def::MAPX] != 0)
        return std::make_pair(false, entExt); // It's an obstacle.
    
    return std::make_pair(true, entExt);
}

bool changeMap(const Ndk::EntityHandle& p)
{
    auto canChange = canChangeMap(p);

    if (!canChange.first)
        return false;

    auto& mapPos = p->GetComponent<MapPositionComponent>();
    auto& pos = p->GetComponent<PositionComponent>();

    MapData map; // Map the entity will move to
    unsigned x {}, y {}; // New position of the entity after changing map

    switch (canChange.second)
    {
    case Direction::Left:
        map = *m_maps->get({ mapPos.x - 1, mapPos.y });

        x = Def::MAPX;
        y = pos.y;

        break;

    case Direction::Right:
        map = *m_maps->get({ mapPos.x + 1, mapPos.y });

        x = 0u;
        y = pos.y;

        break;

    case Direction::Up:
        map = *m_maps->get({ mapPos.x, mapPos.y - 1 });

        x = pos.x;
        y = Def::MAPY;

        break;

    case Direction::Down:
        map = *m_maps->get({ mapPos.x, mapPos.y + 1 });

        x = pos.x;
        y = 0u;

        break;
    }

    m_map->map = map.map;
    m_map->map = map.obs;

    m_pather->Reset(); // Map changed, need to reset pather's cache

    pos.x = x;
    pos.y = y;

    return true;
}

void initMapUtility(MapCore* maps, const std::shared_ptr<MapInstance>& currentMap,
                    micropather::MicroPather* pather)
{
    m_maps = maps;
    m_map = currentMap;
    m_pather = pather;
}

bool isMapUtilityInited()
{
    return m_maps && m_map && m_pather;
}
