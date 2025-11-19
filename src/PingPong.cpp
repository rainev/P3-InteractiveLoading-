#include "PingPong.h"
#include "BaseRunner.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

PingPong::PingPong()
{
    this->windowWidth = static_cast<float>(BaseRunner::WINDOW_WIDTH);
    this->windowHeight = static_cast<float>(BaseRunner::WINDOW_HEIGHT);
    this->ballSpeed = INITIAL_BALL_SPEED;
    this->paddleSpeed = PADDLE_SPEED;
    this->leftScore = 0;
    this->rightScore = 0;
    this->instructionText = nullptr;
    this->scoreText = nullptr;

    // Seed random
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    this->playAreaWidth = 1200.0f;   
    this->playAreaHeight = 800.0f;
    this->playAreaX = (BaseRunner::WINDOW_WIDTH - this->playAreaWidth) / 2.0f;
    this->playAreaY = (BaseRunner::WINDOW_HEIGHT - this->playAreaHeight) / 2.0f;

    this->playAreaBorder = nullptr;
}

PingPong::~PingPong()
{
    if (this->instructionText != nullptr) {
        delete this->instructionText->getFont();
        delete this->instructionText;
    }
    if (this->scoreText != nullptr) {
        delete this->scoreText;
    }

    delete this->playAreaBorder;
}

void PingPong::initialize()
{
    std::cout << "[PongGame] Initializing..." << std::endl;

    // Create play area border
    this->playAreaBorder = new sf::RectangleShape(
        sf::Vector2f(this->playAreaWidth, this->playAreaHeight)
    );
    this->playAreaBorder->setPosition(this->playAreaX, this->playAreaY);
    this->playAreaBorder->setFillColor(sf::Color(10, 10, 10)); // Dark background
    this->playAreaBorder->setOutlineThickness(5.0f);
    this->playAreaBorder->setOutlineColor(sf::Color::White);

    // Setup left paddle (inside play area)
    this->leftPaddle.setSize(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
    this->leftPaddle.setFillColor(sf::Color::White);
    this->leftPaddle.setPosition(
        this->playAreaX + 50.0f,
        this->playAreaY + this->playAreaHeight / 2.0f - PADDLE_HEIGHT / 2.0f
    );

    // Setup right paddle (inside play area)
    this->rightPaddle.setSize(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
    this->rightPaddle.setFillColor(sf::Color::White);
    this->rightPaddle.setPosition(
        this->playAreaX + this->playAreaWidth - 50.0f - PADDLE_WIDTH,
        this->playAreaY + this->playAreaHeight / 2.0f - PADDLE_HEIGHT / 2.0f
    );

    // Setup ball
    this->ball.setRadius(BALL_RADIUS);
    this->ball.setFillColor(sf::Color::White);

    // Setup UI
    this->setupUI();

    // Reset to starting position
    this->reset();

    std::cout << "[PongGame] Initialization complete!" << std::endl;
}

void PingPong::setupUI()
{
    sf::Font* font = new sf::Font();
    font->loadFromFile("Media/Sansation.ttf");

    // Instruction text
    this->instructionText = new sf::Text();
    this->instructionText->setFont(*font);
    this->instructionText->setCharacterSize(24);
    this->instructionText->setFillColor(sf::Color::White);
    this->instructionText->setString("W/S - Move Left Paddle");

    sf::FloatRect bounds = this->instructionText->getLocalBounds();
    this->instructionText->setPosition(
        (this->windowWidth - bounds.width) / 2.0f,
        this->playAreaY - 80.0f
    );

    // Score text
    this->scoreText = new sf::Text();
    this->scoreText->setFont(*font);
    this->scoreText->setCharacterSize(48);
    this->scoreText->setFillColor(sf::Color::White);
    this->updateScoreDisplay();
}

void PingPong::updateScoreDisplay()
{
    this->scoreText->setString(std::to_string(this->leftScore) + "  :  " + std::to_string(this->rightScore));

    sf::FloatRect bounds = this->scoreText->getLocalBounds();
    this->scoreText->setPosition(
        (this->windowWidth - bounds.width) / 2.0f,
        this->playAreaY - 40.0f
    );
}

void PingPong::reset()
{
    // Center the ball
    this->ball.setPosition(
        this->playAreaWidth / 2.0f - BALL_RADIUS,
        this->playAreaHeight / 2.0f - BALL_RADIUS
    );

    // Random initial direction
    float dirX = (std::rand() % 2 == 0) ? -1.0f : 1.0f;
    float dirY = (std::rand() % 2 == 0) ? -1.0f : 1.0f;

    this->ballVelocity = sf::Vector2f(this->ballSpeed * dirX, 220.0f * dirY);

    std::cout << "[PongGame] Ball reset with direction: " << dirX << ", " << dirY << std::endl;
}

void PingPong::processInput(sf::Event event)
{
    // Input is handled in updatePaddles using real-time keyboard state
}

void PingPong::update(sf::Time deltaTime)
{
    float dt = deltaTime.asSeconds();

    this->updatePaddles(dt);
    this->updateBall(dt);
}

void PingPong::updatePaddles(float dt)
{
    // --- Player input: left paddle (W/S) ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        this->leftPaddle.move(0.0f, -this->paddleSpeed * dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        this->leftPaddle.move(0.0f, this->paddleSpeed * dt);
    }

    // Clamp left paddle to PLAY AREA bounds
    sf::Vector2f leftPos = this->leftPaddle.getPosition();
    if (leftPos.y < this->playAreaY) {
        this->leftPaddle.setPosition(leftPos.x, this->playAreaY);
    }
    if (leftPos.y + PADDLE_HEIGHT > this->playAreaY + this->playAreaHeight) {
        this->leftPaddle.setPosition(leftPos.x, this->playAreaY + this->playAreaHeight - PADDLE_HEIGHT);
    }

    // --- Simple AI: right paddle follows ball ---
    float rightCenterY = this->rightPaddle.getPosition().y + PADDLE_HEIGHT / 2.0f;
    float ballCenterY = this->ball.getPosition().y + BALL_RADIUS;

    if (ballCenterY < rightCenterY - 10.0f) {
        this->rightPaddle.move(0.0f, -this->paddleSpeed * dt * 0.9f);
    }
    else if (ballCenterY > rightCenterY + 10.0f) {
        this->rightPaddle.move(0.0f, this->paddleSpeed * dt * 0.9f);
    }

    // Clamp right paddle to PLAY AREA bounds
    sf::Vector2f rightPos = this->rightPaddle.getPosition();
    if (rightPos.y < this->playAreaY) {
        this->rightPaddle.setPosition(rightPos.x, this->playAreaY);
    }
    if (rightPos.y + PADDLE_HEIGHT > this->playAreaY + this->playAreaHeight) {
        this->rightPaddle.setPosition(rightPos.x, this->playAreaY + this->playAreaHeight - PADDLE_HEIGHT);
    }
}

void PingPong::updateBall(float dt)
{
    sf::Vector2f pos = this->ball.getPosition();
    pos += this->ballVelocity * dt;

    // --- Top / Bottom PLAY AREA collision ---
    if (pos.y <= this->playAreaY) {
        pos.y = this->playAreaY;
        this->ballVelocity.y = -this->ballVelocity.y;
    }
    if (pos.y + 2 * BALL_RADIUS >= this->playAreaY + this->playAreaHeight) {
        pos.y = this->playAreaY + this->playAreaHeight - 2 * BALL_RADIUS;
        this->ballVelocity.y = -this->ballVelocity.y;
    }

    // --- Left paddle collision ---
    if (this->ball.getGlobalBounds().intersects(this->leftPaddle.getGlobalBounds())) {
        pos.x = this->leftPaddle.getPosition().x + PADDLE_WIDTH + 0.1f;
        this->ballVelocity.x = std::abs(this->ballVelocity.x);

        this->ballVelocity.x *= SPEED_INCREASE_FACTOR;
        this->ballVelocity.y *= SPEED_INCREASE_FACTOR;
    }

    // --- Right paddle collision ---
    if (this->ball.getGlobalBounds().intersects(this->rightPaddle.getGlobalBounds())) {
        pos.x = this->rightPaddle.getPosition().x - 2 * BALL_RADIUS - 0.1f;
        this->ballVelocity.x = -std::abs(this->ballVelocity.x);

        this->ballVelocity.x *= SPEED_INCREASE_FACTOR;
        this->ballVelocity.y *= SPEED_INCREASE_FACTOR;
    }

    // --- Out of PLAY AREA bounds: score and reset ---
    if (pos.x + 2 * BALL_RADIUS < this->playAreaX) {
        this->rightScore++;
        this->updateScoreDisplay();
        std::cout << "[PongGame] Right scores! " << this->leftScore << " - " << this->rightScore << std::endl;
        this->reset();
        return;
    }

    if (pos.x > this->playAreaX + this->playAreaWidth) {
        this->leftScore++;
        this->updateScoreDisplay();
        std::cout << "[PongGame] Left scores! " << this->leftScore << " - " << this->rightScore << std::endl;
        this->reset();
        return;
    }

    this->ball.setPosition(pos);
}

void PingPong::draw(sf::RenderWindow* targetWindow)
{
    // Draw game objects
    targetWindow->draw(this->leftPaddle);
    targetWindow->draw(this->rightPaddle);
    targetWindow->draw(this->ball);

    // Draw UI
    targetWindow->draw(*this->instructionText);
    targetWindow->draw(*this->scoreText);
}