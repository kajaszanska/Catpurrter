#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

class Shelf {
public:
    Shelf(const sf::Font& font, Player& player);
    void init();
    void update();
    void render(sf::RenderWindow& window);
    void handleInput(sf::Keyboard::Key key);

    bool shouldClose() const;
    void resetCloseFlag();

private:
    const sf::Font& font;
    Player& playerData;
    std::vector<sf::Text> decorationTexts;
    int selectionIndex = 0;
    bool closeRequested = false;
};
