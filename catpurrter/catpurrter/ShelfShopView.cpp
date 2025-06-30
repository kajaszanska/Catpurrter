#include "ShelfShopView.h"
#include "GameManager.h"

#include <iostream>
// Im implementing <ranges> to replace any use of std::find with std::ranges::find
#include <ranges> 
// Im using <thread> to save player data in a separate thread after purchase
#include <thread>


ShelfShopView::ShelfShopView(sf::Font& font, Player& player, GameManager& gm)
    : font(font), playerData(player), gameManager(&gm), selectedIndex(0) {

    decorations = {
        {"car",     "Red Toy Car", 20},
        {"books",   "Very Interesting Books",  30},
        {"plant",   "Dull Plant in Pot ",  50},
        {"picture", "Picture of Cool Cat", 40}
    };

    float y = 150.f;
    for (const auto& deco : decorations) {
        sf::Text option;
        option.setFont(font);
        option.setCharacterSize(28);
        option.setPosition(100.f, y);
        decorationOptions.push_back(option);
        y += 50.f;
    }

    updateOptionColors();
}

void ShelfShopView::init() {
    decorationOptions.clear();
    float y = 150.f;

    for (const auto& deco : decorations) {
        sf::Text option;
        option.setFont(font);
        option.setCharacterSize(28);
        option.setPosition(100.f, y);
        decorationOptions.push_back(option);
        y += 50.f;
    }

    updateOptionColors();
}

void ShelfShopView::handleInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        shouldExit = true;
        return;
    }

    if (key == sf::Keyboard::Up || key == sf::Keyboard::W) {
        if (selectedIndex > 0) selectedIndex--;
    }
    else if (key == sf::Keyboard::Down || key == sf::Keyboard::S) {
        if (selectedIndex < static_cast<int>(decorations.size()) - 1) selectedIndex++;
    }
    else if (key == sf::Keyboard::Enter) {
        const std::string& selectedId = std::get<0>(decorations[selectedIndex]);
        int price = std::get<2>(decorations[selectedIndex]);

        bool alreadyOwned = std::ranges::find(playerData.ownedDecorations, selectedId) != playerData.ownedDecorations.end();

        if (alreadyOwned) {
            std::cout << "Already owned: " << selectedId << "\n";
        }
        else if (playerData.coins >= price) {
            playerData.coins -= price;
            playerData.ownedDecorations.push_back(selectedId);
            std::thread saveThread([&]() {
                playerData.saveToFile("saves/save.json");
                });
            saveThread.detach();
            std::cout << "Bought decoration: " << selectedId << "\n";
        }
        else {
            std::cout << "Not enough coins\n";
        }
    }

    updateOptionColors();
}


void ShelfShopView::updateOptionColors() {
    for (size_t i = 0; i < decorations.size(); ++i) {
        const auto& [id, label, price] = decorations[i];

        bool owned = std::ranges::find(playerData.ownedDecorations, id) != playerData.ownedDecorations.end();

        std::string display = label + " - " + std::to_string(price) + " coins";
        if (owned)
            display += " (Owned)";

        decorationOptions[i].setString(display);
        decorationOptions[i].setFillColor(i == selectedIndex ? sf::Color::Yellow : sf::Color::White);
    }
}

void ShelfShopView::render(sf::RenderWindow& window) {
    sf::RectangleShape bg(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));

    bg.setFillColor(sf::Color(120, 60, 200)); 
    window.draw(bg);

    gameManager->drawSectionTitle(window, font, "Shelf Shop");
    gameManager->drawCoinDisplay(window, font, playerData.coins);

    for (const auto& opt : decorationOptions)
        window.draw(opt);
}

bool ShelfShopView::shouldClose() const {
    return shouldExit;
}

void ShelfShopView::resetCloseFlag() {
    shouldExit = false;
}
