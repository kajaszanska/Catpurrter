#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "Player.h"
#include "Room.h"
#include "Aquarium.h"
#include "Shelf.h"
#include "StorageRack.h"
#include "Computer.h"
#include "ShopCategory.h"

#include "HatShopView.h"
#include "ShelfShopView.h"
#include "FishTankShopView.h"
#include "MiniGameShopView.h"


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

    void drawSectionTitle(sf::RenderWindow& window, sf::Font& font, const std::string& title);
    void drawCoinDisplay(sf::RenderWindow& window, sf::Font& font, int coins);

private:
    sf::RenderWindow window;
    sf::Font font;
    std::vector<sf::Text> menuItems;
    int selectedIndex;

    GameState state;
    Player playerData;

    template<typename T>
    void handleShopNavigationInput(sf::Keyboard::Key key, int& selectionIndex, const std::vector<T>& items);


    void processEvents();
    void update();
    void render();

    void renderStartMenu();

    void updateStartMenu();

    void loadFont();
    void initMenu();
    void moveUp();
    void moveDown();
    void selectOption();

    std::vector<sf::Text> computerOptions;
    int computerSelectionIndex;

    std::vector<sf::Text> storageOptions;
    int storageSelectionIndex = 0;

    std::vector<sf::Text> shelfOptions;
    int shelfSelectionIndex = 0;

    int shopSelectionIndex = 0;
    std::vector<sf::Text> shopVisualItems; 

    void initMiniGame();
    void renderMiniGame();
    void handleMiniGameInput(sf::Keyboard::Key key);

    void handleContinuousMovement();
    std::map<sf::Keyboard::Key, bool> keyState;

    enum class ShopCategory {
        Hat,
        Shelf,
        FishTank,
        MiniGame
    };

    std::vector<std::pair<sf::Text, ShopCategory>> shopCategoryOptions;
    int shopCategoryIndex = 0;

    

    Room* roomView = nullptr;
    Aquarium* aquariumView = nullptr;
    Shelf* shelfView = nullptr; 
    StorageRack* storageRackView = nullptr;
    Computer* computerView = nullptr;
    ShopCategoryView* shopCategoryView = nullptr;
    HatShopView* hatShopView = nullptr;
    ShelfShopView* shelfShopView = nullptr;
    FishTankShopView* fishTankShopView = nullptr;
    MiniGameShopView* miniGameShopView = nullptr;

};
