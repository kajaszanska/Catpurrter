#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "GameManager.h"
#include "MiniGameBase.h"

// DodgeGameState tracks which screen/menu the DodgeGame is currently showing.
enum class DodgeGameState {
    MainMenu,      // On the main menu screen
    Playing,       // Playing the game (active)
    Paused,        // Game is paused (pause menu shown)
    Instructions,  // Instructions/how-to-play screen
    GameOver       // Game over/result screen
};

// DodgeDrop represents a falling or moving object that the player must dodge.
struct DodgeDrop {
    sf::CircleShape shape;  // The graphical shape for this drop
    sf::Vector2f velocity;  // The movement direction and speed
};

// DodgeGame is a mini-game where the player moves around to dodge incoming "drops" from all sides.
// Handles game state, menus, scoring, player movement, drop spawning, input, and coin rewards.
class DodgeGame : public MiniGameBase {
public:
    // Constructs the DodgeGame with font for drawing, and references to player and game manager.
    DodgeGame(const sf::Font& font, Player& player, GameManager& gm);

    // Initializes the game (loads or resets all state, sets up menu, etc).
    void init() override;

    // Updates all logic for the game, including movement, drops, collision, scoring, etc.
    // dt: time elapsed since last frame (seconds)
    void update(float dt) override;

    // Draws the current game state/menu (gameplay, menus, pause, instructions, etc).
    void render(sf::RenderWindow& window) override;

    // Handles keyboard input for menus, movement, pausing, etc.
    void handleInput(sf::Keyboard::Key key) override;

    // Returns true if the player requested to exit this mini-game (e.g., pressed ESC or chose Exit).
    bool shouldClose() const override { return closeRequested; }

    // Resets the close flag (so view won't immediately close when reopened).
    void resetCloseFlag() override { closeRequested = false; }

    // Returns the number of coins earned in the last play session.
    int getCoinsEarned() const override { return coinsEarned; }

    // Returns true if the game has finished (player lost all lives).
    bool finishedGame() const override { return state == DodgeGameState::GameOver; }

private:
    // Drawing helpers for various UI/game states:
    void drawMenu(sf::RenderWindow& window);          // Draws the main menu screen
    void drawGame(sf::RenderWindow& window);          // Draws the gameplay area, player, drops, score, lives
    void drawPause(sf::RenderWindow& window);         // Draws pause menu overlay
    void drawInstructions(sf::RenderWindow& window);  // Draws instructions/how-to-play
    void drawGameOver(sf::RenderWindow& window);      // Draws game over/results screen

    void resetGame();         // Resets all variables and game state for a new game
    void spawnDrop();         // Spawns a new drop from a random edge

    const sf::Font& font;     // Reference to game's font for all UI text
    Player& player;           // Reference to player data (for coins, etc)
    GameManager& gameManager; // Reference to game manager (unused here, but for future)

    DodgeGameState state = DodgeGameState::MainMenu; // Current screen/menu being shown

    // Menu/pause navigation and flags:
    int menuIndex = 0, pauseIndex = 0, gameOverIndex = 0; // Indices for which option is selected in menus
    bool closeRequested = false;  // True if the player pressed ESC or chose Exit
    bool coinsAdded = false;      // True if coins have been awarded for this session

    // Gameplay state:
    int score = 0;        // Current score (points from dodging drops)
    int coinsEarned = 0;  // Coins earned this session (1 per 4 points)
    int lives = 2;        // Player lives remaining (lose one per hit)

    sf::RectangleShape playerRect;      // The player character (square)
    std::vector<DodgeDrop> drops;       // All active falling/moving drops

    float spawnTimer = 0.f;    // Time accumulator for next drop spawn
    float spawnDelay = 1.f;    // Delay between new drop spawns (decreases with score)
    float dropSpeed = 220.f;   // Speed of drops (increases with score)
};
