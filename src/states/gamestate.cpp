// Copyright (C) 2018 Samy Bensaid
// This file is part of the Teal project.
// For conditions of distribution and use, see copyright notice in LICENSE

#include <NDK/Components/NodeComponent.hpp>
#include <Nazara/Platform/EventHandler.hpp>
#include "components/common/inventorycomponent.hpp"
#include "components/common/namecomponent.hpp"
#include "components/common/damagemodifiercomponent.hpp"
#include "components/common/inventorycomponent.hpp"
#include "components/common/mapcomponent.hpp"
#include "components/common/positioncomponent.hpp"
#include "components/common/renderablesstoragecomponent.hpp"
#include "components/common/randommovementcomponent.hpp"
#include "components/common/movecomponent.hpp"
#include "util/cloneutil.hpp"
#include "util/maputil.hpp"
#include "util/gfxutil.hpp"
#include "def/layerdef.hpp"
#include "def/folderdef.hpp"
#include "def/uidef.hpp"
#include "factory.hpp"
#include "systems.hpp"
#include "states/gamestate.hpp"

GameState::GameState(Ndk::WorldHandle world, Nz::RenderWindow& window, const Nz::Vector2ui& mapArea, GameData& gameData)
    : m_world(world), m_window(window), m_mapArea(mapArea), m_tilesetCore(gameData.tilesetCore), m_fightTilesetCore(gameData.fightTilesetCore), m_states(gameData.states)
    , m_skills(gameData.skills), m_ais(gameData.ais), m_animations(gameData.animations), m_items(gameData.items), m_characters(gameData.characters) {}

void GameState::Enter(Ndk::StateMachine& fsm)
{
    m_canvas = std::make_unique<Ndk::Canvas>(m_world->CreateHandle(), m_window.GetEventHandler(), m_window.GetCursorController().CreateHandle());

    addEntities();
    addSystems();

    initializeMapUtility(m_map->GetComponent<MapComponent>().map.get(), m_pather.get(), m_charac);

    initEventHandler();
    addWidgets();
}

void GameState::Leave(Ndk::StateMachine& fsm)
{
    removeSystems();
    uninitializeMapUtility();
    uninitEventHandler();
    killEntities();

    m_canvas.reset();
}

bool GameState::Update(Ndk::StateMachine& fsm, float elapsedTime)
{
    bool paused = !fsm.IsTopState(this);

    if (m_paused != paused)
    {
        m_paused = paused;
        handlePausedStateSwitch();
    }

    return true;
}

void GameState::handlePausedStateSwitch()
{
    m_world->GetSystem<AISystem>().Enable(!m_paused);
    m_world->GetSystem<MovementSystem>().Enable(!m_paused);
    m_world->GetSystem<AnimationSystem>().Enable(!m_paused);
    m_world->GetSystem<RandomMovementSystem>().Enable(!m_paused);
    m_world->GetSystem<FightSystem>().Enable(!m_paused);

    m_window.SetCursor(m_paused ? Nz::SystemCursor_Default : Nz::SystemCursor_Pointer);
}

#include <iostream>
void GameState::showInventory(bool detailled) // [TEST]
{
    auto& inv = m_charac->GetComponent<InventoryComponent>();
    unsigned counter {};

    for (auto& item : inv.items)
    {
        std::cout << "Item #" << counter;

        if (item->HasComponent<NameComponent>())
        {
            auto& name = item->GetComponent<NameComponent>();
            std::cout << " (Name: " << name.name << ')';
        }

        if (detailled)
        {
            if (item->HasComponent<DamageModifierComponent>())
            {
                auto& atk = item->GetComponent<DamageModifierComponent>().attack;
                std::cout << '\n';
                std::cout << "  Attack Modifier:\n";
                std::cout << "    Neutral: " << atk[Element::Neutral] << '\n';
                std::cout << "    Air: " << atk[Element::Air] << '\n';
                std::cout << "    Fire: " << atk[Element::Fire] << '\n';
                std::cout << "    Water: " << atk[Element::Water] << '\n';
                std::cout << "    Earth: " << atk[Element::Earth];

                auto& res = item->GetComponent<DamageModifierComponent>().resistance;
                std::cout << '\n';
                std::cout << "  Resistance Modifier:\n";
                std::cout << "    Neutral: " << res[Element::Neutral] << '\n';
                std::cout << "    Air: " << res[Element::Air] << '\n';
                std::cout << "    Fire: " << res[Element::Fire] << '\n';
                std::cout << "    Water: " << res[Element::Water] << '\n';
                std::cout << "    Earth: " << res[Element::Earth];
            }
        }

        std::cout << '\n';
        ++counter;
    }

    std::cout << std::flush;
}

void GameState::showCharacteristics() // [TEST]
{
    std::cout << "Main character characteristics";
    if (m_charac->HasComponent<DamageModifierComponent>())
    {
        auto& atk = m_charac->GetComponent<DamageModifierComponent>().attack;
        std::cout << '\n';
        std::cout << "  Attack Modifier:\n";
        std::cout << "    Neutral: " << atk[Element::Neutral] << '\n';
        std::cout << "    Air: " << atk[Element::Air] << '\n';
        std::cout << "    Fire: " << atk[Element::Fire] << '\n';
        std::cout << "    Water: " << atk[Element::Water] << '\n';
        std::cout << "    Earth: " << atk[Element::Earth];

        auto& res = m_charac->GetComponent<DamageModifierComponent>().resistance;
        std::cout << '\n';
        std::cout << "  Resistance Modifier:\n";
        std::cout << "    Neutral: " << res[Element::Neutral] << '\n';
        std::cout << "    Air: " << res[Element::Air] << '\n';
        std::cout << "    Fire: " << res[Element::Fire] << '\n';
        std::cout << "    Water: " << res[Element::Water] << '\n';
        std::cout << "    Earth: " << res[Element::Earth];
    }

    std::cout << std::endl;
}


void GameState::addEntities() /// \todo Use lua (map's entities table)
{
    m_map = m_world->CreateEntity();

    auto& mapComp = m_map->AddComponent<MapComponent>();
    mapComp.init(MapDataLibrary::Get("0;0"), Nz::TextureLibrary::Get(":/game/tileset")->GetFilePath(),
                 Nz::TextureLibrary::Get(":/game/fight_tileset")->GetFilePath(), &m_tilesetCore, &m_fightTilesetCore);

    activateMapEntities(MapDataLibrary::Get("0;0"));
    m_pather = std::make_shared<micropather::MicroPather>(mapComp.map.get(), Def::ArrayMapX * Def::ArrayMapY, 8);

    m_charac = cloneCharacter(m_characters, "villager");
    m_charac->GetComponent<PositionComponent>().xy = { 0, 1 };
    m_charac->GetComponent<NameComponent>().name = "You";
    refreshGraphicsPos(m_charac);
}

void GameState::addSystems()
{
    m_world->AddSystem<AISystem>(m_skills, m_ais, Def::FightAIUtilFile, m_pather, m_charac);
    m_world->AddSystem<MovementSystem>(m_charac);
    m_world->AddSystem<FightSystem>();
    m_world->AddSystem<RandomMovementSystem>(m_pather);
    m_world->AddSystem<AnimationSystem>();
}

void GameState::initEventHandler()
{
    auto& eventHandler = m_window.GetEventHandler();

    m_mouseButtonEvent.Connect(eventHandler.OnMouseButtonPressed, [this] (const Nz::EventHandler*, const Nz::WindowEvent::MouseButtonEvent& event)
    { // Lambda to move the player if the user clicked in the map
        if (!m_mapArea.Contains(event.x, event.y) || m_paused)
            return;

        Ndk::EntityList hoveredEntities = mapEntitiesHoveredByCursor({ event.x, event.y });

        if (!hoveredEntities.empty())
        {
            for (auto& entity : hoveredEntities)
            {
                if (isMonsterEntity(entity) && isFightableEntity(entity))
                {
                    std::cout << "todo: implement fight" << std::endl;
                }

                else // todo: if (isNPCEntity(...))
                {
                    auto& move = m_charac->GetComponent<MoveComponent>();
                    AbsTile tile = getTileFromGlobalCoords({ event.x, event.y });

                    move.tile = tile;
                    move.playerInitiated = true;
                }
            }
        }

        else
        {
            auto& move = m_charac->GetComponent<MoveComponent>();
            AbsTile tile = getTileFromGlobalCoords({ event.x, event.y });

            move.tile = tile;
            move.playerInitiated = true;
        }
    });

    m_keyPressEvent.Connect(eventHandler.OnKeyPressed, [this] (const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& event)
    {
        switch (event.code)
        {
            case Nz::Keyboard::I: // Inventory
                showInventory(event.shift);
                break;

            case Nz::Keyboard::C: // Caracteristics
                showCharacteristics();
                break;

            case Nz::Keyboard::D: // Useless debug things
                auto tiles = getVisibleTiles(m_charac->GetComponent<PositionComponent>().xy, 2);

                if (eeee.IsValid())
                    eeee->Kill();

                eeee = m_world->CreateEntity();
                eeee->AddComponent<Ndk::NodeComponent>();
                auto& gfx = eeee->AddComponent<Ndk::GraphicsComponent>();

                for (auto& tilep : tiles)
                {
                    Nz::SpriteRef tile = Nz::Sprite::New(Nz::Material::New("Translucent2D"));
                    tile->SetTexture(Nz::TextureLibrary::Get(":/game/fight_tileset"));
                    tile->SetSize(64, 32);
                    tile->SetTextureRect({ 0, 0, 64, 32 });
                    auto aabb = getTileAABB(tilep);

                    gfx.Attach(tile, Nz::Matrix4f::Translate({ aabb.x, aabb.y, 0 }), 0);
                }

                break;
        }
    });

    m_mouseMovedEvent.Connect(eventHandler.OnMouseMoved, [this] (const Nz::EventHandler*, const Nz::WindowEvent::MouseMoveEvent& event)
    {
        if (m_mapArea.Contains(event.x, event.y) && !m_paused)
            m_window.SetCursor(Nz::SystemCursor_Pointer);

        else
            m_window.SetCursor(Nz::SystemCursor_Default);
    });
}

void GameState::addWidgets()
{
    TealAssert(m_canvas, "Canvas null");
    m_canvas->SetPosition(float(Def::ButtonsMarginX), float(Def::MapSizeY + Def::ButtonsMarginY));
    m_canvas->SetSize({ float(Def::ButtonsSizeX), float(Def::ButtonsSizeY) });

    auto& eventHandler = m_window.GetEventHandler();

    Ndk::ButtonWidget* invButton = m_canvas->Add<Ndk::ButtonWidget>();

    invButton->SetTexture(Nz::TextureLibrary::Get(":/buttons/inv"));
    invButton->SetHoverTexture(invButton->GetTexture());
    invButton->SetPressTexture(Nz::TextureLibrary::Get(":/buttons/inv_pressed"));

    invButton->SetColor(Nz::Color::White, Nz::Color::White);
    invButton->SetHoverColor(invButton->GetColor(), invButton->GetCornerColor());
    invButton->SetPressColor(invButton->GetColor(), invButton->GetCornerColor());

    m_invButtonEvent.Connect(invButton->OnButtonTrigger, [this] (const Ndk::ButtonWidget*)
    {
        if (!m_paused)
            showInventory();
    });
}


void GameState::removeSystems()
{
    m_world->RemoveSystem<AISystem>();
    m_world->RemoveSystem<MovementSystem>();
    m_world->RemoveSystem<FightSystem>();
    m_world->RemoveSystem<RandomMovementSystem>();
    m_world->RemoveSystem<AnimationSystem>();
}

void GameState::killEntities()
{
    m_pather.reset();

    m_map->Kill();
    m_charac->Kill();
}


void GameState::uninitEventHandler()
{
    auto& eventHandler = m_window.GetEventHandler();

    m_mouseButtonEvent.Disconnect();
    m_keyPressEvent.Disconnect();
    m_mouseMovedEvent.Disconnect();
    m_invButtonEvent.Disconnect();
}
