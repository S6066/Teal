// Copyright (C) 2019 Samy Bensaid
// This file is part of the Teal project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "containers/doublestore.hpp"
#include "data/states.hpp"
#include "components/characters/lifecomponent.hpp"
#include "components/characters/fightcomponent.hpp"
#include "components/shared/positioncomponent.hpp"
#include "components/characters/animationcomponent.hpp"
#include "components/shared/blocktilecomponent.hpp"
#include "components/characters/equipmentcomponent.hpp"
#include "components/shared/damagemodifiercomponent.hpp"
#include "def/gamedef.hpp"
#include "def/systemdef.hpp"
#include "systems/fightsystem.hpp"

FightSystem::FightSystem()
{
    Requires<FightComponent, LifeComponent, PositionComponent>();
    SetMaximumUpdateRate(Def::MaxSystemUPS);
    SetUpdateOrder(Def::FightSystemUpdateOrder); // blocktile, animation
}

void FightSystem::OnUpdate(float)
{

}
