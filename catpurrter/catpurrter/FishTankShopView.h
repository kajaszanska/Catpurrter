#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <tuple>
#include <string>
#include "Player.h"

class GameManager;

class FishTankShopView {
public:
    FishTankShopView(sf::Font& font, Player& player, GameManager& gm);
    void init();
    void render(sf::RenderWindow& window);
    void handleInput(sf::Keyboard::Key key);
    bool shouldClose() const;
    void resetCloseFlag();
    void update();



private:
    sf::Font& font;
    Player& playerData;
    std::vector<std::tuple<std::string, std::string, int>> items; 
    std::vector<sf::Text> itemTexts;
    int selectedIndex = 0;
    bool closeRequested = false;
    GameManager& gameManager;
    std::vector<sf::Text> fishOptions;


    void updateOptionColors();

};
