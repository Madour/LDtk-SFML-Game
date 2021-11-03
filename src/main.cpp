#include <iostream>
#include "LDtkLoader/World.hpp"
#include "TileMap.hpp"

auto getPlayerCollider(sf::Shape& player) -> sf::FloatRect {
    auto bounds = player.getGlobalBounds();
    sf::FloatRect rect;
    rect.left = bounds.left;
    rect.width = bounds.width;
    rect.top = bounds.top+bounds.height/2;
    rect.height = bounds.height/2;
    return rect;
}

auto getColliderShape(const sf::FloatRect& rect) -> sf::RectangleShape {
    sf::RectangleShape r{{rect.width, rect.height}};
    r.setPosition(rect.left, rect.top);
    r.setFillColor({200, 0, 0, 95});
    return r;
}

int main() {
    ldtk::World world;
    std::string ldtk_filename = "assets/maps/world1.ldtk";
    try {
        world.loadFromFile(ldtk_filename);
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
    for (ldtk::Entity& col : entities_layer.getEntitiesByName("Collider")) {
        collisions.emplace_back(
                (float)col.getPosition().x, (float)col.getPosition().y,
                (float)col.getSize().x, (float)col.getSize().y
        );
    }

    // get the Player entity, and its 'color' field
    auto& player_ent = entities_layer.getEntitiesByName("Player")[0].get();
    auto& player_color = player_ent.getField<ldtk::Color>("color").value();
    // create player shape
    sf::RectangleShape player({8, 16});
    player.setOrigin(4, 16);
    player.setPosition((float)player_ent.getPosition().x+8, (float)player_ent.getPosition().y+16);
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
    bool show_colliders = false;
    while(window.isOpen()) {
        // EVENTS
        while(window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::F1)
                    show_colliders = !show_colliders;
                else if (event.key.code == sf::Keyboard::F5) {
                    world.loadFromFile(ldtk_filename);
                    tilemap.load(world.getLevel("Level_0"));
                }
            }
        }

        // UPDATE
        // move player with arrows
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            player.move(0, -1.5);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            player.move(0, 1.5);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            player.move(-1.5, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            player.move(1.5, 0);

        // do collision checks
        auto player_collider = getPlayerCollider(player);
        for (auto& rect : collisions) {
            sf::FloatRect intersect;
            if (player_collider.intersects(rect, intersect)) {
                if (intersect.width < intersect.height) {
                    if (player_collider.left < intersect.left)
                        player.move(-intersect.width, 0);
                    else
                        player.move(intersect.width, 0);
                }
                else {
                    if (player_collider.top < intersect.top)
                        player.move(0, -intersect.height);
                    else
                        player.move(0, intersect.height);
                }
            }
        }
        player_collider = getPlayerCollider(player);

        // update camera
        view.move((player.getPosition() - view.getCenter())/5.f);
        // check for camera X limit
        if (view.getCenter().x - view.getSize().x/2 < 0)
            view.setCenter(view.getSize().x/2, view.getCenter().y);
        else if (view.getCenter().x + view.getSize().x/2 > ldtk_level0.size.x)
            view.setCenter(ldtk_level0.size.x - view.getSize().x/2, view.getCenter().y);
        // check for camera Y limit
        if (view.getCenter().y - view.getSize().y/2 < 0)
            view.setCenter(view.getCenter().x, view.getSize().y/2);
        else if (view.getCenter().y + view.getSize().y/2 > ldtk_level0.size.y)
            view.setCenter(view.getCenter().x, ldtk_level0.size.y - view.getSize().y/2);

        // RENDER
        window.clear();
        window.setView(view);

        // draw map background layers
        window.draw(tilemap.getLayer("Ground"));
        window.draw(tilemap.getLayer("Trees"));

        // draw player
        window.draw(player);
        window.draw(getColliderShape(player_collider));

        // draw map top layer
        window.draw(tilemap.getLayer("Trees_top"));

        // draw collisions
        if (show_colliders)
            for (auto& rect : collisions)
                window.draw(getColliderShape(rect));

        window.display();
    }
    return 0;
}
