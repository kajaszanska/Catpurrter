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

    aquariumBigTexture.loadFromFile("assets/graphics/aquarium/aquariumbig.png");
    aquariumBigPlants.loadFromFile("assets/graphics/aquarium/aquariumplantsbig.png");
    aquariumBigCastle.loadFromFile("assets/graphics/aquarium/aquariumcastlebig.png");
    aquariumBigAll.loadFromFile("assets/graphics/aquarium/aquariumallbig.png");

    std::vector<std::string> fishIds = { "fish1", "fish2", "fish3" };
    for (const auto& id : fishIds) {
        sf::Texture texRight, texLeft;
        texRight.loadFromFile("assets/graphics/aquarium/" + id + "rightbig.png");
        texLeft.loadFromFile("assets/graphics/aquarium/" + id + "leftbig.png");
        fishTextures[id + "rightbig"] = std::move(texRight);
        fishTextures[id + "leftbig"] = std::move(texLeft);
    }

    fishes.clear();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> xDist(AQUARIUM_LEFT + FISH_WIDTH / 2, AQUARIUM_RIGHT - FISH_WIDTH / 2);
    std::uniform_real_distribution<float> yDist(AQUARIUM_TOP + FISH_HEIGHT / 2, AQUARIUM_BOTTOM - FISH_HEIGHT / 2);

    for (const auto& id : fishIds) {
        if (std::find(playerData.aquariumContents.begin(), playerData.aquariumContents.end(), id) != playerData.aquariumContents.end()) {
            FishVisual fish;
            fish.id = id;
            fish.facingRight = (gen() % 2 == 0);
            fish.position = sf::Vector2f(xDist(gen), yDist(gen));
            float vx = (fish.facingRight ? 1.0f : -1.0f) * (25.f + float(gen() % 20));
            float vy = (gen() % 2 ? 1.0f : -1.0f) * (15.f + float(gen() % 10));
            fish.velocity = { vx, vy };
            fish.distanceSinceDirectionChange = 0.0f;
            fish.minSwimDistance = 60.f + float(gen() % 40);
            fish.directionTimer = 0.8f + float(gen() % 200) / 100.0f;
            fishes.push_back(fish);
        }
    }
}

void Aquarium::update(float dt) {
    for (auto& fish : fishes) {
        std::string texKey = fish.id + (fish.facingRight ? "rightbig" : "leftbig");
        auto it = fishTextures.find(texKey);
        float fishWidth = 100.f, fishHeight = 64.f;
        if (it != fishTextures.end()) {
            fishWidth = static_cast<float>(it->second.getSize().x);
            fishHeight = static_cast<float>(it->second.getSize().y);
        }

        fish.position += fish.velocity * dt;
        fish.distanceSinceDirectionChange += std::abs(fish.velocity.x * dt) + std::abs(fish.velocity.y * dt);

        if (fish.position.x < AQUARIUM_LEFT + fishWidth / 2) {
            fish.position.x = AQUARIUM_LEFT + fishWidth / 2;
            fish.velocity.x = std::abs(fish.velocity.x);
            fish.facingRight = true;
            fish.distanceSinceDirectionChange = 0;
        }
        if (fish.position.x > AQUARIUM_RIGHT - fishWidth / 2) {
            fish.position.x = AQUARIUM_RIGHT - fishWidth / 2;
            fish.velocity.x = -std::abs(fish.velocity.x);
            fish.facingRight = false;
            fish.distanceSinceDirectionChange = 0;
        }
        if (fish.position.y < AQUARIUM_TOP + fishHeight / 2) {
            fish.position.y = AQUARIUM_TOP + fishHeight / 2;
            fish.velocity.y = std::abs(fish.velocity.y);
            fish.distanceSinceDirectionChange = 0;
        }
        if (fish.position.y > AQUARIUM_BOTTOM - fishHeight / 2) {
            fish.position.y = AQUARIUM_BOTTOM - fishHeight / 2;
            fish.velocity.y = -std::abs(fish.velocity.y);
            fish.distanceSinceDirectionChange = 0;
        }

        fish.directionTimer -= dt;
        bool canChangeDir = (fish.distanceSinceDirectionChange > fish.minSwimDistance);
        if (canChangeDir && fish.directionTimer < 0) {
            if ((rand() % 8) == 0) {
                float baseSpeed = (65.f + float(rand() % 80));
                float speedMultiplier = 0.7f + (float(rand() % 21) / 100.f);
                fish.facingRight = !fish.facingRight;
                fish.velocity.x = (fish.facingRight ? 1.0f : -1.0f) * baseSpeed * speedMultiplier;
                fish.distanceSinceDirectionChange = 0.f;
            }
            if ((rand() % 5) == 0) {
                float baseSpeed = (15.f + float(rand() % 10));
                float speedMultiplier = 0.8f + (float(rand() % 31) / 100.f);
                fish.velocity.y = (rand() % 2 ? 1.f : -1.f) * baseSpeed * speedMultiplier;
                fish.distanceSinceDirectionChange = 0.f;
            }
            fish.directionTimer = 0.2f + float(rand() % 100) / 80.0f;
        }
    }
}

void Aquarium::render(sf::RenderWindow& window) {
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
            sf::Vector2u texSize = it->second.getSize();
            fishSprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);
            fishSprite.setPosition(fish.position);
            window.draw(fishSprite);
        }
    }

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
        closeRequested = true;
    }
}

bool Aquarium::shouldClose() const {
    return closeRequested;
}

void Aquarium::resetCloseFlag() {
    closeRequested = false;
}
