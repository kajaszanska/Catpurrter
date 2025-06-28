#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

class GameManager;

class StorageRack {
public:
    StorageRack(const sf::Font& font, Player& player, GameManager* gm);
    void init();
    void update();
    void render(sf::RenderWindow& window);
    void handleInput(sf::Keyboard::Key key);

    bool shouldClose() const;
    void resetCloseFlag();

private:
    const sf::Font& font;
    Player& playerData;

    GameManager* gameManager = nullptr;

    std::vector<sf::Text> hatOptions;
    int selectionIndex = 0;
    bool closeRequested = false;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    std::map<std::string, sf::Texture> hatTextures;
   
    std::vector<sf::Vector2f> hatPositions;
};
