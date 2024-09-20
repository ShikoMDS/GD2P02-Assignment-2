#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include <SFML/Graphics.hpp>

class LevelSelection : public Scene {
private:
    sf::Font font;
    sf::Text levelText;
    sf::RectangleShape backButton;
    sf::Text backText;
    sf::RectangleShape level1Button;
    sf::Text level1Text;
    sf::RectangleShape level2Button;
    sf::Text level2Text;
    SceneManager& sceneManager;

public:
    LevelSelection(SceneManager& manager);

    void init() override;
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

    void updateButtonPositions(const sf::Vector2u& windowSize) override;
};
