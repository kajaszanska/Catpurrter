#pragma once
#include "ShopViewBase.h"
#include <vector>
#include <tuple>
#include <string>

// FishTankShopView is a shop where the player can buy fish and decorations for the aquarium.
// Displays a list of items (fish, plants, castle), handles purchase logic, input, and visual updates.
class FishTankShopView : public ShopViewBase {
public:
    // Constructs the Fish Tank Shop, using game's font, player data, and main game manager.
    FishTankShopView(sf::Font& font, Player& player, GameManager* gm);

    // Initializes shop state (sets up available items, their display, and option highlighting).
    void init() override;

    // Handles keyboard input for navigation (up/down), purchase (Enter), and closing (ESC).
    void handleInput(sf::Keyboard::Key key) override;

    // Draws the shop UI, item list, and highlights to the window.
    void render(sf::RenderWindow& window) override;

    // Returns true if the player requested to close the shop.
    bool shouldClose() const override;

    // Resets the close flag so the shop view can be reopened.
    void resetCloseFlag() override;

private:
    // List of items for sale: tuple of (id, display name, price)
    std::vector<std::tuple<std::string, std::string, int>> items;

    // Display texts for each shop item (to show on screen)
    std::vector<sf::Text> itemTexts;

    // Updates text color and string to reflect selection and ownership status.
    void updateOptionColors() override;
};
