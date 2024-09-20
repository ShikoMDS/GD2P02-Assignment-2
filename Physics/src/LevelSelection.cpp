#include "LevelSelection.h"
#include "TitleScreen.h"
#include "Level1.h"
#include "Level2.h"

LevelSelection::LevelSelection(SceneManager& manager) : sceneManager(manager) {}

void LevelSelection::init() {
    font.loadFromFile("resources/sugar bread/Sugar Bread.otf");

    // Level Selection Title
    levelText.setFont(font);
    levelText.setString("SELECT LEVEL");
    levelText.setCharacterSize(80);
    sf::FloatRect titleBounds = levelText.getLocalBounds();
    levelText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    levelText.setPosition(800, 100);

    // Level 1 Button
    level1Button.setSize(sf::Vector2f(400, 250));
    level1Button.setFillColor(sf::Color::Transparent);
    level1Button.setOutlineColor(sf::Color::Blue);
    level1Button.setOutlineThickness(5.0f);
    level1Button.setPosition(250, 300);

    // Center the text within the Level 1 button
    level1Text.setFont(font);
    level1Text.setString("LEVEL 1");
    level1Text.setCharacterSize(50);
    sf::FloatRect level1TextBounds = level1Text.getLocalBounds();
    level1Text.setOrigin(level1TextBounds.width / 2.0f, level1TextBounds.height / 2.0f + level1TextBounds.top / 2.0f);  // Adjust for top offset
    level1Text.setPosition(level1Button.getPosition().x + level1Button.getSize().x / 2.0f, level1Button.getPosition().y + level1Button.getSize().y / 2.0f);

    // Level 2 Button
    level2Button.setSize(sf::Vector2f(400, 250));
    level2Button.setFillColor(sf::Color::Transparent);
    level2Button.setOutlineColor(sf::Color::Blue);
    level2Button.setOutlineThickness(5.0f);
    level2Button.setPosition(950, 300);

    // Center the text within the Level 2 button
    level2Text.setFont(font);
    level2Text.setString("LEVEL 2");
    level2Text.setCharacterSize(50);
    sf::FloatRect level2TextBounds = level2Text.getLocalBounds();
    level2Text.setOrigin(level2TextBounds.width / 2.0f, level2TextBounds.height / 2.0f + level2TextBounds.top / 2.0f);  // Adjust for top offset
    level2Text.setPosition(level2Button.getPosition().x + level2Button.getSize().x / 2.0f, level2Button.getPosition().y + level2Button.getSize().y / 2.0f);

    // Back Button
    backButton.setSize(sf::Vector2f(300, 75));
    backButton.setFillColor(sf::Color::Transparent);
    backButton.setOutlineColor(sf::Color::White);
    backButton.setOutlineThickness(2.0f);
    backButton.setPosition(650, 675);

    // Center the text within the Back button
    backText.setFont(font);
    backText.setString("BACK");
    backText.setCharacterSize(50);
    sf::FloatRect backTextBounds = backText.getLocalBounds();
    backText.setOrigin(backTextBounds.width / 2.0f, backTextBounds.height / 2.0f + backTextBounds.top / 2.0f);  // Adjust for top offset
    backText.setPosition(backButton.getPosition().x + backButton.getSize().x / 2.0f, backButton.getPosition().y + backButton.getSize().y / 2.0f);
}

void LevelSelection::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        if (level1Button.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            sceneManager.setScene(std::make_shared<Level1>(sceneManager));
        }
        else if (level2Button.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            sceneManager.setScene(std::make_shared<Level2>(sceneManager));
        }
        else if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            sceneManager.setScene(std::make_shared<TitleScreen>(sceneManager));
        }
    }
}

void LevelSelection::update(float deltaTime) {}

void LevelSelection::draw(sf::RenderWindow& window) {
    window.draw(levelText);
    window.draw(level1Button);
    window.draw(level1Text);
    window.draw(level2Button);
    window.draw(level2Text);
    window.draw(backButton);
    window.draw(backText);
}

void LevelSelection::updateButtonPositions(const sf::Vector2u& windowSize) {
    float scaleX = static_cast<float>(windowSize.x) / 1600.0f;
    float scaleY = static_cast<float>(windowSize.y) / 900.0f;

    // Set the title's position and size
    levelText.setCharacterSize(static_cast<unsigned int>(80 * scaleY));
    sf::FloatRect titleBounds = levelText.getLocalBounds();
    levelText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    levelText.setPosition(windowSize.x / 2.0f, 150 * scaleY);

    // Adjust button size and position
    level1Button.setSize(sf::Vector2f(400 * scaleX, 250 * scaleY));
    level1Button.setPosition((windowSize.x / 2.0f) - 500 * scaleX, 320 * scaleY);
    level1Text.setCharacterSize(static_cast<unsigned int>(50 * scaleY));

    // Adjust the text position to account for vertical centering
    sf::FloatRect level1TextBounds = level1Text.getLocalBounds();
    level1Text.setOrigin(level1TextBounds.width / 2.0f, level1TextBounds.height / 2.0f + level1TextBounds.top * 1.2f);  // Fine-tuned offset
    level1Text.setPosition(level1Button.getPosition().x + level1Button.getSize().x / 2.0f, level1Button.getPosition().y + level1Button.getSize().y / 2.0f);

    // Repeat the same for Level 2
    level2Button.setSize(sf::Vector2f(400 * scaleX, 250 * scaleY));
    level2Button.setPosition((windowSize.x / 2.0f) + 100 * scaleX, 320 * scaleY);
    level2Text.setCharacterSize(static_cast<unsigned int>(50 * scaleY));

    sf::FloatRect level2TextBounds = level2Text.getLocalBounds();
    level2Text.setOrigin(level2TextBounds.width / 2.0f, level2TextBounds.height / 2.0f + level2TextBounds.top * 1.2f);  // Fine-tuned offset
    level2Text.setPosition(level2Button.getPosition().x + level2Button.getSize().x / 2.0f, level2Button.getPosition().y + level2Button.getSize().y / 2.0f);

    // Back button adjustments
    backButton.setSize(sf::Vector2f(300 * scaleX, 75 * scaleY));
    backButton.setPosition((windowSize.x / 2.0f) - (backButton.getSize().x / 2.0f), 675 * scaleY);

    backText.setCharacterSize(static_cast<unsigned int>(50 * scaleY));
    sf::FloatRect backTextBounds = backText.getLocalBounds();
    backText.setOrigin(backTextBounds.width / 2.0f, backTextBounds.height / 2.0f + backTextBounds.top * 1.2f);  // Fine-tuned offset
    backText.setPosition(backButton.getPosition().x + backButton.getSize().x / 2.0f, backButton.getPosition().y + backButton.getSize().y / 2.0f);
}
