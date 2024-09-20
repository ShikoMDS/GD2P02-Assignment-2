#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include <SFML/Graphics.hpp>

class TitleScreen final : public Scene
{
public:
	explicit TitleScreen(SceneManager& Manager);

	void init() override;
	void handleInput(sf::RenderWindow& Window, sf::Event& Event) override;
	void update(float DeltaTime) override;
	void draw(sf::RenderWindow& Window) override;
	void updateButtonPositions(const sf::Vector2u& WindowSize) override;

private:
	SceneManager& MSceneManager;

	sf::Font MFont;
	sf::RectangleShape MPlayButton, MInstructionsButton, MQuitButton;
	sf::Text MTitleText, MPlayText, MInstructionsText, MQuitText;
	sf::Texture MBackgroundTexture;
	sf::Sprite MBackgroundSprite;
};
