#pragma once
#include "MiniGame.h"
#include <SFML/Graphics.hpp>

class PingPong : public MiniGame
{
public:
    PingPong();
    ~PingPong();

    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderWindow* targetWindow) override;
    void reset() override;
    const char* getName() const override { return "Pong"; }

private:
    // Game objects
    sf::RectangleShape leftPaddle;
    sf::RectangleShape rightPaddle;
    sf::CircleShape ball;

    // Physics
    sf::Vector2f ballVelocity;
    float ballSpeed;
    float paddleSpeed;

    // Window bounds
    float windowWidth;
    float windowHeight;

    // UI
    sf::Text* instructionText;
    sf::Text* scoreText;
    int leftScore;
    int rightScore;

    // Helper methods
    void updatePaddles(float dt);
    void updateBall(float dt);
    void setupUI();
    void updateScoreDisplay();

    // ping pong area
    sf::RectangleShape* playAreaBorder;
    float playAreaX;
    float playAreaY;
    float playAreaWidth;
    float playAreaHeight;

    // Constants
    const float PADDLE_WIDTH = 15.0f;
    const float PADDLE_HEIGHT = 100.0f;
    const float BALL_RADIUS = 10.0f;
    const float INITIAL_BALL_SPEED = 400.0f;
    const float PADDLE_SPEED = 500.0f;
    const float SPEED_INCREASE_FACTOR = 1.05f;
};