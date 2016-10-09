// Copyright (C) 2016 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

bool hasComponentsToChangeMap(const Ndk::EntityHandle& p)
{
    return p->HasComponent<PositionComponent>()
        && p->HasComponent<MapPositionComponent>();
}
