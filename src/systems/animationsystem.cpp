﻿// Copyright (C) 2017 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

#include "systems/animationsystem.hpp"

AnimationSystem::AnimationSystem()
{
    Requires<AnimationComponent, Ndk::GraphicsComponent, PositionComponent, OrientationComponent>();
    SetUpdateRate(Def::MAXSYSTEMUPS);
    SetUpdateOrder(5);
}

void AnimationSystem::OnUpdate(float elapsed)
{
    for (auto& e : GetEntities())
    {
        auto& anim = e->GetComponent<AnimationComponent>();
        bool  moving = e->GetComponent<PositionComponent>().moving;
        auto& sprite = e->GetComponent<Ndk::GraphicsComponent>();
        auto  dir = e->GetComponent<OrientationComponent>().dir;

        Nz::Sprite* gfx = getRenderableFromGraphicsComponent<Nz::Sprite>(sprite);

        if (!gfx)
        {
            anim.animated = false;
            continue; // No sprite has been found
        }

        switch (anim.animationState)
        {
        case AnimationComponent::OnMove:
            int const intDir = static_cast<int>(dir);

            unsigned const startX = intDir * anim.size.x; // Get the x and the y
            unsigned const startY = anim.frame * anim.size.y;

            OnMoveAnimation(startX, startY, gfx, anim, moving);
            break;

        case AnimationComponent::OnEmote: // EmoteStore
            break;

        case AnimationComponent::OnFight:
            break;
        }
    }
}

void AnimationSystem::OnMoveAnimation(unsigned startX, unsigned startY, Nz::SpriteRef gfx,
                                      AnimationComponent& anim, bool moving)
{
    if (!moving || anim.maxframe == 0) // Reset animation, and change direction
    {
        anim.frame = 0;
        gfx->SetTextureRect({ startX, 0u, anim.size.x, anim.size.y });
        anim.animated = false;

        return;
    }

    else // Animation !
    {
        gfx->SetTextureRect({ startX, startY, anim.size.x, anim.size.y });

        ++anim.frame;
        anim.animated = true;

        if (anim.frame > anim.maxframe)
            anim.frame = 0;
    }
}
