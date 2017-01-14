// Copyright (C) 2017 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef DESCRIPTIONCOMPONENT_HPP
#define DESCRIPTIONCOMPONENT_HPP

#include <NDK/Component.hpp>
#include <Nazara/Core/String.hpp>

struct DescriptionComponent : public Ndk::Component<DescriptionComponent>
{
    DescriptionComponent(const Nz::String& desc = "") : description(desc) {}

    Nz::String description {};

    static Ndk::ComponentIndex componentIndex;
};

#endif // DESCRIPTIONCOMPONENT_HPP
