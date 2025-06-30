#include "HatShopView.h"
#include "GameManager.h"

#include <iostream>

HatShopView::HatShopView(sf::Font& font, Player& player, GameManager& gm)
    : font(font), playerData(player), gameManager(gm)
{
    hats = {
    {"crown", "Crown", 50},
    {"pirate", "Pirate Hat", 40},
    {"frog", "Froggy Hat", 30},
    {"wizard", "Wizard Hat", 35}
    };


}

void HatShopView::init() {
    hatOptions.clear();
    float y = 150.f;

    for (const auto& hat : hats) {
        sf::Text option;
        option.setFont(font);
        option.setCharacterSize(28);
        option.setPosition(100.f, y);
        hatOptions.push_back(option);
        y += 50.f;
    }

    updateOptionColors();
}

void HatShopView::handleInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        shouldExit = true;
        return;
    }

    if (key == sf::Keyboard::Up || key == sf::Keyboard::W) {
        if (selectedIndex > 0) selectedIndex--;
    }
    else if (key == sf::Keyboard::Down || key == sf::Keyboard::S) {
        if (selectedIndex < static_cast<int>(hats.size()) - 1) selectedIndex++;
    }
    else if (key == sf::Keyboard::Enter) {
        const std::string& selectedId = std::get<0>(hats[selectedIndex]);
        int price = std::get<2>(hats[selectedIndex]);

        bool alreadyOwned = std::find(
            playerData.unlockedHats.begin(),
            playerData.unlockedHats.end(),
            selectedId
        ) != playerData.unlockedHats.end();

        if (alreadyOwned) {
            std::cout << "Already owned: " << selectedId << "\n";
        }
        else if (playerData.coins >= price) {
            playerData.coins -= price;
            playerData.unlockedHats.push_back(selectedId);
            playerData.saveToFile("save.json");
            std::cout << "Bought hat: " << selectedId << " for " << price << " coins\n";
            updateOptionColors();
        }
        else {
            std::cout << "Not enough coins\n";
        }
    }

    updateOptionColors();
}


void HatShopView::updateOptionColors() {
    for (size_t i = 0; i < hats.size(); ++i) {
        const auto& [id, label, price] = hats[i];

        bool owned = std::find(
            playerData.unlockedHats.begin(),
            playerData.unlockedHats.end(),
            id
        ) != playerData.unlockedHats.end();

        std::string display = label + " - " + std::to_string(price) + " coins";
        if (owned)
            display += " (Owned)";

        hatOptions[i].setString(display);
        hatOptions[i].setFillColor(i == selectedIndex ? sf::Color::Yellow : sf::Color::White);
    }
}


void HatShopView::render(sf::RenderWindow& window) {
    // Fill background with purple
    sf::RectangleShape bg(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
    bg.setFillColor(sf::Color(120, 60, 200)); // Pick your favorite purple
    window.draw(bg);

    gameManager.drawSectionTitle(window, font, "Hat Shop");
    gameManager.drawCoinDisplay(window, font, playerData.coins);

    for (const auto& opt : hatOptions)
        window.draw(opt);
}



bool HatShopView::shouldClose() const {
    return shouldExit;
}

void HatShopView::resetCloseFlag() {
    shouldExit = false;
}
