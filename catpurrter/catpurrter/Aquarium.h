#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

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

    sf::Texture aquariumBigTexture;        // aquariumbig.png
    sf::Texture aquariumBigPlants;         // aquariumbigplants.png
    sf::Texture aquariumBigCastle;         // aquariumbigcastle.png
    sf::Texture aquariumBigAll;            // aquariumbigall.png

    struct FishVisual {
        std::string id;         // e.g. "fish1"
        bool facingRight = true; // direction: true = right, false = left
        sf::Vector2f position;   // position of the fish
        sf::Vector2f velocity;   // speed (pixels/frame) in x/y
        float directionTimer = 0; // timer to change direction
        float distanceSinceDirectionChange = 0.0f;
        float minSwimDistance = 60.f;

    };



    std::vector<FishVisual> fishes;
  
    std::unordered_map<std::string, sf::Texture> fishTextures;

};
