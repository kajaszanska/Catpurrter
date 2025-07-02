#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>

// Game entities & views
#include "Player.h"
#include "Room.h"
#include "Aquarium.h"
#include "Shelf.h"
#include "StorageRack.h"
#include "Computer.h"
#include "ShopCategory.h"

// Shop views
#include "HatShopView.h"
#include "ShelfShopView.h"
#include "FishTankShopView.h"
#include "MiniGameShopView.h"

// Forward declarations for minigames
class SnakeGame;
class CatchGame;
class DodgeGame;

// GameState represents all possible game screens/modes.
enum class GameState {
    StartMenu,         // Main menu (new/load/exit)
    RoomView,          // Player's main room view
    ComputerView,      // Computer desktop (apps, minigames, shops)
    StorageView,       // Storage rack (hat equip view)
    ShelfView,         // Big shelf (shows bought decorations)
    ShopCategoryView,  // Menu for choosing which shop
    HatShop,           // Hat shop UI
    ShelfShop,         // Shelf decorations shop UI
    FishTankShop,      // Aquarium decorations/fish shop UI
    MiniGameShop,      // Mini-games shop UI
    AquariumView,      // Fullscreen aquarium view
    MiniGame           // Any active mini-game
};

// GameManager is the central controller for the entire game application.
// Owns the SFML window, manages game state/screens, player data, all views and minigames.
// Handles input, switching screens, and all cross-view logic.
class GameManager {
public:
    // Creates the game manager, initializes window, loads font, and shows start menu.
    GameManager();

    // Main loop: processes events, updates state, renders everything.
    void run();

    // Updates the currently active screen/view/game logic.
    void update(float dt);

    // --- State accessors ---
    // Change the current game state (switch screens)
    void setState(GameState state);
    // Returns the current state/screen being shown
    GameState getState() const;

    // --- Drawing utilities ---
    // Draws a large section title at the top of the current view/screen.
    void drawSectionTitle(sf::RenderWindow& window, sf::Font& font, const std::string& title);
    // Draws the player's coin count in the top-right (or custom) position.
    void drawCoinDisplay(sf::RenderWindow& window, sf::Font& font, int coins, sf::Vector2f pos = { 500.f, 50.f });

    // Returns a pointer to the current Room view (for refreshing visuals, etc).
    Room* getRoomView() { return roomView; }

private:
    // ==== Core SFML ====
    sf::RenderWindow window;       // The main game window.
    sf::Font font;                 // Game UI font (loaded once at startup).
    sf::Clock frameClock;          // Clock for frame timing (delta time).

    // ==== Game State ====
    GameState state;               // Current screen/game state.
    Player playerData;             // Stores all persistent player data.

    // ==== Menu (Start/Menu) ====
    std::vector<sf::Text> menuItems;   // Start menu text options.
    int selectedIndex = 0;             // Highlighted option in the main menu.
    sf::Texture startMenuBgTexture;    // Start menu background image.
    sf::Sprite startMenuBgSprite;      // Sprite for the start menu background.

    // Start menu confirmation popup
    bool showingNewGameConfirm = false;  // True if new game confirmation is open.
    int confirmIndex = 0;                // "Yes"/"No" selected in confirmation.

    // ==== Key State (for continuous movement) ====
    std::map<sf::Keyboard::Key, bool> keyState;  // Tracks held-down keys for movement.

    // ==== Views & UI Screens ====
    Room* roomView = nullptr;                   // Main room view.
    Aquarium* aquariumView = nullptr;           // Fullscreen aquarium.
    Shelf* shelfView = nullptr;                 // Fullscreen shelf view.
    StorageRack* storageRackView = nullptr;     // Hat equip view.
    Computer* computerView = nullptr;           // Desktop view.
    ShopCategoryView* shopCategoryView = nullptr;// Category selection menu.

    // Shop screens
    HatShopView* hatShopView = nullptr;         // Hat shop UI.
    ShelfShopView* shelfShopView = nullptr;     // Shelf decorations shop.
    FishTankShopView* fishTankShopView = nullptr;// Fish tank shop.
    MiniGameShopView* miniGameShopView = nullptr;// Mini-games shop.

    // ==== Mini Games ====
    SnakeGame* snakeGame = nullptr;   // Pointer to Snake mini-game, if active.
    CatchGame* catchGame = nullptr;   // Pointer to Catch mini-game, if active.
    DodgeGame* dodgeGame = nullptr;   // Pointer to Dodge mini-game, if active.

    // ==== Menu/Selection indices (various UI screens) ====
    int computerSelectionIndex = 0;           // Highlight in computer view.
    std::vector<sf::Text> computerOptions;    // Computer desktop option labels.

    int storageSelectionIndex = 0;            // Storage rack selection.
    std::vector<sf::Text> storageOptions;

    int shelfSelectionIndex = 0;              // Shelf selection.
    std::vector<sf::Text> shelfOptions;

    int shopSelectionIndex = 0;               // Shop category menu selection.
    std::vector<sf::Text> shopVisualItems;

    // Shop categories (for navigation)
    enum class ShopCategory {
        Hat,
        Shelf,
        FishTank,
        MiniGame
    };
    std::vector<std::pair<sf::Text, ShopCategory>> shopCategoryOptions;
    int shopCategoryIndex = 0;

    // ==== Core logic helpers ====
    void processEvents();            // Polls SFML events and dispatches them.
    void render();                   // Draws the currently active view.
    void renderStartMenu();          // Draws the start menu UI.
    void updateStartMenu();          // Updates start menu highlight.
    void loadFont();                 // Loads the game's font from file.
    void initMenu();                 // Initializes menu options/texts.
    void moveUp();                   // Moves highlight up in menu.
    void moveDown();                 // Moves highlight down in menu.
    void selectOption();             // Handles selection/activation in menus.
    void handleContinuousMovement(); // Handles WASD/arrow-key walking in room.

    // ==== Mini Game helpers ====
    void initMiniGame();             // Initializes minigame (legacy/unused).
    void renderMiniGame();           // Renders a generic minigame placeholder.

    // ==== Shop Navigation (templated) ====
    template<typename T>
    void handleShopNavigationInput(sf::Keyboard::Key key, int& selectionIndex, const std::vector<T>& items);

    // ==== Input handlers for all game states ====
    void processStartMenuEvents(const sf::Event& event);
    void processRoomViewEvents(const sf::Event& event);
    void processComputerViewEvents(const sf::Event& event);
    void processStorageViewEvents(const sf::Event& event);
    void processHatShopEvents(const sf::Event& event);
    void processMiniGameEvents(const sf::Event& event);
    void processShelfViewEvents(const sf::Event& event);
    void processShopCategoryViewEvents(const sf::Event& event);
    void processShelfShopEvents(const sf::Event& event);
    void processAquariumViewEvents(const sf::Event& event);
    void processFishTankShopEvents(const sf::Event& event);
    void processMiniGameShopEvents(const sf::Event& event);
};
