#include "LevelSelection.h"
#include "TitleScreen.h"
#include "Level1.h"
#include "Level2.h"

LevelSelection::LevelSelection(SceneManager& Manager) : MSceneManager(Manager)
{
}

void LevelSelection::init()
{
	MFont.loadFromFile("resources/sugar bread/Sugar Bread.otf");

	MBackgroundTexture.loadFromFile("resources/kenney physics assets/PNG/Backgrounds/colored_grass.png");
	MBackgroundSprite.setTexture(MBackgroundTexture);

	if (!MLevel1Texture.loadFromFile("resources/kenney physics assets/PNG/Backgrounds/colored_desert.png"))
	{
		std::cerr << "Error loading Level 1 button texture\n";
	}
	if (!MLevel2Texture.loadFromFile("resources/kenney physics assets/PNG/Backgrounds/colored_shroom.png"))
	{
		std::cerr << "Error loading Level 2 button texture\n";
	}

	MLevelText.setFont(MFont);
	MLevelText.setString("SELECT LEVEL");
	MLevelText.setCharacterSize(80);
	MLevelText.setFillColor(sf::Color::Black);
	const sf::FloatRect LTitleBounds = MLevelText.getLocalBounds();
	MLevelText.setOrigin(LTitleBounds.width / 2.0f, LTitleBounds.height / 2.0f);
	MLevelText.setPosition(800, 100);

	MLevel1Button.setSize(sf::Vector2f(400, 250));
	MLevel1Button.setTexture(&MLevel1Texture); // Set texture for Level 1
	MLevel1Button.setOutlineColor(sf::Color::Black);
	MLevel1Button.setOutlineThickness(5.0f);
	MLevel1Button.setPosition(250, 300);

	MLevel1Text.setFont(MFont);
	MLevel1Text.setFillColor(sf::Color::Black);
	MLevel1Text.setString("LEVEL 1");
	MLevel1Text.setCharacterSize(50);
	const sf::FloatRect LLevel1TextBounds = MLevel1Text.getLocalBounds();
	MLevel1Text.setOrigin(LLevel1TextBounds.width / 2.0f, LLevel1TextBounds.height / 2.0f);
	MLevel1Text.setPosition(MLevel1Button.getPosition().x + MLevel1Button.getSize().x / 2.0f,
	                        MLevel1Button.getPosition().y + MLevel1Button.getSize().y / 2.0f);

	MLevel2Button.setSize(sf::Vector2f(400, 250));
	MLevel2Button.setTexture(&MLevel2Texture); // Set texture for Level 2
	MLevel2Button.setOutlineColor(sf::Color::Black);
	MLevel2Button.setOutlineThickness(5.0f);
	MLevel2Button.setPosition(950, 300);

	MLevel2Text.setFont(MFont);
	MLevel2Text.setFillColor(sf::Color::Black);
	MLevel2Text.setString("LEVEL 2");
	MLevel2Text.setCharacterSize(50);
	const sf::FloatRect LLevel2TextBounds = MLevel2Text.getLocalBounds();
	MLevel2Text.setOrigin(LLevel2TextBounds.width / 2.0f, LLevel2TextBounds.height / 2.0f);
	MLevel2Text.setPosition(MLevel2Button.getPosition().x + MLevel2Button.getSize().x / 2.0f,
	                        MLevel2Button.getPosition().y + MLevel2Button.getSize().y / 2.0f);

	MBackButton.setSize(sf::Vector2f(300, 75));
	MBackButton.setFillColor(sf::Color::Transparent);
	MBackButton.setOutlineColor(sf::Color::Black);
	MBackButton.setOutlineThickness(2.0f);
	MBackButton.setPosition(650, 675);

	MBackText.setFont(MFont);
	MBackText.setFillColor(sf::Color::Black);
	MBackText.setString("BACK");
	MBackText.setCharacterSize(50);
	const sf::FloatRect LBackTextBounds = MBackText.getLocalBounds();
	MBackText.setOrigin(LBackTextBounds.width / 2.0f, LBackTextBounds.height / 2.0f);
	MBackText.setPosition(MBackButton.getPosition().x + MBackButton.getSize().x / 2.0f,
	                      MBackButton.getPosition().y + MBackButton.getSize().y / 2.0f);
}

void LevelSelection::handleInput(sf::RenderWindow& Window, sf::Event& Event)
{
	if (Event.type == sf::Event::MouseButtonReleased && Event.mouseButton.button == sf::Mouse::Left)
	{
		const sf::Vector2i LMousePos = sf::Mouse::getPosition(Window);

		if (MLevel1Button.getGlobalBounds().contains(static_cast<float>(LMousePos.x), static_cast<float>(LMousePos.y)))
		{
			MSceneManager.setScene(std::make_shared<Level1>(MSceneManager));
		}
		else if (MLevel2Button.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
		                                                  static_cast<float>(LMousePos.y)))
		{
			MSceneManager.setScene(std::make_shared<Level2>(MSceneManager));
		}
		else if (MBackButton.getGlobalBounds().contains(static_cast<float>(LMousePos.x),
		                                                static_cast<float>(LMousePos.y)))
		{
			MSceneManager.setScene(std::make_shared<TitleScreen>(MSceneManager));
		}
	}
}

void LevelSelection::update(float DeltaTime)
{
}

void LevelSelection::draw(sf::RenderWindow& Window)
{
	Window.draw(MBackgroundSprite);

	Window.draw(MLevelText);

	Window.draw(MLevel1Button);
	Window.draw(MLevel1Text);

	Window.draw(MLevel2Button);
	Window.draw(MLevel2Text);

	Window.draw(MBackButton);
	Window.draw(MBackText);
}

void LevelSelection::updateButtonPositions(const sf::Vector2u& WindowSize)
{
	const float LScaleX = static_cast<float>(WindowSize.x) / 1600.0f;
	const float LScaleY = static_cast<float>(WindowSize.y) / 900.0f;

	// Scale and position the background
	MBackgroundSprite.setScale(
		static_cast<float>(WindowSize.x) / MBackgroundTexture.getSize().x,
		static_cast<float>(WindowSize.y) / MBackgroundTexture.getSize().y
	);

	// Set title position and size
	MLevelText.setCharacterSize(static_cast<unsigned int>(80 * LScaleY));
	const sf::FloatRect LTitleBounds = MLevelText.getLocalBounds();
	MLevelText.setOrigin(LTitleBounds.width / 2.0f, LTitleBounds.height / 2.0f);
	MLevelText.setPosition(WindowSize.x / 2.0f, 150 * LScaleY);

	// Adjust button size and position
	MLevel1Button.setSize(sf::Vector2f(400 * LScaleX, 250 * LScaleY));
	MLevel1Button.setPosition((WindowSize.x / 2.0f) - 500 * LScaleX, 320 * LScaleY);
	MLevel1Text.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));

	// Adjust text position to account for vertical centering
	const sf::FloatRect LLevel1TextBounds = MLevel1Text.getLocalBounds();
	MLevel1Text.setOrigin(LLevel1TextBounds.width / 2.0f,
	                      LLevel1TextBounds.height / 2.0f + LLevel1TextBounds.top * 1.2f); // Fine-tuned offset
	MLevel1Text.setPosition(MLevel1Button.getPosition().x + MLevel1Button.getSize().x / 2.0f,
	                        MLevel1Button.getPosition().y + MLevel1Button.getSize().y / 2.0f);

	// Repeat the same for Level 2
	MLevel2Button.setSize(sf::Vector2f(400 * LScaleX, 250 * LScaleY));
	MLevel2Button.setPosition((WindowSize.x / 2.0f) + 100 * LScaleX, 320 * LScaleY);
	MLevel2Text.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));

	const sf::FloatRect LLevel2TextBounds = MLevel2Text.getLocalBounds();
	MLevel2Text.setOrigin(LLevel2TextBounds.width / 2.0f,
	                      LLevel2TextBounds.height / 2.0f + LLevel2TextBounds.top * 1.2f); // Fine-tuned offset
	MLevel2Text.setPosition(MLevel2Button.getPosition().x + MLevel2Button.getSize().x / 2.0f,
	                        MLevel2Button.getPosition().y + MLevel2Button.getSize().y / 2.0f);

	// Back button adjustments
	MBackButton.setSize(sf::Vector2f(300 * LScaleX, 75 * LScaleY));
	MBackButton.setPosition((WindowSize.x / 2.0f) - (MBackButton.getSize().x / 2.0f), 675 * LScaleY);

	MBackText.setCharacterSize(static_cast<unsigned int>(50 * LScaleY));
	const sf::FloatRect LBackTextBounds = MBackText.getLocalBounds();
	MBackText.setOrigin(LBackTextBounds.width / 2.0f, LBackTextBounds.height / 2.0f + LBackTextBounds.top * 1.2f);
	// Fine-tuned offset
	MBackText.setPosition(MBackButton.getPosition().x + MBackButton.getSize().x / 2.0f,
	                      MBackButton.getPosition().y + MBackButton.getSize().y / 2.0f);
}
