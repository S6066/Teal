// Copyright (C) 2017 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

#include "util/aiutil.hpp"

std::queue<std::pair<DirectionFlags, bool>> computePath(const Ndk::EntityHandle& e, micropather::MicroPather* pather)
{
    auto& path = e->GetComponent<PathComponent>().path;
    auto& pos = e->GetComponent<PositionComponent>();
    auto& move = e->GetComponent<MoveComponent>();

    if (move.diffX == 0 && move.diffY == 0)
        return {}; // This entity doesn't want to move.

    if (pos.moving && !isPositionValid({ pos.x, pos.y }) && pos.inX == 0 && pos.inY == 0)
        return {}; // Invalid position, can't stop it

    // Ok, let's do the path.
    NazaraAssert(pather, "Pather is null, cannot compute path !");

    unsigned endX { itou(utoi(pos.x) + move.diffX) },
             endY { itou(utoi(pos.y) + move.diffY) };

    auto newPath = computePath({ pos.x, pos.y }, { endX, endY }, pather);

    if (newPath.empty())
    {
        move.diffX = 0;
        move.diffY = 0;

        return {};
    }

    return newPath;
}

std::queue<std::pair<DirectionFlags, bool>> computePath(const AbsTile& startPos, const AbsTile& lastPos,
                                                        micropather::MicroPather* pather)
{
    if (startPos == lastPos || !isPositionValid({ startPos.x, startPos.y }) || !isPositionValid({ lastPos.x, lastPos.y }))
        return {};

    NazaraAssert(pather, "Pather is null, cannot compute path !");

    // Compute the path with the position and the move component.
    std::vector<void*> voidPath;
    float totalCost {}; // In case of debugging

    int result = pather->Solve(XYToNode(startPos.x, startPos.y),
                               XYToNode(lastPos.x, lastPos.y),
                               &voidPath, &totalCost); // returns the absolute position, not difference.

    if (result != 0 || voidPath.empty())
        return {};

    // Path done, in void*. Let's add it to the entity's path, in integers
    int oldX {};
    int oldY {};

    std::queue<std::pair<DirectionFlags, bool>> newPath;

    for (std::size_t i {}; i < voidPath.size(); ++i)
    {
        if (i == 0) // First tile is actually the position (micropather's fault)
            continue;

        auto node = voidPath[i];

        unsigned absX {}, absY {}; // Absolute position, not difference
        NodeToXY(node, absX, absY);

        int startX { utoi(startPos.x) };
        int startY { utoi(startPos.y) };

        if (i > 1) // If i == 1 we use the initial position
        {          // Else we use the position micropather generated before
            startX = oldX;
            startY = oldY;
        }

        int diffX { startX - utoi(absX) },
            diffY { startY - utoi(absY) }; // Difference now, but reversed

        diffX = -diffX; // Ok
        diffY = -diffY;

        auto dir = XYToDir({ diffX, diffY });
        bool reExec = !isDiagonal(dir);

        newPath.push(std::make_pair(dir, reExec));

        oldX = utoi(absX);
        oldY = utoi(absY);
    }

    return newPath;
}
