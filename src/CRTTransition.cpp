#include "CRTTransition.h"
#include "BaseRunner.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

CRTTransition::CRTTransition() : AGameObject("CRTTransition")
{
    this->state = FLICKER_OUT;
    this->stateTimer = 0.0f;
    this->flickerTimer = 0.0f;
    this->flickerCount = 0;
    this->showScreen = true;
    this->screenScale = 1.0f;
    this->lineHeight = 0.0f;
    this->lineSpeed = BaseRunner::WINDOW_HEIGHT * 3.0f;

    std::srand(std::time(nullptr));
}

CRTTransition::~CRTTransition()
{
    delete this->screenOverlay;
    delete this->scanLine;
}

void CRTTransition::initialize()
{
    std::cout << "[CRTTransition] Starting CRT transition effect..." << std::endl;

    // Full screen overlay for effects
    this->screenOverlay = new sf::RectangleShape(
        sf::Vector2f(BaseRunner::WINDOW_WIDTH, BaseRunner::WINDOW_HEIGHT)
    );
    this->screenOverlay->setPosition(0, 0);
    this->screenOverlay->setFillColor(sf::Color::Black);

    // Horizontal scan line
    this->scanLine = new sf::RectangleShape(
        sf::Vector2f(BaseRunner::WINDOW_WIDTH, 2.0f)
    );
    this->scanLine->setFillColor(sf::Color::White);
}

void CRTTransition::processInput(sf::Event event)
{
    // No input needed
}

void CRTTransition::update(sf::Time deltaTime)
{
    this->stateTimer += deltaTime.asSeconds();
    this->flickerTimer += deltaTime.asSeconds();

    switch (this->state) {
    case FLICKER_OUT:
    {
        // Flicker effect - rapid on/off
        if (this->flickerTimer >= FLICKER_INTERVAL) {
            this->flickerTimer = 0.0f;
            this->showScreen = !this->showScreen;
            this->flickerCount++;
        }

        // Horizontal squeeze effect (like CRT turning off)
        float progress = this->stateTimer / FLICKER_OUT_DURATION;
        this->screenScale = 1.0f - (progress * 0.95f); // Shrink to 5%

        if (this->stateTimer >= FLICKER_OUT_DURATION) {
            std::cout << "[CRTTransition] Flicker out complete" << std::endl;
            this->state = STATIC_NOISE;
            this->stateTimer = 0.0f;
            this->showScreen = true;
        }
        break;
    }

    case STATIC_NOISE:
    {
        // Random static effect
        this->flickerTimer += deltaTime.asSeconds();
        if (this->flickerTimer >= 0.03f) {
            this->flickerTimer = 0.0f;
            // Random brightness for static
            int brightness = 50 + (std::rand() % 100);
            this->screenOverlay->setFillColor(sf::Color(brightness, brightness, brightness));
        }

        if (this->stateTimer >= STATIC_DURATION) {
            std::cout << "[CRTTransition] Static complete" << std::endl;
            this->state = FLICKER_IN;
            this->stateTimer = 0.0f;
            this->flickerCount = 0;
            this->screenScale = 0.05f;
        }
        break;
    }

    case FLICKER_IN:
    {
        // Flicker as screen turns on
        if (this->flickerTimer >= FLICKER_INTERVAL) {
            this->flickerTimer = 0.0f;
            this->showScreen = !this->showScreen;
            this->flickerCount++;
        }

        // Expand from center (like CRT turning on)
        float progress = this->stateTimer / FLICKER_IN_DURATION;
        this->screenScale = 0.05f + (progress * 0.95f);

        // Scan line effect
        this->lineHeight += this->lineSpeed * deltaTime.asSeconds();
        if (this->lineHeight > BaseRunner::WINDOW_HEIGHT) {
            this->lineHeight = 0.0f;
        }

        if (this->stateTimer >= FLICKER_IN_DURATION) {
            std::cout << "[CRTTransition] Flicker in complete" << std::endl;
            this->state = COMPLETE;
            this->showScreen = true;
            this->screenScale = 1.0f;
        }
        break;
    }

    case COMPLETE:
        // Do nothing
        break;
    }
}

void CRTTransition::draw(sf::RenderWindow* targetWindow)
{
    if (this->state == COMPLETE)
        return;

    if (this->state == FLICKER_OUT || this->state == FLICKER_IN) {
        if (!this->showScreen || this->screenScale < 1.0f) {
            // Calculate screen bounds with scaling
            float centerY = BaseRunner::WINDOW_HEIGHT / 2.0f;
            float scaledHeight = BaseRunner::WINDOW_HEIGHT * this->screenScale;
            float topY = centerY - (scaledHeight / 2.0f);

            // Draw black bars (simulating horizontal squeeze)
            sf::RectangleShape topBar(sf::Vector2f(BaseRunner::WINDOW_WIDTH, topY));
            topBar.setPosition(0, 0);
            topBar.setFillColor(sf::Color::Black);
            targetWindow->draw(topBar);

            sf::RectangleShape bottomBar(sf::Vector2f(BaseRunner::WINDOW_WIDTH, topY));
            bottomBar.setPosition(0, centerY + (scaledHeight / 2.0f));
            bottomBar.setFillColor(sf::Color::Black);
            targetWindow->draw(bottomBar);

            // Thin white line in center (cathode ray)
            if (this->screenScale < 0.1f) {
                sf::RectangleShape centerLine(sf::Vector2f(BaseRunner::WINDOW_WIDTH, 3.0f));
                centerLine.setPosition(0, centerY);
                centerLine.setFillColor(sf::Color(255, 255, 255, 200));
                targetWindow->draw(centerLine);
            }
        }
    }
    else if (this->state == STATIC_NOISE) {
        // Draw static overlay
        targetWindow->draw(*this->screenOverlay);
    }

    // Draw scan line during flicker in
    if (this->state == FLICKER_IN && this->showScreen) {
        this->scanLine->setPosition(0, this->lineHeight);
        targetWindow->draw(*this->scanLine);
    }
}

bool CRTTransition::isComplete() const
{
    return this->state == COMPLETE;
}