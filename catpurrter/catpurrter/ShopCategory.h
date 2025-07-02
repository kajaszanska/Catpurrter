#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// ShopSelection is an enum for all possible shop categories the player can select.
enum class ShopSelection {
    None,          // Nothing selected yet
    HatShop,       // Hat shop
    FishTankShop,  // Fish tank items shop
    ShelfShop,     // Shelf decorations shop
    MiniGameShop,  // Mini games shop
    Back           // Back/exit option
};

// ShopCategoryView displays a list of all shop categories for the player to choose from.
// Handles input, menu navigation, and selection.
class ShopCategoryView {
public:
    // Constructs the shop category menu using the game's font.
    ShopCategoryView(const sf::Font& fontRef);

    // Initializes the list of shop options and resets selection state.
    void init();

    // Updates highlight colors for currently selected option.
    void update();

    // Clears the selection (sets selection to None).
    void clearSelection();

    // Draws the shop category menu (background, title, and all options).
    void render(sf::RenderWindow& window);

    // Handles keyboard input for menu navigation, selecting, and closing.
    void handleInput(sf::Keyboard::Key key);

    // Returns true if the player requested to close/exit the menu.
    bool shouldClose() const;

    // Resets the close flag so the view can be reopened.
    void resetCloseFlag();

    // Returns the current selection (which shop, or None).
    ShopSelection getSelectedOption() const;

    // Directly set the selection index for menu.
    void setSelectionIndex(int idx) { selectionIndex = idx; }

private:
    const sf::Font& font;           // Reference to the game's font for drawing text
    std::vector<sf::Text> options;  // List of shop option menu items
    int selectionIndex = 0;         // Which menu option is currently selected/highlighted
    bool closeRequested = false;    // True if ESC or Back was pressed
    ShopSelection selected = ShopSelection::None; // What the player selected
};
