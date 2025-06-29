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

    sf::Texture aquariumSmTexture;     // aquarium.png (default room view)
    sf::Texture aquariumSmPlants;      // aquariumplantssmall.png (room view, plant only)
    sf::Texture aquariumSmCastle;      // aquariumcastlesmall.png (room view, castle only)
    sf::Texture aquariumSmAll;         // aquariumallsmall.png (room view, both)

    struct FishVisual {
        std::string id;
        sf::Vector2f position;
        sf::Vector2f velocity;
        bool facingRight = true;
        float directionTimer = 0.f;
        float minSwimDistance = 0.f;
        float distanceSinceDirectionChange = 0.f;
        float verticalSwimTimer = 0.f; 
    };

   


    std::vector<FishVisual> fishes;


    std::unordered_map<std::string, sf::Texture> fishTextures;




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

    std::unordered_map<std::string, sf::Texture> aquariumItemTextures;

    // --- Player graphics ---
    sf::Sprite playerSprite;
    std::map<std::string, sf::Texture> playerTextures; // Key: "up1", "up2", "down1", etc.

    std::string playerDir = "down"; // "up", "down", "left", "right"
    int playerFrame = 1;
    float animTimer = 0.f; // time accumulator for frame switch
    float animDelay = 6.f; // delay between frame switches, tweak as needed

};
