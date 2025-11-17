#pragma once
#include "AGameObject.h"

class LoadingScreen;

class AssetLoader : public AGameObject
{
public:
    AssetLoader();

    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;

private:
    LoadingScreen* loadingScreen;
    bool loadingStarted;
    bool loadingFinished;
    float transitionTimer;

    const float TRANSITION_DELAY = 1.0f;
};