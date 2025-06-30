#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include <unordered_map>
#include <vector>
#include <string>

class Aquarium {
public:
    Aquarium(sf::Font& font, Player& player);

    void init();
    void update(float dt);
    void render(sf::RenderWindow& window);
    void handleInput(sf::Keyboard::Key key);

    bool shouldClose() const;
    void resetCloseFlag();

private:
    sf::Font& font;
    Player& playerData;
    bool closeRequested;

    sf::Texture aquariumBigTexture;
    sf::Texture aquariumBigPlants;
    sf::Texture aquariumBigCastle;
    sf::Texture aquariumBigAll;

    struct FishVisual {
        std::string id;
        bool facingRight = true;
        sf::Vector2f position;
        sf::Vector2f velocity;
        float directionTimer = 0;
        float distanceSinceDirectionChange = 0.0f;
        float minSwimDistance = 60.f;
    };

    std::vector<FishVisual> fishes;
    std::unordered_map<std::string, sf::Texture> fishTextures;
};
