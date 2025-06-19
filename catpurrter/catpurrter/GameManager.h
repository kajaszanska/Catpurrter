#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Player.h"

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
    MiniGame
};

struct ShopItem {
    std::string name;
    int cost;
};


class GameManager {
public:
    GameManager();
    void run();

private:
    sf::RenderWindow window;
    sf::Font font;
    std::vector<sf::Text> menuItems;
    int selectedIndex;

    GameState state;
    Player playerData;

    void processEvents();
    void update();
    void render();

    void renderStartMenu();
    void renderRoomView();

    void updateStartMenu();
    void updateRoomView();

    void loadFont();
    void initMenu();
    void moveUp();
    void moveDown();
    void selectOption();

    std::vector<sf::Text> roomObjects;
    int roomSelectionIndex;

    void initRoomObjects();
    void handleRoomInput(sf::Keyboard::Key key);

    std::vector<sf::Text> computerOptions;
    int computerSelectionIndex;

    void initComputerView();
    void renderComputerView();
    void updateComputerView();
    void handleComputerInput(sf::Keyboard::Key key);

    std::vector<sf::Text> storageOptions;
    int storageSelectionIndex = 0;

    void initStorageView();
    void renderStorageView();
    void handleStorageInput(sf::Keyboard::Key key);

    void initHatShopView();
    void renderHatShopView();
    void handleHatShopInput(sf::Keyboard::Key key);

    std::vector<std::pair<std::string, int>> shopCatalog = {
    { "cap", 100 },
    { "crown", 200 },
    { "wizard", 300 },
    { "pirate", 10 }
    };

    std::vector<std::pair<std::string, int>> shelfCatalog = {
    { "Figure", 150 },
    { "Photo", 100 },
    { "Plant", 200 }
    };

    std::vector<sf::Text> shelfOptions;
    int shelfSelectionIndex = 0;

    void initShelfView();
    void renderShelfView();
    void handleShelfInput(sf::Keyboard::Key key);



    std::vector<ShopItem> shopItems;
    int shopSelectionIndex = 0;
    std::vector<sf::Text> shopVisualItems; 

    void initMiniGame();
    void renderMiniGame();
    void handleMiniGameInput(sf::Keyboard::Key key);

    enum class ShopCategory {
        Hat,
        Shelf,
        FishTank,
        MiniGame
    };

    std::vector<std::pair<sf::Text, ShopCategory>> shopCategoryOptions;
    int shopCategoryIndex = 0;


    void initShopCategoryView();
    void renderShopCategoryView();
    void handleShopCategoryInput(sf::Keyboard::Key key);

    std::vector<ShopItem> shelfShopItems;

    void initShelfShop();
    void renderShelfShop();
    void handleShelfShopInput(sf::Keyboard::Key key);

    std::vector<ShopItem> fishTankShopItems;
    void initFishTankShop();
    void renderFishTankShop();
    void handleFishTankShopInput(sf::Keyboard::Key key);

    void initMiniGameShop();
    void renderMiniGameShop();
    void handleMiniGameShopInput(sf::Keyboard::Key key);


};
