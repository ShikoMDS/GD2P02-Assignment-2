#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include <SFML/Graphics.hpp>
#include <iostream>

class LevelSelection final : public Scene
{
public:
	explicit LevelSelection(SceneManager& Manager);

	void init() override;
	void draw(sf::RenderWindow& Window) override;
	void update(float DeltaTime) override;
	void handleInput(sf::RenderWindow& Window, sf::Event& Event) override;
	void updateButtonPositions(const sf::Vector2u& WindowSize) override;

private:
	SceneManager& MSceneManager;

	sf::Font MFont;
	sf::Text MLevelText, MLevel1Text, MLevel2Text, MBackText;
	sf::RectangleShape MLevel1Button, MLevel2Button, MBackButton;
	sf::Texture MBackgroundTexture, MLevel1Texture, MLevel2Texture;
	sf::Sprite MBackgroundSprite;
};
