#pragma once
#include "Scene.h"
#include <memory>

class SceneManager {
private:
    std::shared_ptr<Scene> currentScene;

public:
    // Sets the current scene
    void setScene(std::shared_ptr<Scene> newScene);

    // Handle input for the current scene
    void handleInput(sf::RenderWindow& window, sf::Event& event);

    // Update the current scene
    void update(float deltaTime);

    // Render the current scene
    void draw(sf::RenderWindow& window);

    // Returns the current scene
    std::shared_ptr<Scene> getCurrentScene() const;
};
