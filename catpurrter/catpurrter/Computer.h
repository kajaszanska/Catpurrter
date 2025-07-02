#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Player.h"

// DesktopIcon represents an icon on the "computer desktop" view.
// Each icon can be the shop or a mini-game the player owns.
struct DesktopIcon {
    std::string id;           // Icon identifier ("shop", "snake", etc)
    std::string label;        // Text label displayed under the icon
    sf::RectangleShape rect;  // Rectangle shape used to draw the icon
};

// ComputerSelection enumerates what the player has selected in the computer view.
enum class ComputerSelection {
    None,       // Nothing selected yet
    Shop,       // Shop icon selected
    MiniGame,   // A mini-game icon selected
    Back        // (Unused - was used for an exit/back button)
};

// Computer shows a "desktop" screen with icons for Shop and unlocked mini-games.
// Lets the player select which app (shop/minigame) to open.
// Handles rendering, input navigation, and selection.
class Computer {
public:
    // Constructs the Computer view with the game's font and player data.
    Computer(const sf::Font& font);
    Computer(sf::Font& fontRef, Player& playerRef);

    // Initializes the desktop: loads background, sets up icons, resets selection state.
    void init();

    // Updates selection or animation (not used in current implementation).
    void update();

    // Draws the desktop background and all icons to the window.
    void render(sf::RenderWindow& window);

    // Handles keyboard input for icon navigation and selection (arrows/WASD + Enter + ESC).
    void handleInput(sf::Keyboard::Key key);

    // Returns true if the player pressed ESC to close the computer view.
    bool shouldClose() const;

    // Resets the close flag so this view won't immediately close when reopened.
    void resetCloseFlag();

    // Returns what the player has selected (Shop, MiniGame, None, etc.).
    ComputerSelection getSelectedOption() const;

    // If a mini-game was selected, returns its string id ("snake", "catch", etc.).
    const std::string& getSelectedMiniGame() const;

    // Clears the selected mini-game (for state management after launching a game).
    void clearSelectedMiniGame();

private:
    sf::Texture desktopBgTexture;      // Texture for desktop background image.
    sf::Sprite desktopBgSprite;        // Sprite for desktop background.

    const sf::Font& font;              // Reference to game's font.
    Player& playerData;                // Reference to player data (to show owned games).

    std::vector<DesktopIcon> icons;    // List of desktop icons (shop + owned mini-games).
    int iconsPerRow = 4;               // How many icons are shown per row.
    int selectedIndex = 0;             // Index of the currently highlighted icon.

    bool closeRequested = false;       // True if player pressed ESC to exit.
    ComputerSelection selected = ComputerSelection::None;  // What the player has currently selected.
    std::string selectedMiniGame;      // If selected, which mini-game id.
};
