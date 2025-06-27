#include "Room.h"

#include <iostream>
#include <cmath>
#include <memory>



Room::Room(sf::Font& font, Player& player)
    : font(font), playerData(player) {
    init();
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
    float minDist = 70.f;
    sf::Vector2f playerFeet = playerRect.getPosition();
    playerFeet.x += playerRect.getSize().x / 2;
    playerFeet.y += playerRect.getSize().y;

    for (size_t i = 0; i < objects.size(); ++i) {
        sf::FloatRect objBounds = objects[i].rect.getGlobalBounds();
        // Option 1: If feet are inside object rectangle (for solid/clickable objects)
        if (objBounds.contains(playerFeet)) {
            highlightedIndex = static_cast<int>(i);
            break;
        }
        // Option 2: Use proximity if you prefer (for non-solid, e.g., wall-hanging shelves)
        float dx = playerFeet.x - (objBounds.left + objBounds.width / 2);
        float dy = playerFeet.y - (objBounds.top + objBounds.height / 2);
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < minDist) {
            highlightedIndex = static_cast<int>(i);
            break;
        }
    }

}

void Room::render(sf::RenderWindow& window) {
    window.draw(backgroundSprite);  // Draw background first!

    // Draw all room objects (highlight, etc.)
    for (size_t i = 0; i < objects.size(); ++i)
    {
        auto& obj = objects[i];
           window.draw(obj.rect);

        // Draw label
        sf::Text label(obj.name, font, 18);
        label.setFillColor(sf::Color::White);
        label.setPosition(obj.rect.getPosition().x, obj.rect.getPosition().y - 22);
        window.draw(label);
    }

    if (isNearObject()) {
        // Draw interact label at top
        sf::Text interactText;
        interactText.setFont(font);
        interactText.setCharacterSize(32);
        interactText.setFillColor(sf::Color::Yellow);
        interactText.setString(">" + getNearbyObject() + "<");
        sf::FloatRect textRect = interactText.getLocalBounds();
        interactText.setOrigin(textRect.width / 2, 0);
        interactText.setPosition(window.getSize().x / 2.f, 20.f);
        window.draw(interactText);

        // Draw a 50x50px square above the interactable object, centered
        const auto& obj = objects[static_cast<size_t>(highlightedIndex)];
        sf::Vector2f objPos = obj.rect.getPosition();
        sf::Vector2f objSize = obj.rect.getSize();

        sf::RectangleShape interactSquare({ 30.f, 30.f });
        // Center horizontally above object, and put 50px above object top
        interactSquare.setPosition(
            objPos.x + objSize.x / 2.f - 15.f,
            objPos.y - 50.f
        );
        interactSquare.setFillColor(sf::Color(255, 255, 0, 128)); // Semi-transparent yellow
        interactSquare.setOutlineColor(sf::Color::Black);
        interactSquare.setOutlineThickness(2.f);
        window.draw(interactSquare);
    }



    // Draw player
    window.draw(playerRect);
}

// Utility to check if player is near/interacting
bool Room::isNearObject() const { return highlightedIndex != -1; }
std::string Room::getNearbyObject() const {
    if (highlightedIndex != -1)
        return objects[static_cast<size_t>(highlightedIndex)].name;
    return "";
}

void Room::movePlayer(int dx, int dy) {
    sf::Vector2f delta{ static_cast<float>(dx) * playerSpeed, static_cast<float>(dy) * playerSpeed };
    playerPos += delta;

    // Clamp to room bounds
    float playerWidth = playerRect.getSize().x;
    float playerHeight = playerRect.getSize().y;
    float minX = 0.f, maxX = 800.f;
    float minFeetY = 350.f, maxFeetY = 585.f;

    if (playerPos.x < minX) {
        playerPos.x = minX;
    }
    if (playerPos.x + playerWidth > maxX) {
        playerPos.x = maxX - playerWidth;
    }
    if (playerPos.y + playerHeight < minFeetY) {
        playerPos.y = minFeetY - playerHeight;
    }
    if (playerPos.y + playerHeight > maxFeetY) {
        playerPos.y = maxFeetY - playerHeight;
    }

    playerRect.setPosition(playerPos);
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
