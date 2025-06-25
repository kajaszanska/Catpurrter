#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Player.h"


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
    const std::string& getSelectedMiniGame() const { return selectedMiniGame; }
    void clearSelectedMiniGame() { selectedMiniGame.clear(); }


private:

    const sf::Font& font;
    Player& playerData;
    std::vector<sf::Text> options;
    int selectionIndex = 0;
    bool closeRequested = false;
    ComputerSelection selected = ComputerSelection::None;
    std::string selectedMiniGame;

};
