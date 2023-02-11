// Created by Modar Nasser on 13/06/2021.

#pragma once

#include <vector>
#include <map>

#include <SFML/Graphics.hpp>
#include <LDtkLoader/Level.hpp>

class TileMap {
public:
    static std::string path;

    class Textures {
        Textures() = default;
        std::map<std::string, sf::Texture> data;
        static auto instance() -> Textures&;
    public:
        Textures(const Textures&) = delete;
        static auto get(const std::string& name)  -> sf::Texture&;
    };

    class Layer : public sf::Drawable{
        friend TileMap;
        Layer(const ldtk::Layer& layer, sf::RenderTexture& render_texture);
        sf::Texture* m_tileset_texture;
        sf::RenderTexture& m_render_texture;
        sf::VertexArray m_vertex_array;
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };

    TileMap() = default;
    void load(const ldtk::Level& level);
    auto getLayer(const std::string& name) const -> const Layer&;

private:
    mutable sf::RenderTexture m_render_texture;
    std::map<std::string, Layer> m_layers;
};
