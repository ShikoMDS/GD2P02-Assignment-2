#include "Level2.h"

Level2::Level2(SceneManager& manager) : sceneManager(manager), world(b2Vec2(0.0f, 9.8f)) {}

void Level2::init() {
    // Initialize Box2D world, bodies, and scene setup here
    // You can add objects that differ from Level 1 to make this a new unique level.
}

void Level2::handleInput(sf::RenderWindow& window, sf::Event& event) {
    // Handle user input for the game level
}

void Level2::update(float deltaTime) {
    // Step the Box2D world
    world.Step(deltaTime, 6, 2);

    // Handle win/loss conditions
    // if (game over or player wins) {
    //     sceneManager.setScene(std::make_shared<TitleScreen>(sceneManager));
    // }
}

void Level2::draw(sf::RenderWindow& window) {
    // Draw objects in the scene
}
