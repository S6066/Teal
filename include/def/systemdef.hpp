// Copyright (C) 2017 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef SYSTEMDEF_HPP
#define SYSTEMDEF_HPP

namespace Def
{

constexpr int AISystemUpdateOrder { 1 };
constexpr int MovementSystemUpdateOrder { AISystemUpdateOrder + 1 };
constexpr int FightSystemUpdateOrder { MovementSystemUpdateOrder + 1 };
constexpr int RandomMovementSystemUpdateOrder { FightSystemUpdateOrder + 1 };
constexpr int AnimationSystemUpdateOrder { RandomMovementSystemUpdateOrder + 1 };

}

#endif // SYSTEMDEF_HPP
