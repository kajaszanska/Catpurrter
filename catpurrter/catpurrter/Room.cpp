#include "Room.h"

#include <iostream>
#include <cmath>

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

    // Define room objects with positions and sizes
    objects.clear();
    objects.push_back({ sf::RectangleShape({60, 60}), "Computer" });
    objects.push_back({ sf::RectangleShape({60, 60}), "Aquarium" });
    objects.push_back({ sf::RectangleShape({60, 60}), "Storage Rack" });
    objects.push_back({ sf::RectangleShape({60, 60}), "Shelves" });
    objects.push_back({ sf::RectangleShape({60, 60}), "Doors" });

    // Position each object in the room
    objects[0].rect.setPosition(400, 340); // Computer
    objects[1].rect.setPosition(670, 520); // Aquarium
    objects[2].rect.setPosition(130, 520); // Storage
    objects[3].rect.setPosition(600, 340); // Shelves
    objects[4].rect.setPosition(40, 340);  // Doors

    for (auto& obj : objects)
        obj.rect.setFillColor(sf::Color(120, 120, 120));
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
    // Check proximity to objects for highlighting
    highlightedIndex = -1;
    float minDist = 70.f; // Interaction threshold
    for (size_t i = 0; i < objects.size(); ++i) {
        float dx = playerRect.getPosition().x - objects[i].rect.getPosition().x;
        float dy = playerRect.getPosition().y - objects[i].rect.getPosition().y;
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
        obj.rect.setFillColor(i == highlightedIndex ? sf::Color::Yellow : sf::Color(120, 120, 120));
        window.draw(obj.rect);

        // Draw label
        sf::Text label(obj.name, font, 18);
        label.setFillColor(sf::Color::White);
        label.setPosition(obj.rect.getPosition().x, obj.rect.getPosition().y - 22);
        window.draw(label);
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
