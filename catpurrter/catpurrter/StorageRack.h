#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

// Forward declaration to avoid circular dependency
class GameManager;

// StorageRack displays a large view of the player's unlocked hats.
// Allows the player to equip or unequip hats and shows which hat is currently equipped.
// Handles user input, navigation, and rendering of the hats in a grid.
class StorageRack {
public:
    // Constructs the storage rack view with the game's font, player data, and game manager.
    StorageRack(const sf::Font& font, Player& player, GameManager* gm);

    // Initializes the storage rack (loads background, hat textures, positions, etc.).
    void init();

    // Updates the selection highlight (not used for animation in this view, but present for consistency).
    void update();

    // Draws the rack background, all hats, selection highlight, and equipped info to the window.
    void render(sf::RenderWindow& window);

    // Handles keyboard input for navigating hats, equipping/unequipping, and exiting (ESC).
    void handleInput(sf::Keyboard::Key key);

    // Returns true if the user has requested to close this view.
    bool shouldClose() const;

    // Resets the close flag so the view won't immediately close next time it's opened.
    void resetCloseFlag();

private:
    const sf::Font& font;           // Reference to the game's font for drawing text.
    Player& playerData;             // Reference to player data (owns hats, knows equipped hat).

    GameManager* gameManager = nullptr; // Pointer to the main GameManager (not directly used here).

    std::vector<sf::Text> hatOptions;   // (Unused in implementation, can be used for future features or UI.)
    int selectionIndex = 0;             // Which hat is currently highlighted/selected.
    bool closeRequested = false;        // True if the player pressed ESC to exit.

    sf::Texture backgroundTexture;      // Texture for the rack background image.
    sf::Sprite backgroundSprite;        // Sprite for the rack background.

    std::map<std::string, sf::Texture> hatTextures; // Textures for each unlocked hat (by id).

    std::vector<sf::Vector2f> hatPositions;         // Screen positions for displaying hats in the rack grid.
};
