#include "AssetLoader.h"
#include "LoadingScreen.h"
#include "TextureManager.h"
#include "GameObjectManager.h"
#include "SceneManager.h"
#include <iostream>

AssetLoader::AssetLoader() : AGameObject("AssetLoader")
{
    this->loadingStarted = false;
    this->loadingFinished = false;
    this->transitionTimer = 0.0f;
    this->loadingScreen = nullptr;
}

void AssetLoader::initialize()
{
    std::cout << "[AssetLoader] Initialized" << std::endl;

    // Find the loading screen object
    this->loadingScreen = (LoadingScreen*)GameObjectManager::getInstance()->findObjectByName("LoadingScreen");
}

void AssetLoader::processInput(sf::Event event)
{
}

void AssetLoader::update(sf::Time deltaTime)
{
    // Start loading on first update
    if (!this->loadingStarted) {
        std::cout << "[AssetLoader] Starting asset load..." << std::endl;
        TextureManager::getInstance()->loadGameAssetsAsync("Media/Streaming");
        this->loadingStarted = true;
    }

    // Update loading progress
    if (this->loadingStarted && !this->loadingFinished) {
        float progress = TextureManager::getInstance()->getLoadingProgress();

        if (this->loadingScreen != nullptr) {
            this->loadingScreen->updateProgress(progress);
        }

        // Check if loading is complete
        if (progress >= 1.0f) {
            std::cout << "[AssetLoader] Loading complete!" << std::endl;
            this->loadingFinished = true;
            SceneManager::getInstance()->setLoadingComplete(true);
        }
    }

    // Transition after delay
    if (this->loadingFinished) {
        this->transitionTimer += deltaTime.asSeconds();

        if (this->transitionTimer >= TRANSITION_DELAY) {
            std::cout << "[AssetLoader] Transitioning to game scene..." << std::endl;
            SceneManager::getInstance()->setCurrentScene(SceneManager::GAME_SCENE);

            // TODO: Load game scene objects (next step)
        }
    }
}


