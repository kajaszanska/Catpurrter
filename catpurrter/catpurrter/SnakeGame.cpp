#include "SnakeGame.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

#include "Player.h"
#include "GameManager.h"


SnakeGame::SnakeGame(const sf::Font& font, Player& player, GameManager& gm)
    : font(font), player(player), gameManager(gm)
{
    init();
}

void SnakeGame::init() {
    state = SnakeGameState::MainMenu;
    menuIndex = 0;
    pauseIndex = 0;
    resetGame();
}

void SnakeGame::resetGame() {
    snake.clear();
    snake.push_back({ gridWidth / 2, gridHeight / 2 });
    direction = { 1, 0 };
    nextDirection = { 1, 0 };
    moveTimer = 0.f;
    score = 0;
    gameOver = false;
    gameFinished = false;
    coinsEarned = 0;
    spawnFood();
}

void SnakeGame::spawnFood() {
    static std::mt19937 rng((unsigned)time(nullptr));
    std::uniform_int_distribution<int> xDist(0, gridWidth - 1);
    std::uniform_int_distribution<int> yDist(0, gridHeight - 1);
    while (true) {
        food = { xDist(rng), yDist(rng) };
        // Don't spawn food on the snake
        if (std::find(snake.begin(), snake.end(), food) == snake.end())
            break;
    }
}

void SnakeGame::update(float dt) {
    if (state != SnakeGameState::Playing || gameOver)
        return;

    moveTimer += dt;
    if (moveTimer >= moveDelay) {
        moveTimer = 0.f;
        moveSnake();
    }
}

void SnakeGame::moveSnake() {
    direction = nextDirection;
    sf::Vector2i newHead = snake.front() + direction;

    // Check wall collisions
    if (newHead.x < 0 || newHead.x >= gridWidth || newHead.y < 0 || newHead.y >= gridHeight) {
        gameOver = true;
        gameFinished = true;
        coinsEarned = score * 2; // 2 coins per food
        return;
    }
    // Check self-collision
    if (std::find(snake.begin(), snake.end(), newHead) != snake.end()) {
        gameOver = true;
        gameFinished = true;
        coinsEarned = score * 2;
        return;
    }

    snake.push_front(newHead);

    // Eat food
    if (newHead == food) {
        score++;
        spawnFood();
    }
    else {
        snake.pop_back();
    }
}

void SnakeGame::handleInput(sf::Keyboard::Key key) {
    if (state == SnakeGameState::MainMenu) {
        if (key == sf::Keyboard::Up || key == sf::Keyboard::W) {
            if (menuIndex > 0) menuIndex--;
        }
        if (key == sf::Keyboard::Down || key == sf::Keyboard::S) {
            if (menuIndex < 2) menuIndex++;
        }
        if (key == sf::Keyboard::Enter) {
            if (menuIndex == 0) { // Play
                resetGame();
                state = SnakeGameState::Playing;
            }
            else if (menuIndex == 1) { // Instructions
                state = SnakeGameState::Instructions;
            }
            else if (menuIndex == 2) { // Exit
                closeRequested = true;
            }
        }
        if (key == sf::Keyboard::Escape) {
            closeRequested = true;
        }
    }
    else if (state == SnakeGameState::Instructions) {
        if (key == sf::Keyboard::Escape || key == sf::Keyboard::Enter)
            state = SnakeGameState::MainMenu;
    }
    else if (state == SnakeGameState::Playing) {
        // Directional keys
        if ((key == sf::Keyboard::Up || key == sf::Keyboard::W) && direction.y != 1)
            nextDirection = { 0, -1 };
        else if ((key == sf::Keyboard::Down || key == sf::Keyboard::S) && direction.y != -1)
            nextDirection = { 0, 1 };
        else if ((key == sf::Keyboard::Left || key == sf::Keyboard::A) && direction.x != 1)
            nextDirection = { -1, 0 };
        else if ((key == sf::Keyboard::Right || key == sf::Keyboard::D) && direction.x != -1)
            nextDirection = { 1, 0 };

        if (key == sf::Keyboard::Escape) {
            state = SnakeGameState::Paused;
            pauseIndex = 0;
        }
    }
    else if (state == SnakeGameState::Paused) {
        if (key == sf::Keyboard::Left || key == sf::Keyboard::A) {
            pauseIndex = 0;
        }
        else if (key == sf::Keyboard::Right || key == sf::Keyboard::D) {
            pauseIndex = 1;
        }
        else if (key == sf::Keyboard::Enter) {
            if (pauseIndex == 0) { // Resume
                state = SnakeGameState::Playing;
            }
            else { // Exit
                state = SnakeGameState::MainMenu;
            }
        }
        if (key == sf::Keyboard::Escape) {
            state = SnakeGameState::Playing;
        }
    }
}

void SnakeGame::render(sf::RenderWindow& window) {
    if (state == SnakeGameState::MainMenu) {
        drawMenu(window);
    }
    else if (state == SnakeGameState::Playing) {
        drawGame(window);
    }
    else if (state == SnakeGameState::Paused) {
        drawGame(window);
        drawPause(window);
    }
    else if (state == SnakeGameState::Instructions) {
        drawInstructions(window);
    }
}

void SnakeGame::drawMenu(sf::RenderWindow& window) {
    sf::RectangleShape bg(sf::Vector2f(gridWidth * tileSize + 200, gridHeight * tileSize + 100));
    bg.setFillColor(sf::Color(80, 0, 120, 230));
    bg.setPosition(60, 60);
    window.draw(bg);

    sf::Text title("Snake Minigame", font, 48);
    title.setFillColor(sf::Color(255, 255, 0));
    title.setPosition(120, 90);
    window.draw(title);

    std::string options[] = { "Play", "Instructions", "Exit" };
    for (int i = 0; i < 3; ++i) {
        sf::Text t(options[i], font, 32);
        t.setPosition(160, 180 + i * 50);
        t.setFillColor(menuIndex == i ? sf::Color(255, 255, 40) : sf::Color(200, 160, 255));
        window.draw(t);
    }

    if (gameOver) {
        sf::Text over("Game Over! Coins earned: " + std::to_string(coinsEarned), font, 28);
        over.setFillColor(sf::Color(255, 80, 80));
        over.setPosition(120, 350);
        window.draw(over);
    }
}

void SnakeGame::drawGame(sf::RenderWindow& window) {
    // Draw play area
    sf::RectangleShape bg(sf::Vector2f(gridWidth * tileSize, gridHeight * tileSize));
    bg.setFillColor(sf::Color(30, 0, 80));
    bg.setPosition(100, 100);
    window.draw(bg);

    // Draw food
    sf::RectangleShape foodRect(sf::Vector2f(tileSize - 2, tileSize - 2));
    foodRect.setFillColor(sf::Color::Red);
    foodRect.setPosition(100 + food.x * tileSize + 1, 100 + food.y * tileSize + 1);
    window.draw(foodRect);

    // Draw snake
    for (size_t i = 0; i < snake.size(); ++i) {
        sf::RectangleShape part(sf::Vector2f(tileSize - 4, tileSize - 4));
        part.setPosition(100 + snake[i].x * tileSize + 2, 100 + snake[i].y * tileSize + 2);
        part.setFillColor(i == 0 ? sf::Color(255, 255, 60) : sf::Color(150, 255, 100));
        window.draw(part);
    }

    // Score
    sf::Text scoreText("Score: " + std::to_string(score), font, 22);
    scoreText.setFillColor(sf::Color(255, 255, 180));
    scoreText.setPosition(100, 70);
    window.draw(scoreText);
}

void SnakeGame::drawPause(sf::RenderWindow& window) {
    sf::RectangleShape rect(sf::Vector2f(300, 120));
    rect.setFillColor(sf::Color(120, 0, 80, 230));
    rect.setOutlineColor(sf::Color::Yellow);
    rect.setOutlineThickness(2);
    rect.setPosition(180, 250);
    window.draw(rect);

    sf::Text pauseText("Paused", font, 32);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setPosition(310, 260);
    window.draw(pauseText);

    std::string opts[] = { "Resume", "Exit" };
    for (int i = 0; i < 2; ++i) {
        sf::Text t(opts[i], font, 28);
        t.setPosition(220 + i * 130, 310);
        t.setFillColor(i == pauseIndex ? sf::Color(230, 200, 40) : sf::Color(180, 80, 200));
        window.draw(t);
    }
}

void SnakeGame::drawInstructions(sf::RenderWindow& window) {
    sf::RectangleShape bg(sf::Vector2f(520, 370));
    bg.setFillColor(sf::Color(120, 0, 80, 230));
    bg.setPosition(80, 80);
    window.draw(bg);

    sf::Text title("How to Play Snake", font, 38);
    title.setFillColor(sf::Color(255, 255, 0));
    title.setPosition(120, 100);
    window.draw(title);

    std::string msg =
        "Control the snake with arrow keys or WASD.\n"
        "Eat the red food. Each food gives you 2 coins.\n"
        "Avoid hitting walls and your own body.\n"
        "Game ends on collision.\n\n"
        "Press ESC anytime to pause.\n"
        "Press ESC or Enter here to go back.";

    sf::Text info(msg, font, 22);
    info.setFillColor(sf::Color(255, 255, 255));
    info.setPosition(100, 170);
    window.draw(info);
}
