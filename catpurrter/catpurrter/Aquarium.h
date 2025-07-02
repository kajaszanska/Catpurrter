#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include <unordered_map>
#include <vector>
#include <string>

// Aquarium represents the fullscreen view of the player's aquarium.
// It animates fish and decorations the player has purchased and placed in the aquarium.
// Handles input, fish animation logic, and rendering the aquarium scene.
class Aquarium {
public:
    // Constructs an Aquarium view with references to the game's font and the Player data.
    Aquarium(sf::Font& font, Player& player);

    // Initializes aquarium state (loads textures, positions fish, etc).
    void init();

    // Updates fish animations and any other aquarium logic.
    // dt: time elapsed since last frame (seconds).
    void update(float dt);

    // Draws the aquarium, all fish, and decorations to the window.
    void render(sf::RenderWindow& window);

    // Handles keyboard input. ESC closes the aquarium view.
    void handleInput(sf::Keyboard::Key key);

    // Returns true if the player has requested to close the aquarium view.
    bool shouldClose() const;

    // Resets the close flag so the view won't immediately close next time it's opened.
    void resetCloseFlag();

private:
    sf::Font& font;        // Reference to game font for rendering text.
    Player& playerData;    // Reference to player data (to access owned fish/decorations).
    bool closeRequested;   // True if the player pressed ESC to exit aquarium.

    // Background aquarium images for different decoration combinations:
    sf::Texture aquariumBigTexture;    // Base aquarium image (no decorations).
    sf::Texture aquariumBigPlants;     // With plants decoration.
    sf::Texture aquariumBigCastle;     // With castle decoration.
    sf::Texture aquariumBigAll;        // With both plants and castle.

    // FishVisual represents one animated fish in the aquarium.
    struct FishVisual {
        std::string id;                  // Fish type identifier (e.g., "fish1")
        bool facingRight = true;         // True if fish is facing right.
        sf::Vector2f position;           // Current position of the fish.
        sf::Vector2f velocity;           // Current velocity (pixels/sec).
        float directionTimer = 0;        // Timer until next random direction change.
        float distanceSinceDirectionChange = 0.0f; // How far since last change.
        float minSwimDistance = 60.f;    // Minimum swim distance before changing direction.
    };

    std::vector<FishVisual> fishes;                    // All fish currently swimming.
    std::unordered_map<std::string, sf::Texture> fishTextures; // Fish textures (by id + direction).
};
