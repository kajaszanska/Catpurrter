#include "Aquarium.h"
#include <iostream>
#include <random>


const float AQUARIUM_LEFT = 185.f;
const float AQUARIUM_TOP = 230.f;
const float AQUARIUM_RIGHT = 620.f;  
const float AQUARIUM_BOTTOM = 385.f; 

const float FISH_WIDTH = 100.f;   
const float FISH_HEIGHT = 64.f;

Aquarium::Aquarium(sf::Font& fontRef, Player& playerRef)
    : font(fontRef), playerData(playerRef), closeRequested(false) {
}

void Aquarium::init() {
    closeRequested = false;

    // Load all needed textures (do only once per session if you want!)
    aquariumBigTexture.loadFromFile("assets/graphics/aquarium/aquariumbig.png");
    aquariumBigPlants.loadFromFile("assets/graphics/aquarium/aquariumplantsbig.png");
    aquariumBigCastle.loadFromFile("assets/graphics/aquarium/aquariumcastlebig.png");
    aquariumBigAll.loadFromFile("assets/graphics/aquarium/aquariumallbig.png");

    // Load fish textures (right/left)
    std::vector<std::string> fishIds = { "fish1", "fish2", "fish3" };
    for (const auto& id : fishIds) {
        sf::Texture texRight, texLeft;
        texRight.loadFromFile("assets/graphics/aquarium/" + id + "rightbig.png");
        texLeft.loadFromFile("assets/graphics/aquarium/" + id + "leftbig.png");
        fishTextures[id + "rightbig"] = std::move(texRight);
        fishTextures[id + "leftbig"] = std::move(texLeft);
    }

    // --- Setup fish visuals for each bought fish ---
    fishes.clear();
    std::random_device rd;
    std::mt19937 gen(rd());

    // Randomize fish start positions within bounds
    std::uniform_real_distribution<float> xDist(AQUARIUM_LEFT + FISH_WIDTH / 2, AQUARIUM_RIGHT - FISH_WIDTH / 2);
    std::uniform_real_distribution<float> yDist(AQUARIUM_TOP + FISH_HEIGHT / 2, AQUARIUM_BOTTOM - FISH_HEIGHT / 2);

    for (const auto& id : fishIds) {
        if (std::find(playerData.aquariumContents.begin(), playerData.aquariumContents.end(), id) != playerData.aquariumContents.end()) {
            FishVisual fish;
            fish.id = id;
            fish.facingRight = (gen() % 2 == 0);

            // Start at random spot inside bounds
            fish.position = sf::Vector2f(xDist(gen), yDist(gen));

            // SLOW random start velocities
            float vx = (fish.facingRight ? 1.0f : -1.0f) * (25.f + float(gen() % 20)); // 25–45 px/sec
            float vy = (gen() % 2 ? 1.0f : -1.0f) * (15.f + float(gen() % 10));         // 15–25 px/sec
            fish.velocity = { vx, vy };

            // For new logic: distance/intervals before direction change
            fish.distanceSinceDirectionChange = 0.0f;
            fish.minSwimDistance = 60.f + float(gen() % 40); // 60–100 px
            fish.directionTimer = 0.8f + float(gen() % 200) / 100.0f; // 0.8–2.8s until next change

            fishes.push_back(fish);
        }
    }
}




void Aquarium::update(float dt) {
    for (auto& fish : fishes) {
        // Get correct image size
        std::string texKey = fish.id + (fish.facingRight ? "rightbig" : "leftbig");
        auto it = fishTextures.find(texKey);
        float fishWidth = 100.f, fishHeight = 64.f;
        if (it != fishTextures.end()) {
            fishWidth = it->second.getSize().x;
            fishHeight = it->second.getSize().y;
        }

        // Move the fish (dt already makes it frame-independent!)
        sf::Vector2f oldPos = fish.position;
        fish.position += fish.velocity * dt;
        fish.distanceSinceDirectionChange += std::abs(fish.velocity.x * dt) + std::abs(fish.velocity.y * dt);

        // Clamp X
        if (fish.position.x < AQUARIUM_LEFT + fishWidth / 2) {
            fish.position.x = AQUARIUM_LEFT + fishWidth / 2;
            fish.velocity.x = std::abs(fish.velocity.x); // move right
            fish.facingRight = true;
            fish.distanceSinceDirectionChange = 0;
        }
        if (fish.position.x > AQUARIUM_RIGHT - fishWidth / 2) {
            fish.position.x = AQUARIUM_RIGHT - fishWidth / 2;
            fish.velocity.x = -std::abs(fish.velocity.x); // move left
            fish.facingRight = false;
            fish.distanceSinceDirectionChange = 0;
        }

        // Clamp Y
        if (fish.position.y < AQUARIUM_TOP + fishHeight / 2) {
            fish.position.y = AQUARIUM_TOP + fishHeight / 2;
            fish.velocity.y = std::abs(fish.velocity.y); // move down
            fish.distanceSinceDirectionChange = 0;
        }
        if (fish.position.y > AQUARIUM_BOTTOM - fishHeight / 2) {
            fish.position.y = AQUARIUM_BOTTOM - fishHeight / 2;
            fish.velocity.y = -std::abs(fish.velocity.y); // move up
            fish.distanceSinceDirectionChange = 0;
        }

        // --- Direction change logic ---
        fish.directionTimer -= dt;
        bool canChangeDir = (fish.distanceSinceDirectionChange > fish.minSwimDistance);
        if (canChangeDir && fish.directionTimer < 0) {
            
            if ((rand() % 8) == 0) {
                float baseSpeed = (65.f + float(rand() % 80)); // 65–144 px/sec
                float speedMultiplier = 0.7f + (float(rand() % 21) / 100.f); // 0.8–1.2
                fish.facingRight = !fish.facingRight; // flip direction
                fish.velocity.x = (fish.facingRight ? 1.0f : -1.0f) * baseSpeed * speedMultiplier;
                fish.distanceSinceDirectionChange = 0.f;
            }
            // Y direction: 20% chance
            if ((rand() % 5) == 0) {
                float baseSpeed = (15.f + float(rand() % 10));
                float speedMultiplier = 0.8f + (float(rand() % 31) / 100.f); // 0.8–1.2
                fish.velocity.y = (rand() % 2 ? 1.f : -1.f) * baseSpeed * speedMultiplier;
                fish.distanceSinceDirectionChange = 0.f;
            }

            fish.directionTimer = 0.2f + float(rand() % 100) / 80.0f; // Next check
        }
    }
}




void Aquarium::render(sf::RenderWindow& window) {
    // --- Draw correct aquarium background based on playerData.aquariumContents ---
    bool hasPlant = false, hasCastle = false;
    for (const auto& item : playerData.aquariumContents) {
        if (item == "plant") hasPlant = true;
        if (item == "castle") hasCastle = true;
    }

    sf::Sprite bgSprite;
    if (hasPlant && hasCastle)
        bgSprite.setTexture(aquariumBigAll);
    else if (hasPlant)
        bgSprite.setTexture(aquariumBigPlants);
    else if (hasCastle)
        bgSprite.setTexture(aquariumBigCastle);
    else
        bgSprite.setTexture(aquariumBigTexture);
    bgSprite.setPosition(0, 0);

    window.draw(bgSprite);

    for (const auto& fish : fishes) {
        std::string texKey = fish.id + (fish.facingRight ? "rightbig" : "leftbig");
        auto it = fishTextures.find(texKey);
        if (it != fishTextures.end()) {
            sf::Sprite fishSprite;
            fishSprite.setTexture(it->second);

            // Center the sprite origin to the middle of the texture
            sf::Vector2u texSize = it->second.getSize();
            fishSprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);

            fishSprite.setPosition(fish.position);
            window.draw(fishSprite);
        }
    }


    // Optional: draw title/texts on top if you want
    sf::Text title;
    title.setFont(font);
    title.setString("Aquarium View");
    title.setCharacterSize(30);
    title.setFillColor(sf::Color::Cyan);
    title.setPosition(100.f, 30.f);
    window.draw(title);
}



void Aquarium::handleInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        std::cout << "Leaving Aquarium View\n";
        closeRequested = true;
    }
}

bool Aquarium::shouldClose() const {
    return closeRequested;
}

void Aquarium::resetCloseFlag() {
    closeRequested = false;
}
