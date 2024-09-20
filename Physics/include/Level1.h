#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>

class Level1 : public Scene {
private:
    b2World world;
    SceneManager& sceneManager;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Texture bushTexture;
    sf::Sprite bushSprite;

    bool isPaused = false;
    sf::RectangleShape darkOverlay;
    sf::Text resumeButton;
    sf::Text restartButton;
    sf::Text menuButton;

    sf::Font font;

    bool pKeyPressed = false;  // To track the state of the 'P' key

public:
    Level1(SceneManager& manager);

    void init() override;
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

    void togglePause();
};
