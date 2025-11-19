#include "SceneManager.h"

SceneManager* SceneManager::sharedInstance = nullptr;

SceneManager::SceneManager() : currentScene(LOADING_SCENE), loadingComplete(false)
{
}

SceneManager* SceneManager::getInstance()
{
    if (sharedInstance == nullptr) {
        sharedInstance = new SceneManager();
    }
    return sharedInstance;
}

void SceneManager::setCurrentScene(SceneType scene)
{
    this->currentScene = scene;
}

SceneManager::SceneType SceneManager::getCurrentScene() const
{
    return this->currentScene;
}

bool SceneManager::isLoadingComplete() const
{
    return this->loadingComplete;
}

void SceneManager::setLoadingComplete(bool complete)
{
    this->loadingComplete = complete;
}