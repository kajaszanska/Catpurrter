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
    ShopView
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

    void initShopView();
    void renderShopView();
    void handleShopInput(sf::Keyboard::Key key);

    struct ShopItem {
        std::string name;
        int cost;
    };
    std::vector<std::pair<std::string, int>> shopCatalog = {
    { "cap", 100 },
    { "crown", 200 },
    { "wizard", 300 },
    { "pirate", 10 }
    };


    std::vector<ShopItem> shopItems;
    int shopSelectionIndex = 0;
    std::vector<sf::Text> shopVisualItems; 

};
