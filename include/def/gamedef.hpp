﻿// Copyright (C) 2016 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef GAMEDEF_HPP
#define GAMEDEF_HPP

#include <array>
#include <utility>
#include <limits>

namespace Def
{

constexpr unsigned MAPX           { 7u };
constexpr unsigned MAPY           { 14u };

constexpr unsigned ARRAYMAPX      { MAPX + 1u };
constexpr unsigned ARRAYMAPY      { MAPY + 1u };


constexpr unsigned TILEARRAYSIZE  { ARRAYMAPX * ARRAYMAPY };
extern     float   TILESETSIZE;      // Must only be modified in game.cpp:initTilesetCore()
extern     float   FIGHTTILESETSIZE; // ^

constexpr unsigned TILEXSIZE      { 64u }; // For logic
constexpr unsigned TILEYSIZE      { 32u };


constexpr   int    MAXPOSINTILE   { 4 }; // Must be dividable by TILEXSIZE & TILEYSIZE
constexpr unsigned MAXGXPOSINTILE { (TILEXSIZE / 2) / MAXPOSINTILE };
constexpr unsigned MAXGYPOSINTILE { (TILEYSIZE / 2) / MAXPOSINTILE };

constexpr  float   MAXSYSTEMUPS   { 10 }; // Updates per second
constexpr   int    MAXDIR         { 4  }; // Max directions: Up, down, left, right
constexpr std::pair<unsigned, unsigned> NOMOVEPOS { std::numeric_limits<unsigned>::max(), std::numeric_limits<unsigned>::max() }; // with MoveComponent


constexpr std::array<int,   8> MAP_DISTANCE_EVEN_X   { 0,    0,   1,   0,   0,   -1,  -1,  -1   }; // See Orientation
constexpr std::array<int,   8> MAP_DISTANCE_EVEN_Y   { 2,    1,   0,  -1,  -2,   -1,   0,   1   }; // COORDFIX_REDO check ?

constexpr std::array<int,   8> MAP_DISTANCE_UNEVEN_X { 0,    1,   1,   1,   0,    0,  -1,   0   };
constexpr std::array<int,   8> MAP_DISTANCE_UNEVEN_Y { 2,    1,   0,  -1,  -2,   -1,   0,   1   };

constexpr std::array<float, 8> MAP_DISTANCE_COST     { 1.5f, 1.f, 2.f, 1.f, 1.5f, 1.f, 2.f, 1.f };

}

#endif // GAMEDEF_HPP
