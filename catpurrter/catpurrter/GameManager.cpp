#include "GameManager.h"
#include "Player.h"
#include <iostream>

GameManager::GameManager()
    : window(sf::VideoMode(800, 600), "Catpurrter - Start Menu"), selectedIndex(0), state(GameState::StartMenu)
{
    window.setKeyRepeatEnabled(false); 
    loadFont();
    initMenu();
    roomSelectionIndex = 0;
    initRoomObjects();
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
            if (state == GameState::StartMenu) {
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) moveUp();
                if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) moveDown();
                if (event.key.code == sf::Keyboard::Enter) selectOption();
            }
            else if (state == GameState::RoomView) {
                     handleRoomInput(event.key.code);
                     }
            else if (state == GameState::ComputerView) {
                if (event.key.code == sf::Keyboard::Escape) {
                    std::cout << "Closed Computer\n";
                    state = GameState::RoomView;
                }
                else {
                    handleComputerInput(event.key.code);
                }
            }
            else if (state == GameState::StorageView) {
                if (event.key.code == sf::Keyboard::Escape) {
                    std::cout << "Leaving Storage\n";
                    state = GameState::RoomView;
                }
                else {
                    handleStorageInput(event.key.code);  
                }
            }
            else if (state == GameState::ShopView) {
                if (event.key.code == sf::Keyboard::Escape) {
                    std::cout << "Leaving Shop\n";
                    state = GameState::ComputerView;
                }
                else {
                    handleShopInput(event.key.code);
                }
            }



        }
    }
}



void GameManager::update() {
    if (state == GameState::StartMenu)
        updateStartMenu();
    else if (state == GameState::RoomView)
        updateRoomView();
    else if (state == GameState::ComputerView)
        updateComputerView();

}


void GameManager::render() {
    window.clear();
    if (state == GameState::StartMenu)
        renderStartMenu();
    else if (state == GameState::RoomView)
        renderRoomView();
    else if (state == GameState::ComputerView)
        renderComputerView();
    else if (state == GameState::StorageView)
        renderStorageView();
    else if (state == GameState::ShopView) {
        renderShopView();
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
    case 0: // New Game
        std::cout << "New Game Selected\n";
        playerData = Player(); // Reset all values
        playerData.coins = 100;
        playerData.equippedHat = "none";
        playerData.unlockedHats = { "cap", "crown" }; // default hats
        playerData.saveToFile("save.json");
        roomSelectionIndex = 0;
        roomObjects.clear();
        initRoomObjects();
        state = GameState::RoomView;
        break;


    case 1: // Load Game
        std::cout << "Load Game Selected\n";
        if (playerData.loadFromFile("save.json")) {
            std::cout << "Coins: " << playerData.coins << ", Hat: " << playerData.equippedHat << "\n";
            roomSelectionIndex = 0;
            roomObjects.clear();
            initRoomObjects(); 
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
    for (const auto& item : menuItems) {
        window.draw(item);
    }
}

void GameManager::renderRoomView() {
    sf::Text roomText("Welcome to your room!", font, 30);
    roomText.setPosition(200.f, 250.f);
    roomText.setFillColor(sf::Color::Cyan);
    window.draw(roomText);

    // Draw interactable items
    for (const auto& obj : roomObjects) {
        window.draw(obj);
    }
}


void GameManager::updateStartMenu() {
    if (menuItems.empty()) return; 

    for (size_t i = 0; i < menuItems.size(); ++i) {
        menuItems[i].setFillColor(i == selectedIndex ? sf::Color::Yellow : sf::Color::White);
    }
}


void GameManager::updateRoomView() {
    // Nothing yet — placeholder for future
}

void GameManager::initRoomObjects() {
    std::vector<std::string> labels = {
        "Aquarium", "Computer", "Storage Rack", "Shelves", "Doors"
    };

    for (size_t i = 0; i < labels.size(); ++i) {
        sf::Text item;
        item.setFont(font);
        item.setString(labels[i]);
        item.setCharacterSize(24);
        item.setPosition(100.f + i * 130.f, 500.f);
        item.setFillColor(i == roomSelectionIndex ? sf::Color::Yellow : sf::Color::White);
        roomObjects.push_back(item);
    }
}

void GameManager::handleRoomInput(sf::Keyboard::Key key) {
    if ((key == sf::Keyboard::A || key == sf::Keyboard::Left) && roomSelectionIndex > 0) {
        roomSelectionIndex--;
    }
    if ((key == sf::Keyboard::D || key == sf::Keyboard::Right) && roomSelectionIndex < static_cast<int>(roomObjects.size()) - 1) {
        roomSelectionIndex++;
    }

    for (size_t i = 0; i < roomObjects.size(); ++i) {
        roomObjects[i].setFillColor(i == roomSelectionIndex ? sf::Color::Yellow : sf::Color::White);
    }

    if (key == sf::Keyboard::Enter) {
        std::string selected = roomObjects[roomSelectionIndex].getString().toAnsiString();
        std::cout << "Selected: " << selected << "\n";

        if (selected == "Computer") {
            state = GameState::ComputerView;
            initComputerView();
            std::cout << "Opened Computer\n";
        }
        else if (selected == "Doors") {
            std::cout << "Returning to Start Menu via Doors\n";
            state = GameState::StartMenu;
            selectedIndex = 0;
            menuItems.clear();
            initMenu();
        }
        else if (selected == "Storage Rack") {
            std::cout << "Entered Storage Rack\n";
            state = GameState::StorageView;
            initStorageView(); 
        }

    }

}

void GameManager::initComputerView() {
    computerOptions.clear();
    computerSelectionIndex = 0;

    std::vector<std::string> labels = { "Shop", "MiniGames", "Back" };

    for (size_t i = 0; i < labels.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(labels[i]);
        text.setCharacterSize(28);
        text.setPosition(200.f + i * 180.f, 400.f);
        text.setFillColor(i == computerSelectionIndex ? sf::Color::Yellow : sf::Color::White);
        computerOptions.push_back(text);
    }
}

void GameManager::renderComputerView() {
    sf::Text title("Computer Apps", font, 32);
    title.setPosition(250.f, 200.f);
    title.setFillColor(sf::Color::Cyan);
    window.draw(title);

    for (const auto& app : computerOptions) {
        window.draw(app);
    }
}

void GameManager::updateComputerView() {
    for (size_t i = 0; i < computerOptions.size(); ++i) {
        computerOptions[i].setFillColor(i == computerSelectionIndex ? sf::Color::Yellow : sf::Color::White);
    }
}
void GameManager::handleComputerInput(sf::Keyboard::Key key) {
    if ((key == sf::Keyboard::A || key == sf::Keyboard::Left) && computerSelectionIndex > 0) {
        computerSelectionIndex--;
    }
    if ((key == sf::Keyboard::D || key == sf::Keyboard::Right) && computerSelectionIndex < static_cast<int>(computerOptions.size()) - 1) {
        computerSelectionIndex++;
    }

    if (key == sf::Keyboard::Enter) {
        std::string selected = computerOptions[computerSelectionIndex].getString().toAnsiString();
        std::cout << "Selected Computer Option: " << selected << "\n";

        if (selected == "Back") {
            state = GameState::RoomView;
        }
        else if (selected == "Shop") {
            std::cout << "Opened Shop\n";
            state = GameState::ShopView;
            initShopView();
        }
        // Later: add Shop or MiniGames logic here
    }
}

void GameManager::initStorageView() {
    storageOptions.clear();
    storageSelectionIndex = 0;

    float y = 200.f;
    for (const std::string& hat : playerData.unlockedHats) {
        std::string label = hat;
        if (hat == playerData.equippedHat) {
            label += " (Equipped)";
        }

        sf::Text text;
        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(36);
        text.setFillColor(sf::Color::White);
        text.setPosition(100.f, y);
        storageOptions.push_back(text);

        y += 50.f;
    }
}


void GameManager::renderStorageView() {
    for (size_t i = 0; i < storageOptions.size(); ++i) {
        if (i == storageSelectionIndex)
            storageOptions[i].setFillColor(sf::Color::Yellow);
        else
            storageOptions[i].setFillColor(sf::Color::White);

        window.draw(storageOptions[i]);
    }
}

void GameManager::handleStorageInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        std::cout << "Leaving Storage Rack\n";
        state = GameState::RoomView;
        return;
    }

    if (storageOptions.empty()) return;

    if (key == sf::Keyboard::W || key == sf::Keyboard::Up) {
        if (storageSelectionIndex > 0) storageSelectionIndex--;
    }
    else if (key == sf::Keyboard::S || key == sf::Keyboard::Down) {
        if (storageSelectionIndex < static_cast<int>(storageOptions.size()) - 1)
            storageSelectionIndex++;
    }
    else if (key == sf::Keyboard::Enter) {
        int previousIndex = storageSelectionIndex; 

        std::string selectedHat = playerData.unlockedHats[storageSelectionIndex];
        playerData.equippedHat = selectedHat;
        playerData.saveToFile("save.json");

        std::cout << "Equipped hat: " << selectedHat << "\n";

        initStorageView(); 

     
        if (previousIndex >= static_cast<int>(storageOptions.size())) {
            storageSelectionIndex = static_cast<int>(storageOptions.size()) - 1;
        }
        else {
            storageSelectionIndex = previousIndex;
        }
    }


}


void GameManager::initShopView() {
    shopItems.clear();
    shopVisualItems.clear();
    shopSelectionIndex = 0;

    float y = 100.f;
    for (const auto& hat : shopCatalog) {
        std::string name = hat.first;
        int price = hat.second;

        shopItems.push_back({ name, price });

        std::string label = name + " - " + std::to_string(price) + " coins";
        if (std::find(playerData.unlockedHats.begin(), playerData.unlockedHats.end(), name) != playerData.unlockedHats.end()) {
            label += " (Owned)";
        }

        sf::Text item;
        item.setFont(font);
        item.setString(label);
        item.setCharacterSize(24);
        item.setPosition(100.f, y);
        item.setFillColor(sf::Color::White);
        shopVisualItems.push_back(item);

        y += 40.f;
    }

}


void GameManager::renderShopView() {
    sf::Text title;
    title.setFont(font);
    title.setString("Welcome to the Hat Shop!");
    title.setCharacterSize(30);
    title.setFillColor(sf::Color::Cyan);
    title.setPosition(100.f, 30.f);
    window.draw(title);

    // Display player's current coins
    sf::Text coinText;
    coinText.setFont(font);
    coinText.setString("Coins: " + std::to_string(playerData.coins));
    coinText.setCharacterSize(24);
    coinText.setFillColor(sf::Color::White);
    coinText.setPosition(100.f, 70.f); 
    window.draw(coinText);


    // Draw visual items
    for (size_t i = 0; i < shopVisualItems.size(); ++i) {
        if (i == shopSelectionIndex)
            shopVisualItems[i].setFillColor(sf::Color::Yellow);
        else
            shopVisualItems[i].setFillColor(sf::Color::White);

        window.draw(shopVisualItems[i]);
    }
}



void GameManager::handleShopInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        std::cout << "Leaving Shop\n";
        state = GameState::ComputerView;
        return;
    }

    if (shopItems.empty()) return;

    if (key == sf::Keyboard::W || key == sf::Keyboard::Up) {
        if (shopSelectionIndex > 0) shopSelectionIndex--;
    }
    else if (key == sf::Keyboard::S || key == sf::Keyboard::Down) {
        if (shopSelectionIndex < static_cast<int>(shopItems.size()) - 1) shopSelectionIndex++;
    }
    else if (key == sf::Keyboard::Enter) {
        std::string selectedHat = shopItems[shopSelectionIndex].name;
        int hatCost = shopItems[shopSelectionIndex].cost;

        // Check if already unlocked
        if (std::find(playerData.unlockedHats.begin(), playerData.unlockedHats.end(), selectedHat) != playerData.unlockedHats.end()) {
            std::cout << "Already owned: " << selectedHat << "\n";
        }
        else if (playerData.coins >= hatCost) {
            // Buy and unlock
            playerData.coins -= hatCost;
            playerData.unlockedHats.push_back(selectedHat);
            playerData.saveToFile("save.json");
            std::cout << "Unlocked hat: " << selectedHat << " for " << hatCost << " coins\n";
        }
        else {
            std::cout << "Not enough coins for: " << selectedHat << "\n";
        }

    }
}

