#include "SnakeGame.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
// Im using <thread> to save player data in a separate thread after game over
#include <thread>


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
        state = SnakeGameState::GameOver; 
        coinsAdded = false; 
        return;
    }
    // Check self-collision
    if (std::find(snake.begin(), snake.end(), newHead) != snake.end()) {
        gameOver = true;
        gameFinished = true;
        coinsEarned = score * 2;
        state = SnakeGameState::GameOver;
        coinsAdded = false;
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
        constexpr int numOptions = 3; 
        if (key == sf::Keyboard::Up || key == sf::Keyboard::W) {
            if (menuIndex > 0) menuIndex--;
        }
        if (key == sf::Keyboard::Down || key == sf::Keyboard::S) {
            if (menuIndex < numOptions - 1) menuIndex++;
        }
        if (key == sf::Keyboard::Enter) {
            if (menuIndex == 0) {  
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
        if (key == sf::Keyboard::Escape || key == sf::Keyboard::Enter) {
            state = SnakeGameState::MainMenu;
            menuIndex = 0; 
        }
    }
    else if (state == SnakeGameState::Playing) {  
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
    else if (state == SnakeGameState::GameOver) {
        if (!coinsAdded) {
            player.coins += coinsEarned; 
            std::thread saveThread([&]() {
                player.saveToFile("saves/save.json");
                });
            saveThread.detach();
            coinsAdded = true;
        }
        if (key == sf::Keyboard::Left || key == sf::Keyboard::A) {
            gameOverIndex = 0;
        }
        else if (key == sf::Keyboard::Right || key == sf::Keyboard::D) {
            gameOverIndex = 1;
        }
        else if (key == sf::Keyboard::Enter) {
            if (gameOverIndex == 0) { // Restart
                resetGame();
                state = SnakeGameState::Playing;
                coinsAdded = false;
            }
            else { // Back to menu
                state = SnakeGameState::MainMenu;
                menuIndex = 0;
                gameOver = false;
                coinsAdded = false;
            }
        }
        if (key == sf::Keyboard::Escape) {
            state = SnakeGameState::MainMenu;
            menuIndex = 0;
            gameOver = false;
            coinsAdded = false;
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
    else if (state == SnakeGameState::GameOver) {
        drawGameOver(window);
    }

}

void SnakeGame::drawMenu(sf::RenderWindow& window) {
    float menuWidth = static_cast<float>(gridWidth) * static_cast<float>(tileSize) + 200.f;
    float menuHeight = static_cast<float>(gridHeight) * static_cast<float>(tileSize) + 100.f;

    float centerX = 60 + menuWidth / 2.0f;
    float centerY = 60 + menuHeight / 2.0f;

    sf::RectangleShape bg(sf::Vector2f(menuWidth, menuHeight));
    bg.setFillColor(sf::Color(80, 0, 120, 230));
    bg.setPosition(60, 60);
    window.draw(bg);

    // Title
    sf::Text title("Snake Minigame", font, 48);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);

    // Menu options
    std::string options[] = { "Play", "Instructions", "Exit" };
    int numOptions = 3;
    float optionSpacing = 14.f; 
    float optionFontSize = 32.f;

    float totalMenuHeight = titleBounds.height;
    std::vector<sf::FloatRect> optionBounds(numOptions);

    for (int i = 0; i < numOptions; ++i) {
        sf::Text temp(options[i], font, (unsigned)optionFontSize);
        optionBounds[i] = temp.getLocalBounds();
        totalMenuHeight += optionBounds[i].height;
        if (i > 0) totalMenuHeight += optionSpacing;
    }
 
    float titleToOption = 28.f; 
    totalMenuHeight += titleToOption;

    float startY = centerY - totalMenuHeight / 2.0f;

    // Draw title
    title.setPosition(centerX, startY + titleBounds.height / 2.0f);
    title.setFillColor(sf::Color::Cyan);
    window.draw(title);

    float currY = startY + titleBounds.height + titleToOption;
    for (int i = 0; i < numOptions; ++i) {
        sf::Text t(options[i], font, (unsigned)optionFontSize);
        sf::FloatRect tBounds = t.getLocalBounds();
        t.setOrigin(tBounds.left + tBounds.width / 2.0f, tBounds.top + tBounds.height / 2.0f);
        t.setPosition(centerX, currY + tBounds.height / 2.0f);
        t.setFillColor(menuIndex == i ? sf::Color(255, 255, 40) : sf::Color(200, 160, 255));
        window.draw(t);

        currY += tBounds.height + optionSpacing;
    }

    // Game Over message
    if (gameOver) {
        float popupWidth = static_cast<float>(gridWidth * tileSize) + 200.f;
        float popupHeight = static_cast<float>(gridHeight * tileSize) + 100.f;

        float centerX = 60 + popupWidth / 2.0f;
        float centerY = 60 + popupHeight / 2.0f;

        sf::Text over("Game Over! Coins earned: " + std::to_string(coinsEarned), font, 28);
        sf::FloatRect overBounds = over.getLocalBounds();
        over.setOrigin(overBounds.left + overBounds.width / 2.f, overBounds.top + overBounds.height / 2.f);
        over.setPosition(centerX, centerY);
        over.setFillColor(sf::Color::White); 
        window.draw(over);
    }

}


void SnakeGame::drawGame(sf::RenderWindow& window) {   
    window.clear(sf::Color(80, 0, 120));  

    // play area 
    sf::RectangleShape bg(sf::Vector2f(
        static_cast<float>(gridWidth * tileSize),
        static_cast<float>(gridHeight * tileSize)
    ));

    bg.setFillColor(sf::Color(30, 0, 80));
    bg.setPosition(100, 100);
    window.draw(bg);

    // food
    sf::RectangleShape foodRect(sf::Vector2f(
        static_cast<float>(tileSize - 2),
        static_cast<float>(tileSize - 2)
    ));

    foodRect.setFillColor(sf::Color::Red);
    foodRect.setPosition(
        static_cast<float>(100 + food.x * tileSize + 1),
        static_cast<float>(100 + food.y * tileSize + 1)
    );

    window.draw(foodRect);

    //  snake
    for (size_t i = 0; i < snake.size(); ++i) {
        sf::RectangleShape part(sf::Vector2f(static_cast<float>(tileSize - 4), static_cast<float>(tileSize - 4)));
        part.setPosition(
            static_cast<float>(100 + snake[i].x * tileSize + 2),
            static_cast<float>(100 + snake[i].y * tileSize + 2)
        );
        part.setFillColor(i == 0 ? sf::Color(255, 255, 60) : sf::Color(150, 255, 100));
        window.draw(part);
    }

    // score
    sf::Text scoreText("Score: " + std::to_string(score), font, 32);
    scoreText.setFillColor(sf::Color(255, 220, 60));  
    scoreText.setPosition(100, 60);
    window.draw(scoreText);
}


void SnakeGame::drawPause(sf::RenderWindow& window) {
    sf::Vector2f rectSize(300, 120);
    sf::Vector2f rectPos(180, 250);

    sf::RectangleShape rect(rectSize);
    rect.setFillColor(sf::Color(120, 0, 80, 230));
    rect.setOutlineColor(sf::Color::Yellow);
    rect.setOutlineThickness(2);
    rect.setPosition(rectPos);
    window.draw(rect);

    sf::Text pauseText("Paused", font, 32);
    sf::FloatRect pauseBounds = pauseText.getLocalBounds();
    pauseText.setOrigin(pauseBounds.left + pauseBounds.width / 2.0f, pauseBounds.top + pauseBounds.height / 2.0f);
    pauseText.setPosition(rectPos.x + rectSize.x / 2.0f, rectPos.y + 30);
    pauseText.setFillColor(sf::Color::White);
    window.draw(pauseText);

    std::string opts[] = { "Resume", "Exit" };
    for (int i = 0; i < 2; ++i) {
        sf::Text t(opts[i], font, 28);
        sf::FloatRect tBounds = t.getLocalBounds();
        t.setOrigin(tBounds.left + tBounds.width / 2.0f, tBounds.top + tBounds.height / 2.0f);
       
        t.setPosition(rectPos.x + rectSize.x / 4.0f + i * rectSize.x / 2.0f, rectPos.y + 80);
        t.setFillColor(i == pauseIndex ? sf::Color(230, 200, 40) : sf::Color(180, 80, 200));
        window.draw(t);
    }
}


void SnakeGame::drawInstructions(sf::RenderWindow& window) {
    float popupWidth = static_cast<float>(gridWidth) * static_cast<float>(tileSize) + 200.f;
    float popupHeight = static_cast<float>(gridHeight) * static_cast<float>(tileSize) + 100.f;

    float centerX = 60 + popupWidth / 2.0f;
    float centerY = 60 + popupHeight / 2.0f;

   
    sf::RectangleShape bg(sf::Vector2f(popupWidth, popupHeight));
    bg.setFillColor(sf::Color(80, 0, 120, 230));
    bg.setPosition(60, 60);
    window.draw(bg);
      
    sf::Text title("How to Play Snake", font, 38);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
    float titleY = centerY - 90;
    title.setPosition(centerX, titleY);
    title.setFillColor(sf::Color::Cyan);
    window.draw(title);

    // Instruction
    std::string msg =
        "Control the snake with arrow keys or WASD.\n"
        "Eat the red food. Each food gives you 2 coins.\n"
        "Avoid hitting walls and your own body.\n"
        "Game ends on collision.\n"
        "Press ESC anytime to pause the game.\n\n"
        "Press ESC or Enter here to go back.";

    sf::Text info(msg, font, 22);
    sf::FloatRect infoBounds = info.getLocalBounds();
    info.setOrigin(infoBounds.left + infoBounds.width / 2.0f, 0);
    float infoY = titleY + titleBounds.height / 2.f + 30.f;
    info.setPosition(centerX, infoY);
    info.setFillColor(sf::Color::White);
    window.draw(info);
}


void SnakeGame::drawGameOver(sf::RenderWindow& window) {
    float popupWidth = static_cast<float>(gridWidth) * static_cast<float>(tileSize) + 200.f;
    float popupHeight = static_cast<float>(gridHeight) * static_cast<float>(tileSize) + 100.f;

    float centerX = 60 + popupWidth / 2.0f;
    float centerY = 60 + popupHeight / 2.0f;

    sf::RectangleShape bg(sf::Vector2f(popupWidth, popupHeight));
    bg.setFillColor(sf::Color(60, 0, 30, 230));
    bg.setPosition(60, 60);
    window.draw(bg);
 
    sf::Text over("Game Over! Coins earned: " + std::to_string(coinsEarned), font, 36);
    sf::FloatRect overBounds = over.getLocalBounds();
    over.setOrigin(overBounds.left + overBounds.width / 2.f, overBounds.top + overBounds.height / 2.f);
    over.setPosition(centerX, centerY - 60); 
    over.setFillColor(sf::Color(255, 80, 80));
    window.draw(over);

    std::string opts[] = { "Restart", "Back to Menu" };
    sf::Text left(opts[0], font, 28);
    sf::Text right(opts[1], font, 28);

    sf::FloatRect leftBounds = left.getLocalBounds();
    sf::FloatRect rightBounds = right.getLocalBounds();

    float desiredGap = 50.f;

    float totalWidth = leftBounds.width + rightBounds.width + desiredGap;

    float leftX = centerX - totalWidth / 2.f + leftBounds.width / 2.f;
    float rightX = centerX + totalWidth / 2.f - rightBounds.width / 2.f;
    float y = centerY + 10.f;

    left.setOrigin(leftBounds.left + leftBounds.width / 2.f, leftBounds.top + leftBounds.height / 2.f);
    left.setPosition(leftX, y);
    left.setFillColor(gameOverIndex == 0 ? sf::Color(200, 170, 40) : sf::Color::White);
    window.draw(left);

    right.setOrigin(rightBounds.left + rightBounds.width / 2.f, rightBounds.top + rightBounds.height / 2.f);
    right.setPosition(rightX, y);
    right.setFillColor(gameOverIndex == 1 ? sf::Color(200, 170, 40) : sf::Color::White);
    window.draw(right);
}



