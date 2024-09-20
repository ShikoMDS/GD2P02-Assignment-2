#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include <box2d/box2d.h>

class Level1 : public Scene {
private:
    b2World world;
    SceneManager& sceneManager;

public:
    Level1(SceneManager& manager);

    void init() override;
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
};
