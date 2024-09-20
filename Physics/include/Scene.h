#pragma once
#include <SFML/Graphics.hpp>

class Scene {
public:
    virtual void init() = 0;
    virtual void handleInput(sf::RenderWindow& window, sf::Event& event) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    // Virtual function to update button positions on window resize
    virtual void updateButtonPositions(const sf::Vector2u& windowSize) {
        // Default: do nothing. Child classes should override this if needed.
    }

    virtual ~Scene() = default;
};
