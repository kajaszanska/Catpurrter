#include "Computer.h"
#include <iostream>
#include <cmath>

Computer::Computer(sf::Font& fontRef, Player& playerRef)
    : font(fontRef), playerData(playerRef) {
}

void Computer::init() {
    if (!desktopBgTexture.loadFromFile("assets/graphics/desktop.png")) {
        std::cout << "Failed to load desktop background!\n";
    }
    desktopBgSprite.setTexture(desktopBgTexture);
    icons.clear();
    selectedIndex = 0;
    selected = ComputerSelection::None;
    closeRequested = false;

    DesktopIcon shopIcon;
    shopIcon.id = "shop";
    shopIcon.label = "Shop";
    shopIcon.rect.setSize({ 70.f, 70.f });
    shopIcon.rect.setFillColor(sf::Color(100, 100, 255));
    icons.push_back(shopIcon);

    for (const auto& mg : playerData.ownedMiniGames) {
        DesktopIcon icon;
        icon.id = mg;
        if (mg == "snake") icon.label = "Snake";
        else if (mg == "catch") icon.label = "Catch Game";
        else if (mg == "dodge") icon.label = "Dodge Game";
        else icon.label = mg; 
        icon.rect.setSize({ 70.f, 70.f });
        icon.rect.setFillColor(sf::Color(120, 180, 120));
        icons.push_back(icon);
    }
    selectedIndex = 0;
}

void Computer::update() {
  }

void Computer::render(sf::RenderWindow& window) {
    window.draw(desktopBgSprite);

    float startX = 120.f, startY = 120.f;
    float gapX = 120.f, gapY = 110.f;
    int perRow = iconsPerRow;
    for (size_t i = 0; i < icons.size(); ++i) {
        int row = i / perRow;
        int col = i % perRow;

        float x = startX + col * gapX;
        float y = startY + row * gapY;

        auto rect = icons[i].rect;
        rect.setPosition(x, y);
        rect.setOutlineColor(i == selectedIndex ? sf::Color::Yellow : sf::Color(40, 40, 40));
        rect.setOutlineThickness(i == selectedIndex ? 4.f : 2.f);
        window.draw(rect);

        sf::Text label(icons[i].label, font, 24);
        label.setFillColor(sf::Color::White);
        sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin(bounds.width / 2.f, 0);
        label.setPosition(x + 35.f, y + 75.f);
        window.draw(label);
    }
}

void Computer::handleInput(sf::Keyboard::Key key) {
    int perRow = iconsPerRow;
    int iconCount = static_cast<int>(icons.size());
    int row = selectedIndex / perRow;
    int col = selectedIndex % perRow;

    if (key == sf::Keyboard::Escape) {
        closeRequested = true;
        return;
    }
    if (key == sf::Keyboard::Left || key == sf::Keyboard::A) {
        if (col > 0) selectedIndex--;
    }
    else if (key == sf::Keyboard::Right || key == sf::Keyboard::D) {
        if (col < perRow - 1 && selectedIndex + 1 < iconCount) selectedIndex++;
    }
    else if (key == sf::Keyboard::Up || key == sf::Keyboard::W) {
        if (row > 0) selectedIndex -= perRow;
    }
    else if (key == sf::Keyboard::Down || key == sf::Keyboard::S) {
        if (selectedIndex + perRow < iconCount) selectedIndex += perRow;
    }
    else if (key == sf::Keyboard::Enter) {
        const std::string& id = icons[selectedIndex].id;
        if (id == "shop") {
            selected = ComputerSelection::Shop;
        }
             else {
            selectedMiniGame = id;
            selected = ComputerSelection::MiniGame;
            std::cout << "Selected mini game: " << selectedMiniGame << "\n";
        }
    }
}

bool Computer::shouldClose() const {
    return closeRequested;
}

void Computer::resetCloseFlag() {
    closeRequested = false;
}

ComputerSelection Computer::getSelectedOption() const {
    return selected;
}

const std::string& Computer::getSelectedMiniGame() const { return selectedMiniGame; }
void Computer::clearSelectedMiniGame() { selectedMiniGame.clear(); }
