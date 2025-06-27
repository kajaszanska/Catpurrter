#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include "Player.h"

class Room {
public:
    Room(sf::Font& font, Player& player);

    void movePlayer(int dx, int dy);

    void init();
    void handleInput(sf::Keyboard::Key key);
    void update();
    void render(sf::RenderWindow& window);

    // For highlighting/interactions:
    bool isNearObject() const;
    std::string getNearbyObject() const;

private:
    sf::Font& font;
    Player& playerData;

    // Player (avatar)
    sf::RectangleShape playerRect;
    sf::Vector2f playerPos;
    float playerSpeed = 0.20f;

    struct RoomObject {
        sf::RectangleShape rect;
        std::string name;
        std::shared_ptr<sf::Texture> texture; // This is fine!
        // Always keep this pointer alive as long as the rect exists!
    };

    std::vector<RoomObject> objects;
    int highlightedIndex = -1;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    // Object creators
    RoomObject createComputer();
    RoomObject createAquarium();
    RoomObject createStorageRack();
    RoomObject createShelves();
    RoomObject createDoors();
};
