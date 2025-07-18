#include "FishTankShopView.h"
#include "GameManager.h"

#include <algorithm>
#include <iostream>
// Im implementing <ranges> to replace any use of std::find with std::ranges::find
#include <ranges> 
// Im using <thread> to save player data in a separate thread after purchase
#include <thread>


FishTankShopView::FishTankShopView(sf::Font& font, Player& player, GameManager* gm)
    : ShopViewBase(font, player, gm) {
    init();
}

void FishTankShopView::init() {
    items = {
    { "plant", "Aquatic Plant", 100 },
    { "castle", "Sand Castle", 150 },
    { "fish1", "Gold Fish", 100 },
    { "fish2", "Blue Tang", 100 },
    { "fish3", "Puffer Fish", 100 }
    };
    itemTexts.clear();
    float y = 150.f;
    for (const auto& [id, label, price] : items) {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(28);
        text.setFillColor(sf::Color::White);
        text.setPosition(100.f, y);
        itemTexts.push_back(text);
        y += 40.f;
    }

    updateOptionColors();
}

void FishTankShopView::render(sf::RenderWindow& window) {
    sf::RectangleShape bg(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
    bg.setFillColor(sf::Color(120, 60, 200));
    window.draw(bg);

    gameManager->drawSectionTitle(window, font, "Fish Tank Shop");
    gameManager->drawCoinDisplay(window, font, playerData.coins);

    for (const auto& opt : itemTexts)
        window.draw(opt);
}


void FishTankShopView::handleInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        closeFlag = true;
        return;
    }
    if (key == sf::Keyboard::Up || key == sf::Keyboard::W) {
        if (selectedIndex > 0) selectedIndex--;
    }
    else if (key == sf::Keyboard::Down || key == sf::Keyboard::S) {
        if (selectedIndex < static_cast<int>(items.size()) - 1) selectedIndex++;
    }
    else if (key == sf::Keyboard::Enter) {
        const std::string& selectedId = std::get<0>(items[selectedIndex]);
        int price = std::get<2>(items[selectedIndex]);

        bool alreadyOwned = std::ranges::find(playerData.aquariumContents, selectedId) != playerData.aquariumContents.end();

        if (alreadyOwned) {
            std::cout << "Already owned: " << selectedId << "\n";
        }
        else if (playerData.coins >= price) {
            playerData.coins -= price;
            playerData.aquariumContents.push_back(selectedId);
            std::thread saveThread([&]() {
                playerData.saveToFile("saves/save.json");
                });
            saveThread.detach();
            std::cout << "Bought: " << selectedId << "\n";
            updateOptionColors();
            if (gameManager->getRoomView())
                gameManager->getRoomView()->refreshAquariumVisuals();
        }
        else {
            std::cout << "Not enough coins\n";
        }
    }

    updateOptionColors();
}

void FishTankShopView::updateOptionColors() {
    for (size_t i = 0; i < items.size(); ++i) {
        const auto& [id, label, price] = items[i];
        bool owned = std::ranges::find(playerData.aquariumContents, id) != playerData.aquariumContents.end();

        std::string display = label + " - " + std::to_string(price) + " coins";
        if (owned) display += " (Owned)";
        itemTexts[i].setString(display);
        itemTexts[i].setFillColor(i == selectedIndex ? sf::Color::Yellow : sf::Color::White);
    }
}

bool FishTankShopView::shouldClose() const {
    return closeFlag; 
}

void FishTankShopView::resetCloseFlag() {
    closeFlag = false; 
}
