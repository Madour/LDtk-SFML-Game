#include <iostream>

#include "LDtkLoader/World.hpp"
#include "TileMap.hpp"

int main() {
    ldtk::World world;
    try {
        world.loadFromFile("assets/maps/world1.ldtk");
        std::cout << "LDtk World \"" << world.getFilePath() << "\" was loaded successfully." << std::endl;
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    // get first level from the world
    auto& ldtk_level0 = world.getLevel("Level_0");

    // load the TileMap
    TileMap::path = world.getFilePath().directory();
    TileMap tilemap(ldtk_level0);

    // get Entities layer from level_0
    auto& entities_layer = ldtk_level0.getLayer("Entities");

    // retrieve collider entities from entities layer and store them in the vector
    std::vector<sf::FloatRect> collisions;
    for (auto& col : entities_layer.getEntities("Collider")) {
        collisions.emplace_back(
                (float)col.getPosition().x, (float)col.getPosition().y,
                (float)col.getSize().x, (float)col.getSize().y
        );
    }

    // get the Player entity, and its skin_color field
    auto& player_ent = entities_layer.getEntities("Player")[0];
    auto& player_color = player_ent.getField<ldtk::Color>("skin_color").value();
    // create player shape
    sf::CircleShape player(8);
    player.setPosition((float)player_ent.getPosition().x, (float)player_ent.getPosition().y);
    player.setFillColor({player_color.r, player_color.g, player_color.b});

    // create the window
    sf::RenderWindow window;
    window.create(sf::VideoMode(800, 500), "LDtkLoader - SFML");
    window.setFramerateLimit(60);

    // create the camera view
    sf::View view({0, 0, 400, 250});
    view.zoom(0.55f);
    view.setCenter(player.getPosition());

    // start game loop
    sf::Event event{};
    while(window.isOpen()) {
        while(window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // move camera arrows
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            view.move(0, -1.5);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            view.move(0, 1.5);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            view.move(-1.5, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            view.move(1.5, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
            view.zoom(0.95f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
            view.zoom(1.05f);

        // check for camera limits
        // left X limit
        if (view.getCenter().x - view.getSize().x/2 < 0)
            view.setCenter(view.getSize().x/2, view.getCenter().y);
        // right X limit
        else if (view.getCenter().x + view.getSize().x/2 > ldtk_level0.size.x)
            view.setCenter(ldtk_level0.size.x - view.getSize().x/2, view.getCenter().y);
        // top Y limit
        if (view.getCenter().y - view.getSize().y/2 < 0)
            view.setCenter(view.getCenter().x, view.getSize().y/2);
        // bottom Y limit
        else if (view.getCenter().y + view.getSize().y/2 > ldtk_level0.size.y)
            view.setCenter(view.getCenter().x, ldtk_level0.size.y - view.getSize().y/2);

        window.clear();
        window.setView(view);

        // draw map background layers
        window.draw(tilemap.getLayer("Ground"));
        window.draw(tilemap.getLayer("Trees"));

        // draw player
        window.draw(player);

        // draw map top layer
        window.draw(tilemap.getLayer("Trees_top"));

        // draw collisions
        for (auto& rect : collisions) {
            sf::RectangleShape r{{rect.width, rect.height}};
            r.setPosition(rect.left, rect.top);
            r.setFillColor({200, 0, 0, 95});
            window.draw(r);
        }

        window.setView(window.getDefaultView());
        window.display();
    }

    return 0;
}
