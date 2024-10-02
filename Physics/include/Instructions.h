#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include "TitleScreen.h"

class Instructions final : public Scene
{
public:
	explicit Instructions(SceneManager& Manager);

	void init() override;
	void draw(sf::RenderWindow& Window) override;
	void update(float DeltaTime) override;
	void handleInput(sf::RenderWindow& Window, sf::Event& Event) override;
	void updateButtonPositions(const sf::Vector2u& WindowSize) override;

private:
	SceneManager& MSceneManager;

	sf::Font MFont;
	sf::RectangleShape MBackButton;
	sf::Text MTitleText, MInstructionsText, MBackText;
	sf::Texture MBackgroundTexture;
	sf::Sprite MBackgroundSprite;
};
