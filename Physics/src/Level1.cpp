#include "Level1.h"

Level1::Level1(SceneManager& manager) : sceneManager(manager), world(b2Vec2(0.0f, 9.8f)) {}

void Level1::init() {
    // Initialize Box2D world, bodies, and scene setup here
    // Example: Set up the ground and any interactive objects
}

void Level1::handleInput(sf::RenderWindow& window, sf::Event& event) {
    // Handle user input for the game level
}

void Level1::update(float deltaTime) {
    // Step the Box2D world
    world.Step(deltaTime, 6, 2);

    // Handle win/loss conditions
    // if (game over or player wins) {
    //     sceneManager.setScene(std::make_shared<TitleScreen>(sceneManager));
    // }
}

void Level1::draw(sf::RenderWindow& window) {
    // Draw objects in the scene
}
