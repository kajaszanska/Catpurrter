#include "Computer.h"
#include <iostream>

Computer::Computer(sf::Font& fontRef, Player& playerRef)
    : font(fontRef), playerData(playerRef) {
}


void Computer::init() {
    options.clear();
    selectionIndex = 0;
    selected = ComputerSelection::None;
    closeRequested = false;

   
    options.push_back(sf::Text("Shop", font, 32));

    // Dynamically add mini games
    for (const auto& game : playerData.ownedMiniGames) {
        options.emplace_back(sf::Text(game, font, 32));
    }

    options.push_back(sf::Text("Back", font, 32));

    float y = 200.f;
    for (auto& text : options) {
        text.setFillColor(sf::Color::White);
        text.setPosition(100.f, y);
        y += 60.f;
    }
}


void Computer::update() {
    for (size_t i = 0; i < options.size(); ++i) {
        options[i].setFillColor(i == selectionIndex ? sf::Color::Yellow : sf::Color::White);
    }
}

void Computer::render(sf::RenderWindow& window) {
    sf::Text title("Computer", font, 36);
    title.setFillColor(sf::Color::Cyan);
    title.setPosition(100.f, 50.f);
    window.draw(title);

    for (const auto& opt : options)
        window.draw(opt);

    /* Draw owned mini games
    float y = 150.f;
    for (const auto& game : playerData.ownedMiniGames) {
        sf::Text gameText;
        gameText.setFont(font);
        gameText.setCharacterSize(24);
        gameText.setPosition(100.f, y);
        gameText.setString(game.c_str());
        gameText.setFillColor(sf::Color::White);
        window.draw(gameText);
        y += 30.f;
    }
    */
}




void Computer::handleInput(sf::Keyboard::Key key) {
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
        if (selectionIndex == 0) {
            selected = ComputerSelection::Shop;
        }
        else if (selectionIndex == static_cast<int>(options.size()) - 1) {
            closeRequested = true;
        }
        else {
            selectedMiniGame = playerData.ownedMiniGames[selectionIndex - 1];
            std::cout << "Selected mini game: " << selectedMiniGame << "\n";
            selected = ComputerSelection::MiniGame;
         
        }
    }
}



bool Computer::shouldClose() const {
    return closeRequested;
}

void Computer::resetCloseFlag() {
    closeRequested = false;
}

ComputerSelection Computer::getSelectedOption() const {
    return selected;
}
