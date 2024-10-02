#pragma once
#include <SFML/Graphics.hpp>

class Scene
{
public:
	virtual void init() = 0;
	virtual void draw(sf::RenderWindow& Window) = 0;
	virtual void update(float DeltaTime) = 0;
	virtual void handleInput(sf::RenderWindow& Window, sf::Event& Event) = 0;

	// Virtual function to update button positions on window resize
	virtual void updateButtonPositions(const sf::Vector2u& WindowSize)
	{
		// Default: do nothing. Child classes should override this if needed.
	}

	virtual ~Scene() = default;
};
