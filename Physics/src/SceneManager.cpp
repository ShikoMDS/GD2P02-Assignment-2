#include "SceneManager.h"

// Sets the current scene and initializes it
void SceneManager::setScene(std::shared_ptr<Scene> newScene) {
    currentScene = newScene;
    currentScene->init();
}

// Handles input for the current scene
void SceneManager::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (currentScene) {
        currentScene->handleInput(window, event);
    }
}

// Updates the current scene
void SceneManager::update(float deltaTime) {
    if (currentScene) {
        currentScene->update(deltaTime);
    }
}

// Renders the current scene
void SceneManager::draw(sf::RenderWindow& window) {
    if (currentScene) {
        currentScene->draw(window);
    }
}

// Returns the current scene
std::shared_ptr<Scene> SceneManager::getCurrentScene() const {
    return currentScene;
}
