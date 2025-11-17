#pragma once
#include "AGameObject.h"

class LoadingScreen : public AGameObject
{
public:
    LoadingScreen();
    ~LoadingScreen();

    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderWindow* targetWindow) override;

    void updateProgress(float progress); 

private:
    sf::Text* loadingText;
    sf::RectangleShape* progressBarBackground;
    sf::RectangleShape* progressBarFill;

    float currentProgress;

    const float BAR_WIDTH = 600.0f;
    const float BAR_HEIGHT = 40.0f;
};