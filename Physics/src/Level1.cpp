#include "Level1.h"

#include "TitleScreen.h"

Level1::Level1(SceneManager& manager) : sceneManager(manager), world(b2Vec2(0.0f, 9.8f)) {}

void Level1::init() {
    // Load the background texture
    if (!backgroundTexture.loadFromFile("resources/kenney physics assets/PNG/Backgrounds/colored_desert.png")) {
        // Handle error
    }
    backgroundSprite.setTexture(backgroundTexture);

    // Load font for the buttons
    if (!font.loadFromFile("resources/sugar bread/Sugar Bread.otf")) {
        // Handle error
    }

    // Initialize pause menu buttons
    resumeButton.setFont(font);
    resumeButton.setString("Resume");
    resumeButton.setCharacterSize(50);
    resumeButton.setFillColor(sf::Color::White);
    resumeButton.setPosition(600, 300);

    restartButton.setFont(font);
    restartButton.setString("Restart");
    restartButton.setCharacterSize(50);
    restartButton.setFillColor(sf::Color::White);
    restartButton.setPosition(600, 400);

    menuButton.setFont(font);
    menuButton.setString("Menu");
    menuButton.setCharacterSize(50);
    menuButton.setFillColor(sf::Color::White);
    menuButton.setPosition(600, 500);

    // Initialize dark overlay for pause
    darkOverlay.setSize(sf::Vector2f(1600.0f, 900.0f));
    darkOverlay.setFillColor(sf::Color(0, 0, 0, 150)); // Semi-transparent black
}

void Level1::togglePause() {
    isPaused = !isPaused;
}

void Level1::handleInput(sf::RenderWindow& window, sf::Event& event) {
    // Handle pausing with the 'P' key without using KeyReleased and avoiding key repeat
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
        if (!pKeyPressed) {
            pKeyPressed = true;  // Mark the key as pressed
            togglePause();  // Toggle pause when 'P' is pressed once
        }
    }
    else {
        pKeyPressed = false;  // Reset the flag when the key is released
    }

    if (isPaused) {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            if (resumeButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                togglePause();  // Resume the game
            }
            else if (restartButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                sceneManager.setScene(std::make_shared<Level1>(sceneManager));  // Restart the level
            }
            else if (menuButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                sceneManager.setScene(std::make_shared<TitleScreen>(sceneManager));  // Return to menu
            }
        }
    }
}

void Level1::update(float deltaTime) {
    if (!isPaused) {
        // Step the Box2D world if the game is not paused
        world.Step(deltaTime, 6, 2);
    }

    // Handle win/loss conditions here
}

void Level1::draw(sf::RenderWindow& window) {
    // Get the window size
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = backgroundTexture.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    backgroundSprite.setScale(scaleX, scaleY);
    backgroundSprite.setPosition(0.0f, 0.0f);
    window.draw(backgroundSprite);

    if (isPaused) {
        // Draw dark overlay and pause menu buttons
        window.draw(darkOverlay);
        window.draw(resumeButton);
        window.draw(restartButton);
        window.draw(menuButton);
    }

    // Draw other objects in the level
}
