#include "Room.h"

#include <iostream>
#include <cmath>
#include <memory>



Room::Room(sf::Font& font, Player& player)
    : font(font), playerData(player) {
    init();
}

std::string Room::getNearbyObject() const {
    if (highlightedIndex != -1)
        return objects[static_cast<size_t>(highlightedIndex)].name;
    return "";
}

const Room::RoomObject* Room::getHighlightedObject() const {
    if (highlightedIndex >= 0 && highlightedIndex < static_cast<int>(objects.size()))
        return &objects[highlightedIndex];
    return nullptr;
}


void Room::init() {
    // Load room background texture
    if (!backgroundTexture.loadFromFile("assets/graphics/room.png")) {
        std::cout << "Failed to load room background image!\n";
    }
    backgroundSprite.setTexture(backgroundTexture);

    // Player starting position
    playerPos = { 100, 300 };
    playerRect.setSize({ 40, 60 });
    playerRect.setFillColor(sf::Color::Green);
    playerRect.setPosition(playerPos);

    // Define and create room objects
    objects.clear();
    objects.push_back(createComputer());
    objects.push_back(createAquarium());
    objects.push_back(createStorageRack());
    objects.push_back(createShelves());
    objects.push_back(createDoors());

    // -------- Load Decoration Textures (for small icons) --------
    decorationTextures.clear();
    // If you have a known set of ids:
    std::vector<std::string> decoIds = { "car", "books", "plant", "picture" };
    for (const auto& id : decoIds) {
        sf::Texture tex;
        std::string path = "assets/graphics/shelves/" + id + "small.png";
        if (tex.loadFromFile(path)) {
            decorationTextures[id] = std::move(tex);
        }
        else {
            std::cout << "Failed to load shelf decoration: " << path << "\n";
        }
    }

    hatTextures.clear();
    std::vector<std::string> hatIds = { "crown", "pirate", "froggy", "wizard" };
    for (const auto& id : hatIds) {
        sf::Texture tex;
        std::string path = "assets/graphics/storagerack/" + id + "small.png";
        if (tex.loadFromFile(path)) {
            hatTextures[id] = std::move(tex);
        }
    }

}




void Room::handleInput(sf::Keyboard::Key key) {
    sf::Vector2f delta{ 0, 0 };
    if (key == sf::Keyboard::A || key == sf::Keyboard::Left) delta.x = -playerSpeed;
    if (key == sf::Keyboard::D || key == sf::Keyboard::Right) delta.x = playerSpeed;
    if (key == sf::Keyboard::W || key == sf::Keyboard::Up) delta.y = -playerSpeed;
    if (key == sf::Keyboard::S || key == sf::Keyboard::Down) delta.y = playerSpeed;

    playerPos += delta;

    // Clamp to room bounds
    float playerWidth = playerRect.getSize().x;
    float playerHeight = playerRect.getSize().y;
    float minX = 0.f, maxX = 800.f; // window width
    float minFeetY = 350.f, maxFeetY = 585.f;

    // Horizontal bounds
    if (playerPos.x < minX) {
        playerPos.x = minX;
    }
    if (playerPos.x + playerWidth > maxX) {
        playerPos.x = maxX - playerWidth;
    }
    // Vertical bounds (use feet)
    if (playerPos.y + playerHeight < minFeetY) {
        playerPos.y = minFeetY - playerHeight;
    }
    if (playerPos.y + playerHeight > maxFeetY) {
        playerPos.y = maxFeetY - playerHeight;
    }

    playerRect.setPosition(playerPos);
}

void Room::update() {
    highlightedIndex = -1;
    float interactRange = 40.f; // Increase if needed!
    sf::Vector2f playerFeet = playerRect.getPosition();
    playerFeet.x += playerRect.getSize().x / 2;
    playerFeet.y += playerRect.getSize().y;

    for (size_t i = 0; i < objects.size(); ++i) {
        sf::FloatRect objBounds = objects[i].rect.getGlobalBounds();

        // Check if player FEET are within interaction range (allow a buffer zone)
        float dx = std::max(objBounds.left - playerFeet.x, std::max(0.f, playerFeet.x - (objBounds.left + objBounds.width)));
        float dy = std::max(objBounds.top - playerFeet.y, std::max(0.f, playerFeet.y - (objBounds.top + objBounds.height)));
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < interactRange) {
            highlightedIndex = static_cast<int>(i);
            break;
        }
    }
}


void Room::render(sf::RenderWindow& window) {
    window.draw(backgroundSprite); // Draw background

    // 1. Draw all room objects (except storage rack)
    for (size_t i = 0; i < objects.size(); ++i) {
        if (i == Room::STORAGE_RACK) continue; // Skip storage rack for now
        window.draw(objects[i].rect);

        // Draw label for other objects (optional)
        sf::Text label(objects[i].name, font, 18);
        label.setFillColor(sf::Color::White);
        label.setPosition(objects[i].rect.getPosition().x, objects[i].rect.getPosition().y - 22);
        window.draw(label);
    }

    // 2. Draw shelf decorations (as before)
    std::vector<sf::Vector2f> shelfPositions = {
        {580.f, 75.f},  {660.f, 75.f},
        {580.f, 155.f}, {660.f, 155.f}
    };
    int decoIdx = 0;
    for (const auto& decoId : playerData.ownedDecorations) {
        if (decoIdx >= static_cast<int>(shelfPositions.size())) break;
        auto it = decorationTextures.find(decoId);
        if (it != decorationTextures.end()) {
            sf::Sprite decoSprite;
            decoSprite.setTexture(it->second);
            decoSprite.setPosition(shelfPositions[decoIdx]);
            window.draw(decoSprite);
        }
        decoIdx++;
    }

    // 3. Draw player (BEFORE rack and hats!)
    window.draw(playerRect);

    // 4. Draw storage rack above player
    window.draw(objects[Room::STORAGE_RACK].rect);

    // 5. Draw hats on rack above player
    std::vector<sf::Vector2f> rackPositions = {
        {45.f,  420.f},  // Top Left
        {105.f, 420.f},  // Top Right
        {45.f,  480.f},  // Bottom Left
        {105.f, 480.f}   // Bottom Right
    };
    int hatIdx = 0;
    for (const auto& hatId : playerData.unlockedHats) {
        if (hatIdx >= rackPositions.size()) break;
        auto it = hatTextures.find(hatId);
        if (it != hatTextures.end()) {
            sf::Sprite hatSprite;
            hatSprite.setTexture(it->second);
            hatSprite.setPosition(rackPositions[hatIdx]);
            window.draw(hatSprite);
        }
        hatIdx++;
    }

    // 6. Draw interact label and 30x30 square (on top of all)
    if (const RoomObject* obj = getHighlightedObject()) {
        // Draw label at top center
        sf::Text interactText;
        interactText.setFont(font);
        interactText.setCharacterSize(32);
        interactText.setFillColor(sf::Color::Yellow);
        interactText.setString(">" + obj->name + "<");
        sf::FloatRect textRect = interactText.getLocalBounds();
        interactText.setOrigin(textRect.width / 2, 0);
        interactText.setPosition(window.getSize().x / 2.f, 20.f);
        window.draw(interactText);

        // Draw 30x30px square above the object, centered
        sf::Vector2f objPos = obj->rect.getPosition();
        sf::Vector2f objSize = obj->rect.getSize();

        sf::RectangleShape interactSquare({ 30.f, 30.f });
        interactSquare.setPosition(
            objPos.x + objSize.x / 2.f - 15.f,
            objPos.y - 50.f
        );
        interactSquare.setFillColor(sf::Color(255, 255, 0, 128)); // Semi-transparent yellow
        interactSquare.setOutlineColor(sf::Color::Black);
        interactSquare.setOutlineThickness(2.f);
        window.draw(interactSquare);
    }
}




bool Room::isNearObject() const {
    sf::FloatRect playerBounds(playerRect.getPosition().x, playerRect.getPosition().y, playerRect.getSize().x, playerRect.getSize().y);
    const float INTERACT_DISTANCE = 32.f; // or bigger

    for (size_t i = 0; i < objects.size(); ++i) {
        sf::FloatRect objBounds = objects[i].rect.getGlobalBounds();
        // Expand object bounds by INTERACT_DISTANCE in all directions
        objBounds.left -= INTERACT_DISTANCE;
        objBounds.top -= INTERACT_DISTANCE;
        objBounds.width += 2 * INTERACT_DISTANCE;
        objBounds.height += 2 * INTERACT_DISTANCE;

        if (objBounds.intersects(playerBounds)) {
            // Save i as the highlighted object, etc.
            return true;
        }
    }
    return false;
}


void Room::movePlayer(int dx, int dy) {
    sf::Vector2f oldPos = playerPos;
    float playerWidth = playerRect.getSize().x;
    float playerHeight = playerRect.getSize().y;

    float minX = 0.f, maxX = 800.f;
    float minFeetY = 350.f, maxFeetY = 585.f;

    // Try X movement first
    playerPos.x += dx * playerSpeed;
    // Clamp
    if (playerPos.x < minX) playerPos.x = minX;
    if (playerPos.x + playerWidth > maxX) playerPos.x = maxX - playerWidth;

    // Check X collision (only feet)
    sf::FloatRect feetBoxX(playerPos.x, playerPos.y + playerHeight - 2, playerWidth, 2);
    sf::FloatRect computerBounds = objects[Room::COMPUTER].rect.getGlobalBounds();
    if (computerBounds.intersects(feetBoxX)) {
        // Don't move in X if this move causes a new collision
        playerPos.x = oldPos.x;
    }

    // Try Y movement
    playerPos.y += dy * playerSpeed;
    // Clamp (feet)
    if (playerPos.y + playerHeight < minFeetY) playerPos.y = minFeetY - playerHeight;
    if (playerPos.y + playerHeight > maxFeetY) playerPos.y = maxFeetY - playerHeight;

    sf::FloatRect feetBoxY(playerPos.x, playerPos.y + playerHeight - 2, playerWidth, 2);
    if (computerBounds.intersects(feetBoxY)) {
        playerPos.y = oldPos.y;
    }

    playerRect.setPosition(playerPos);

    sf::FloatRect rackFull = objects[Room::STORAGE_RACK].rect.getGlobalBounds();
    sf::FloatRect rackBottom(
        rackFull.left,
        rackFull.top + rackFull.height - 40,
        rackFull.width,
        40
    );
    sf::FloatRect playerFeet(playerPos.x, playerPos.y + playerHeight - 2, playerWidth, 2);
    if (rackBottom.intersects(playerFeet)) {
        playerPos = oldPos;
        playerRect.setPosition(playerPos);
        return;
    }
}





Room::RoomObject Room::createComputer() {
    RoomObject obj;
    obj.texture = std::make_shared<sf::Texture>();
    if (obj.texture->loadFromFile("assets/graphics/computer.png")) {
        sf::Vector2u texSize = obj.texture->getSize();
        obj.rect.setSize(sf::Vector2f(static_cast<float>(texSize.x), static_cast<float>(texSize.y)));
        obj.rect.setTexture(obj.texture.get());
        obj.rect.setTextureRect(sf::IntRect(0, 0, texSize.x, texSize.y));
        obj.rect.setPosition(300, 200);
        std::cout << "Loaded computer.png, size: "
            << texSize.x << "x" << texSize.y << std::endl;
    }
    else {
        obj.rect.setSize({ 200, 100 });
        obj.rect.setPosition(300, 200);
        obj.rect.setFillColor(sf::Color(120, 120, 120));
        std::cout << "Failed to load computer texture!\n";
    }
    obj.name = "Computer";
    return obj;
}


Room::RoomObject Room::createAquarium() {
    RoomObject obj;
    obj.rect.setSize({ 200, 157 });
    obj.rect.setPosition(550, 400);

    obj.texture = std::make_shared<sf::Texture>();
    if (obj.texture->loadFromFile("assets/graphics/aquarium/aquarium.png")) {
        obj.rect.setTexture(obj.texture.get());
    }
    else {
        obj.rect.setFillColor(sf::Color(120, 120, 120));
        std::cout << "Failed to load aquarium texture!\n";
    }
    obj.name = "Aquarium";
    return obj;
}

Room::RoomObject Room::createStorageRack() {
    RoomObject obj;
    obj.rect.setSize({ 150, 150 });
    obj.rect.setPosition(30, 400);

    obj.texture = std::make_shared<sf::Texture>();
    if (obj.texture->loadFromFile("assets/graphics/storagerack/sr.png")) {
        obj.rect.setTexture(obj.texture.get());
    }
    else {
        obj.rect.setFillColor(sf::Color(120, 120, 120));
        std::cout << "Failed to load storage rack texture!\n";
    }
    obj.name = "Storage Rack";
    return obj;
}

Room::RoomObject Room::createShelves() {
    RoomObject obj;
    obj.rect.setSize({ 200, 300 });
    obj.rect.setPosition(550, 70);

    obj.texture = std::make_shared<sf::Texture>();
    if (obj.texture->loadFromFile("assets/graphics/shelves/shelves.png")) {
        obj.rect.setTexture(obj.texture.get());
    }
    else {
        obj.rect.setFillColor(sf::Color(120, 120, 120));
        std::cout << "Failed to load shelves texture!\n";
    }
    obj.name = "Shelves";
    return obj;
}

Room::RoomObject Room::createDoors() {
    RoomObject obj;
    obj.rect.setSize({ 100, 200 });
    obj.rect.setPosition(60, 155);

    obj.texture = std::make_shared<sf::Texture>();
    if (obj.texture->loadFromFile("assets/graphics/doors.png")) {
        obj.rect.setTexture(obj.texture.get());
    }
    else {
        obj.rect.setFillColor(sf::Color(120, 120, 120));
        std::cout << "Failed to load doors texture!\n";
    }
    obj.name = "Doors";
    return obj;
}

