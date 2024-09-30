#include <SFML/Graphics.hpp>
#include "SceneManager.h"
#include "TitleScreen.h"

bool isFullscreen = false;

void toggleFullscreen(sf::RenderWindow& Window, bool& isFullscreen)
{
	if (isFullscreen)
	{
		Window.create(sf::VideoMode(1600, 900), "2D Physics Game", sf::Style::Resize | sf::Style::Close);
	}
	else
	{
		Window.create(sf::VideoMode::getDesktopMode(), "2D Physics Game", sf::Style::Fullscreen);
	}
	isFullscreen = !isFullscreen;
}

void enforceAspectRatio(sf::RenderWindow& Window, const float AspectRatio)
{
	const sf::Vector2u LWindowSize = Window.getSize(); // Get the current window size
	const float LCurrentAspectRatio = static_cast<float>(LWindowSize.x) / static_cast<float>(LWindowSize.y);

	// If the current aspect ratio does not match the desired one
	if (LCurrentAspectRatio > AspectRatio)
	{
		// Too wide: adjust the width
		const unsigned int LNewWidth = static_cast<unsigned int>(LWindowSize.y * AspectRatio);
		Window.setSize(sf::Vector2u(LNewWidth, LWindowSize.y));
	}
	else if (LCurrentAspectRatio < AspectRatio)
	{
		// Too tall: adjust the height
		const unsigned int LNewHeight = static_cast<unsigned int>(LWindowSize.x / AspectRatio);
		Window.setSize(sf::Vector2u(LWindowSize.x, LNewHeight));
	}

	// Adjust the view to fit the new window size
	const sf::FloatRect LVisibleArea(0, 0, Window.getSize().x, Window.getSize().y);
	Window.setView(sf::View(LVisibleArea));
}

int main()
{
	sf::RenderWindow LWindow(sf::VideoMode(1600, 900), "2D Physics Game", /*sf::Style::Resize | */sf::Style::Close);

	// Desired aspect ratio (16:9)
	constexpr float LAspectRatio = 16.0f / 9.0f;

	SceneManager LSceneManager;

	// Start with the title screen
	LSceneManager.setScene(std::make_shared<TitleScreen>(LSceneManager));

	sf::Clock LClock;

	while (LWindow.isOpen())
	{
		LSceneManager.getCurrentScene()->updateButtonPositions(LWindow.getSize()); // Update to current window size

		sf::Event LEvent;
		while (LWindow.pollEvent(LEvent))
		{
			if (LEvent.type == sf::Event::Closed)
				LWindow.close();

			/* --Removed resize function because of issues with physics world
			// Fullscreen toggle (F11)
			if (LEvent.type == sf::Event::KeyPressed && LEvent.key.code == sf::Keyboard::F11)
			{
				toggleFullscreen(LWindow, isFullscreen);
			}
			*/

			// Enforce aspect ratio on resize
			if (LEvent.type == sf::Event::Resized)
			{
				enforceAspectRatio(LWindow, LAspectRatio);

				// Recalculate positions and sizes
				auto LCurrentScene = LSceneManager.getCurrentScene();
				if (LCurrentScene)
				{
					LCurrentScene->updateButtonPositions(LWindow.getSize());
				}
			}

			// Forward event to current scene input handler
			LSceneManager.handleInput(LWindow, LEvent);
		}

		float LDeltaTime = LClock.restart().asSeconds();

		LSceneManager.update(LDeltaTime);

		LWindow.clear();
		LSceneManager.draw(LWindow);
		LWindow.display();
	}

	return 0;
}
