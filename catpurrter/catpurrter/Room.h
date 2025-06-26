#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
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

    int playerX;
    int playerY;

    // Player (avatar)
    sf::RectangleShape playerRect;
    sf::Vector2f playerPos;
    float playerSpeed = 0.20f;

    // Room objects (rectangles and names)
    struct RoomObject {
        sf::RectangleShape rect;
        std::string name;
    };
    std::vector<RoomObject> objects;
    int highlightedIndex = -1;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
};
