// Copyright (C) 2016 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

InventoryComponent::InventoryComponent(Ndk::WorldHandle& world) : m_world(world)
{
    reset();
}

void InventoryComponent::Group::add(const EntityType& e)
{
    auto it = entities.find(e);

    if (it == entities.end())
        entities.insert(e);
}

void InventoryComponent::Group::remove(const EntityType& e)
{
    auto it = entities.find(e);

    if (it != entities.end())
        entities.erase(it);
}

const InventoryComponent::EntityCache& InventoryComponent::getAll()
{
    return m_groups["all"].entities;
}

const InventoryComponent::Group& InventoryComponent::group(const std::string& name)
{
    return m_groups[name];
}

bool InventoryComponent::isItem(const Ndk::EntityHandle& entity)
{
    NazaraAssert(entity.IsValid(), "Handle isn't valid !");
    NazaraAssert(entity->IsValid(), "Entity isn't valid !");

    return entity->HasComponent<Items::ItemComponent>();
}
