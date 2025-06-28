#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include "Player.h"
#include <map>


class Room {
public:
    Room(sf::Font& font, Player& player);

    struct RoomObject {
        sf::RectangleShape rect;
        std::string name;
        std::shared_ptr<sf::Texture> texture; 
    };

    void movePlayer(int dx, int dy);

    void init();
    void handleInput(sf::Keyboard::Key key);
    void update();
    void render(sf::RenderWindow& window);

    // For highlighting/interactions:
    bool isNearObject() const;
    std::string getNearbyObject() const;
    const RoomObject* getHighlightedObject() const;



    enum RoomObjectIndex {
        COMPUTER = 0,
        AQUARIUM = 1,
        STORAGE_RACK = 2,
        SHELVES = 3,
        DOORS = 4
    };


private:
    sf::Font& font;
    Player& playerData;

    // Player (avatar)
    sf::RectangleShape playerRect;
    sf::Vector2f playerPos;
    float playerSpeed = 0.20f;

   

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

 
    struct DecorationInfo {
        std::string id;   
        std::string name;   
        int price;
    };

    std::vector<DecorationInfo> decorations;
    std::map<std::string, sf::Texture> decorationTextures;

    std::map<std::string, sf::Texture> hatTextures;
};
