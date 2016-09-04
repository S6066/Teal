// Copyright (C) 2016 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef CDIRECTIONCOMPONENT_HPP
#define CDIRECTIONCOMPONENT_HPP

#include <NDK/Component.hpp>
#include "global.hpp"

///
/// \struct CDirectionComponent
///
/// \brief Contains the Orientation enum
///
/// \todo Change name to Orientation ?
///

struct CDirectionComponent : public Ndk::Component<CDirectionComponent>
{
    CDirectionComponent(const Orientation& o = Orientation::Down) : dir { o } {}
    CDirectionComponent(const CDirectionComponent&) = default;
    Orientation dir;

    static Ndk::ComponentIndex componentIndex;
};

#endif // CDIRECTIONCOMPONENT_HPP