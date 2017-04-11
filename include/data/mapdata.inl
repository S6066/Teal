#include "mapdata.hpp"
// Copyright (C) 2016 Samy Bensaid
// This file is part of the TealDemo project.
// For conditions of distribution and use, see copyright notice in LICENSE

template<class... Args>
MapDataRef MapData::New(Args&&... args)
{
    std::unique_ptr<MapData> object(new MapData(std::forward<Args>(args)...));
    object->SetPersistent(false);

    return object.release();
}

const TILEARRAY& MapData::tiles() const
{
    return m_tiles;
}

const STRINGTILEARRAY& MapData::map() const
{
    return m_map;
}

const UNSIGNEDTILEARRAY& MapData::obs() const
{
    return m_obs;
}

const TileData& MapData::tile(unsigned x, unsigned y) const
{
    return m_tiles[XYToIndex(x, y)];
}

const TileData& MapData::tile(unsigned index) const
{
    return m_tiles[index];
}

std::unordered_map<Nz::Vector2ui, TileData> MapData::adjacentTiles(unsigned x, unsigned y)
{
    std::unordered_map<Nz::Vector2ui, TileData> data;

    for (std::size_t i {}; i < Def::MAP_DISTANCE_X.size(); ++i)
    {
        unsigned newX = x + Def::MAP_DISTANCE_X[i];
        unsigned newY = y + Def::MAP_DISTANCE_Y[i];

        unsigned index = XYToIndex(newX, newY);

        if (index < 0 || index > 119)
            continue;

        data[{ newX, newY }] = m_tiles[index];
    }

    return data;
}

void MapData::setTiles(const TILEARRAY& tiles)
{
    m_tiles = tiles;
    updateOldTileArray();
}

void MapData::setMap(const STRINGTILEARRAY& map)
{
    m_map = map;
    updateTileArray();
}

void MapData::setObs(const UNSIGNEDTILEARRAY& obs)
{
    m_obs = obs;
    updateTileArray();
}


void MapData::addEntity(const Ndk::EntityHandle& e)
{
    m_entities.Insert(e);
}

const Ndk::EntityList& MapData::getEntities() const
{
    return m_entities;
}

Ndk::EntityList& MapData::getEntities()
{
    return m_entities;
}

unsigned MapData::XYToIndex(unsigned x, unsigned y) const
{
    return x + y * Def::MAPX;
}
