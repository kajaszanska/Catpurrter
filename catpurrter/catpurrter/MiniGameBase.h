#pragma once
#include <SFML/Graphics.hpp>

// MiniGameBase is an abstract interface for all mini-games in your project.
// It ensures all mini-games support a common set of methods for running, updating, rendering, input, and status.
class MiniGameBase {
public:
    virtual ~MiniGameBase() = default;

    // Initializes the mini-game state (called at start or reset).
    virtual void init() = 0;

    // Updates game logic.
    // dt: time elapsed since last frame (seconds).
    virtual void update(float dt) = 0;

    // Draws the current game state (UI, game objects, menus, etc).
    virtual void render(sf::RenderWindow& window) = 0;

    // Handles keyboard input for game control, menus, etc.
    virtual void handleInput(sf::Keyboard::Key key) = 0;

    // Returns true if the player requested to exit/close the mini-game.
    virtual bool shouldClose() const = 0;

    // Resets the close flag so the game can be replayed/reopened.
    virtual void resetCloseFlag() = 0;

    // Returns the number of coins earned in the most recent play session.
    virtual int getCoinsEarned() const = 0;

    // Returns true if the current session has finished (e.g., game over).
    virtual bool finishedGame() const = 0;
};
