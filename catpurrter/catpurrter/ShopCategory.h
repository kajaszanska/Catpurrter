#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class ShopSelection {
    None,
    HatShop,
    FishTankShop,
    ShelfShop,
    MiniGameShop,
    Back
};

class ShopCategoryView {
public:
    ShopCategoryView(const sf::Font& fontRef);

    void init();
    void update();
    void clearSelection();
    void render(sf::RenderWindow& window);
    void handleInput(sf::Keyboard::Key key);

    bool shouldClose() const;
    void resetCloseFlag();
    ShopSelection getSelectedOption() const;

    void setSelectionIndex(int idx) { selectionIndex = idx; }


private:
    const sf::Font& font;
    std::vector<sf::Text> options;
    int selectionIndex = 0;
    bool closeRequested = false;
    ShopSelection selected = ShopSelection::None;
};
