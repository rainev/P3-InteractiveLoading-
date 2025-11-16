#include "PingPong.hpp"
#include <cstdlib> // for std::rand

PingPong::PingPong(float width, float height)
    : windowWidth(width), windowHeight(height)
{
    // paddles
    const float paddleWidth  = 15.f;
    const float paddleHeight = 100.f;

    leftPaddle.setSize({paddleWidth, paddleHeight});
    leftPaddle.setFillColor(sf::Color::White);
    leftPaddle.setPosition(50.f, windowHeight / 2.f - paddleHeight / 2.f);

    rightPaddle.setSize({paddleWidth, paddleHeight});
    rightPaddle.setFillColor(sf::Color::White);
    rightPaddle.setPosition(windowWidth - 50.f - paddleWidth,
                            windowHeight / 2.f - paddleHeight / 2.f);

    // ball
    const float ballRadius = 10.f;
    ball.setRadius(ballRadius);
    ball.setFillColor(sf::Color::White);

    reset();
}

void PingPong::reset()
{
    float ballRadius = ball.getRadius();
    ball.setPosition(windowWidth / 2.f - ballRadius,
                     windowHeight / 2.f - ballRadius);

    // random initial direction
    float dirX = (std::rand() % 2 == 0) ? -1.f : 1.f;
    float dirY = (std::rand() % 2 == 0) ? -1.f : 1.f;
    ballVelocity = {ballSpeed * dirX, 220.f * dirY};
}

void PingPong::update(float dt)
{
    updatePaddles(dt);
    updateBall(dt);
}

void PingPong::updatePaddles(float dt)
{
    const float paddleHeight = leftPaddle.getSize().y;

    // --- Player input: left paddle (W/S) ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        leftPaddle.move(0.f, -paddleSpeed * dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        leftPaddle.move(0.f, paddleSpeed * dt);
    }

    // Clamp left paddle
    if (leftPaddle.getPosition().y < 0.f)
        leftPaddle.setPosition(leftPaddle.getPosition().x, 0.f);
    if (leftPaddle.getPosition().y + paddleHeight > windowHeight)
        leftPaddle.setPosition(leftPaddle.getPosition().x,
                               windowHeight - paddleHeight);

    // --- Simple AI: right paddle follows ball ---
    float rightCenterY = rightPaddle.getPosition().y + paddleHeight / 2.f;
    float ballCenterY  = ball.getPosition().y + ball.getRadius();
    if (ballCenterY < rightCenterY - 10.f)
        rightPaddle.move(0.f, -paddleSpeed * dt * 0.9f);
    else if (ballCenterY > rightCenterY + 10.f)
        rightPaddle.move(0.f, paddleSpeed * dt * 0.9f);

    // Clamp right paddle
    if (rightPaddle.getPosition().y < 0.f)
        rightPaddle.setPosition(rightPaddle.getPosition().x, 0.f);
    if (rightPaddle.getPosition().y + paddleHeight > windowHeight)
        rightPaddle.setPosition(rightPaddle.getPosition().x,
                                windowHeight - paddleHeight);
}

void PingPong::updateBall(float dt)
{
    const float speedIncreaseFactor = 1.05f;
    float ballRadius = ball.getRadius();
    sf::Vector2f pos = ball.getPosition();
    pos += ballVelocity * dt;

    // Top / bottom
    if (pos.y <= 0.f) {
        pos.y = 0.f;
        ballVelocity.y = -ballVelocity.y;
    }
    if (pos.y + 2 * ballRadius >= windowHeight) {
        pos.y = windowHeight - 2 * ballRadius;
        ballVelocity.y = -ballVelocity.y;
    }

    // Paddle collisions
    if (ball.getGlobalBounds().intersects(leftPaddle.getGlobalBounds())) {
        pos.x = leftPaddle.getPosition().x +
                leftPaddle.getSize().x + 0.1f;
        ballVelocity.x = std::abs(ballVelocity.x);
        
        ballVelocity.x *= speedIncreaseFactor;
        ballVelocity.y *= speedIncreaseFactor;
    }

    if (ball.getGlobalBounds().intersects(rightPaddle.getGlobalBounds())) {
        pos.x = rightPaddle.getPosition().x -
                2 * ballRadius - 0.1f;
        ballVelocity.x = -std::abs(ballVelocity.x);

        ballVelocity.x *= speedIncreaseFactor;
        ballVelocity.y *= speedIncreaseFactor;
    }

    // Out of bounds: reset
    if (pos.x + 2 * ballRadius < 0.f || pos.x > windowWidth) {
        reset();
        return;
    }

    ball.setPosition(pos);
}

void PingPong::render(sf::RenderWindow& window)
{
    window.draw(leftPaddle);
    window.draw(rightPaddle);
    window.draw(ball);
}

void PingPong::updateSpeed(float newBallSpeed, float newPaddleSpeed)
{
    ballSpeed = newBallSpeed;
    paddleSpeed = newPaddleSpeed;

    float dirX = (ballVelocity.x >= 0.f) ? 1.f : -1.f;
    float dirY = (ballVelocity.y >= 0.f) ? 1.f : -1.f;
    ballVelocity = {ballSpeed * dirX, 220.f * dirY};
}