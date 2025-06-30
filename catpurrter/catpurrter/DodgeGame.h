#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "GameManager.h"

enum class DodgeGameState { MainMenu, Playing, Paused, Instructions, GameOver };

struct DodgeDrop {
    sf::CircleShape shape;
    sf::Vector2f velocity;
};

class DodgeGame {
public:
    DodgeGame(const sf::Font& font, Player& player, GameManager& gm);
    bool shouldClose() const { return closeRequested; }

    void init();
    void resetGame();
    void spawnDrop();
    void update(float dt);
    void handleInput(sf::Keyboard::Key key);
    void render(sf::RenderWindow& window);

private:
    void drawMenu(sf::RenderWindow& window);
    void drawGame(sf::RenderWindow& window);
    void drawPause(sf::RenderWindow& window);
    void drawInstructions(sf::RenderWindow& window);
    void drawGameOver(sf::RenderWindow& window);

    const sf::Font& font;
    Player& player;
    GameManager& gameManager;

    DodgeGameState state;

    int menuIndex, pauseIndex, gameOverIndex;
    bool closeRequested, coinsAdded;

    int score, coinsEarned, lives;
    sf::RectangleShape playerRect;
    std::vector<DodgeDrop> drops;
    float spawnTimer, spawnDelay, dropSpeed;
};
