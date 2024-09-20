#include "SceneManager.h"

// Sets the current scene and initializes it
void SceneManager::setScene(const std::shared_ptr<Scene>& NewScene)
{
	MCurrentScene = NewScene;
	MCurrentScene->init();
}

// Handles input for the current scene
void SceneManager::handleInput(sf::RenderWindow& Window, sf::Event& Event) const
{
	if (MCurrentScene)
	{
		MCurrentScene->handleInput(Window, Event);
	}
}

// Updates the current scene
void SceneManager::update(const float DeltaTime) const
{
	if (MCurrentScene)
	{
		MCurrentScene->update(DeltaTime);
	}
}

// Renders the current scene
void SceneManager::draw(sf::RenderWindow& Window) const
{
	if (MCurrentScene)
	{
		MCurrentScene->draw(Window);
	}
}

// Returns the current scene
std::shared_ptr<Scene> SceneManager::getCurrentScene() const
{
	return MCurrentScene;
}
