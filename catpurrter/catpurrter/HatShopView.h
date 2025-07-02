#pragma once
#include "ShopViewBase.h"
#include <vector>
#include <tuple>
#include <SFML/Graphics.hpp>

// HatShopView is a shop where the player can buy hats to wear.
// Handles displaying all hats for sale, showing ownership, navigation, purchases, and shop UI.
class HatShopView : public ShopViewBase {
public:
    // Constructs the Hat Shop, using the game's font, player data, and main game manager.
    HatShopView(sf::Font& font, Player& player, GameManager* gm);

    // Initializes the shop (sets up available hats and menu option texts).
    void init() override;

    // Handles keyboard input for up/down navigation, buying, and exiting.
    void handleInput(sf::Keyboard::Key key) override;

    // Draws the hat shop background, menu, and all options.
    void render(sf::RenderWindow& window) override;

    // Returns true if the player requested to close/exit the shop.
    bool shouldClose() const override;

    // Resets the close flag so the view can be reopened.
    void resetCloseFlag() override;

private:
    // List of hats for sale: tuple of (id, display name, price)
    std::vector<std::tuple<std::string, std::string, int>> hats;

    // Text for displaying each hat option on screen
    std::vector<sf::Text> hatOptions;

    // Updates menu option colors and ownership status text.
    void updateOptionColors() override;
};
