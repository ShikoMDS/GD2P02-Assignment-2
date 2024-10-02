#pragma once
#include "Scene.h"
#include <memory>

class SceneManager {
public:
    // Sets the current scene
    void setScene(const std::shared_ptr<Scene>& NewScene);

    // Render the current scene
    void draw(sf::RenderWindow& Window) const;

    // Update the current scene
    void update(float DeltaTime) const;

    // Handle input for the current scene
    void handleInput(sf::RenderWindow& Window, sf::Event& Event) const;

    // Returns the current scene
    [[nodiscard]] std::shared_ptr<Scene> getCurrentScene() const;

private:
    std::shared_ptr<Scene> MCurrentScene;
};
