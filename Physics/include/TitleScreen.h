#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include <SFML/Graphics.hpp>

class TitleScreen : public Scene {
public:
    TitleScreen(SceneManager& manager);

    // Override required functions
    void init() override;
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

    // Override updateButtonPositions to adjust button positions on window resize
    void updateButtonPositions(const sf::Vector2u& windowSize) override;

private:
    sf::Font font;
    sf::Text title;
    sf::RectangleShape playButton, instructionsButton, quitButton;

    // Button labels
    sf::Text playText;
    sf::Text instructionsText;
    sf::Text quitText;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    SceneManager& sceneManager;

};
