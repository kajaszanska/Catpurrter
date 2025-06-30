#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Player.h"


struct DesktopIcon {
    std::string id; // "shop", "snake", etc
    std::string label; // text under icon
    sf::RectangleShape rect; // icon graphic
};


enum class ComputerSelection {
    None,
    Shop,
    MiniGame,
    Back
};

class Computer {
public:
    Computer(const sf::Font& font);
    void init();
    void update();
    Computer(sf::Font& fontRef, Player& playerRef);
    void render(sf::RenderWindow& window);
    void handleInput(sf::Keyboard::Key key);

    bool shouldClose() const;
    void resetCloseFlag();
    ComputerSelection getSelectedOption() const;
    const std::string& getSelectedMiniGame() const;
    void clearSelectedMiniGame();





private:

    sf::Texture desktopBgTexture;
    sf::Sprite desktopBgSprite;

    const sf::Font& font;
    Player& playerData;
    std::vector<DesktopIcon> icons;
    int iconsPerRow = 4;
    int selectedIndex = 0;

    bool closeRequested = false;
    ComputerSelection selected = ComputerSelection::None;
    std::string selectedMiniGame;

};
