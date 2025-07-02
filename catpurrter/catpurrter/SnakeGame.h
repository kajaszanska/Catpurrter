#pragma once
#include "MiniGameBase.h"
#include <SFML/Graphics.hpp>
#include <deque>
#include <random>
#include <string>

// Forward declarations to avoid circular dependencies
class Player;
class GameManager;

// SnakeGameState tracks the current state of the Snake mini-game
enum class SnakeGameState {
    MainMenu,      // On the main menu screen
    Playing,       // Actively playing the game
    Paused,        // Paused state (pause menu)
    Instructions,  // How-to-play instructions screen
    GameOver       // Game over/result screen
};

// SnakeGame implements the classic Snake mini-game, playable from your game's computer desktop.
// Handles all game logic, rendering, menus, pause/game over, input, and scoring/coin rewards.
class SnakeGame : public MiniGameBase {
public:
    // Creates the SnakeGame with font for drawing, references to player and game manager.
    SnakeGame(const sf::Font& font, Player& player, GameManager& gm);

    // Initializes state for starting or restarting the game (loads menu, resets scores, etc).
    void init() override;

    // Updates game logic, movement, collision, scoring, etc.
    // dt: time elapsed since last frame (seconds)
    void update(float dt) override;

    // Draws the current state (menu, gameplay, paused, game over, etc) to the window.
    void render(sf::RenderWindow& window) override;

    // Handles keyboard input for controlling the snake, navigating menus, pausing, etc.
    void handleInput(sf::Keyboard::Key key) override;

    // Returns true if player requested to exit the Snake mini-game.
    bool shouldClose() const override { return closeRequested; }

    // Resets the close flag for when game is opened again.
    void resetCloseFlag() override { closeRequested = false; }

    // Returns the number of coins earned in the last completed game session.
    int getCoinsEarned() const override { return coinsEarned; }

    // Returns true if the current game session is over.
    bool finishedGame() const override { return state == SnakeGameState::GameOver; }

private:
    int gameOverIndex = 0; // For game over menu: 0=Restart, 1=Back to menu

    // Renders the game over screen and options.
    void drawGameOver(sf::RenderWindow& window);

    bool coinsAdded = false; // True if coins for this game have been awarded to player

    // Game logic helpers
    void resetGame();               // Resets the game state for new or replay session
    void spawnFood();               // Places a new food item on the board
    void moveSnake();               // Moves the snake by one grid space

    // UI drawing helpers
    void drawMenu(sf::RenderWindow& window);          // Draws main menu
    void drawGame(sf::RenderWindow& window);          // Draws the play area, snake, food, and score
    void drawPause(sf::RenderWindow& window);         // Draws pause menu overlay
    void drawInstructions(sf::RenderWindow& window);  // Draws the how-to-play instructions

    Player& player;                // Reference to player data (for coin rewards)
    GameManager& gameManager;      // Reference to main game manager (if needed)

    const sf::Font& font;          // Reference to game's font for UI rendering
    SnakeGameState state = SnakeGameState::MainMenu; // Current game/menu state

    sf::Vector2i direction = { 1, 0 };     // Current movement direction of the snake
    sf::Vector2i nextDirection = { 1, 0 }; // Next direction (set by input)
    std::deque<sf::Vector2i> snake;        // Snake body segments (front is the head)
    sf::Vector2i food;                     // Position of the food on the grid

    int gridWidth = 20;        // Board width (tiles)
    int gridHeight = 16;       // Board height (tiles)
    int tileSize = 24;         // Size of each grid tile (pixels)
    float moveTimer = 0;       // Time accumulator for snake movement
    float moveDelay = 0.12f;   // Delay (seconds) between snake moves (controls speed)

    bool closeRequested = false; // True if player pressed ESC or exited via menu
    bool gameOver = false;       // True if game ended (collision)
    bool gameFinished = false;   // True if a play session is over
    int score = 0;               // Current score (number of foods eaten)
    int coinsEarned = 0;         // Number of coins earned in last play session

    // Menu and pause navigation indices
    int menuIndex = 0;  // 0=Play, 1=Instructions, 2=Exit (main menu)
    int pauseIndex = 0; // 0=Resume, 1=Exit (pause menu)
};
