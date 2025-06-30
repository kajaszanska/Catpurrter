#include "Shelf.h"
#include <iostream>

Shelf::Shelf(const sf::Font& font, Player& player)
    : font(font), playerData(player) {
}

void Shelf::init() {
    // Load shelf closeup background
    if (!shelfBackgroundTexture.loadFromFile("assets/graphics/shelves/shelvesclose.png")) {
        std::cout << "Failed to load shelvesclose.png!\n";
    }
    shelfBackgroundSprite.setTexture(shelfBackgroundTexture);

    // Prepare big decoration sprites
    bigDecorationTextures.clear();
    bigDecorationSprites.clear();

    // Example shelf positions (adjust as needed)
 std::vector<sf::Vector2f> shelfPositions = {
    {170.f, 25.f},   // Position for first item (left, top shelf)
    {400.f, 25.f},   // Position for second item (right, top shelf)
    {170.f, 255.f},  // Position for third item (left, bottom shelf)
    {400.f, 255.f}   // Position for fourth item (right, bottom shelf)
};

 int idx = 0;
 for (const std::string& decoId : playerData.ownedDecorations) {
     std::string path = "assets/graphics/shelves/" + decoId + "big.png";
     sf::Texture tex;
     if (tex.loadFromFile(path)) {
         bigDecorationTextures[decoId] = tex;
         sf::Sprite spr(bigDecorationTextures[decoId]);
         if (idx < shelfPositions.size()) {
             spr.setPosition(shelfPositions[idx]);
         }
         else {
             // If too many items, stack extras at last position or offset as you wish
             spr.setPosition(shelfPositions.back());
         }
         bigDecorationSprites.push_back(spr);
     }
     ++idx;
 }


    selectionIndex = 0;
}


void Shelf::update() {
    for (size_t i = 0; i < decorationTexts.size(); ++i) {
        decorationTexts[i].setFillColor(i == selectionIndex ? sf::Color::Yellow : sf::Color::White);
    }
}

void Shelf::render(sf::RenderWindow& window) {
    // Draw the closeup shelf background
    window.draw(shelfBackgroundSprite);

    // Draw big decoration sprites on the shelf
    for (const auto& spr : bigDecorationSprites) {
        window.draw(spr);
    }

    // Optionally: Draw title and navigation (optional)
    sf::Text title;
    title.setFont(font);
    title.setString("Your Shelf Decorations");
    title.setCharacterSize(30);
    title.setFillColor(sf::Color::Cyan);
    title.setPosition(40.f, 20.f);
    window.draw(title);

    // Optionally: Highlight current selection (if you want navigation)
    // You can draw a yellow box or change sprite tint for the selected decoration if desired.
}


void Shelf::handleInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Escape) {
        closeRequested = true;
        return;
    }

    if (decorationTexts.empty()) return;

    if (key == sf::Keyboard::W || key == sf::Keyboard::Up) {
        if (selectionIndex > 0) selectionIndex--;
    }
    else if (key == sf::Keyboard::S || key == sf::Keyboard::Down) {
        if (selectionIndex < static_cast<int>(decorationTexts.size()) - 1) selectionIndex++;
    }
    else if (key == sf::Keyboard::Enter) {
        std::string selected = playerData.ownedDecorations[selectionIndex];
        std::cout << "Selected decoration: " << selected << "\n";
    }
}

bool Shelf::shouldClose() const {
    return closeRequested;
}

void Shelf::resetCloseFlag() {
    closeRequested = false;
}
