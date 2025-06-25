#include "Aquarium.h"
#include <iostream>

Aquarium::Aquarium(sf::Font& fontRef, Player& playerRef)
    : font(fontRef), playerData(playerRef), closeRequested(false) {
}

void Aquarium::init() {
    closeRequested = false;
}


void Aquarium::update() {
    // No dynamic logic needed for now
}

void Aquarium::render(sf::RenderWindow& window) {
    sf::Text title;
    title.setFont(font);
    title.setString("Aquarium View");
    title.setCharacterSize(30);
    title.setFillColor(sf::Color::Cyan);
    title.setPosition(100.f, 30.f);
    window.draw(title);

    float y = 100.f;
    for (const auto& item : playerData.aquariumContents) {
        sf::Text text;
        text.setFont(font);
        text.setString(item);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setPosition(100.f, y);
        window.draw(text);
        y += 40.f;
    }
}

void Aquarium::handleInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        std::cout << "Leaving Aquarium View\n";
        closeRequested = true;
    }
}

bool Aquarium::shouldClose() const {
    return closeRequested;
}

void Aquarium::resetCloseFlag() {
    closeRequested = false;
}
