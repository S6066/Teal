// Copyright (C) 2016 Samy Bensaid
// This file is part of the Teal game.
// For conditions of distribution and use, see copyright notice in LICENSE

#include "graphicsscene.hpp"

GraphicsScene::GraphicsScene(anax::World& world, QLabel* outputFps, QObject* parent) : QGraphicsScene(parent),
    m_world(world), m_map(), m_charac(), m_chrono(), m_outputFps{outputFps}, m_maprenderSys(*this),
    m_renderSys(*this), m_inputSys(), m_AISystem(), m_pather(), m_moveSys(), m_posRefresh(), m_animSys()
{
    addSystems();
    addEntities();
    initEntities();
    initSystems();

    connect(&m_looper, &QTimer::timeout, this, &GraphicsScene::loop);
    m_looper.start(0);
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    //Clicked in the map
    //Let's add this to the event queue

    auto point = e->scenePos();
    Event ev { Events::MouseClick {e->button(), dToU(point.x()), dToU(point.y())} };

    m_eventQueue.add(ev);
}

void GraphicsScene::setFpsOutput(QLabel* output)
{
    m_outputFps = output;
}

void GraphicsScene::addSystems()
{
    m_world.addSystem(m_maprenderSys);
    
    m_world.addSystem(m_moveSys);
    m_world.addSystem(m_posRefresh);
    m_world.addSystem(m_animSys);

    m_world.addSystem(m_renderSys);

    m_world.addSystem(m_AISystem);
    m_world.refresh();
}

void GraphicsScene::addEntities()
{
    m_map = m_world.createEntity();
    m_map.addComponent<Components::Map>();

    CharacterInfos mainCharacInfos { {113, 99}, QPixmap{":/game/char/villager"},
                           15, {-25, -66}, {1, 1}, 100, Orientation::Down };
    m_charac = make_character(m_world, mainCharacInfos);
}

void GraphicsScene::initEntities()
{
    m_map.getComponent<Components::Map>().map =
    {
            2, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
            1, 1, 1, 2, 1, 2, 2, 2, 3, 2, 3, 3, 3, 3, 3,
            1, 1, 1, 1, 2, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
            1, 1, 1, 2, 1, 1, 2, 2, 1, 2, 2, 2, 2, 2, 3,
            0, 0, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 2, 2,
            0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 1, 1, 2, 2, 2,
            0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2
    }; //Test
    m_map.activate();

    auto& gfx = m_charac.getComponent<Components::GraphicsItem>();
    gfx.gotoDefaultPos();
    gfx.show();

    m_charac.activate();
    m_world.refresh();
}

void GraphicsScene::initSystems()
{
    m_maprenderSys.update(m_map.getComponent<Components::Map>());

    m_pather = std::make_shared<micropather::MicroPather>(&m_map.getComponent<Components::Map>());
    m_AISystem.setPather(m_pather);

    m_inputSys.setOnClickMove(&m_charac.getComponent<Components::MoveTo>());
    m_inputSys.setOnClickPos(&m_charac.getComponent<Components::Position>());
}