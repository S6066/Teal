// Copyright (C) 2019 Samy Bensaid
// This file is part of the Teal project.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef DAMAGEMODIFIERCOMPONENT_HPP
#define DAMAGEMODIFIERCOMPONENT_HPP

#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Core/String.hpp>
#include <NDK/Component.hpp>
#include <unordered_map>
#include "data/elementdata.hpp"

struct DamageModifierData
{
    int attack; // In percentage
    int resistance;
};

using DamageModifierList = std::unordered_map<Element, DamageModifierData>;

struct DamageModifierComponent;
using DamageModifierComponentHandle = Nz::ObjectHandle<DamageModifierComponent>;

struct DamageModifierComponent : public Ndk::Component<DamageModifierComponent>
{
    DamageModifierList data;

    static inline const char* componentName() { return "damage_modifier"; }

    static Ndk::ComponentIndex componentIndex;
};

#include <Nazara/Lua/LuaState.hpp>

namespace Nz
{

inline unsigned int LuaImplQueryArg(const LuaState& state, int index, DamageModifierComponent* component, TypeTag<DamageModifierComponent>)
{
    state.CheckType(index, Nz::LuaType_Table);

    if (state.GetField("attack", index) == Nz::LuaType_Table)
        for (Element e {}; e <= Element::Max; ++e)
            component->data[e].attack = state.CheckField<int>(elementToString(e), 0, -1);

    state.Pop();

    if (state.GetField("resistance", index) == Nz::LuaType_Table)
        for (Element e {}; e <= Element::Max; ++e)
            component->data[e].resistance = state.CheckField<int>(elementToString(e), 0, -1);

    state.Pop();

    return 1;
}

inline int LuaImplReplyVal(const LuaState& state, DamageModifierComponentHandle&& component, TypeTag<DamageModifierComponentHandle>)
{
    state.PushTable();
    {
        state.PushTable();
        {
            for (Element e {}; e <= Element::Max; ++e)
                state.PushField(elementToString(e), component->data[e].attack);
        }

        state.SetField("attack");


        state.PushTable();
        {
            for (Element e {}; e <= Element::Max; ++e)
                state.PushField(elementToString(e), component->data[e].resistance);
        }

        state.SetField("resistance");
    }

    return 1;
}

} // namespace Nz

#endif // DAMAGEMODIFIERCOMPONENT_HPP
