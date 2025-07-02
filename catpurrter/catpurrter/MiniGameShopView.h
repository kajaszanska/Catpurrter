#pragma once
#include "ShopViewBase.h"
#include <vector>
#include <tuple>

// MiniGameShopView is a shop where the player can buy/unlock mini-games.
// Displays all available mini-games, shows which are owned, handles purchases, input, and UI updates.
class MiniGameShopView : public ShopViewBase {
public:
    // Constructs the Mini Game Shop, using the game's font, player data, and main game manager.
    MiniGameShopView(sf::Font& font, Player& player, GameManager* gm);

    // Initializes shop (sets up list of mini-games and display options).
    void init() override;

    // Draws the shop background, menu options, and coin balance.
    void render(sf::RenderWindow& window) override;

    // Handles keyboard input for navigating options, buying, and exiting.
    void handleInput(sf::Keyboard::Key key) override;

    // Returns true if the player requested to close this shop.
    bool shouldClose() const override;

    // Resets the close flag so the view can be reopened.
    void resetCloseFlag() override;

private:
    // List of games for sale: tuple of (id, display name, price)
    std::vector<std::tuple<std::string, std::string, int>> games;

    // Text for displaying each game option on screen
    std::vector<sf::Text> gameOptions;

    // Updates option colors and ownership status.
    void updateOptionColors() override;
};
