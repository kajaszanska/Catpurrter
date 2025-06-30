#pragma once
#include <SFML/Graphics.hpp>
#include <deque>
#include <vector>
#include "Player.h"
#include "GameManager.h"

enum class CatchGameState {
    MainMenu,
    Instructions,
    Playing,
    Paused,
    GameOver
};

class CatchGame {
public:
    CatchGame(const sf::Font& font, Player& player, GameManager& gm);

    void init();
    void update(float dt);
    void render(sf::RenderWindow& window);
    void handleInput(sf::Keyboard::Key key);
    bool shouldClose() const { return closeRequested; }

private:
    // Core references
    const sf::Font& font;
    Player& player;
    GameManager& gameManager;

    // State
    CatchGameState state = CatchGameState::MainMenu;
    int menuIndex = 0;
    int pauseIndex = 0;
    int gameOverIndex = 0;
    bool closeRequested = false;
    bool coinsAdded = false;

    // Game logic
    int score = 0;
    int coinsEarned = 0;
    int lives = 2;
    float spawnTimer = 0.f;
    float spawnDelay = 1.0f;
    float fallSpeed = 180.f;

    struct Drop {
        sf::RectangleShape shape;
        bool good;
    };
    std::vector<Drop> drops;

    // Player
    sf::RectangleShape playerRect;

    // Utility methods
    void resetGame();
    void spawnDrop();
    void drawMenu(sf::RenderWindow& window);
    void drawGame(sf::RenderWindow& window);
    void drawPause(sf::RenderWindow& window);
    void drawInstructions(sf::RenderWindow& window);
    void drawGameOver(sf::RenderWindow& window);
};
