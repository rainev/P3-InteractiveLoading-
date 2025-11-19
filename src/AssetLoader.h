#pragma once
#include "AGameObject.h"

class LoadingScreen;
class CRTTransition;


class AssetLoader : public AGameObject
{
public:
    AssetLoader();

    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;

private:
    LoadingScreen* loadingScreen;
    CRTTransition* crtTransition;

    bool loadingStarted;
    bool loadingFinished;
    bool transitionStarted;
    float transitionTimer;

    const float TRANSITION_DELAY = 1.0f;
};