#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include "Player.h"
#include <map>

// The Room class represents the main interactive room view where the player moves around.
// It handles drawing the player, objects (aquarium, computer, shelves, storage rack, doors),
// collision, interaction highlights, and showing owned decorations and hats.
class Room {
public:
    // Constructs the Room with references to the font and player data.
    Room(sf::Font& font, Player& player);

    // RoomObject represents an interactive object (computer, aquarium, etc.) in the room.
    struct RoomObject {
        sf::RectangleShape rect;            // Rectangle area and texture for the object.
        std::string name;                   // Name for display and interaction (e.g., "Aquarium").
        std::shared_ptr<sf::Texture> texture; // Shared pointer to the object's texture.
    };

    // Rebuilds/refreshes the fish visuals in the aquarium (e.g., after buying new fish).
    void refreshAquariumVisuals();

    // Moves the player by (dx, dy), handling collision and updating player position.
    void movePlayer(int dx, int dy);

    // Initializes all room state: loads textures, sets up player and objects, places decorations, etc.
    void init();

    // Handles keyboard input for movement (WASD/arrow keys).
    void handleInput(sf::Keyboard::Key key);

    // Updates all state: player movement, animation, collision detection, fish animation, highlights, etc.
    void update();

    // Draws the room, player, objects, decorations, hats, fish, and interaction highlights.
    void render(sf::RenderWindow& window);

    // Returns true if the player is close enough to any object for interaction.
    bool isNearObject() const;

    // Returns the name of the object the player is nearest to, or empty string if none.
    std::string getNearbyObject() const;

    // Returns a pointer to the currently highlighted object (if any).
    const RoomObject* getHighlightedObject() const;

    // Enum for easy access to each room object by index.
    enum RoomObjectIndex {
        COMPUTER = 0,
        AQUARIUM = 1,
        STORAGE_RACK = 2,
        SHELVES = 3,
        DOORS = 4
    };

    // Aquarium background textures for small (room) aquarium with various decorations:
    sf::Texture aquariumSmTexture;     // Default aquarium (no decorations)
    sf::Texture aquariumSmPlants;      // Aquarium with plant decoration
    sf::Texture aquariumSmCastle;      // Aquarium with castle decoration
    sf::Texture aquariumSmAll;         // Aquarium with both plant and castle

    // FishVisual represents one fish swimming in the small aquarium in the room.
    struct FishVisual {
        std::string id;                      // Fish type (e.g., "fish1")
        sf::Vector2f position;               // Fish position in the room aquarium area
        sf::Vector2f velocity;               // Current velocity (pixels/sec)
        bool facingRight = true;             // True if fish is facing right
        float directionTimer = 0.f;          // Timer for when fish will change direction again
        float minSwimDistance = 0.f;         // Minimum distance before changing direction
        float distanceSinceDirectionChange = 0.f; // How far the fish has swum since last direction change
        float verticalSwimTimer = 0.f;       // Timer for how long fish swims vertically
    };

    std::vector<FishVisual> fishes;                      // List of all fish in the room aquarium.
    std::unordered_map<std::string, sf::Texture> fishTextures; // Fish textures (by id+direction).

private:
    sf::Font& font;                              // Reference to the game's font.
    Player& playerData;                          // Reference to player data (decorations, fish, hats).

    sf::RectangleShape playerRect;               // Rectangle for player's collision and position.
    sf::Vector2f playerPos;                      // Player's current position in the room.
    float playerSpeed = 0.20f;                   // Player's movement speed.

    sf::Texture indicatorTexture;                // Texture for the "interact" indicator icon.
    sf::Sprite indicatorSprite;                  // Sprite for drawing the indicator.

    std::vector<RoomObject> objects;             // All interactive objects in the room.
    int highlightedIndex = -1;                   // Which object (if any) is currently highlighted for interaction.

    sf::Texture backgroundTexture;               // Background room image.
    sf::Sprite backgroundSprite;                 // Sprite for drawing the background.

    // Creates a RoomObject for each room feature.
    RoomObject createComputer();
    RoomObject createAquarium();
    RoomObject createStorageRack();
    RoomObject createShelves();
    RoomObject createDoors();

    // For possible future use: Information about decorations available in the shop.
    struct DecorationInfo {
        std::string id;      // Decoration ID string.
        std::string name;    // Human-readable name.
        int price;           // Price in coins.
    };

    std::vector<DecorationInfo> decorations;         // All possible decorations.
    std::map<std::string, sf::Texture> decorationTextures; // Textures for small shelf decorations.

    std::map<std::string, sf::Texture> hatTextures;        // Small textures for unlocked hats (room rack).

    std::unordered_map<std::string, sf::Texture> aquariumItemTextures; // Not used directly in render, for future expansion.

    sf::Sprite playerSprite;                        // Sprite for drawing the player with animation/hat.
    std::map<std::string, std::shared_ptr<sf::Texture>> playerTextures; // Player sprite textures (hat+direction+frame).

    std::string playerDir = "down";                 // Player's current facing direction ("up", "down", etc.).
    int playerFrame = 1;                            // Animation frame (1 or 2).
    float animTimer = 0.f;                          // Timer for switching animation frames.
    float animDelay = 6.f;                          // Number of update cycles between frame switches.
};
