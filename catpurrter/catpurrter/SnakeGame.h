#pragma once
#include <SFML/Graphics.hpp>
#include <deque>
#include <random>
#include <string>

class Player;
class GameManager;


enum class SnakeGameState {
    MainMenu,
    Playing,
    Paused,
    Instructions,
    GameOver
};

class SnakeGame {
public:
    SnakeGame(const sf::Font& font, Player& player, GameManager& gm);
    void init();
    void update(float dt);
    void render(sf::RenderWindow& window);
    void handleInput(sf::Keyboard::Key key);

    bool shouldClose() const { return closeRequested; }
    void resetCloseFlag() { closeRequested = false; }
    int getCoinsEarned() const { return coinsEarned; }
    bool finishedGame() const { return gameFinished; }

private:
    int gameOverIndex = 0; // 0 = Restart, 1 = Back to menu
    void drawGameOver(sf::RenderWindow& window);

    bool coinsAdded = false;

    void resetGame();
    void spawnFood();
    void moveSnake();
    bool checkCollision();
    void drawMenu(sf::RenderWindow& window);
    void drawGame(sf::RenderWindow& window);
    void drawPause(sf::RenderWindow& window);
    void drawInstructions(sf::RenderWindow& window);

    Player& player;
    GameManager& gameManager;

    // Game parameters
    const sf::Font& font;
    SnakeGameState state = SnakeGameState::MainMenu;

    sf::Vector2i direction = { 1, 0 }; // Start moving right
    sf::Vector2i nextDirection = { 1, 0 };
    std::deque<sf::Vector2i> snake;
    sf::Vector2i food;

    int gridWidth = 20;
    int gridHeight = 16;
    int tileSize = 24;
    float moveTimer = 0;
    float moveDelay = 0.12f; // Move every 0.12s

    bool closeRequested = false;
    bool gameOver = false;
    bool gameFinished = false;
    int score = 0;
    int coinsEarned = 0;

    // Menu navigation
    int menuIndex = 0; // 0=Play, 1=Instructions, 2=Exit
    int pauseIndex = 0; // 0=Resume, 1=Exit
};

