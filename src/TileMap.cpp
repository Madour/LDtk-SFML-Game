// Created by Modar Nasser on 13/06/2021.

#include "TileMap.hpp"

auto TileMap::Textures::instance() -> Textures& {
    static Textures instance;
    return instance;
}

auto TileMap::Textures::get(const std::string& name) -> sf::Texture& {
    auto& data = instance().data;
    if (data.count(name) == 0)
        data[name].loadFromFile(TileMap::path+name);
    return instance().data.at(name);
}

TileMap::Layer::Layer(const TileMap& tilemap, const ldtk::Layer& layer) : m_map(tilemap) {
    m_texture = &Textures::get(layer.getTileset().path);
    m_vertex_array.resize(layer.allTiles().size()*4);
    m_vertex_array.setPrimitiveType(sf::PrimitiveType::Quads);
    int i = 0;
    for (const auto& tile : layer.allTiles()) {
        for (int j = 0; j < 4; ++j) {
            m_vertex_array[i*4+j].position.x = tile.vertices[j].pos.x;
            m_vertex_array[i*4+j].position.y = tile.vertices[j].pos.y;
            m_vertex_array[i*4+j].texCoords.x = static_cast<float>(tile.vertices[j].tex.x);
            m_vertex_array[i*4+j].texCoords.y = static_cast<float>(tile.vertices[j].tex.y);
        }
        i++;
    }
}

void TileMap::Layer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.texture = m_texture;
    target.draw(m_vertex_array, states);
}

std::string TileMap::path;

TileMap::TileMap(const ldtk::Level& level) {
    for (auto& layer : level.allLayers()) {
        if (layer.getType() == ldtk::LayerType::AutoLayer) {
            m_layers.insert({layer.getName(), {*this, layer}});
        }
    }
}

auto TileMap::getLayer(const std::string& name) const -> const Layer& {
    return m_layers.at(name);
}
