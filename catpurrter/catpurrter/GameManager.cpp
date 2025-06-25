#include "GameManager.h"
#include "Player.h"
#include "Room.h"
#include "Aquarium.h"
#include "Shelf.h"
#include "StorageRack.h"
#include "Computer.h"



#include <iostream>


GameManager::GameManager()
    : window(sf::VideoMode(800, 600), "Catpurrter - Start Menu"), selectedIndex(0), state(GameState::StartMenu)
{
    window.setKeyRepeatEnabled(false); 
    loadFont();
    initMenu();
  }



void GameManager::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void GameManager::loadFont() {
    if (!font.loadFromFile("assets/fonts/Roboto-VariableFont_wdth,wght.ttf")) {
        std::cerr << "Error loading font\n";
    }
}

void GameManager::initMenu() {
    std::vector<std::string> options = { "New Game", "Load Game", "Exit" };

    for (size_t i = 0; i < options.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(options[i]);
        text.setCharacterSize(40);
        text.setPosition(300.f, 200.f + i * 60.f);
        text.setFillColor(i == selectedIndex ? sf::Color::Yellow : sf::Color::White);
        menuItems.push_back(text);
    }
}

void GameManager::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::KeyPressed) {
            switch (state) {
            case GameState::StartMenu:
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
                    moveUp();
                if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
                    moveDown();
                if (event.key.code == sf::Keyboard::Enter)
                    selectOption();
                break;

            case GameState::RoomView:
                if (roomView)
                    roomView->handleInput(event.key.code);

                // Handle view change trigger
                if (roomView && roomView->shouldChangeView()) {
                    RoomSelection next = roomView->getSelectedView();
                    roomView->resetViewChangeFlag();

                    switch (next) {
                    case RoomSelection::Computer:
                        state = GameState::ComputerView;
                        if (computerView) delete computerView;
                        computerView = new Computer(font, playerData);
                        computerView->init();
                        std::cout << "Opened Computer\n";
                        break;

                    case RoomSelection::Doors:
                        std::cout << "Returning to Start Menu via Doors\n";
                        state = GameState::StartMenu;
                        selectedIndex = 0;
                        menuItems.clear();
                        initMenu();
                        break;
                    case RoomSelection::StorageRack:
                        std::cout << "Entered Storage Rack\n";
                        state = GameState::StorageView;
                        if (storageRackView) delete storageRackView;
                        storageRackView = new StorageRack(font, playerData);
                        storageRackView->init();
                        break;
                    case RoomSelection::Shelves:
                        std::cout << "Entered Shelf View\n";
                        state = GameState::ShelfView;

                        if (shelfView) delete shelfView;
                        shelfView = new Shelf(font, playerData);
                        shelfView->init();
                        break;

                    case RoomSelection::Aquarium:
                        std::cout << "Entered Aquarium View\n";
                        state = GameState::AquariumView;

                        if (aquariumView) delete aquariumView;
                        aquariumView = new Aquarium(font, playerData);
                        aquariumView->init();
                        break;
                    }
                }
                break;

            case GameState::ComputerView:
                if (event.type == sf::Event::KeyPressed) {
                    computerView->handleInput(event.key.code);

                    if (computerView->shouldClose()) {
                        delete computerView;
                        computerView = nullptr;
                        state = GameState::RoomView;
                        break;
                    }

                    if (event.key.code == sf::Keyboard::Enter) {
                        ComputerSelection choice = computerView->getSelectedOption();
                        if (choice == ComputerSelection::Shop) {
                            state = GameState::ShopCategoryView;
                            if (shopCategoryView) delete shopCategoryView;
                            shopCategoryView = new ShopCategoryView(font);
                            shopCategoryView->init();
                        }
                        else if (choice == ComputerSelection::Back) {
                            delete computerView;
                            computerView = nullptr;
                            state = GameState::RoomView;
                        }

                        const std::string& selectedGame = computerView->getSelectedMiniGame();
                        if (!selectedGame.empty()) {
                            std::cout << "Mini Game selected: " << selectedGame << "\n";
                            initMiniGame();
                            state = GameState::MiniGame;
                        }

                        computerView->clearSelectedMiniGame();
                    }
                }
                break;



            case GameState::StorageView:
                if (storageRackView)
                    storageRackView->handleInput(event.key.code);

                if (storageRackView && storageRackView->shouldClose()) {
                    storageRackView->resetCloseFlag();
                    state = GameState::RoomView;
                }
                break;


            case GameState::HatShop:
                if (hatShopView)
                    hatShopView->handleInput(event.key.code);

                if (hatShopView && hatShopView->shouldClose()) {
                    hatShopView->resetCloseFlag();
                    delete hatShopView;
                    hatShopView = nullptr;
                    state = GameState::ShopCategoryView;
                }
                break;


            case GameState::MiniGame:
                handleMiniGameInput(event.key.code);
                break;

            case GameState::ShelfView:
                if (shelfView)
                    shelfView->handleInput(event.key.code);

                if (shelfView && shelfView->shouldClose()) {
                    shelfView->resetCloseFlag();
                    state = GameState::RoomView;
                }
                break;

            case GameState::ShopCategoryView:
                if (shopCategoryView)
                    shopCategoryView->handleInput(event.key.code);

                if (shopCategoryView && shopCategoryView->shouldClose()) {
                    std::cout << "Leaving Shop Category\n";
                    shopCategoryView->resetCloseFlag();
                    state = GameState::ComputerView;
                }
                else if (shopCategoryView) {
                    ShopSelection selected = shopCategoryView->getSelectedOption();
                    if (selected != ShopSelection::None) {
                        switch (selected) {
                        case ShopSelection::HatShop:
                            std::cout << "Hat Shop selected\n";
                            if (hatShopView) delete hatShopView;
                            hatShopView = new HatShopView(font, playerData, *this);
                            hatShopView->init(); 
                            state = GameState::HatShop;
                            break;

                        case ShopSelection::FishTankShop:
                            std::cout << "Fish Tank Shop selected\n";
                            if (fishTankShopView) delete fishTankShopView;
                            fishTankShopView = new FishTankShopView(font, playerData, *this);
                            fishTankShopView->init();
                            state = GameState::FishTankShop;
                            break;


                        case ShopSelection::ShelfShop:
                            std::cout << "Shelf Shop selected\n";
                            if (shelfShopView) delete shelfShopView;
                            shelfShopView = new ShelfShopView(font, playerData, *this);
                            shelfShopView->init();
                            state = GameState::ShelfShop;
                            break;

                        case ShopSelection::MiniGameShop:
                            std::cout << "Mini Game Shop selected\n";
                            if (miniGameShopView) delete miniGameShopView;
                            miniGameShopView = new MiniGameShopView(font, playerData, *this);
                            miniGameShopView->init();
                            state = GameState::MiniGameShop;
                            break;

                        default:
                            break;
                        }

                        shopCategoryView->clearSelection();
                    }
                }


                break;

            case GameState::ShelfShop:
                if (shelfShopView)
                    shelfShopView->handleInput(event.key.code);

                if (shelfShopView && shelfShopView->shouldClose()) {
                    shelfShopView->resetCloseFlag();
                    state = GameState::ShopCategoryView;
                }
                break;


            case GameState::AquariumView:
                if (aquariumView)
                    aquariumView->handleInput(event.key.code);

                if (aquariumView && aquariumView->shouldClose()) {
                    aquariumView->resetCloseFlag();
                    state = GameState::RoomView;
                }
                break;

            case GameState::FishTankShop:
                if (fishTankShopView)
                    fishTankShopView->handleInput(event.key.code);

                if (fishTankShopView && fishTankShopView->shouldClose()) {
                    fishTankShopView->resetCloseFlag();
                    delete fishTankShopView;
                    fishTankShopView = nullptr;
                    state = GameState::ShopCategoryView;
                }
                break;

            case GameState::MiniGameShop:
                if (miniGameShopView)
                    miniGameShopView->handleInput(event.key.code);

                if (miniGameShopView && miniGameShopView->shouldClose()) {
                    miniGameShopView->resetCloseFlag();
                    delete miniGameShopView;
                    miniGameShopView = nullptr;

                    if (computerView) delete computerView;
                    computerView = new Computer(font, playerData);
                    computerView->init();

                    state = GameState::ComputerView;
                }
                break;


            }
        }
    }
}


void GameManager::update() {
    if (state == GameState::StartMenu)
        updateStartMenu();
    else if (state == GameState::RoomView && roomView)
        roomView->update();
    else if (state == GameState::ComputerView && computerView)
        computerView->update();
    else if (state == GameState::AquariumView && aquariumView)
        aquariumView->update();
    else if (state == GameState::ShelfView && shelfView)
        shelfView->update();
    else if (state == GameState::StorageView && storageRackView)
        storageRackView->update();
    else if (state == GameState::ShopCategoryView && shopCategoryView)
        shopCategoryView->update();
    else if (state == GameState::FishTankShop && fishTankShopView)
        fishTankShopView->update(); 


}



void GameManager::render() {
    window.clear();
    if (state == GameState::StartMenu)
        renderStartMenu();
    else if (state == GameState::RoomView && roomView)
        roomView->render(window);
    else if (state == GameState::ComputerView && computerView)
        computerView->render(window);
    else if (state == GameState::StorageView && storageRackView)
        storageRackView->render(window);
    else if (state == GameState::MiniGame)
        renderMiniGame();
    else if (state == GameState::ShelfView && shelfView)
        shelfView->render(window);
    else if (state == GameState::ShopCategoryView && shopCategoryView)
        shopCategoryView->render(window);
    else if (state == GameState::ShelfShop && shelfShopView)
        shelfShopView->render(window);
    else if (state == GameState::FishTankShop && fishTankShopView)
        fishTankShopView->render(window); 
    else if (state == GameState::MiniGameShop && miniGameShopView)
        miniGameShopView->render(window);
    else if (state == GameState::AquariumView && aquariumView)
        aquariumView->render(window);
    else if (state == GameState::HatShop && hatShopView)
        hatShopView->render(window);



    window.display();
}


void GameManager::moveUp() {
    if (menuItems.empty()) return; 
    if (selectedIndex > 0)
        selectedIndex--;
}

void GameManager::moveDown() {
    if (menuItems.empty()) return; 
    if (selectedIndex < static_cast<int>(menuItems.size()) - 1)
        selectedIndex++;
}


void GameManager::selectOption() {
    if (menuItems.empty() || selectedIndex >= static_cast<int>(menuItems.size())) {
        std::cout << "Invalid menu state. Skipping selection.\n";
        return;
    }

    switch (selectedIndex) {
    case 0: // New Game
        std::cout << "New Game Selected\n";
        playerData = Player(); // Reset all values
        playerData.coins = 100;
        playerData.equippedHat = "none";
        playerData.unlockedHats = { "cap", "crown" }; // default hats
        playerData.saveToFile("save.json");

        if (roomView) delete roomView;
        roomView = new Room(font, playerData);
        roomView->init();

        state = GameState::RoomView;
        break;

    case 1: // Load Game
        std::cout << "Load Game Selected\n";
        if (playerData.loadFromFile("save.json")) {
            std::cout << "Coins: " << playerData.coins << ", Hat: " << playerData.equippedHat << "\n";

            if (roomView) delete roomView;
            roomView = new Room(font, playerData);
            roomView->init();

            state = GameState::RoomView;
        }
        else {
            std::cout << "No save file found.\n";
        }
        break;

    case 2: // Exit
        window.close();
        break;
    }
}



void GameManager::renderStartMenu() {
    for (const auto& item : menuItems) {
        window.draw(item);
    }
}



void GameManager::updateStartMenu() {
    if (menuItems.empty()) return; 

    for (size_t i = 0; i < menuItems.size(); ++i) {
        menuItems[i].setFillColor(i == selectedIndex ? sf::Color::Yellow : sf::Color::White);
    }
}



void GameManager::initMiniGame() {
    std::cout << "[Mini Game Initialized]\n";
}

void GameManager::renderMiniGame() {
    sf::Text text;
    text.setFont(font);
    text.setString("Mini Game Coming Soon! Press ESC to return.");
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::White);
    text.setPosition(100.f, 200.f);
    window.draw(text);
}

void GameManager::handleMiniGameInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        std::cout << "Exiting Mini Game\n";
        state = GameState::ComputerView;
    }
}


void GameManager::drawSectionTitle(sf::RenderWindow& window, sf::Font& font, const std::string& title) {
    sf::Text titleText(title, font, 36);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setPosition(100.f, 50.f);
    window.draw(titleText);
}

void GameManager::drawCoinDisplay(sf::RenderWindow& window, sf::Font& font, int coins) {
    sf::Text coinText;
    coinText.setFont(font);
    coinText.setCharacterSize(24);
    coinText.setFillColor(sf::Color::White);
    coinText.setString("Coins: " + std::to_string(coins));
    coinText.setPosition(500.f, 50.f);
    window.draw(coinText);
}

template<typename T>
void GameManager::handleShopNavigationInput(sf::Keyboard::Key key, int& selectionIndex, const std::vector<T>& items) {
    if (key == sf::Keyboard::W || key == sf::Keyboard::Up) {
        if (selectionIndex > 0) selectionIndex--;
    }
    else if (key == sf::Keyboard::S || key == sf::Keyboard::Down) {
        if (selectionIndex < static_cast<int>(items.size()) - 1) selectionIndex++;
    }
}

