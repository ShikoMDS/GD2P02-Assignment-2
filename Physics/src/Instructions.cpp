#include "Instructions.h"
#include "TitleScreen.h"

Instructions::Instructions(SceneManager& manager) : sceneManager(manager) {}

void Instructions::init() {
    font.loadFromFile("resources/sugar bread/Sugar Bread.otf");

    // Load the background texture
    backgroundTexture.loadFromFile("resources/kenney physics assets/PNG/Backgrounds/blue_land.png");
    backgroundSprite.setTexture(backgroundTexture);

    // Instructions Title
    instructionsTitle.setFont(font);
    instructionsTitle.setFillColor(sf::Color::Black);
    instructionsTitle.setString("INSTRUCTIONS");
    instructionsTitle.setCharacterSize(80);
    sf::FloatRect titleBounds = instructionsTitle.getLocalBounds();
    instructionsTitle.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    instructionsTitle.setPosition(800, 150);

    // Instructions Text
    instructionsText.setFont(font);
    instructionsText.setFillColor(sf::Color::Black);
    instructionsText.setString("- Fling projectiles to destroy enemies\n- Use the mouse to aim and shoot");
    instructionsText.setCharacterSize(50);
    sf::FloatRect instructionsTextBounds = instructionsText.getLocalBounds();
    instructionsText.setOrigin(instructionsTextBounds.width / 2.0f, instructionsTextBounds.height / 2.0f);
    instructionsText.setPosition(800, 350);

    // Back Button
    backButton.setSize(sf::Vector2f(300, 75));
    backButton.setFillColor(sf::Color::Transparent);
    backButton.setOutlineColor(sf::Color::Black);
    backButton.setOutlineThickness(2.0f);
    backButton.setPosition(650, 675);

    // Center the text within the Back button
    backText.setFont(font);
    backText.setFillColor(sf::Color::Black);
    backText.setString("BACK");
    backText.setCharacterSize(50);
    sf::FloatRect backTextBounds = backText.getLocalBounds();
    backText.setOrigin(backTextBounds.width / 2.0f, backTextBounds.height / 2.0f + backTextBounds.top / 2.0f);  // Adjust for top offset
    backText.setPosition(backButton.getPosition().x + backButton.getSize().x / 2.0f, backButton.getPosition().y + backButton.getSize().y / 2.0f);
}

void Instructions::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            sceneManager.setScene(std::make_shared<TitleScreen>(sceneManager));
        }
    }
}

void Instructions::update(float deltaTime) {}

void Instructions::draw(sf::RenderWindow& window) {
    // Draw the background first
    window.draw(backgroundSprite);

    window.draw(instructionsTitle);
    window.draw(instructionsText);
    window.draw(backButton);
    window.draw(backText);
}

void Instructions::updateButtonPositions(const sf::Vector2u& windowSize) {
    float scaleX = static_cast<float>(windowSize.x) / 1600.0f;
    float scaleY = static_cast<float>(windowSize.y) / 900.0f;

    // Scale and position the background
    backgroundSprite.setScale(
        static_cast<float>(windowSize.x) / backgroundTexture.getSize().x,
        static_cast<float>(windowSize.y) / backgroundTexture.getSize().y
    );

    // Update title size and position
    instructionsTitle.setCharacterSize(static_cast<unsigned int>(80 * scaleY));
    sf::FloatRect titleBounds = instructionsTitle.getLocalBounds();
    instructionsTitle.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    instructionsTitle.setPosition(windowSize.x / 2.0f, 150 * scaleY);

    // Update instruction text size and position
    instructionsText.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
    sf::FloatRect instructionsTextBounds = instructionsText.getLocalBounds();
    instructionsText.setOrigin(instructionsTextBounds.width / 2.0f, instructionsTextBounds.height / 2.0f);
    instructionsText.setPosition(windowSize.x / 2.0f, 350 * scaleY);

    // Update back button size and position
    backButton.setSize(sf::Vector2f(300 * scaleX, 75 * scaleY));
    backButton.setPosition((windowSize.x / 2.0f) - (backButton.getSize().x / 2.0f), 675 * scaleY);

    // Update back button text size and position (moved up for centering)
    backText.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
    sf::FloatRect backTextBounds = backText.getLocalBounds();
    backText.setOrigin(backTextBounds.width / 2.0f, backTextBounds.height / 2.0f + backTextBounds.top);  // Adjust for top offset like in level select
    backText.setPosition(backButton.getPosition().x + backButton.getSize().x / 2.0f, backButton.getPosition().y + backButton.getSize().y / 2.0f);
}
