#include "ShopCategory.h"
#include <iostream>

ShopCategoryView::ShopCategoryView(const sf::Font& fontRef)
    : font(fontRef) {
}

void ShopCategoryView::init() {
    options.clear();
    selectionIndex = 0;
    selected = ShopSelection::None;
    closeRequested = false;

    std::vector<std::string> labels = {
        "Hat Shop",
        "Fish Tank Items",
        "Shelf Decorations",
        "Mini Games",
        "Back"
    };

    float y = 200.f;
    for (const auto& label : labels) {
        sf::Text text;
        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(32);
        text.setFillColor(sf::Color::White);
        text.setPosition(100.f, y);
        options.push_back(text);
        y += 50.f;
    }
}

void ShopCategoryView::update() {
    for (size_t i = 0; i < options.size(); ++i) {
        options[i].setFillColor(i == selectionIndex ? sf::Color::Yellow : sf::Color::White);
    }
}

void ShopCategoryView::clearSelection() {
    selected = ShopSelection::None;
}


void ShopCategoryView::render(sf::RenderWindow& window) {
      
    sf::RectangleShape bg(sf::Vector2f(static_cast<float>(window.getSize().x),static_cast<float>(window.getSize().y)));

    bg.setFillColor(sf::Color(120, 60, 200)); 
    window.draw(bg);


    sf::Text title("Shop Categories", font, 36);
    title.setFillColor(sf::Color::Cyan);
    title.setPosition(100.f, 50.f);
    window.draw(title);

    for (const auto& opt : options)
        window.draw(opt);
}

void ShopCategoryView::handleInput(sf::Keyboard::Key key) {

    if (key == sf::Keyboard::Escape) {
        closeRequested = true;
        return;
    }

    if (key == sf::Keyboard::Up || key == sf::Keyboard::W) {
        if (selectionIndex > 0) selectionIndex--;
    }
    else if (key == sf::Keyboard::Down || key == sf::Keyboard::S) {
        if (selectionIndex < static_cast<int>(options.size()) - 1) selectionIndex++;
    }
    else if (key == sf::Keyboard::Enter) {
        switch (selectionIndex) {
        case 0:
            selected = ShopSelection::HatShop;
            break;
        case 1:
            selected = ShopSelection::FishTankShop;
            break;
        case 2:
            selected = ShopSelection::ShelfShop;
            break;
        case 3:
            selected = ShopSelection::MiniGameShop;
            break;
        case 4:
            closeRequested = true;
            break;
        }
    }
}

bool ShopCategoryView::shouldClose() const {
    return closeRequested;
}

void ShopCategoryView::resetCloseFlag() {
    closeRequested = false;
}

ShopSelection ShopCategoryView::getSelectedOption() const {
    return selected;
}

