#include <iostream>

#include <SFML/Graphics.hpp>
#include <LDtkLoader/Project.hpp>

#include "TileMap.hpp"


auto getPlayerCollider(sf::Shape& player) -> sf::FloatRect {
    auto bounds = player.getGlobalBounds();
    sf::FloatRect rect;
    rect.left = bounds.left;
    rect.width = bounds.width;
    rect.top = bounds.top + bounds.height / 2;
    rect.height = bounds.height / 2;
    return rect;
}

auto getColliderShape(const sf::FloatRect& rect) -> sf::RectangleShape {
    sf::RectangleShape r;
    r.setSize({rect.width, rect.height});
    r.setPosition(rect.left, rect.top);
    r.setFillColor({200, 0, 0, 95});
    return r;
}

struct Game {
    TileMap tilemap;
    sf::RectangleShape player;

    std::vector<sf::FloatRect> colliders;
    bool show_colliders = false;

    sf::View camera;
    sf::FloatRect camera_bounds;

    void init(const ldtk::Project& ldtk, bool reloading = false) {
        // get the world from the project
        auto& world = ldtk.getWorld();

        // get first level from the world
        auto& ldtk_level0 = world.getLevel("Level_0");

        // load the TileMap from the level
        TileMap::path = ldtk.getFilePath().directory();
        tilemap.load(ldtk_level0);

        // get Entities layer from level_0
        auto& entities_layer = ldtk_level0.getLayer("Entities");

        // retrieve collider entities from entities layer and store them in the colliders vector
        for (ldtk::Entity& col : entities_layer.getEntitiesByName("Collider")) {
            colliders.emplace_back(
                (float)col.getPosition().x, (float)col.getPosition().y,
                (float)col.getSize().x, (float)col.getSize().y
            );
        }

        // get the Player entity, and its 'color' field
        auto& player_ent = entities_layer.getEntitiesByName("Player")[0].get();
        auto& player_color = player_ent.getField<ldtk::Color>("color").value();

        // initialize player shape
        player.setSize({8, 16});
        player.setOrigin(4, 16);
        if (!reloading) {
            player.setPosition(static_cast<float>(player_ent.getPosition().x + 8),
                               static_cast<float>(player_ent.getPosition().y + 16));
        }
        player.setFillColor({player_color.r, player_color.g, player_color.b});

        // create camera view
        camera.setSize({400, 250});
        camera.zoom(0.55f);
        camera.setCenter(player.getPosition());
        camera_bounds.left = 0;
        camera_bounds.top = 0;
        camera_bounds.width = static_cast<float>(ldtk_level0.size.x);
        camera_bounds.height = static_cast<float>(ldtk_level0.size.y);
    }

    void update() {
        // move player with keyboard arrows
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
        for (auto& rect : colliders) {
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

        // update camera
        camera.move((player.getPosition() - camera.getCenter())/5.f);

        auto cam_size = camera.getSize();
        auto cam_pos = camera.getCenter();

        // check for camera X limit
        if (cam_pos.x - cam_size.x / 2 < camera_bounds.left) {
            camera.setCenter(camera_bounds.left + cam_size.x / 2, cam_pos.y);
        }
        else if (cam_pos.x + cam_size.x / 2 > camera_bounds.left + camera_bounds.width) {
            camera.setCenter(camera_bounds.left + camera_bounds.width - cam_size.x / 2, cam_pos.y);
        }

        // check for camera Y limit
        if (cam_pos.y - cam_size.y / 2 < camera_bounds.top) {
            camera.setCenter(cam_pos.x, camera_bounds.top + cam_size.y / 2);
        }
        else if (cam_pos.y + cam_size.y / 2 > camera_bounds.top + camera_bounds.height) {
            camera.setCenter(cam_pos.x, camera_bounds.top + camera_bounds.height - cam_size.y / 2);
        }
    }

    void render(sf::RenderTarget& target) {
        target.setView(camera);

        // draw map background layers
        target.draw(tilemap.getLayer("Ground"));
        target.draw(tilemap.getLayer("Trees"));

        // draw player
        target.draw(player);
        if (show_colliders) {
            target.draw(getColliderShape(getPlayerCollider(player)));
        }

        // draw map top layer
        target.draw(tilemap.getLayer("Trees_top"));

        if (show_colliders) {
            // draw colliders
            for (auto& rect : colliders)
                target.draw(getColliderShape(rect));
        }
    }
};

int main() {
    ldtk::Project project;
    std::string ldtk_filename = "assets/maps/world1.ldtk";
    try {
        project.loadFromFile(ldtk_filename);
        std::cout << "LDtk World \"" << project.getFilePath() << "\" was loaded successfully." << std::endl;
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    // initialize the game from the LDtk project data
    Game game;
    game.init(project);

    // create the window
    sf::RenderWindow window;
    window.create(sf::VideoMode(800, 500), "LDtkLoader - SFML");
    window.setFramerateLimit(60);

    // start game loop
    while(window.isOpen()) {

        // EVENTS
        sf::Event event{};
        while(window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::F1)
                    game.show_colliders = !game.show_colliders;
                else if (event.key.code == sf::Keyboard::F5) {
                    // reload the LDtk project and reinitialize the game
                    project.loadFromFile(ldtk_filename);
                    game.init(project, true);
                    std::cout << "Reloaded project " << project.getFilePath() << std::endl;
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
        }

        // UPDATE
        game.update();

        // RENDER
        window.clear();
        game.render(window);
        window.display();

    }
    return 0;
}
