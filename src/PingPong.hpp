#pragma once
#include <SFML/Graphics.hpp>

class PingPong {
    public:
        PingPong(float windowWidth, float windowHeight);
        void update(float dt);
        void render(sf::RenderWindow& window);
        void reset();
        void updateSpeed(float newBallSpeed, float newPaddleSpeed);
    private:
        float windowWidth;
        float windowHeight;

        sf::RectangleShape leftPaddle;
        sf::RectangleShape rightPaddle;
        float paddleSpeed = 400.f;

        sf::CircleShape ball;
        sf::Vector2f ballVelocity;
        float ballSpeed = 300.f;
        
        void updatePaddles(float dt);
        void updateBall(float dt);
};