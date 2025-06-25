#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <tuple>
#include <string>
#include "Player.h"

class GameManager;

class MiniGameShopView {
public:
    MiniGameShopView(sf::Font& font, Player& player, GameManager& gm);
    void init();
    void render(sf::RenderWindow& window);
    void handleInput(sf::Keyboard::Key key);
    bool shouldClose() const;
    void resetCloseFlag();

private:
    sf::Font& font;
    Player& playerData;
    std::vector<std::tuple<std::string, std::string, int>> games; 
    std::vector<sf::Text> gameOptions;
    int selectedIndex = 0;
    bool exitRequested = false;

    void updateOptionColors();
    GameManager& gameManager;
};
