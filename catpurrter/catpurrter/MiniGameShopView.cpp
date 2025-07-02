#include "MiniGameShopView.h"
#include "GameManager.h"

#include <iostream>
#include <algorithm>
// Im implementing <ranges> to replace any use of std::find with std::ranges::find
#include <ranges> 
// Im using <thread> to save player data in a separate thread after purchase
#include <thread>

MiniGameShopView::MiniGameShopView(sf::Font& font, Player& player, GameManager* gm)
    : ShopViewBase(font, player, gm)
{
    games = {
        { "snake", "Snake", 100 },
        {"catch", "Catch Game", 150 },
        { "dodge", "Dodge Game", 120 }
    };

    init();
}

void MiniGameShopView::init() {
    gameOptions.clear();
    float y = 150.f;

    for (const auto& [id, name, price] : games) {
        sf::Text option;
        option.setFont(font);
        option.setCharacterSize(28);
        option.setPosition(100.f, y);
        gameOptions.push_back(option);
        y += 50.f;
    }

    updateOptionColors();
}

void MiniGameShopView::updateOptionColors() {
    for (size_t i = 0; i < games.size(); ++i) {
        const auto& [id, name, price] = games[i];
        bool owned = std::ranges::find(playerData.ownedMiniGames, id) != playerData.ownedMiniGames.end();

        std::string label = name + " - " + std::to_string(price) + " coins";
        if (owned) label += " (Owned)";

        gameOptions[i].setString(label);
        gameOptions[i].setFillColor(i == selectedIndex ? sf::Color::Yellow : sf::Color::White);
    }
}

void MiniGameShopView::handleInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        closeFlag = true;
        return;
    }


    if (key == sf::Keyboard::Up || key == sf::Keyboard::W) {
        if (selectedIndex > 0) selectedIndex--;
    }
    else if (key == sf::Keyboard::Down || key == sf::Keyboard::S) {
        if (selectedIndex < static_cast<int>(games.size()) - 1) selectedIndex++;
    }
    else if (key == sf::Keyboard::Enter) {
        const auto& [id, label, price] = games[selectedIndex];

        bool owned = std::ranges::find(playerData.ownedMiniGames, id) != playerData.ownedMiniGames.end();

        if (owned) {
            std::cout << "Already owned: " << id << "\n";
        }
        else if (playerData.coins >= price) {
            playerData.coins -= price;
            playerData.ownedMiniGames.push_back(id);
            std::thread saveThread([&]() {
                playerData.saveToFile("saves/save.json");
                });
            saveThread.detach();
            std::cout << "Bought mini game: " << id << "\n";
            updateOptionColors();
        }
        else {
            std::cout << "Not enough coins\n";
        }
    }

    updateOptionColors();
}


void MiniGameShopView::render(sf::RenderWindow& window) {
    sf::RectangleShape bg(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
    bg.setFillColor(sf::Color(120, 60, 200));
    window.draw(bg);

    gameManager->drawSectionTitle(window, font, "Mini Game Shop");
    gameManager->drawCoinDisplay(window, font, playerData.coins);

    for (const auto& option : gameOptions) {
        window.draw(option);
    }
}


bool MiniGameShopView::shouldClose() const {
    return closeFlag;
}

void MiniGameShopView::resetCloseFlag() {
    closeFlag = false;
}

