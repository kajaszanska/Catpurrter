#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

// Forward declaration to avoid circular dependency
class GameManager;

// ShopViewBase is an abstract base class for all shop screens (hats, shelf, fish tank, mini-games).
// Provides a common interface and shared member variables for input, option selection, and rendering.
class ShopViewBase {
public:
    virtual ~ShopViewBase() = default;

    // Initializes the shop's internal state and menu options.
    virtual void init() = 0;

    // Draws the shop's UI/menu to the given window.
    virtual void render(sf::RenderWindow& window) = 0;

    // Handles keyboard input for navigating/buying/closing the shop.
    virtual void handleInput(sf::Keyboard::Key key) = 0;

    // Returns true if the player requested to close/exit the shop.
    virtual bool shouldClose() const = 0;

    // Resets the close flag so the shop can be reopened.
    virtual void resetCloseFlag() = 0;

    // Updates which options are highlighted/owned (after input or purchase).
    virtual void updateOptionColors() = 0;

protected:
    // Base constructor: takes a reference to the game's font, player data, and game manager.
    ShopViewBase(sf::Font& font, Player& player, GameManager* gm)
        : font(font), playerData(player), gameManager(gm) {
    }

    sf::Font& font;                 // Reference to the game's font for UI
    Player& playerData;             // Reference to the player's data (coins, inventory, etc.)
    GameManager* gameManager;       // Pointer to the main game manager (for callbacks, etc.)
    int selectedIndex = 0;          // Which shop menu option is currently highlighted/selected
    bool closeFlag = false;         // True if ESC/exit has been requested
};
