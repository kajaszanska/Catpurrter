#include "ShelfShopView.h"
#include "GameManager.h"
#include <iostream>

ShelfShopView::ShelfShopView(sf::Font& font, Player& player, GameManager& gm)
    : font(font), playerData(player), gameManager(&gm), selectedIndex(0) {

    decorations = {
    {"vase", "Vase", 20},
    {"books", "Books", 30},
    {"plant", "Plant", 50}
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

        bool alreadyOwned = std::find(
            playerData.ownedDecorations.begin(),
            playerData.ownedDecorations.end(),
            selectedId
        ) != playerData.ownedDecorations.end();

        if (alreadyOwned) {
            std::cout << "Already owned: " << selectedId << "\n";
        }
        else if (playerData.coins >= price) {
            playerData.coins -= price;
            playerData.ownedDecorations.push_back(selectedId);
            playerData.saveToFile("save.json");
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

        bool owned = std::find(
            playerData.ownedDecorations.begin(),
            playerData.ownedDecorations.end(),
            id
        ) != playerData.ownedDecorations.end();

        std::string display = label + " - " + std::to_string(price) + " coins";
        if (owned)
            display += " (Owned)";

        decorationOptions[i].setString(display);
        decorationOptions[i].setFillColor(i == selectedIndex ? sf::Color::Yellow : sf::Color::White);
    }
}

void ShelfShopView::render(sf::RenderWindow& window) {
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
