#include "StorageRack.h"
#include "GameManager.h"
#include <iostream>

StorageRack::StorageRack(const sf::Font& font, Player& player, GameManager* gm)
    : font(font), playerData(player), gameManager(gm) {
}


void StorageRack::init() {
    // 1. Load background
    if (!backgroundTexture.loadFromFile("assets/graphics/storagerack/srclose.png")) {
        std::cout << "Failed to load storage rack bg!\n";
    }
    backgroundSprite.setTexture(backgroundTexture);

    // 2. Define positions (adjust as needed)
    hatPositions = {
        {190.f, 150.f},
        {410.f, 150.f},
        {190.f, 340.f},
        {410.f, 340.f}
    };

    // 3. Load all big hat textures
    hatTextures.clear();
    for (const auto& hat : playerData.unlockedHats) {
        sf::Texture tex;
        std::string path = "assets/graphics/storagerack/" + hat + "big.png";
        if (tex.loadFromFile(path)) {
            hatTextures[hat] = tex;
        }
        else {
            std::cout << "Couldn't load hat: " << path << "\n";
        }
    }

    // 4. Set selectionIndex to the equipped hat (if found)
    selectionIndex = 0; // default to 0 if none found
    for (size_t i = 0; i < playerData.unlockedHats.size(); ++i) {
        if (playerData.unlockedHats[i] == playerData.equippedHat) {
            selectionIndex = static_cast<int>(i);
            break;
        }
    }
}



void StorageRack::update() {
   
}

void StorageRack::render(sf::RenderWindow& window) {
    window.draw(backgroundSprite);

    // Draw hats (up to 4)
    for (size_t i = 0; i < playerData.unlockedHats.size() && i < hatPositions.size(); ++i) {
        const auto& hatId = playerData.unlockedHats[i];
        auto it = hatTextures.find(hatId);
        if (it != hatTextures.end()) {
            sf::Sprite hatSprite;
            hatSprite.setTexture(it->second);
            hatSprite.setPosition(hatPositions[i]);

            // Red tint if this hat is equipped
            if (hatId == playerData.equippedHat) {
                hatSprite.setColor(sf::Color(255, 100, 100)); // Red highlight
            }
            else {
                hatSprite.setColor(sf::Color::White); // Normal color
            }

            window.draw(hatSprite);

            // Yellow rectangle outline for the selection cursor
            if (i == selectionIndex) {
                sf::RectangleShape selectOutline({
                    hatSprite.getGlobalBounds().width,
                    hatSprite.getGlobalBounds().height
                    });
                selectOutline.setPosition(hatSprite.getPosition());
                selectOutline.setFillColor(sf::Color::Transparent);
                selectOutline.setOutlineColor(sf::Color::Yellow);
                selectOutline.setOutlineThickness(4.f);
                window.draw(selectOutline);
            }
        }
    }

    // Draw equipped label at top center
    sf::Text info;
    info.setFont(font);
    info.setCharacterSize(28);
    info.setFillColor(sf::Color::White);
    // Replace the line below:
    info.setString("Equipped: " + (playerData.equippedHat.empty() ? std::string("none") : playerData.equippedHat));
    sf::FloatRect textRect = info.getLocalBounds();
    info.setOrigin(textRect.width / 2.f, 0);
    info.setPosition(window.getSize().x / 2.f, 30.f); // Top center
    window.draw(info);

}




void StorageRack::handleInput(sf::Keyboard::Key key) {
    std::cout << "[StorageRack::handleInput] Got key: " << key << std::endl;



    if (key == sf::Keyboard::Escape) {
        std::cout << "[StorageRack] ESC pressed!\n";
        closeRequested = true;
        return;
    }


    size_t hatsCount = playerData.unlockedHats.size();
    if (hatsCount == 0) return;





    int cols = 2; // How many columns in your grid?
 

    // Left/Right navigation
    if ((key == sf::Keyboard::Right || key == sf::Keyboard::D) && selectionIndex + 1 < static_cast<int>(hatsCount)) {
        selectionIndex++;
    }

    else if ((key == sf::Keyboard::Left || key == sf::Keyboard::A) && selectionIndex > 0) {
        selectionIndex--;
    }
    // Up/Down navigation (assuming 2 columns, adjust as needed)
    else if ((key == sf::Keyboard::Up || key == sf::Keyboard::W) && selectionIndex - cols >= 0) {
        selectionIndex -= cols;
    }
    else if ((key == sf::Keyboard::Down || key == sf::Keyboard::S) && selectionIndex + static_cast<int>(cols) < static_cast<int>(hatsCount)) {
        selectionIndex += static_cast<int>(cols);
    }

    // Enter to equip
    if (key == sf::Keyboard::Enter || key == sf::Keyboard::Space) {
        if (selectionIndex < static_cast<int>(hatsCount)) {
            std::string selectedHat = playerData.unlockedHats[selectionIndex];
            if (playerData.equippedHat == selectedHat) {
                // Unwear hat
                playerData.equippedHat = "";
                std::cout << "Unequipped hat.\n";
            }
            else {
                // Equip hat
                playerData.equippedHat = selectedHat;
                std::cout << "Equipped hat: " << playerData.equippedHat << std::endl;
            }
            playerData.saveToFile("save.json");
        }
    }
}



bool StorageRack::shouldClose() const {
    return closeRequested;
}

void StorageRack::resetCloseFlag() {
    closeRequested = false;
}
