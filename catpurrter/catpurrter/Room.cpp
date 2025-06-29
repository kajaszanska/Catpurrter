#include "Room.h"

#include <iostream>
#include <cmath>
#include <memory>
#include <random>


// Room aquarium area for fish movement (ADJUST HERE)
const float ROOM_AQUARIUM_LEFT = 590.f;
const float ROOM_AQUARIUM_TOP = 440.f;
const float ROOM_AQUARIUM_RIGHT = 730.f;
const float ROOM_AQUARIUM_BOTTOM = 495.f;

// Fish image size (adjust if needed, or get from texture if you want)
const float FISH_ROOM_WIDTH = 33.f;
const float FISH_ROOM_HEIGHT = 21.f;


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
    // --- Load room background texture ---
    if (!backgroundTexture.loadFromFile("assets/graphics/room.png")) {
        std::cout << "Failed to load room background image!\n";
    }
    backgroundSprite.setTexture(backgroundTexture);

    // --- Player starting position ---
    playerPos = { 100, 300 };
    playerRect.setSize({ 40, 60 });
    playerRect.setFillColor(sf::Color::Green);
    playerRect.setPosition(playerPos);

    // --- Load player textures (default) ---
    playerTextures.clear();
    std::vector<std::string> hats = { "default", "frog", "crown", "pirate", "wizard" };
    for (const std::string& hat : hats) {
        for (const std::string& dir : { "down", "up", "left", "right" }) {
            for (int f = 1; f <= 2; ++f) {
                std::string key = hat + "_" + dir + std::to_string(f); // eg: frog_left2
                std::string path = "assets/graphics/player/" + hat + "/" + dir + std::to_string(f) + ".png";
                auto tex = std::make_shared<sf::Texture>();
                if (tex->loadFromFile(path)) {
                    playerTextures[key] = tex;
                }
                else {
                    if (hat == "default")
                        std::cout << "Missing player texture: " << path << std::endl;
                }
            }
        }
    }


    // Set up playerSprite to start with
    playerDir = "down";
    playerFrame = 1;
    animTimer = 0.f;
    playerSprite.setTexture(*playerTextures["default_down1"]);
    playerSprite.setPosition(playerPos);

    // --- Define and create room objects ---
    objects.clear();
    objects.push_back(createComputer());
    objects.push_back(createAquarium());
    objects.push_back(createStorageRack());
    objects.push_back(createShelves());
    objects.push_back(createDoors());

    // --- Load Decoration Textures (shelves) ---
    decorationTextures.clear();
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

    // --- Load Hat Textures (storage rack) ---
    hatTextures.clear();
    std::vector<std::string> hatIds = { "crown", "pirate", "frog", "wizard" };
    for (const auto& id : hatIds) {
        sf::Texture tex;
        std::string path = "assets/graphics/storagerack/" + id + "small.png";
        if (tex.loadFromFile(path)) {
            hatTextures[id] = std::move(tex);
        }
    }

    // --- Load Aquarium small backgrounds (plant, castle, both, none) ---
    aquariumSmTexture.loadFromFile("assets/graphics/aquarium/aquarium.png");
    aquariumSmPlants.loadFromFile("assets/graphics/aquarium/aquariumplantssmall.png");
    aquariumSmCastle.loadFromFile("assets/graphics/aquarium/aquariumcastlesmall.png");
    aquariumSmAll.loadFromFile("assets/graphics/aquarium/aquariumallsmall.png");

    // --- Load fish textures ---
    fishTextures.clear();
    std::vector<std::string> fishIds = { "fish1", "fish2", "fish3" };
    for (const auto& id : fishIds) {
        sf::Texture texRight, texLeft;
        if (texRight.loadFromFile("assets/graphics/aquarium/" + id + "rightsmall.png"))
            fishTextures[id + "rightsmall"] = std::move(texRight);
        if (texLeft.loadFromFile("assets/graphics/aquarium/" + id + "leftsmall.png"))
            fishTextures[id + "leftsmall"] = std::move(texLeft);
    }

    // --- Setup fish visuals based on bought fish ---
    fishes.clear();
    std::random_device rd;
    std::mt19937 gen(rd());
    // Spawn range: always inside area, never touching the border!
    std::uniform_real_distribution<float> xDist(
        ROOM_AQUARIUM_LEFT + FISH_ROOM_WIDTH / 2,
        ROOM_AQUARIUM_RIGHT - FISH_ROOM_WIDTH / 2
    );
    std::uniform_real_distribution<float> yDist(
        ROOM_AQUARIUM_TOP + FISH_ROOM_HEIGHT / 2,
        ROOM_AQUARIUM_BOTTOM - FISH_ROOM_HEIGHT / 2
    );
    // Make the fish swim *slow* (tweak these as you want)
    std::uniform_real_distribution<float> vxDist(0.2f, 0.8f);


    for (const auto& id : fishIds) {
        if (std::find(playerData.aquariumContents.begin(), playerData.aquariumContents.end(), id) != playerData.aquariumContents.end()) {
            FishVisual fish;
            fish.id = id;
            fish.facingRight = (gen() % 2 == 0);
            fish.position = sf::Vector2f(xDist(gen), yDist(gen));
            float vx = vxDist(gen) * (fish.facingRight ? 1.f : -1.f);
            fish.velocity = { vx, 0.f };
            fish.minSwimDistance = 25.f + (gen() % 20);     // lower for smaller aquarium!
            fish.directionTimer = 0.7f + float(gen() % 200) / 100.0f;
            fishes.push_back(fish);
        }
    }

}



void Room::handleInput(sf::Keyboard::Key key) {
    int dx = 0, dy = 0;
    if (key == sf::Keyboard::A || key == sf::Keyboard::Left) dx = -1;
    if (key == sf::Keyboard::D || key == sf::Keyboard::Right) dx = 1;
    if (key == sf::Keyboard::W || key == sf::Keyboard::Up) dy = -1;
    if (key == sf::Keyboard::S || key == sf::Keyboard::Down) dy = 1;

    // Only call movePlayer, don't touch playerPos here!
    movePlayer(dx, dy);
}


void Room::update() {
    highlightedIndex = -1;
    float interactRange = 40.f;
    sf::Vector2f playerFeet = playerRect.getPosition();
    playerFeet.x += playerRect.getSize().x / 2;
    playerFeet.y += playerRect.getSize().y;

    for (size_t i = 0; i < objects.size(); ++i) {
        sf::FloatRect objBounds = objects[i].rect.getGlobalBounds();
        float dx = std::max(objBounds.left - playerFeet.x, std::max(0.f, playerFeet.x - (objBounds.left + objBounds.width)));
        float dy = std::max(objBounds.top - playerFeet.y, std::max(0.f, playerFeet.y - (objBounds.top + objBounds.height)));
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < interactRange) {
            highlightedIndex = static_cast<int>(i);
            break;
        }
    }

    // --- Animate Player Sprite ---
    float moveX = 0, moveY = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  moveX = -1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) moveX = 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    moveY = -1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  moveY = 1;

    // Only update dir if actually moving
    if (moveX != 0 || moveY != 0) {
        if (std::abs(moveX) > std::abs(moveY)) playerDir = (moveX > 0) ? "right" : "left";
        else playerDir = (moveY > 0) ? "down" : "up";
        // Animation frame switching
        animTimer += 0.016f; // assuming 60 FPS, otherwise pass dt!
        if (animTimer >= animDelay) {
            playerFrame = (playerFrame == 1) ? 2 : 1;
            animTimer = 0.f;
        }
    }
    else {
        playerFrame = 1; // Idle, always show frame 1
    }
    std::string hatKey = playerData.equippedHat.empty() ? "default" : playerData.equippedHat;
    std::string key = hatKey + "_" + playerDir + std::to_string(playerFrame);
    auto it = playerTextures.find(key);
    // If missing, fall back to default
    if (it != playerTextures.end()) {
        playerSprite.setTexture(*it->second);
    }
    else {
        std::string fallbackKey = "default_" + playerDir + std::to_string(playerFrame);
        auto fallbackIt = playerTextures.find(fallbackKey);
        if (fallbackIt != playerTextures.end()) {
            playerSprite.setTexture(*fallbackIt->second);
        }
    }



    playerSprite.setPosition(playerPos);


    for (auto& fish : fishes) {
        // --- Get actual texture size ---
        std::string texKey = fish.id + (fish.facingRight ? "rightsmall" : "leftsmall");
        float fishWidth = 33.f, fishHeight = 21.f; // fallback
        auto it = fishTextures.find(texKey);
        if (it != fishTextures.end()) {
            fishWidth = it->second.getSize().x;
            fishHeight = it->second.getSize().y;
        }

        // --- Move fish ---
        fish.position += fish.velocity * 0.016f;
        fish.distanceSinceDirectionChange += std::abs(fish.velocity.x * 0.016f) + std::abs(fish.velocity.y * 0.016f);

        // --- Clamp position so center always stays inside ---
        bool clampedX = false, clampedY = false;
        if (fish.position.x < ROOM_AQUARIUM_LEFT + fishWidth / 2) {
            fish.position.x = ROOM_AQUARIUM_LEFT + fishWidth / 2;
            clampedX = true;
        }
        if (fish.position.x > ROOM_AQUARIUM_RIGHT - fishWidth / 2) {
            fish.position.x = ROOM_AQUARIUM_RIGHT - fishWidth / 2;
            clampedX = true;
        }
        if (fish.position.y < ROOM_AQUARIUM_TOP + fishHeight / 2) {
            fish.position.y = ROOM_AQUARIUM_TOP + fishHeight / 2;
            clampedY = true;
        }
        if (fish.position.y > ROOM_AQUARIUM_BOTTOM - fishHeight / 2) {
            fish.position.y = ROOM_AQUARIUM_BOTTOM - fishHeight / 2;
            clampedY = true;
        }

        // If clamped X, reverse X direction and reset distance
        if (clampedX) {
            fish.velocity.x *= -1.f;
            fish.facingRight = (fish.velocity.x > 0);
            fish.distanceSinceDirectionChange = 0.f;
        }
        // If clamped Y, stop Y movement and reset timer
        if (clampedY) {
            fish.velocity.y = 0.f; // stop vertical movement!
            fish.verticalSwimTimer = 0.f;
            fish.distanceSinceDirectionChange = 0.f;
        }

        // --- Direction change logic ---
        fish.directionTimer -= 0.016f;
        bool canChangeDir = (fish.distanceSinceDirectionChange > fish.minSwimDistance);

        // Only allow new Y direction if fish is not already swimming vertically
        if (canChangeDir && fish.directionTimer < 0) {
            // 10% chance to flip horizontal
            if ((rand() % 10) == 0) {
                fish.velocity.x *= -1.f;
                fish.facingRight = (fish.velocity.x > 0);
                fish.distanceSinceDirectionChange = 0.f;
            }
            // 35% chance to nudge vertical ONLY if not already moving vertically
            if (fish.velocity.y == 0.f && (rand() % 3) == 0) {
                float speedY = 0.02f + float(rand() % 5) * 0.02f; // 2–6 px/sec, SLOW for small aquarium
                fish.velocity.y = (rand() % 2 ? 1.f : -1.f) * speedY;
                fish.verticalSwimTimer = 0.9f + float(rand() % 40) / 100.f; // 0.6–1.0 sec up/down
                fish.distanceSinceDirectionChange = 0.f;
            }
            fish.directionTimer = 0.45f + float(rand() % 130) / 100.0f;
        }

        // --- End vertical movement after timer ---
        if (fish.verticalSwimTimer > 0.f) {
            fish.verticalSwimTimer -= 0.016f;
            if (fish.verticalSwimTimer <= 0.f) {
                fish.velocity.y = 0.f;
            }
        }
    }
}



void Room::render(sf::RenderWindow& window) {
    window.draw(backgroundSprite);

    // 1. Draw all room objects except aquarium and storage rack
    for (size_t i = 0; i < objects.size(); ++i) {
        if (i == Room::STORAGE_RACK || i == Room::AQUARIUM) continue;
        window.draw(objects[i].rect);

        sf::Text label(objects[i].name, font, 18);
        label.setFillColor(sf::Color::White);
        label.setPosition(objects[i].rect.getPosition().x, objects[i].rect.getPosition().y - 22);
        window.draw(label);
    }

    // 2. Draw shelf decorations
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

    // --- Z-ORDER SECTION ---

    // Use the **sprite's** visual feet for correct Z
    float playerFeetY = playerSprite.getPosition().y + playerSprite.getTexture()->getSize().y;

    // --- AQUARIUM ---
    const auto& aquariumObj = objects[Room::AQUARIUM];
    float aquariumCutoffY = aquariumObj.rect.getPosition().y + aquariumObj.rect.getSize().y; // 507

    sf::Sprite aquariumBgSprite;
    bool hasPlant = false, hasCastle = false;
    for (const auto& item : playerData.aquariumContents) {
        if (item == "plant") hasPlant = true;
        if (item == "castle") hasCastle = true;
    }
    if (hasPlant && hasCastle)
        aquariumBgSprite.setTexture(aquariumSmAll);
    else if (hasPlant)
        aquariumBgSprite.setTexture(aquariumSmPlants);
    else if (hasCastle)
        aquariumBgSprite.setTexture(aquariumSmCastle);
    else
        aquariumBgSprite.setTexture(aquariumSmTexture);
    aquariumBgSprite.setPosition(aquariumObj.rect.getPosition());

    // --- STORAGE RACK ---
    const auto& rackObj = objects[Room::STORAGE_RACK];
    float rackCutoffY = rackObj.rect.getPosition().y + rackObj.rect.getSize().y; // 500

    std::vector<sf::Vector2f> rackPositions = {
        {45.f,  420.f},
        {105.f, 420.f},
        {45.f,  480.f},
        {105.f, 480.f}
    };

    // --- DRAW LOGIC ---
    bool behindAquarium = playerFeetY < aquariumCutoffY;
    bool behindRack = playerFeetY < rackCutoffY;

    // There are 4 possible cases:
    if (behindAquarium && behindRack) {
        // Player is behind both
        window.draw(playerSprite);

        window.draw(aquariumBgSprite);
        for (const auto& fish : fishes) {
            std::string texKey = fish.id + (fish.facingRight ? "rightsmall" : "leftsmall");
            auto it = fishTextures.find(texKey);
            if (it != fishTextures.end()) {
                sf::Sprite fishSprite;
                fishSprite.setTexture(it->second);
                fishSprite.setOrigin(it->second.getSize().x / 2.f, it->second.getSize().y / 2.f);
                fishSprite.setPosition(fish.position);
                window.draw(fishSprite);
            }
        }

        window.draw(rackObj.rect);
        for (size_t i = 0; i < rackPositions.size(); ++i) {
            if (i >= playerData.unlockedHats.size()) break;
            const auto& hatId = playerData.unlockedHats[i];
            if (hatId == playerData.equippedHat) continue;
            auto it = hatTextures.find(hatId);
            if (it != hatTextures.end()) {
                sf::Sprite hatSprite;
                hatSprite.setTexture(it->second);
                hatSprite.setPosition(rackPositions[i]);
                window.draw(hatSprite);
            }
        }
    }
    else if (!behindAquarium && !behindRack) {
        // Player is in front of both
        window.draw(aquariumBgSprite);
        for (const auto& fish : fishes) {
            std::string texKey = fish.id + (fish.facingRight ? "rightsmall" : "leftsmall");
            auto it = fishTextures.find(texKey);
            if (it != fishTextures.end()) {
                sf::Sprite fishSprite;
                fishSprite.setTexture(it->second);
                fishSprite.setOrigin(it->second.getSize().x / 2.f, it->second.getSize().y / 2.f);
                fishSprite.setPosition(fish.position);
                window.draw(fishSprite);
            }
        }
        window.draw(rackObj.rect);
        for (size_t i = 0; i < rackPositions.size(); ++i) {
            if (i >= playerData.unlockedHats.size()) break;
            const auto& hatId = playerData.unlockedHats[i];
            if (hatId == playerData.equippedHat) continue;
            auto it = hatTextures.find(hatId);
            if (it != hatTextures.end()) {
                sf::Sprite hatSprite;
                hatSprite.setTexture(it->second);
                hatSprite.setPosition(rackPositions[i]);
                window.draw(hatSprite);
            }
        }
        window.draw(playerSprite);
    }
    else if (behindAquarium && !behindRack) {
        // Player is between: behind aquarium, in front of rack
        window.draw(playerSprite);

        window.draw(aquariumBgSprite);
        for (const auto& fish : fishes) {
            std::string texKey = fish.id + (fish.facingRight ? "rightsmall" : "leftsmall");
            auto it = fishTextures.find(texKey);
            if (it != fishTextures.end()) {
                sf::Sprite fishSprite;
                fishSprite.setTexture(it->second);
                fishSprite.setOrigin(it->second.getSize().x / 2.f, it->second.getSize().y / 2.f);
                fishSprite.setPosition(fish.position);
                window.draw(fishSprite);
            }
        }
        window.draw(rackObj.rect);
        for (size_t i = 0; i < rackPositions.size(); ++i) {
            if (i >= playerData.unlockedHats.size()) break;
            const auto& hatId = playerData.unlockedHats[i];
            if (hatId == playerData.equippedHat) continue;
            auto it = hatTextures.find(hatId);
            if (it != hatTextures.end()) {
                sf::Sprite hatSprite;
                hatSprite.setTexture(it->second);
                hatSprite.setPosition(rackPositions[i]);
                window.draw(hatSprite);
            }
        }
    }
    else { // (!behindAquarium && behindRack)
        // Player is between: in front of aquarium, behind rack
        window.draw(aquariumBgSprite);
        for (const auto& fish : fishes) {
            std::string texKey = fish.id + (fish.facingRight ? "rightsmall" : "leftsmall");
            auto it = fishTextures.find(texKey);
            if (it != fishTextures.end()) {
                sf::Sprite fishSprite;
                fishSprite.setTexture(it->second);
                fishSprite.setOrigin(it->second.getSize().x / 2.f, it->second.getSize().y / 2.f);
                fishSprite.setPosition(fish.position);
                window.draw(fishSprite);
            }
        }
        window.draw(playerSprite);

        window.draw(rackObj.rect);
        for (size_t i = 0; i < rackPositions.size(); ++i) {
            if (i >= playerData.unlockedHats.size()) break;
            const auto& hatId = playerData.unlockedHats[i];
            if (hatId == playerData.equippedHat) continue;
            auto it = hatTextures.find(hatId);
            if (it != hatTextures.end()) {
                sf::Sprite hatSprite;
                hatSprite.setTexture(it->second);
                hatSprite.setPosition(rackPositions[i]);
                window.draw(hatSprite);
            }
        }
    }

    // --- Draw interact label and square (unchanged) ---
    if (const RoomObject* obj = getHighlightedObject()) {
        sf::Text interactText;
        interactText.setFont(font);
        interactText.setCharacterSize(32);
        interactText.setFillColor(sf::Color::Yellow);
        interactText.setString(">" + obj->name + "<");
        sf::FloatRect textRect = interactText.getLocalBounds();
        interactText.setOrigin(textRect.width / 2, 0);
        interactText.setPosition(window.getSize().x / 2.f, 20.f);
        window.draw(interactText);

        sf::Vector2f objPos = obj->rect.getPosition();
        sf::Vector2f objSize = obj->rect.getSize();

        sf::RectangleShape interactSquare({ 30.f, 30.f });
        interactSquare.setPosition(
            objPos.x + objSize.x / 2.f - 15.f,
            objPos.y - 50.f
        );
        interactSquare.setFillColor(sf::Color(255, 255, 0, 128));
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
    float playerWidth = playerRect.getSize().x;
    float playerHeight = playerRect.getSize().y;
    float minX = 0.f, maxX = 800.f;
    float minFeetY = 390.f, maxFeetY = 600.f;

    // Adjust collision box for feet area
    const float playerSpriteWidth = playerSprite.getTexture()->getSize().x;
    const float playerSpriteHeight = playerSprite.getTexture()->getSize().y;
    const float COLLISION_FEET_TOP = 110.f;
    const float COLLISION_FEET_LEFT = 35.f;
    const float COLLISION_FEET_WIDTH = 80.f;
    const float COLLISION_FEET_HEIGHT = 15.f;


    // 1. Move X only
    if (dx != 0) {
        float newX = playerPos.x + dx * playerSpeed;
        if (newX < minX) newX = minX;
        if (newX + playerSpriteWidth > maxX) newX = maxX - playerSpriteWidth;

        sf::FloatRect feetBoxX(
            newX + COLLISION_FEET_LEFT,
            playerPos.y + COLLISION_FEET_TOP,
            COLLISION_FEET_WIDTH,
            COLLISION_FEET_HEIGHT
        );


        bool blockX = false;
        {
            sf::FloatRect computerBounds = objects[Room::COMPUTER].rect.getGlobalBounds();
            sf::FloatRect rackFull = objects[Room::STORAGE_RACK].rect.getGlobalBounds();
            sf::FloatRect rackBottom(
                rackFull.left,
                rackFull.top + rackFull.height - 40,
                rackFull.width,
                40
            );
            sf::FloatRect aquariumFull = objects[Room::AQUARIUM].rect.getGlobalBounds();
            sf::FloatRect aquariumBottom(
                aquariumFull.left,
                aquariumFull.top + aquariumFull.height - 40,
                aquariumFull.width,
                40
            );
            if (computerBounds.intersects(feetBoxX) ||
                rackBottom.intersects(feetBoxX) ||
                aquariumBottom.intersects(feetBoxX))
                blockX = true;

        }
        if (!blockX) playerPos.x = newX;
    }

    // 2. Move Y only
    if (dy != 0) {
        float newY = playerPos.y + dy * playerSpeed;
        if (newY + playerSpriteHeight < minFeetY) newY = minFeetY - playerSpriteHeight;
        if (newY + playerSpriteHeight > maxFeetY) newY = maxFeetY - playerSpriteHeight;

        sf::FloatRect feetBoxY(
            playerPos.x + COLLISION_FEET_LEFT,
            newY + COLLISION_FEET_TOP,
            COLLISION_FEET_WIDTH,
            COLLISION_FEET_HEIGHT
        );


        bool blockY = false;
        {
            sf::FloatRect computerBounds = objects[Room::COMPUTER].rect.getGlobalBounds();
            sf::FloatRect rackFull = objects[Room::STORAGE_RACK].rect.getGlobalBounds();
            sf::FloatRect rackBottom(
                rackFull.left,
                rackFull.top + rackFull.height - 40,
                rackFull.width,
                40
            );
            sf::FloatRect aquariumFull = objects[Room::AQUARIUM].rect.getGlobalBounds();
            sf::FloatRect aquariumBottom(
                aquariumFull.left,
                aquariumFull.top + aquariumFull.height - 40,
                aquariumFull.width,
                40
            );
            if (computerBounds.intersects(feetBoxY) ||
                rackBottom.intersects(feetBoxY) ||
                aquariumBottom.intersects(feetBoxY))
                blockY = true;

        }
        if (!blockY) playerPos.y = newY;
    }

    // Final update
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

