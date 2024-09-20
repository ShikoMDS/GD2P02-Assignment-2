#include "TitleScreen.h"
#include "LevelSelection.h"
#include "Instructions.h"

TitleScreen::TitleScreen(SceneManager& manager) : sceneManager(manager) {}

void TitleScreen::init() {
    font.loadFromFile("resources/sugar bread/Sugar Bread.otf");

    // Title Text
    title.setFont(font);
    title.setString("2D Physics Game");
    title.setCharacterSize(80);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    title.setPosition(800, 150);  // Center the title

    // Play Button
    playButton.setSize(sf::Vector2f(300, 75));
    playButton.setFillColor(sf::Color::Transparent);
    playButton.setOutlineColor(sf::Color::White);
    playButton.setOutlineThickness(2.0f);

    playText.setFont(font);
    playText.setString("PLAY");
    playText.setCharacterSize(40);
    playText.setFillColor(sf::Color::White);

    // Instructions Button
    instructionsButton.setSize(sf::Vector2f(300, 75));
    instructionsButton.setFillColor(sf::Color::Transparent);
    instructionsButton.setOutlineColor(sf::Color::White);
    instructionsButton.setOutlineThickness(2.0f);

    instructionsText.setFont(font);
    instructionsText.setString("INSTRUCTIONS");
    instructionsText.setCharacterSize(40);
    instructionsText.setFillColor(sf::Color::White);

    // Quit Button
    quitButton.setSize(sf::Vector2f(300, 75));
    quitButton.setFillColor(sf::Color::Transparent);
    quitButton.setOutlineColor(sf::Color::White);
    quitButton.setOutlineThickness(2.0f);

    quitText.setFont(font);
    quitText.setString("QUIT");
    quitText.setCharacterSize(40);
    quitText.setFillColor(sf::Color::White);

    // Update positions for the first time
    updateButtonPositions(sf::Vector2u(1600, 900));  // Default window size
}

void TitleScreen::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Check if the Play button is clicked (now moved above)
        if (playButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            // Add logic to start the game or go to the level selection screen
            sceneManager.setScene(std::make_shared<LevelSelection>(sceneManager));
        }

        // Check if the Instructions button is clicked (now moved below)
        else if (instructionsButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            sceneManager.setScene(std::make_shared<Instructions>(sceneManager));
        }

        // Check if the Quit button is clicked
        else if (quitButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            window.close();
        }
    }
}

void TitleScreen::update(float deltaTime) {
    // No updates needed for the title screen
}

void TitleScreen::draw(sf::RenderWindow& window) {
    window.draw(title);

    // Draw the buttons
    window.draw(playButton);
    window.draw(instructionsButton);
    window.draw(quitButton);

    // Draw the button text
    window.draw(playText);
    window.draw(instructionsText);
    window.draw(quitText);
}

void TitleScreen::updateButtonPositions(const sf::Vector2u& windowSize) {
    float scaleX = static_cast<float>(windowSize.x) / 1600.0f;
    float scaleY = static_cast<float>(windowSize.y) / 900.0f;

    // Align title
    title.setCharacterSize(static_cast<unsigned int>(80 * scaleY));
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    title.setPosition(windowSize.x / 2.0f, 150 * scaleY);

    // Set Button sizes
    playButton.setSize(sf::Vector2f(300 * scaleX, 75 * scaleY));
    instructionsButton.setSize(sf::Vector2f(300 * scaleX, 75 * scaleY));
    quitButton.setSize(sf::Vector2f(300 * scaleX, 75 * scaleY));

    // Position Buttons
    float buttonSpacing = 40 * scaleY;
    quitButton.setPosition((windowSize.x / 2.0f) - (quitButton.getSize().x / 2.0f), 675 * scaleY);  // Position of Quit button
    instructionsButton.setPosition((windowSize.x / 2.0f) - (instructionsButton.getSize().x / 2.0f), quitButton.getPosition().y - instructionsButton.getSize().y - buttonSpacing);
    playButton.setPosition((windowSize.x / 2.0f) - (playButton.getSize().x / 2.0f), instructionsButton.getPosition().y - playButton.getSize().y - buttonSpacing);

    // Center text within buttons
    sf::FloatRect playTextRect = playText.getLocalBounds();
    playText.setOrigin(playTextRect.left + playTextRect.width / 2.0f, playTextRect.top + playTextRect.height / 2.0f);
    playText.setPosition(playButton.getPosition().x + playButton.getSize().x / 2.0f, playButton.getPosition().y + playButton.getSize().y / 2.0f);

    sf::FloatRect instructionsTextRect = instructionsText.getLocalBounds();
    instructionsText.setOrigin(instructionsTextRect.left + instructionsTextRect.width / 2.0f, instructionsTextRect.top + instructionsTextRect.height / 2.0f);
    instructionsText.setPosition(instructionsButton.getPosition().x + instructionsButton.getSize().x / 2.0f, instructionsButton.getPosition().y + instructionsButton.getSize().y / 2.0f);

    sf::FloatRect quitTextRect = quitText.getLocalBounds();
    quitText.setOrigin(quitTextRect.left + quitTextRect.width / 2.0f, quitTextRect.top + quitTextRect.height / 2.0f);
    quitText.setPosition(quitButton.getPosition().x + quitButton.getSize().x / 2.0f, quitButton.getPosition().y + quitButton.getSize().y / 2.0f);
}