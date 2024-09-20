#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include <SFML/Graphics.hpp>
#include <iostream>

class LevelSelection : public Scene {
public:
    LevelSelection(SceneManager& manager);

    void init() override;
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

    void updateButtonPositions(const sf::Vector2u& windowSize) override;

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

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
	sf::Texture level1Texture;
	sf::Texture level2Texture;
};
