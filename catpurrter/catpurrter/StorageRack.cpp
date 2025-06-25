#include "StorageRack.h"
#include <iostream>

StorageRack::StorageRack(const sf::Font& font, Player& player)
    : font(font), playerData(player) {
}

void StorageRack::init() {
    hatOptions.clear();
    selectionIndex = 0;

    float y = 200.f;
    for (const auto& hat : playerData.unlockedHats) {
        std::string label = hat;
        if (hat == playerData.equippedHat) {
            label += " (Equipped)";
        }

        sf::Text text;
        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(36);
        text.setFillColor(sf::Color::White);
        text.setPosition(100.f, y);
        hatOptions.push_back(text);

        y += 50.f;
    }
}

void StorageRack::update() {
    for (size_t i = 0; i < hatOptions.size(); ++i) {
        hatOptions[i].setFillColor(i == selectionIndex ? sf::Color::Yellow : sf::Color::White);
    }
}

void StorageRack::render(sf::RenderWindow& window) {
    sf::Text title("Storage Rack", font, 32);
    title.setPosition(100.f, 30.f);
    title.setFillColor(sf::Color::Cyan);
    window.draw(title);

    for (const auto& item : hatOptions) {
        window.draw(item);
    }
}

void StorageRack::handleInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        closeRequested = true;
        return;
    }

    if (hatOptions.empty()) return;

    if (key == sf::Keyboard::Up || key == sf::Keyboard::W) {
        if (selectionIndex > 0) selectionIndex--;
    }
    else if (key == sf::Keyboard::Down || key == sf::Keyboard::S) {
        if (selectionIndex < static_cast<int>(hatOptions.size()) - 1) selectionIndex++;
    }
    else if (key == sf::Keyboard::Enter) {
        std::string selectedHat = playerData.unlockedHats[selectionIndex];
        playerData.equippedHat = selectedHat;
        playerData.saveToFile("save.json");
        std::cout << "Equipped hat: " << selectedHat << "\n";
        int oldIndex = selectionIndex;
        init();
        selectionIndex = oldIndex;

    }
}

bool StorageRack::shouldClose() const {
    return closeRequested;
}

void StorageRack::resetCloseFlag() {
    closeRequested = false;
}
