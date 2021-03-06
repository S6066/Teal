// Copyright (C) 2019 Samy Bensaid
// This file is part of the Teal project.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef LUAPARSER_HPP
#define LUAPARSER_HPP

#include <NDK/LuaAPI.hpp>
#include <Nazara/Lua/LuaState.hpp>
#include <Nazara/Core/String.hpp>
#include "def/typedef.hpp"

inline LuaArguments parseLua(const Nz::LuaState& lua); // Uses recursion. To understand recursion, read the next comment
extern void parseLua(const Nz::LuaState& lua, LuaArguments& table);    // To understand recursion, read the previous comment

inline bool hasField(const Nz::LuaState& lua, const Nz::String& fieldName, int tableIndex = -1);

#include "luaparser.inl"

#endif // LUAPARSER_HPP
