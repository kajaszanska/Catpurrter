#pragma once

#include "ShopViewBase.h"
#include <vector>
#include <string>
#include <tuple>
#include <SFML/Graphics.hpp>

// ShelfShopView is a shop where the player can buy big decorations to display on their room shelf.
// Handles displaying all available decorations, their prices, which ones are owned, purchasing, navigation, and shop UI.
class ShelfShopView : public ShopViewBase {
public:
    // Constructs the Shelf Shop, using the game's font, player data, and main game manager.
    ShelfShopView(sf::Font& font, Player& player, GameManager* gm);

    // Initializes the shop (sets up available decorations and option display).
    void init() override;

    // Handles keyboard input for up/down navigation, buying, and closing.
    void handleInput(sf::Keyboard::Key key) override;

    // Draws the shop background, menu, and all decoration options.
    void render(sf::RenderWindow& window) override;

    // Returns true if the player requested to close/exit the shop.
    bool shouldClose() const override;

    // Resets the close flag so the view can be reopened.
    void resetCloseFlag() override;

private:
    // List of decorations for sale: tuple of (id, display name, price)
    std::vector<std::tuple<std::string, std::string, int>> decorations;

    // Text for displaying each decoration option on screen
    std::vector<sf::Text> decorationOptions;

    // Updates option colors and ownership status.
    void updateOptionColors() override;
};
