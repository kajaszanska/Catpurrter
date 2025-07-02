#include "DodgeGame.h"
#include <random>
#include <cmath>
// Im using <thread> to save player data in a separate thread after game over
#include <thread>


DodgeGame::DodgeGame(const sf::Font& font, Player& player, GameManager& gm)
    : font(font), player(player), gameManager(gm)
{
    init();
}

void DodgeGame::init() {
    state = DodgeGameState::MainMenu;
    menuIndex = 0;
    pauseIndex = 0;
    gameOverIndex = 0;
    closeRequested = false;
    coinsAdded = false;
    resetGame();
}

void DodgeGame::resetGame() {
    score = 0;
    coinsEarned = 0;
    lives = 2;
    drops.clear();
    playerRect.setSize(sf::Vector2f(40, 40));
    playerRect.setFillColor(sf::Color(200, 255, 200));
    playerRect.setOrigin(20, 20);
    playerRect.setPosition(400, 340); 
    spawnTimer = 0.f;
    spawnDelay = 1.0f;
    dropSpeed = 220.f;
}

void DodgeGame::spawnDrop() {
    static std::mt19937 rng((unsigned)time(nullptr));
    static std::uniform_int_distribution<int> edgeDist(0, 3); 
    static std::uniform_real_distribution<float> wX(120.f, 680.f);
    static std::uniform_real_distribution<float> hY(80.f, 600.f);

    int edge = edgeDist(rng);
    sf::Vector2f pos, vel;
    float speed = dropSpeed;

    if (edge == 0) { // top
        pos = { wX(rng), 80.f };
        vel = { 0.f, speed };
    }
    else if (edge == 1) { // right
        pos = { 680.f, hY(rng) };
        vel = { -speed, 0.f };
    }
    else if (edge == 2) { // bottom
        pos = { wX(rng), 600.f };
        vel = { 0.f, -speed };
    }
    else { // left
        pos = { 120.f, hY(rng) };
        vel = { speed, 0.f };
    }

    DodgeDrop drop;
    drop.shape.setRadius(18.f);
    drop.shape.setOrigin(18, 18);
    drop.shape.setPosition(pos);
    drop.shape.setFillColor(sf::Color(230, 40, 40));
    drop.velocity = vel;
    drops.push_back(drop);
}

void DodgeGame::update(float dt) {
    if (state != DodgeGameState::Playing) return;

    const float moveSpeed = 370.f;
    sf::Vector2f move(0, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        move.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        move.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        move.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        move.y += 1.f;
    if (move.x != 0 || move.y != 0) {
        float len = std::sqrt(move.x * move.x + move.y * move.y);
        move /= len; 
        playerRect.move(move * moveSpeed * dt);
    }
  
    auto pos = playerRect.getPosition();
    pos.x = std::max(120.f, std::min(680.f, pos.x));
    pos.y = std::max(80.f, std::min(600.f, pos.y));
    playerRect.setPosition(pos);

    // Spawning drops
    spawnTimer += dt;
    if (spawnTimer >= spawnDelay) {
        spawnTimer = 0.f;
        spawnDrop();
    }

    // Update drops
    for (auto& drop : drops) {
        drop.shape.move(drop.velocity * dt);
    }

    // Check collisions and count scores
    auto it = drops.begin();
    while (it != drops.end()) {
        // Out of arena: reward player
        sf::Vector2f dpos = it->shape.getPosition();
        bool gone = (dpos.x < 110.f || dpos.x > 690.f || dpos.y < 70.f || dpos.y > 610.f);
        if (gone) {
            score++;
            if (score % 4 == 0) coinsEarned++;
            if (score % 7 == 0 && dropSpeed < 400.f) dropSpeed += 16.f;
            it = drops.erase(it);
        }
        // Collision: lose life
        else if (it->shape.getGlobalBounds().intersects(playerRect.getGlobalBounds())) {
            lives--;
            it = drops.erase(it);
        }
        else {
            ++it;
        }
    }
    // Difficulty: decrease spawn delay as score increases
    float minDelay = 0.17f;
    spawnDelay = std::max(1.0f - 0.019f * score, minDelay);

    if (lives < 0) state = DodgeGameState::GameOver;
}

void DodgeGame::handleInput(sf::Keyboard::Key key) {
    if (state == DodgeGameState::MainMenu) {
        if (key == sf::Keyboard::Up || key == sf::Keyboard::W) { if (menuIndex > 0) menuIndex--; }
        if (key == sf::Keyboard::Down || key == sf::Keyboard::S) { if (menuIndex < 2) menuIndex++; }
        if (key == sf::Keyboard::Enter) {
            if (menuIndex == 0) { resetGame(); state = DodgeGameState::Playing; }
            else if (menuIndex == 1) { state = DodgeGameState::Instructions; }
            else if (menuIndex == 2) { closeRequested = true; }
        }
        if (key == sf::Keyboard::Escape) closeRequested = true;
    }
    else if (state == DodgeGameState::Instructions) {
        if (key == sf::Keyboard::Escape || key == sf::Keyboard::Enter) state = DodgeGameState::MainMenu;
    }
    else if (state == DodgeGameState::Playing) {
        if (key == sf::Keyboard::Escape) { state = DodgeGameState::Paused; pauseIndex = 0; }
    }
    else if (state == DodgeGameState::Paused) {
        if (key == sf::Keyboard::Left || key == sf::Keyboard::A) pauseIndex = 0;
        else if (key == sf::Keyboard::Right || key == sf::Keyboard::D) pauseIndex = 1;
        else if (key == sf::Keyboard::Enter) {
            if (pauseIndex == 0) state = DodgeGameState::Playing;
            else state = DodgeGameState::MainMenu;
        }
        if (key == sf::Keyboard::Escape) state = DodgeGameState::Playing;
    }
    else if (state == DodgeGameState::GameOver) {
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
            if (gameOverIndex == 0) { resetGame(); state = DodgeGameState::Playing; coinsAdded = false; }
            else { state = DodgeGameState::MainMenu; menuIndex = 0; coinsAdded = false; }
        }
        if (key == sf::Keyboard::Escape) { state = DodgeGameState::MainMenu; menuIndex = 0; coinsAdded = false; }
    }
}


void DodgeGame::render(sf::RenderWindow& window) {
    if (state == DodgeGameState::MainMenu) drawMenu(window);
    else if (state == DodgeGameState::Playing) drawGame(window);
    else if (state == DodgeGameState::Paused) { drawGame(window); drawPause(window); }
    else if (state == DodgeGameState::Instructions) drawInstructions(window);
    else if (state == DodgeGameState::GameOver) drawGameOver(window);
}

void DodgeGame::drawMenu(sf::RenderWindow& window) {
    float menuWidth = 700.f;
    float menuHeight = 480.f;
    float centerX = 60 + menuWidth / 2.0f;
    float centerY = 60 + menuHeight / 2.0f;

    sf::RectangleShape bg(sf::Vector2f(menuWidth, menuHeight));
    bg.setFillColor(sf::Color(80, 0, 120, 230));
    bg.setPosition(60, 60);
    window.draw(bg);

    sf::Text title("Dodge Game", font, 48);
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

    // Title
    title.setPosition(centerX, startY + titleBounds.height / 2.0f);
    title.setFillColor(sf::Color::Cyan);
    window.draw(title);

    // Options
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

void DodgeGame::drawGame(sf::RenderWindow& window) {
    // Backgrounds
    window.clear(sf::Color(80, 0, 120));
    sf::RectangleShape bg(sf::Vector2f(560.f, 520.f));
    bg.setFillColor(sf::Color(30, 0, 80));
    bg.setPosition(120, 80);
    window.draw(bg);

    // Drops
    for (const auto& drop : drops)
        window.draw(drop.shape);

    // Player
    window.draw(playerRect);

    // Score
    sf::Text scoreText("Score: " + std::to_string(score), font, 32);
    scoreText.setFillColor(sf::Color(255, 220, 60));
    scoreText.setPosition(120, 40);
    window.draw(scoreText);

    // Lives
    sf::Text livesText("Lives: " + std::to_string(lives), font, 32);
    livesText.setFillColor(sf::Color::Cyan);
    livesText.setPosition(400, 40);
    window.draw(livesText);
}

void DodgeGame::drawPause(sf::RenderWindow& window) {
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

void DodgeGame::drawInstructions(sf::RenderWindow& window) {
    float popupWidth = 700.f;
    float popupHeight = 440.f;
    float centerX = 60 + popupWidth / 2.0f;
    float centerY = 60 + popupHeight / 2.0f;

    sf::RectangleShape bg(sf::Vector2f(popupWidth, popupHeight));
    bg.setFillColor(sf::Color(80, 0, 120, 230));
    bg.setPosition(60, 60);
    window.draw(bg);

    sf::Text title("How to Play Dodge", font, 38);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
    float titleY = centerY - 90;
    title.setPosition(centerX, titleY);
    title.setFillColor(sf::Color::Cyan);
    window.draw(title);

    std::string msg =
        "Move in all directions with arrow keys or WASD.\n"
        "Dodge the RED circles coming from all sides!\n"
        "You have 2 lives. Touching a circle costs 1 life.\n"
        "When a drop leaves the screen, you get 1 point.\n"
        "Every 4 points = 1 coin.\n"
        "Game speeds up as your score increases.\n"
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

void DodgeGame::drawGameOver(sf::RenderWindow& window) {
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
