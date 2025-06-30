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

enum class GameState {
    StartMenu,
    RoomView,
    ComputerView,
    StorageView,
    ShelfView,
    ShopCategoryView,
    HatShop,
    ShelfShop,
    FishTankShop,
    MiniGameShop,
    AquariumView,
    MiniGame
};

class GameManager {
public:
    GameManager();
    void run();
    void update(float dt);

    // State accessors
    void setState(GameState state);
    GameState getState() const;

    // Drawing utilities
    void drawSectionTitle(sf::RenderWindow& window, sf::Font& font, const std::string& title);
    void drawCoinDisplay(sf::RenderWindow& window, sf::Font& font, int coins, sf::Vector2f pos = { 500.f, 50.f });

    // Views
    Room* getRoomView() { return roomView; }

private:
    // ==== Core SFML ====
    sf::RenderWindow window;
    sf::Font font;
    sf::Clock frameClock;

    // ==== Game State ====
    GameState state;
    Player playerData;

    // ==== Menu (Start/Menu) ====
    std::vector<sf::Text> menuItems;
    int selectedIndex = 0;
    sf::Texture startMenuBgTexture;
    sf::Sprite startMenuBgSprite;

    // Start menu confirmation
    bool showingNewGameConfirm = false;
    int confirmIndex = 0;

    // ==== Key State (for continuous movement) ====
    std::map<sf::Keyboard::Key, bool> keyState;

    // ==== Views & UI Screens ====
    Room* roomView = nullptr;
    Aquarium* aquariumView = nullptr;
    Shelf* shelfView = nullptr;
    StorageRack* storageRackView = nullptr;
    Computer* computerView = nullptr;
    ShopCategoryView* shopCategoryView = nullptr;

    // Shop screens
    HatShopView* hatShopView = nullptr;
    ShelfShopView* shelfShopView = nullptr;
    FishTankShopView* fishTankShopView = nullptr;
    MiniGameShopView* miniGameShopView = nullptr;

    // ==== Mini Games ====
    SnakeGame* snakeGame = nullptr;
    CatchGame* catchGame = nullptr;
    DodgeGame* dodgeGame = nullptr;

    // ==== Menu/Selection indices (various UI screens) ====
    int computerSelectionIndex = 0;
    std::vector<sf::Text> computerOptions;

    int storageSelectionIndex = 0;
    std::vector<sf::Text> storageOptions;

    int shelfSelectionIndex = 0;
    std::vector<sf::Text> shelfOptions;

    int shopSelectionIndex = 0;
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
    void processEvents();
    void render();
    void renderStartMenu();
    void updateStartMenu();
    void loadFont();
    void initMenu();
    void moveUp();
    void moveDown();
    void selectOption();
    void handleContinuousMovement();

    // ==== Mini Game helpers ====
    void initMiniGame();
    void renderMiniGame();

    // ==== Shop Navigation (templated) ====
    template<typename T>
    void handleShopNavigationInput(sf::Keyboard::Key key, int& selectionIndex, const std::vector<T>& items);

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
