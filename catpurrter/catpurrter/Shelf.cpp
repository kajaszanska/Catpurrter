#include "Shelf.h"
#include <iostream>

Shelf::Shelf(const sf::Font& font, Player& player)
    : font(font), playerData(player) {
}

void Shelf::init() {
    decorationTexts.clear();
    selectionIndex = 0;

    float y = 200.f;
    for (const std::string& item : playerData.ownedDecorations) {
        sf::Text text;
        text.setFont(font);
        text.setString(item);
        text.setCharacterSize(30);
        text.setFillColor(sf::Color::White);
        text.setPosition(100.f, y);
        decorationTexts.push_back(text);
        y += 50.f;
    }
}

void Shelf::update() {
    for (size_t i = 0; i < decorationTexts.size(); ++i) {
        decorationTexts[i].setFillColor(i == selectionIndex ? sf::Color::Yellow : sf::Color::White);
    }
}

void Shelf::render(sf::RenderWindow& window) {
    sf::Text title;
    title.setFont(font);
    title.setString("Shelf Decorations");
    title.setCharacterSize(30);
    title.setFillColor(sf::Color::Cyan);
    title.setPosition(100.f, 30.f);
    window.draw(title);

    for (const auto& item : decorationTexts) {
        window.draw(item);
    }
}

void Shelf::handleInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        closeRequested = true;
        return;
    }

    if (decorationTexts.empty()) return;

    if (key == sf::Keyboard::W || key == sf::Keyboard::Up) {
        if (selectionIndex > 0) selectionIndex--;
    }
    else if (key == sf::Keyboard::S || key == sf::Keyboard::Down) {
        if (selectionIndex < static_cast<int>(decorationTexts.size()) - 1) selectionIndex++;
    }
    else if (key == sf::Keyboard::Enter) {
        std::string selected = playerData.ownedDecorations[selectionIndex];
        std::cout << "Selected decoration: " << selected << "\n";
    }
}

bool Shelf::shouldClose() const {
    return closeRequested;
}

void Shelf::resetCloseFlag() {
    closeRequested = false;
}
