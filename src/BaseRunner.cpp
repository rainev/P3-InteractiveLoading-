#include "BaseRunner.h"
#include  "GameObjectManager.h"
#include "BGObject.h"
#include "TextureManager.h"
#include "TextureDisplay.h"
#include "FPSCounter.h"
#include <iostream>

// [P3] stuff
#include "SceneManager.h"
#include "LoadingScreen.h"
#include "AssetLoader.h"

// 


/// <summary>
/// This demonstrates a running parallax background where after X seconds, a batch of assets will be streamed and loaded.
/// </summary>
const sf::Time BaseRunner::TIME_PER_FRAME = sf::seconds(1.f / 60.f);

BaseRunner::BaseRunner() :
	window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "HO: Entity Component", sf::Style::Close) {

	// background music
	if (this->backgroundMusic.openFromFile("Media/Audio/backgroundMusic.ogg")) {
		this->backgroundMusic.setVolume(50.0f);  
		this->backgroundMusic.setLoop(true);
		this->backgroundMusic.play();
		std::cout << "[BaseRunner] Background music started" << std::endl;
	}
	else {
		std::cout << "[BaseRunner] Failed to load background music" << std::endl;
	}

	//loading screen stuff [P3]
	// Start with loading scene
	SceneManager::getInstance()->setCurrentScene(SceneManager::LOADING_SCENE);

	// Create loading screen
	LoadingScreen* loadingScreen = new LoadingScreen();
	GameObjectManager::getInstance()->addObject(loadingScreen);

	//load initial textures
	TextureManager::getInstance()->loadFromAssetList();

	//asset loader
	AssetLoader* assetLoader = new AssetLoader();
	GameObjectManager::getInstance()->addObject(assetLoader);

	//load objects
	//BGObject* bgObject = new BGObject("BGObject");
	//GameObjectManager::getInstance()->addObject(bgObject);

	//TextureDisplay* display = new TextureDisplay();
	//GameObjectManager::getInstance()->addObject(display);

	FPSCounter* fpsCounter = new FPSCounter();
	GameObjectManager::getInstance()->addObject(fpsCounter);
}

void BaseRunner::run() {
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (this->window.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TIME_PER_FRAME)
		{
			timeSinceLastUpdate -= TIME_PER_FRAME;

			processEvents();
			update(TIME_PER_FRAME);
			//update(elapsedTime);
		}

		render();
	}
}

void BaseRunner::processEvents()
{
	sf::Event event;
	if (this->window.pollEvent(event)) {
		switch (event.type) {
		
		default: GameObjectManager::getInstance()->processInput(event); break;
		case sf::Event::Closed:
			this->window.close();
			break;

		}
	}
}

void BaseRunner::update(sf::Time elapsedTime) {
	GameObjectManager::getInstance()->update(elapsedTime);
}

void BaseRunner::render() {
	this->window.clear();

	SceneManager::SceneType currentScene = SceneManager::getInstance()->getCurrentScene();


	GameObjectManager::getInstance()->draw(&this->window);
	this->window.display();
}