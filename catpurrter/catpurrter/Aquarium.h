#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

class Aquarium {
public:
    Aquarium(sf::Font& font, Player& player);

    void init();
    void update();
    void render(sf::RenderWindow& window);
    void handleInput(sf::Keyboard::Key key);

    bool shouldClose() const;
    void resetCloseFlag();

private:
    sf::Font& font;
    Player& playerData;
    bool closeRequested;
};
