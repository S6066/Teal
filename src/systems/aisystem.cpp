// Copyright (C) 2016 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

#include "systems/aisystem.hpp"

void AISystem::OnUpdate(float elapsed)
{
    NazaraAssert(m_pather, "Pather is null !");
    NazaraUnused(elapsed);

    for (auto& e : GetEntities())
    {
        auto& path = e->GetComponent<PathComponent>().path;
        auto& pos = e->GetComponent<PositionComponent>();
        auto& move = e->GetComponent<MoveToComponent>();

        if (move.diffX == 0 && move.diffY == 0)
            continue; // This entity doesn't want to move.

        if (pos.moving) // It's already moving !
        {               // Let's try to stop it
            if (isPositionValid({ pos.x, pos.y }))
                pos.moving = false;
            else
                continue; // Invalid position, can't stop it
        }

        // Ok, let's do the path.

        // First, make sure to erase the previous path (if any)
        while (!path.empty())
            path.pop();

        // Now, compute the path with the position and the move component.
        std::vector<void*> voidPath;
        float totalCost {}; // In case of debugging

        int endX { static_cast<int>(pos.x) + move.diffX },
            endY { static_cast<int>(pos.y) + move.diffY };

        m_pather->Solve(MapComponent::XYToNode(pos.x, pos.y),
                        MapComponent::XYToNode(endX, endY),
                        &voidPath, &totalCost); // returns the absolute position, not difference.

                                                // Path done, in void*. Let's add it to the entity's path, in ints
        int oldX {};
        int oldY {};

        for (std::size_t i {}; i < voidPath.size(); ++i)
        {
            if (i == 0) // First tile is actually the position (micropather's fault)
                continue;

            auto node = voidPath[i];

            unsigned absX {}, absY {}; // Absolute position, not difference
            MapComponent::NodeToXY(node, absX, absY);

            int startX { static_cast<int>(pos.x) };
            int startY { static_cast<int>(pos.y) };

            if (i > 1) // If i == 1 we use the initial position
            {          // Else we use the position micropather generated before
                startX = oldX;
                startY = oldY;
            }

            int x { startX - static_cast<int>(absX) },
                y { startY - static_cast<int>(absY) }; // Difference now, but reversed

            x = -x; //Ok
            y = -y;

            Direction::Dir dir = XYToDir({ x, y });
            bool reExec = isDiagonal(dir); // [WORKAROUND 1]

            path.push(std::make_pair(dir, reExec)); // [WORKAROUND 1]

            oldX = static_cast<int>(absX);
            oldY = static_cast<int>(absY);
        }

        // All done. Now, purge the move and the inter-pos if any

        move.diffX = 0;
        move.diffY = 0;

        pos.inX = 0; // Inter-pos
        pos.inY = 0;
    }
}