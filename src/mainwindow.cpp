// Copyright (C) 2016 Samy Bensaid
// This file is part of the Teal game.
// For conditions of distribution and use, see copyright notice in LICENSE

#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget* parent, const QSize& scenesize, const QSize& winsize)
    : QFrame(parent), m_world()
{
    setFixedSize(winsize);

    m_scene = new GraphicsScene(m_world, nullptr, this);
    m_buttonBar = new Buttons(0, Def::MAPYSIZE+1, {winsize.width(), Def::BUTTONSYSIZE}, this);

    m_scene->setSceneRect(0, 0, scenesize.width()-2, scenesize.height()-2);

    m_view = new QGraphicsView(m_scene, this);
    m_view->setFixedSize(scenesize);
    m_view->setGeometry(0, 0, scenesize.width(), scenesize.height());

    QPointer<MainWindow> wThis = this;
    connect(m_buttonBar->button1(), &QPushButton::clicked, [wThis]()
    {
        Q_ASSERT(wThis);

        wThis->m_scene->getPerso().getComponent<Components::GraphicsItem>().show();
    });
    connect(m_buttonBar->button2(), &QPushButton::clicked, [wThis]()
    {
        Q_ASSERT(wThis);

        wThis->m_scene->getPerso().getComponent<Components::GraphicsItem>().hide();
    });
    connect(m_buttonBar->button3(), &QPushButton::clicked, [wThis]()
    {
        Q_ASSERT(wThis);

        //auto& perso = wThis->m_scene->getPerso();
        //auto& inv = perso.getComponent<Components::Inventory>();

        //qDebug() << "Inv:" << inv.getAll();
    });
}