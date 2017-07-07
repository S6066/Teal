﻿// Copyright (C) 2016 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef UTIL_HPP
#define UTIL_HPP

#include "global.hpp"

template<class T>
inline T distance(const T& x, const T& y);

inline DiffTile AbsPosToDiff(const AbsTile& from, const AbsTile& to);

inline int utoi(unsigned u);
inline int lltoi(long long ll);

inline unsigned itou(int i);

#include "util.inl"

#endif // UTIL_HPP
