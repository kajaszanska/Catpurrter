#include "CatchGame.h"
#include <random>
#include <iostream>
#include <sstream>
// Im using <thread> to save player data in a separate thread after game over
#include <thread>


CatchGame::CatchGame(const sf::Font& font, Player& player, GameManager& gm)
    : font(font), player(player), gameManager(gm)
{
    init();
}

void CatchGame::init() {
    state = CatchGameState::MainMenu;
    menuIndex = 0;
    pauseIndex = 0;
    gameOverIndex = 0;
    closeRequested = false;
    coinsAdded = false;
    resetGame();
}

void CatchGame::resetGame() {
    score = 0;
    lives = 2;
    drops.clear();
    playerRect.setSize(sf::Vector2f(70, 25));
    playerRect.setFillColor(sf::Color(200, 200, 255));
    playerRect.setOrigin(35, 12.5f);
    playerRect.setPosition(400, 530);
    spawnTimer = 0.f;
    spawnDelay = 1.3f;
    fallSpeed = 180.f;
}


void CatchGame::spawnDrop() {
    static std::mt19937 rng((unsigned)time(nullptr));
    static std::uniform_real_distribution<float> xpos(120.f, 680.f);
    static std::uniform_int_distribution<int> type(0, 3);

    Drop drop;
    drop.shape.setSize(sf::Vector2f(30, 30));
    drop.shape.setOrigin(15, 15);
    drop.shape.setPosition(xpos(rng), 80);
    int t = type(rng);
       
    if (t == 0) { drop.shape.setFillColor(sf::Color(70, 255, 120)); drop.good = true; }
    else if (t == 1) { drop.shape.setFillColor(sf::Color(90, 210, 255)); drop.good = true; }
    else if (t == 2) { drop.shape.setFillColor(sf::Color(220, 60, 60)); drop.good = false; }
    else { drop.shape.setFillColor(sf::Color(20, 20, 20)); drop.good = false; }
    drops.push_back(drop);
}

void CatchGame::update(float dt) {
    if (state != CatchGameState::Playing) return;

    const float moveSpeed = 1000.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        playerRect.move(-moveSpeed * dt, 0);
        if (playerRect.getPosition().x < 120) playerRect.setPosition(120, playerRect.getPosition().y);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        playerRect.move(moveSpeed * dt, 0);
        if (playerRect.getPosition().x > 680) playerRect.setPosition(680, playerRect.getPosition().y);
    }

    // Spawning drops
    spawnTimer += dt;
    if (spawnTimer >= spawnDelay) {
        spawnTimer = 0.f;
        spawnDrop();
    }

    // Drops movement/collision
    for (auto& drop : drops) {
        drop.shape.move(0, fallSpeed * dt);
    }

    auto it = drops.begin();
    while (it != drops.end()) {
        if (it->shape.getGlobalBounds().intersects(playerRect.getGlobalBounds())) {
            if (it->good) {
                score++;
                coinsEarned += 1;
                if (score % 7 == 0 && fallSpeed < 350.f)
                    fallSpeed += 10.f;
            }
            else {
                lives--;
                score = std::max(0, score - 1);
            }
            it = drops.erase(it);
        }
        else if (it->shape.getPosition().y > 590) {
            if (it->good) {
                lives--;
            }
            it = drops.erase(it);
        }
        else {
            ++it;
        }
    }

    float baseDelay = 1.3f;
    float minDelay = 0.25f;
    spawnDelay = std::max(baseDelay - score * 0.021f, minDelay);

    if (fallSpeed > 400.f) fallSpeed = 400.f;

    if (score < 0) score = 0;
    if (lives < 0) {
        coinsEarned = std::max(0, score);
        state = CatchGameState::GameOver;
    }
}

void CatchGame::handleInput(sf::Keyboard::Key key) {
    if (state == CatchGameState::MainMenu) {
        if (key == sf::Keyboard::Up || key == sf::Keyboard::W) { if (menuIndex > 0) menuIndex--; }
        if (key == sf::Keyboard::Down || key == sf::Keyboard::S) { if (menuIndex < 2) menuIndex++; }
        if (key == sf::Keyboard::Enter) {
            if (menuIndex == 0) {
                coinsEarned = 0;
                resetGame();
                state = CatchGameState::Playing;
            }
            else if (menuIndex == 1) { state = CatchGameState::Instructions; }
            else if (menuIndex == 2) { closeRequested = true; }
        }
        if (key == sf::Keyboard::Escape) closeRequested = true;
    }
    else if (state == CatchGameState::Instructions) {
        if (key == sf::Keyboard::Escape || key == sf::Keyboard::Enter) state = CatchGameState::MainMenu;
    }
    else if (state == CatchGameState::Playing) {
        if (key == sf::Keyboard::Escape) { state = CatchGameState::Paused; pauseIndex = 0; }
    }
    else if (state == CatchGameState::Paused) {
        if (key == sf::Keyboard::Left || key == sf::Keyboard::A) pauseIndex = 0;
        else if (key == sf::Keyboard::Right || key == sf::Keyboard::D) pauseIndex = 1;
        else if (key == sf::Keyboard::Enter) {
            if (pauseIndex == 0) state = CatchGameState::Playing;
            else state = CatchGameState::MainMenu;
        }
        if (key == sf::Keyboard::Escape) state = CatchGameState::Playing;
    }
    else if (state == CatchGameState::GameOver) {
        if (!coinsAdded) {
            player.coins += coinsEarned;
            std::thread saveThread([&]() {
                player.saveToFile("saves/save.json");
                });
            saveThread.detach();
            coinsAdded = true;
        }


        if (key == sf::Keyboard::Left || key == sf::Keyboard::A) gameOverIndex = 0;
        else if (key == sf::Keyboard::Right || key == sf::Keyboard::D) gameOverIndex = 1;
        else if (key == sf::Keyboard::Enter) {
            if (gameOverIndex == 0) { resetGame(); state = CatchGameState::Playing; coinsAdded = false; }
            else { state = CatchGameState::MainMenu; menuIndex = 0; coinsAdded = false; }
        }
        if (key == sf::Keyboard::Escape) { state = CatchGameState::MainMenu; menuIndex = 0; coinsAdded = false; }
    }
}


void CatchGame::render(sf::RenderWindow& window) {
    if (state == CatchGameState::MainMenu) drawMenu(window);
    else if (state == CatchGameState::Playing) drawGame(window);
    else if (state == CatchGameState::Paused) { drawGame(window); drawPause(window); }
    else if (state == CatchGameState::Instructions) drawInstructions(window);
    else if (state == CatchGameState::GameOver) drawGameOver(window);
}


void CatchGame::drawMenu(sf::RenderWindow& window) {
    float menuWidth = 700.f;
    float menuHeight = 480.f;
    float centerX = 60 + menuWidth / 2.0f;
    float centerY = 60 + menuHeight / 2.0f;

    sf::RectangleShape bg(sf::Vector2f(menuWidth, menuHeight));
    bg.setFillColor(sf::Color(80, 0, 120, 230));
    bg.setPosition(60, 60);
    window.draw(bg);

    sf::Text title("Catch Game", font, 48);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);

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
}

void CatchGame::drawGame(sf::RenderWindow& window) {

    window.clear(sf::Color(80, 0, 120));

    // Play area 
    sf::RectangleShape bg(sf::Vector2f(560.f, 420.f));
    bg.setFillColor(sf::Color(30, 0, 80));
    bg.setPosition(120, 80);
    window.draw(bg);

    // Drops
    for (const auto& drop : drops) {
        sf::RectangleShape d = drop.shape;
        d.move(0, 0); 
        window.draw(d);
    }
    // Player
    sf::RectangleShape playerBox = playerRect;
    playerBox.move(0, 0); 
    window.draw(playerBox);

    sf::Text scoreText("Score: " + std::to_string(score), font, 32);
    scoreText.setFillColor(sf::Color(255, 220, 60));
    scoreText.setPosition(120, 40);
    window.draw(scoreText);

    sf::Text livesText("Lives: " + std::to_string(lives), font, 32);
    livesText.setFillColor(sf::Color::Cyan);
    livesText.setPosition(400, 40);
    window.draw(livesText);
}


void CatchGame::drawPause(sf::RenderWindow& window) {
    sf::Vector2f rectSize(300, 120);
    sf::Vector2f rectPos(250, 220);

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

void CatchGame::drawInstructions(sf::RenderWindow& window) {
    float popupWidth = 700.f;
    float popupHeight = 440.f;
    float centerX = 60 + popupWidth / 2.0f;
    float centerY = 60 + popupHeight / 2.0f;

    sf::RectangleShape bg(sf::Vector2f(popupWidth, popupHeight));
    bg.setFillColor(sf::Color(80, 0, 120, 230));
    bg.setPosition(60, 60);
    window.draw(bg);

    sf::Text title("How to Play Catch", font, 38);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
    float titleY = centerY - 90;
    title.setPosition(centerX, titleY);
    title.setFillColor(sf::Color::Cyan);
    window.draw(title);

    std::string msg =
        "Move left/right with arrow keys or A/D.\n"
        "Catch GREEN and CYAN drops for points, 1 point = 1 coin!\n"
        "Avoid RED and BLACK drops - they cost a life and 1 points.\n"
        "If you miss GREEN and CYAN drops you also get -life.\n"
        "Game ends when you get -life, when you have none left.\n"
        "Press ESC anytime to pause the game.\n\n"
        "Press ESC or Enter to return to menu.";
    sf::Text info(msg, font, 22);
    sf::FloatRect infoBounds = info.getLocalBounds();
    info.setOrigin(infoBounds.left + infoBounds.width / 2.0f, 0);
    float infoY = titleY + titleBounds.height / 2.f + 30.f;
    info.setPosition(centerX, infoY);
    info.setFillColor(sf::Color::White);
    window.draw(info);
}

void CatchGame::drawGameOver(sf::RenderWindow& window) {
    float popupWidth = 700.f;
    float popupHeight = 440.f;
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
