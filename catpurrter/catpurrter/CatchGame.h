#pragma once
#include <SFML/Graphics.hpp>
#include <deque>
#include <vector>
#include "MiniGameBase.h"
#include "Player.h"
#include "GameManager.h"

// CatchGameState tracks the current menu/screen in the Catch mini-game
enum class CatchGameState {
    MainMenu,       // Main menu screen
    Instructions,   // How to play screen
    Playing,        // Actively playing the game
    Paused,         // Game is paused (pause menu)
    GameOver        // Game over/results screen
};

// CatchGame implements the falling-object "catch or dodge" mini-game.
// Player moves left/right to catch good drops and avoid bad ones.
// Handles all game logic, UI, scoring, and coin rewards.
class CatchGame : public MiniGameBase {
public:
    // Constructs the CatchGame, using the font for UI and referencing player/game manager.
    CatchGame(const sf::Font& font, Player& player, GameManager& gm);

    // Initializes menu and resets all state for starting/restarting the game.
    void init() override;

    // Updates game logic: moves player, spawns and moves drops, handles scoring/lives, etc.
    // dt: time elapsed since last frame (seconds)
    void update(float dt) override;

    // Draws the current screen (menu, game, pause, etc) to the window.
    void render(sf::RenderWindow& window) override;

    // Handles keyboard input for left/right movement, menus, pausing, etc.
    void handleInput(sf::Keyboard::Key key) override;

    // Returns true if the player requested to close/exit the mini-game.
    bool shouldClose() const override { return closeRequested; }

    // Resets the close flag (so view won't immediately close on next open).
    void resetCloseFlag() override { closeRequested = false; }

    // Returns how many coins were earned in the last completed play session.
    int getCoinsEarned() const override { return coinsEarned; }

    // Returns true if the current play session has finished.
    bool finishedGame() const override { return state == CatchGameState::GameOver; }

private:
    // Core references
    const sf::Font& font;        // Reference to game's font for UI text
    Player& player;              // Reference to player data (coins, etc)
    GameManager& gameManager;    // Reference to game manager (if needed)

    // State
    CatchGameState state = CatchGameState::MainMenu; // Current menu/screen
    int menuIndex = 0;        // Which option is selected on main menu
    int pauseIndex = 0;       // Which option is selected on pause menu
    int gameOverIndex = 0;    // Which option is selected on game over menu
    bool closeRequested = false; // True if ESC/exit pressed
    bool coinsAdded = false;     // True if coins have been awarded this session

    // Game logic
    int score = 0;           // Player's score this session
    int coinsEarned = 0;     // Coins earned this session
    int lives = 2;           // Lives remaining
    float spawnTimer = 0.f;  // Time accumulator for spawning drops
    float spawnDelay = 1.0f; // Time between drop spawns (decreases with score)
    float fallSpeed = 180.f; // How fast drops fall (increases with score)

    // Drop represents one falling object (good or bad)
    struct Drop {
        sf::RectangleShape shape; // Rectangle for drawing and collision
        bool good;                // True if catching is good, false = bad drop
    };
    std::vector<Drop> drops;      // All drops currently falling

    // Player
    sf::RectangleShape playerRect; // The player's paddle/box

    // Utility methods for game/menu logic and UI
    void resetGame();                      // Resets the game state for a new play session
    void spawnDrop();                      // Spawns a new falling object
    void drawMenu(sf::RenderWindow& window);          // Draws main menu screen
    void drawGame(sf::RenderWindow& window);          // Draws the game area, player, drops, score, lives
    void drawPause(sf::RenderWindow& window);         // Draws pause overlay
    void drawInstructions(sf::RenderWindow& window);  // Draws how-to-play instructions
    void drawGameOver(sf::RenderWindow& window);      // Draws game over/results screen
};
