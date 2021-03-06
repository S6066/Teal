// Copyright (C) 2019 Samy Bensaid
// This file is part of the Teal project.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef AISYSTEM_HPP
#define AISYSTEM_HPP

#include <Nazara/Lua/LuaCoroutine.hpp>
#include <Nazara/Lua/LuaInstance.hpp>
#include <Nazara/Core/String.hpp>
#include <NDK/System.hpp>
#include <NDK/EntityList.hpp>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <limits>
#include "micropather.h"
#include "global.hpp"
#include "containers/doublestore.hpp"
#include "containers/aicore.hpp"

namespace AISystemDetail
{

struct FightData
{
    std::unique_ptr<Nz::LuaInstance> lua;
    std::unique_ptr<Nz::LuaCoroutine> coroutine;

    std::function<bool()> forceContinueFight = [] () { return false; };
    std::function<bool()> canResume = [] () { return true; };

    Ndk::EntityHandle currentEntity;
    Ndk::EntityHandle nextEntity;

    std::vector<Ndk::EntityHandle> fighters; // Contains main character
    Ndk::EntityList entities; // Others entities: Traps, unanimated objects, ...
};

} // namespace Detail

///
/// \class AISystem
///
/// \brief Compute a path using the Position and Move component
///        And write it to the Path component.
///
/// \note It uses micropather
///

class AISystem : public Ndk::System<AISystem>
{
public:
    AISystem(const std::shared_ptr<SkillStore> skills, const AICore& ais, const Nz::String& utilFilepath,
             const std::shared_ptr<micropather::MicroPather>& pather, const Ndk::EntityHandle& mainCharacter);
    AISystem(const AISystem& other);
    ~AISystem() = default;

    void resetPather();
    void setPather(const std::shared_ptr<micropather::MicroPather>& pather);

    static Ndk::SystemIndex systemIndex;

private:
    void OnUpdate(float elapsed) override;

    void cleanAndContinueFight();
    void cleanLuaInstance();
    void removeFromFight(const Ndk::EntityHandle& e);

    bool prepareLuaAI(Nz::LuaInstance& lua);
    void bindFunctions(Nz::LuaInstance& lua);

    void Teal_MoveCharacter(unsigned x, unsigned y);
    void Teal_TakeCover();
    void Teal_AttackCharacter(unsigned characterIndex, Nz::String skillCodename);
    void Teal_MoveAndAttackCharacter(unsigned characterIndex, Nz::String skillCodename);
    unsigned Teal_ChooseTarget();
    unsigned Teal_ChooseAttack(unsigned characterIndex);
    unsigned ChooseAttack(unsigned characterIndex, const AbsTile& pos);
    bool Teal_CanCastSpell(unsigned characterIndex);
    bool Teal_CanUseSkill(unsigned characterIndex, unsigned skillIndex);
    std::unordered_map<Element, unsigned> getMaximumDamage(const AbsTile& from, const AbsTile& target, const SkillData& skill);

    Ndk::EntityList getEnemies(const Ndk::EntityHandle& e);
    Ndk::EntityList getAllies(const Ndk::EntityHandle& e);

    std::shared_ptr<micropather::MicroPather> m_pather {};
    Ndk::EntityHandle m_mainCharacter;
    Nz::String m_utilityLuaFile;

    AISystemDetail::FightData m_currentFight;
    bool m_isFightActive {};

    const std::shared_ptr<SkillStore> m_skills;
    const AICore& m_ais;
};

#endif // AISYSTEM_HPP
