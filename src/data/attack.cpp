// Copyright (C) 2018 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

#include "data/damagedata.hpp"
#include "data/statedata.hpp"
#include "data/effectdata.hpp"
#include "data/attack.hpp"

Attack::AttackType Attack::stringToAttackType(Nz::String string)
{
    string = string.ToLower();

    if (string == "damage")
        return AttackType::Damage;

    if (string == "state")
        return AttackType::State;

    if (string == "effect")
        return AttackType::Effect;

    throw std::runtime_error { "Invalid attack type" };
}

const char* Attack::attackTypeToString(AttackType attackType)
{
    switch (attackType)
    {
        case Attack::AttackType::Damage:
            return "damage";

        case Attack::AttackType::State:
            return "state";

        case Attack::AttackType::Effect:
            return "effect";
    }

    throw std::runtime_error { "Invalid attack type" };
}


Attack::Target Attack::stringToTarget(Nz::String string)
{
    string = string.ToLower();

    if (string == "allies")
        return Target::Allies;

    if (string == "enemies")
        return Target::Enemies;

    if (string == "both")
        return Target::Both;

    throw std::runtime_error { "Invalid target" };
}

const char* Attack::targetToString(Target target)
{
    switch (target)
    {
        case Target::Allies:
            return "allies";

        case Target::Enemies:
            return "enemies";

        case Target::Both:
            return "both";
    }

    throw std::runtime_error { "Invalid target" };
}

namespace Nz
{

unsigned int LuaImplQueryArg(const LuaState& state, int index, std::shared_ptr<Attack>* attack, TypeTag<std::shared_ptr<Attack>>)
{
    state.CheckType(index, Nz::LuaType_Table);

    const char* attackType = state.CheckField<const char*>("type", index);

    if (attackType == Attack::attackTypeToString(Attack::AttackType::Damage))
        return LuaImplQueryArg(state, index, attack, TypeTag<DamageData>());

    if (attackType == Attack::attackTypeToString(Attack::AttackType::State))
        return LuaImplQueryArg(state, index, attack, TypeTag<StateData>());

    if (attackType == Attack::attackTypeToString(Attack::AttackType::Effect))
        return LuaImplQueryArg(state, index, attack, TypeTag<EffectData>());

    throw std::runtime_error { "Invalid attack type" };
}

int LuaImplReplyVal(const LuaState& state, std::shared_ptr<Attack>&& attack, TypeTag<std::shared_ptr<Attack>>)
{
    switch (attack->getAttackType())
    {
        case Attack::AttackType::Damage:
            return LuaImplReplyVal(state, std::move(attack), TypeTag<DamageData>());

        case Attack::AttackType::State:
            return LuaImplReplyVal(state, std::move(attack), TypeTag<StateData>());

        case Attack::AttackType::Effect:
            return LuaImplReplyVal(state, std::move(attack), TypeTag<EffectData>());
    }

    throw std::runtime_error { "Invalid attack type" };
}

} // namespace Nz