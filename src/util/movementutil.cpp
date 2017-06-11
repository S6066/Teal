﻿// Copyright (C) 2017 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

#include "util/movementutil.hpp"

void moveEntity(const Ndk::EntityHandle& e)
{
    auto& pathComp = e->GetComponent<PathComponent>();
    auto& path = pathComp.path;
    auto& pos = e->GetComponent<PositionComponent>();

    if (path.empty())
        return; // No path, no move.

    auto& orient = e->GetComponent<OrientationComponent>().dir;
    auto& dir = path.front();

    orient = DirToOrient(dir.first);
    auto xy = DirToXY(dir.first);

    int moveX { xy.x };
    int moveY { xy.y };

    if (pathComp.totalSize == 1) // Walk mode if path is short
    {
        moveX = (moveX == 2 || moveX == -2) ? moveX / 2 : moveX;
        moveY = (moveY == 2 || moveY == -2) ? moveY / 2 : moveY;
    }

    if (!pos.moving)
    {
        pos.moving = true;
        return; // Return so the animation system can animate.
    }

    pos.inX += moveX;
    pos.inY += moveY;
    
    bool resetPosInX { true };
    
    if (pos.inX > 0 && pos.inX >= Def::MAXPOSINTILE)
        pos.x += (pos.inX / Def::MAXPOSINTILE);

    else if (-pos.inX >= Def::MAXPOSINTILE)
        pos.x -= (-pos.inX / Def::MAXPOSINTILE);

    else
        resetPosInX = false;

    pos.inX = resetPosInX ? 0 : pos.inX;


    bool resetPosInY { true };

    if (pos.inY > 0 && pos.inY >= Def::MAXPOSINTILE)
        pos.y += (pos.inY / Def::MAXPOSINTILE);

    else if (-pos.inY >= Def::MAXPOSINTILE)
        pos.y -= (-pos.inY / Def::MAXPOSINTILE);

    else
        resetPosInY = false;

    pos.inY = resetPosInY ? 0 : pos.inY;

    if (pos.inX == 0 && pos.inY == 0) // Next tile reached
    {
        if (!dir.second)
        {
            path.erase(path.begin()); // To get next tile

            if (!path.empty() && e->HasComponent<MapPositionComponent>() && e->HasComponent<MoveComponent>())
            {
                auto& mapPos = e->GetComponent<MapPositionComponent>();
                MapDataRef currentMap = MapDataLibrary::Get(mapXYToString(mapPos.x, mapPos.y));

                for (unsigned i {}, x {}, y {}, posX { pos.x }, posY { pos.y }; i < path.size(); ++i)
                {
                    auto moveXY = DirToXY(path[i].first);

                    x = posX + moveXY.x;
                    y = posY + moveXY.y;

                    XYToArray(x, y);

                    posX += moveXY.x;
                    posY += moveXY.y;

                    auto& tile = currentMap->tile(x, y);

                    if (tile.obstacle != 0 || tile.occupied)
                    {
                        auto& move = e->GetComponent<MoveComponent>();
                        auto dirs = directionsToPositions(path, { pos.x, pos.y });

                        move.tile = dirs.back();
                        move.playerInitiated = false;
                    }
                }
            }
        }
        else
            dir.second = false;
    }

    if (path.empty()) // Finished path
    {
        pos.moving = false; // Not moving anymore
        pathComp.totalSize = 0u;

        if (e == getMainCharacter() && hasComponentsToChangeMap(e))
            changeMap();

        if (e->HasComponent<BlockTileComponent>() && e->GetComponent<BlockTileComponent>().blockTile)
        {
            refreshOccupiedTiles();
            clearPatherCache();
        }
    }

    if (isMapEntity(e))
        refreshGraphicsPos(e);
}
