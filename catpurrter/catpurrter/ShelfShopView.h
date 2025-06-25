#ifndef SHELFSHOPVIEW_H
#define SHELFSHOPVIEW_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <utility>
#include "Player.h"

class GameManager;


class ShelfShopView {
public:
    ShelfShopView(sf::Font& font, Player& player, GameManager& gm);

    void init();
    void handleInput(sf::Keyboard::Key key);
    void updateOptionColors();
    void render(sf::RenderWindow& window);

    bool shouldClose() const;
    void resetCloseFlag();
   

private:
    sf::Font& font;
    Player& playerData;
    int selectedIndex = 0;
    bool shouldExit = false;

    std::vector<std::tuple<std::string, std::string, int>> decorations;
    std::vector<sf::Text> decorationOptions;

    GameManager* gameManager;

};

#endif
