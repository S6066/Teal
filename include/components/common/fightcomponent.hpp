﻿// Copyright (C) 2016 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef FIGHTCOMPONENT_HPP
#define FIGHTCOMPONENT_HPP

#include <NDK/Component.hpp>
#include "cache/doublestore.hpp"
#include "global.hpp"
#include "components/common/pathcomponent.hpp"
#include "data/states.hpp"

struct FightComponent : public Ndk::Component<FightComponent>
{
    bool isFighting { false };
    bool myTurn { false }; // Waiting to kill you
    bool automaticallyAttack { false }; // In the map

    unsigned movementPoints { 3 };
    unsigned actionPoints { 6 };

    SkillStore::LightId attack {};
    AbsTile wantedPos { toVector(Def::NOMOVEPOS) };

    PathComponent::PathPool path;
    unsigned totalSize {};

    std::vector<std::shared_ptr<State>> states;

    static Ndk::ComponentIndex componentIndex;
};

#endif // FIGHTCOMPONENT_HPP
