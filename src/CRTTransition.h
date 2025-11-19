#pragma once
#include "AGameObject.h"

class CRTTransition : public AGameObject
{
public:
    CRTTransition();
    ~CRTTransition();

    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderWindow* targetWindow) override;

    bool isComplete() const;

private:
    sf::RectangleShape* screenOverlay;
    sf::RectangleShape* scanLine;

    enum TransitionState {
        FLICKER_OUT,      // flickering
        STATIC_NOISE,     // static fx
        FLICKER_IN,       // New screen flickers in
        COMPLETE
    };

    TransitionState state;
    float stateTimer;
    float flickerTimer;
    int flickerCount;

    float lineHeight;
    float lineSpeed;

    bool showScreen;
    float screenScale;

    // Timings
    const float FLICKER_OUT_DURATION = 0.8f;
    const float STATIC_DURATION = 0.3f;
    const float FLICKER_IN_DURATION = 0.6f;
    const float FLICKER_INTERVAL = 0.05f;
    const int MAX_FLICKERS = 8;
};