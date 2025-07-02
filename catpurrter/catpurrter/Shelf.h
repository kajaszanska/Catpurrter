#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

// Shelf displays a fullscreen view of the player's owned shelf decorations.
// It shows large versions of decorations the player has purchased and placed on the shelf.
// Allows selection (for future expansion) and handles closing the view.
class Shelf {
public:
    // Constructs the Shelf view with the game's font and reference to the player's data.
    Shelf(const sf::Font& font, Player& player);

    // Initializes the shelf state (loads background and decoration textures, sets positions).
    void init();

    // Updates selection highlight for the decorations list (for possible selection interaction).
    void update();

    // Draws the shelf background, all big decorations, and the title to the window.
    void render(sf::RenderWindow& window);

    // Handles keyboard input for navigating decorations (W/S or Up/Down), selecting, or closing (ESC).
    void handleInput(sf::Keyboard::Key key);

    // Returns true if the user has requested to close the shelf view.
    bool shouldClose() const;

    // Resets the close flag so the view won't immediately close when reopened.
    void resetCloseFlag();

private:
    const sf::Font& font;            // Reference to the font for drawing text.
    Player& playerData;              // Reference to player data (owns the decorations).

    std::vector<sf::Text> decorationTexts;   // List of decoration names for potential selection/highlight.
    int selectionIndex = 0;                  // Currently highlighted decoration (if used for selection).
    bool closeRequested = false;             // True if the player pressed ESC to exit.

    sf::Texture shelfBackgroundTexture;      // Texture for the shelf background image.
    sf::Sprite shelfBackgroundSprite;        // Sprite for drawing the shelf background.

    std::map<std::string, sf::Texture> bigDecorationTextures;  // Textures for big decoration images (by id).
    std::vector<sf::Sprite> bigDecorationSprites;              // Sprites for each big decoration displayed.
};
