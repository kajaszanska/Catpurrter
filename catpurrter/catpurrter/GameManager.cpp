#include "GameManager.h"
#include "Player.h"
#include "Room.h"
#include "Aquarium.h"
#include "Shelf.h"
#include "StorageRack.h"
#include "Computer.h"
#include "SnakeGame.h"
#include "CatchGame.h"
#include "DodgeGame.h"

#include <iostream>

GameManager::GameManager()
    : window(sf::VideoMode(800, 600), "Catpurrter - Start Menu"), selectedIndex(0), state(GameState::StartMenu)
{
    window.setKeyRepeatEnabled(false);
    loadFont();
    initMenu();
}

void GameManager::loadFont() {
    if (!font.loadFromFile("assets/fonts/Roboto-VariableFont_wdth,wght.ttf")) {
        std::cerr << "Error loading font\n";
    }
}

void GameManager::initMenu() {
    if (!startMenuBgTexture.loadFromFile("assets/graphics/start.png")) {
        std::cout << "Failed to load start menu background!\n";
    }
    startMenuBgSprite.setTexture(startMenuBgTexture);

    std::vector<std::string> options = { "New Game", "Load Game", "Exit" };
    for (size_t i = 0; i < options.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(options[i]);
        text.setCharacterSize(40);
        text.setPosition(300.f, 200.f + i * 60.f);
        text.setFillColor(i == selectedIndex ? sf::Color(200, 170, 40) : sf::Color(120, 60, 255));
        menuItems.push_back(text);
    }
}

void GameManager::run() {
    while (window.isOpen()) {
        float dt = frameClock.restart().asSeconds();
        processEvents();
        handleContinuousMovement();
        update(dt);
        render();
    }
}

void GameManager::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::KeyPressed)
            keyState[event.key.code] = true;
        if (event.type == sf::Event::KeyReleased)
            keyState[event.key.code] = false;

        if (event.type == sf::Event::KeyPressed) {
            switch (state) {
            case GameState::StartMenu:
                processStartMenuEvents(event);
                break;
            case GameState::RoomView:
                processRoomViewEvents(event);
                break;
            case GameState::ComputerView:
                processComputerViewEvents(event);
                break;
            case GameState::StorageView:
                processStorageViewEvents(event);
                break;
            case GameState::HatShop:
                processHatShopEvents(event);
                break;
            case GameState::MiniGame:
                processMiniGameEvents(event);
                break;
            case GameState::ShelfView:
                processShelfViewEvents(event);
                break;
            case GameState::ShopCategoryView:
                processShopCategoryViewEvents(event);
                break;
            case GameState::ShelfShop:
                processShelfShopEvents(event);
                break;
            case GameState::AquariumView:
                processAquariumViewEvents(event);
                break;
            case GameState::FishTankShop:
                processFishTankShopEvents(event);
                break;
            case GameState::MiniGameShop:
                processMiniGameShopEvents(event);
                break;
            }
        }
    }
}

void GameManager::processStartMenuEvents(const sf::Event& event) {
    if (showingNewGameConfirm) {
        if ((event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) && confirmIndex > 0)
            confirmIndex--;
        else if ((event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) && confirmIndex < 1)
            confirmIndex++;
        else if (event.key.code == sf::Keyboard::Enter) {
            if (confirmIndex == 0) {
                playerData = Player();
                playerData.coins = 10000;
                playerData.equippedHat = "none";
                playerData.unlockedHats = {};
                playerData.saveToFile("save.json");
                if (roomView) delete roomView;
                roomView = new Room(font, playerData);
                roomView->init();
                state = GameState::RoomView;
            }
            showingNewGameConfirm = false;
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            showingNewGameConfirm = false;
        }
        return;
    }
    if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
        moveUp();
    if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
        moveDown();
    if (event.key.code == sf::Keyboard::Enter)
        selectOption();
}

void GameManager::processRoomViewEvents(const sf::Event& event) {
    if (roomView) {
        roomView->handleInput(event.key.code);
        roomView->update();
        if ((event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) && roomView->isNearObject()) {
            std::string obj = roomView->getNearbyObject();
            if (obj == "Computer") {
                state = GameState::ComputerView;
                if (computerView) delete computerView;
                computerView = new Computer(font, playerData);
                computerView->init();
            }
            else if (obj == "Aquarium") {
                state = GameState::AquariumView;
                if (aquariumView) delete aquariumView;
                aquariumView = new Aquarium(font, playerData);
                aquariumView->init();
            }
            else if (obj == "Shelves") {
                state = GameState::ShelfView;
                if (shelfView) delete shelfView;
                shelfView = new Shelf(font, playerData);
                shelfView->init();
            }
            else if (obj == "Storage Rack") {
                state = GameState::StorageView;
                if (storageRackView) delete storageRackView;
                storageRackView = new StorageRack(font, playerData, this);
                storageRackView->init();
            }
            else if (obj == "Doors") {
                state = GameState::StartMenu;
                selectedIndex = 0;
                menuItems.clear();
                initMenu();
            }
        }
    }
}

void GameManager::processComputerViewEvents(const sf::Event& event) {
    computerView->handleInput(event.key.code);
    if (computerView->shouldClose()) {
        delete computerView;
        computerView = nullptr;
        state = GameState::RoomView;
        return;
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
            if (selectedGame == "snake") {
                if (snakeGame) delete snakeGame;
                snakeGame = new SnakeGame(font, playerData, *this);
                snakeGame->init();
                state = GameState::MiniGame;
            }
            else if (selectedGame == "catch") {
                if (catchGame) delete catchGame;
                catchGame = new CatchGame(font, playerData, *this);
                catchGame->init();
                state = GameState::MiniGame;
            }
            else if (selectedGame == "dodge") {
                if (dodgeGame) delete dodgeGame;
                dodgeGame = new DodgeGame(font, playerData, *this);
                dodgeGame->init();
                state = GameState::MiniGame;
            }
            computerView->clearSelectedMiniGame();
            return;
        }
    }
}

void GameManager::processStorageViewEvents(const sf::Event& event) {
    if (storageRackView)
        storageRackView->handleInput(event.key.code);
    if (storageRackView && storageRackView->shouldClose()) {
        storageRackView->resetCloseFlag();
        delete storageRackView;
        storageRackView = nullptr;
        state = GameState::RoomView;
    }
}

void GameManager::processHatShopEvents(const sf::Event& event) {
    if (hatShopView)
        hatShopView->handleInput(event.key.code);
    if (hatShopView && hatShopView->shouldClose()) {
        hatShopView->resetCloseFlag();
        delete hatShopView;
        hatShopView = nullptr;
        state = GameState::ShopCategoryView;
    }
}

void GameManager::processMiniGameEvents(const sf::Event& event) {
    if (snakeGame)
        snakeGame->handleInput(event.key.code);
    else if (catchGame)
        catchGame->handleInput(event.key.code);
    else if (dodgeGame)
        dodgeGame->handleInput(event.key.code);
}

void GameManager::processShelfViewEvents(const sf::Event& event) {
    if (shelfView)
        shelfView->handleInput(event.key.code);
    if (shelfView && shelfView->shouldClose()) {
        shelfView->resetCloseFlag();
        state = GameState::RoomView;
    }
}

void GameManager::processShopCategoryViewEvents(const sf::Event& event) {
    if (shopCategoryView)
        shopCategoryView->handleInput(event.key.code);
    if (shopCategoryView && shopCategoryView->shouldClose()) {
        shopCategoryView->resetCloseFlag();
        state = GameState::ComputerView;
    }
    else if (shopCategoryView) {
        ShopSelection selected = shopCategoryView->getSelectedOption();
        if (selected != ShopSelection::None) {
            switch (selected) {
            case ShopSelection::HatShop:
                if (hatShopView) delete hatShopView;
                hatShopView = new HatShopView(font, playerData, *this);
                hatShopView->init();
                state = GameState::HatShop;
                break;
            case ShopSelection::FishTankShop:
                if (fishTankShopView) delete fishTankShopView;
                fishTankShopView = new FishTankShopView(font, playerData, *this);
                fishTankShopView->init();
                state = GameState::FishTankShop;
                break;
            case ShopSelection::ShelfShop:
                if (shelfShopView) delete shelfShopView;
                shelfShopView = new ShelfShopView(font, playerData, *this);
                shelfShopView->init();
                state = GameState::ShelfShop;
                break;
            case ShopSelection::MiniGameShop:
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
}

void GameManager::processShelfShopEvents(const sf::Event& event) {
    if (shelfShopView)
        shelfShopView->handleInput(event.key.code);
    if (shelfShopView && shelfShopView->shouldClose()) {
        shelfShopView->resetCloseFlag();
        state = GameState::ShopCategoryView;
    }
}

void GameManager::processAquariumViewEvents(const sf::Event& event) {
    if (aquariumView)
        aquariumView->handleInput(event.key.code);
    if (aquariumView && aquariumView->shouldClose()) {
        aquariumView->resetCloseFlag();
        state = GameState::RoomView;
    }
}

void GameManager::processFishTankShopEvents(const sf::Event& event) {
    if (fishTankShopView)
        fishTankShopView->handleInput(event.key.code);
    if (fishTankShopView && fishTankShopView->shouldClose()) {
        fishTankShopView->resetCloseFlag();
        delete fishTankShopView;
        fishTankShopView = nullptr;
        state = GameState::ShopCategoryView;
    }
}

void GameManager::processMiniGameShopEvents(const sf::Event& event) {
    if (miniGameShopView)
        miniGameShopView->handleInput(event.key.code);
    if (miniGameShopView && miniGameShopView->shouldClose()) {
        miniGameShopView->resetCloseFlag();
        delete miniGameShopView;
        miniGameShopView = nullptr;
        if (computerView) delete computerView;
        computerView = new Computer(font, playerData);
        computerView->init();
        if (shopCategoryView) delete shopCategoryView;
        shopCategoryView = new ShopCategoryView(font);
        shopCategoryView->init();
        shopCategoryView->setSelectionIndex(3);
        state = GameState::ShopCategoryView;
    }
}



void GameManager::update(float dt) {
    switch (state) {
    case GameState::StartMenu:
        updateStartMenu();
        break;
    case GameState::RoomView:
        if (roomView) roomView->update();
        break;
    case GameState::ComputerView:
        if (computerView) computerView->update();
        break;
    case GameState::AquariumView:
        if (aquariumView) aquariumView->update(dt);
        break;
    case GameState::ShelfView:
        if (shelfView) shelfView->update();
        break;
    case GameState::StorageView:
        if (storageRackView) storageRackView->update();
        break;
    case GameState::ShopCategoryView:
        if (shopCategoryView) shopCategoryView->update();
        break;
    case GameState::FishTankShop:
        if (fishTankShopView) fishTankShopView->update();
        break;
    case GameState::MiniGame:
        if (snakeGame)
            snakeGame->update(dt);
        if (catchGame)
            catchGame->update(dt);
        if (dodgeGame)
            dodgeGame->update(dt);
        if (snakeGame && snakeGame->shouldClose()) {
            delete snakeGame;
            snakeGame = nullptr;
            state = GameState::ComputerView;
        }
        if (catchGame && catchGame->shouldClose()) {
            delete catchGame;
            catchGame = nullptr;
            state = GameState::ComputerView;
        }
        if (dodgeGame && dodgeGame->shouldClose()) {
            delete dodgeGame;
            dodgeGame = nullptr;
            state = GameState::ComputerView;
        }
        break;
    }
}


void GameManager::render() {
    window.clear();
    switch (state) {
    case GameState::StartMenu:
        renderStartMenu();
        break;
    case GameState::RoomView:
        if (roomView) roomView->render(window);
        drawCoinDisplay(window, font, playerData.coins, sf::Vector2f(30.f, 20.f));
        break;
    case GameState::ComputerView:
        if (computerView) computerView->render(window);
        break;
    case GameState::AquariumView:
        if (aquariumView) aquariumView->render(window);
        break;
    case GameState::ShelfView:
        if (shelfView) shelfView->render(window);
        break;
    case GameState::StorageView:
        if (storageRackView) storageRackView->render(window);
        break;
    case GameState::ShopCategoryView:
        if (shopCategoryView) shopCategoryView->render(window);
        break;
    case GameState::ShelfShop:
        if (shelfShopView) shelfShopView->render(window);
        break;
    case GameState::FishTankShop:
        if (fishTankShopView) fishTankShopView->render(window);
        break;
    case GameState::MiniGameShop:
        if (miniGameShopView) miniGameShopView->render(window);
        break;
    case GameState::HatShop:
        if (hatShopView) hatShopView->render(window);
        break;
    case GameState::MiniGame:
        if (snakeGame)
            snakeGame->render(window);
        if (catchGame)
            catchGame->render(window);
        if (dodgeGame)
            dodgeGame->render(window);
        break;
    }
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
    case 0:
        std::cout << "New Game Selected - confirmation required\n";
        showingNewGameConfirm = true;
        confirmIndex = 0;
        break;
    case 1:
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
    case 2:
        window.close();
        break;
    }
}

void GameManager::renderStartMenu() {
    window.draw(startMenuBgSprite);
    for (const auto& item : menuItems)
        window.draw(item);

    if (showingNewGameConfirm) {
        sf::RectangleShape popup(sf::Vector2f(540, 170));
        popup.setFillColor(sf::Color::White);
        popup.setOutlineThickness(4.f);
        popup.setOutlineColor(sf::Color(120, 60, 255));
        popup.setPosition(130, 180);
        window.draw(popup);

        sf::Text question("Are you sure you want to run new game?\nPrevious save will be deleted", font, 26);
        question.setFillColor(sf::Color(120, 60, 255));
        question.setPosition(150, 200);
        window.draw(question);

        sf::Text yesText("Yes", font, 32);
        sf::Text noText("No", font, 32);

        sf::Color purple(120, 60, 255);
        sf::Color yellow(200, 170, 40);

        yesText.setFillColor(confirmIndex == 0 ? yellow : purple);
        noText.setFillColor(confirmIndex == 1 ? yellow : purple);

        yesText.setPosition(250, 300);
        noText.setPosition(450, 300);

        window.draw(yesText);
        window.draw(noText);
    }
}

void GameManager::updateStartMenu() {
    if (menuItems.empty()) return;
    for (size_t i = 0; i < menuItems.size(); ++i) {
        menuItems[i].setFillColor(
            i == selectedIndex ? sf::Color(200, 170, 40) : sf::Color(120, 60, 255)
        );
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

void GameManager::drawSectionTitle(sf::RenderWindow& window, sf::Font& font, const std::string& title) {
    sf::Text titleText(title, font, 36);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setPosition(100.f, 50.f);
    window.draw(titleText);
}

void GameManager::drawCoinDisplay(sf::RenderWindow& window, sf::Font& font, int coins, sf::Vector2f pos) {
    sf::Text currencyText;
    currencyText.setFont(font);
    currencyText.setCharacterSize(24);
    currencyText.setFillColor(sf::Color::White);
    currencyText.setString("Coins: " + std::to_string(coins));
    currencyText.setPosition(pos);
    window.draw(currencyText);
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

void GameManager::handleContinuousMovement() {
    if (state == GameState::RoomView && roomView) {
        int dx = 0, dy = 0;
        if (keyState[sf::Keyboard::A] || keyState[sf::Keyboard::Left])  dx -= 1;
        if (keyState[sf::Keyboard::D] || keyState[sf::Keyboard::Right]) dx += 1;
        if (keyState[sf::Keyboard::W] || keyState[sf::Keyboard::Up])    dy -= 1;
        if (keyState[sf::Keyboard::S] || keyState[sf::Keyboard::Down])  dy += 1;
        if (dx != 0 || dy != 0)
            roomView->movePlayer(dx, dy);
    }
}

void GameManager::setState(GameState s) {
    state = s;
}

GameState GameManager::getState() const {
    return state;
}
