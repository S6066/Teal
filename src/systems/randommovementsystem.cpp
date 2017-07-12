﻿// Copyright (C) 2016 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

#include "systems/randommovementsystem.hpp"

RandomMovementSystem::RandomMovementSystem() : m_uni(0, 7)
{
    Requires<PositionComponent, MoveComponent, RandomMovementComponent>();
    SetUpdateRate(10.f); // Can be removed
    SetUpdateOrder(4);
}

RandomMovementSystem::RandomMovementSystem(const std::weak_ptr<MapInstance>& map)
    : RandomMovementSystem()
{
    setMap(map);
}

void RandomMovementSystem::setMap(const std::weak_ptr<MapInstance>& map)
{
    m_map = map;
}

void RandomMovementSystem::OnUpdate(float elapsed)
{
    RandomNumber<std::mt19937> rng;

    for (auto& e : GetEntities())
    {
        auto& rd = e->GetComponent<RandomMovementComponent>();
        auto& mov = e->GetComponent<MoveComponent>();
        auto& pos = e->GetComponent<PositionComponent>();

        rd.currentInterval += elapsed;
        bool goSomewhere { false };

        while (rd.currentInterval > rd.movingInterval)
        {
            rd.currentInterval -= rd.movingInterval;
            goSomewhere = true;
        }

        if (goSomewhere)
        {
            if (m_map.expired())
                for (unsigned counter {}; counter < rd.nbTiles; ++counter)
                {
                    unsigned direction = m_uni(rng);
                    Orientation orient = static_cast<Orientation>(direction);
                    DiffTile xy = OrientToDiff(orient);

                    mov.tile = pos.xy + AbsTile { static_cast<unsigned>(xy.x), static_cast<unsigned>(xy.y) };
                    mov.playerInitiated = false;
                }

            else
            {
                auto map = m_map.lock();
                TealAssert(map->getMap().IsValid(), "Map isn't valid !");

                for (unsigned counter {}, x {}, y {}; counter < rd.nbTiles; ++counter)
                { // TODO: Redo this. Only works with 1 tile
                    x = pos.xy.x;
                    y = pos.xy.y;

                    XYToArray(x, y);

                    auto adjacentTiles = map->getMap()->adjacentTiles(x, y);

                    if (adjacentTiles.size() == 0)
                        break;

                    DiffTile xy;

                    if (adjacentTiles.size() > 1)
                    {
                        unsigned direction = m_uni(rng);

                        if (adjacentTiles.size() != 8)
                            direction %= adjacentTiles.size();

                        Orientation orient = static_cast<Orientation>(direction);
                        xy = OrientToDiff(orient);
                    }

                    else
                        xy = AbsPosToDiff(pos.xy, adjacentTiles.begin()->first);

                    unsigned newXpos = (pos.xy + AbsTile { static_cast<unsigned>(xy.x), static_cast<unsigned>(xy.y) }).x;
                    unsigned newYpos = (pos.xy + AbsTile { static_cast<unsigned>(xy.x), static_cast<unsigned>(xy.y) }).y;

                    XYToArray(newXpos, newYpos);

                    if (map->getMap()->tile(XYToIndex(newXpos, newYpos)).obstacle == 0)
                    {
                        mov.tile = pos.xy + AbsTile { static_cast<unsigned>(xy.x), static_cast<unsigned>(xy.y) };
                        mov.playerInitiated = false;

                        break;
                    }
                }
            }
        }
    }
}
