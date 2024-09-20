#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include "TitleScreen.h"

class Instructions : public Scene {
private:
    sf::Font font;
    sf::Text instructionsTitle;
    sf::Text instructionsText;
    sf::RectangleShape backButton;
    sf::Text backText;
    SceneManager& sceneManager;

public:
    Instructions(SceneManager& manager);

    void init() override;
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

    void updateButtonPositions(const sf::Vector2u& windowSize) override;
};