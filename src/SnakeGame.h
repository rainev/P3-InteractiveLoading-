#pragma once
#include "MiniGame.h"
#include <vector>
#include <deque>

class SnakeGame : public MiniGame
{
public:
    SnakeGame();
    ~SnakeGame();

    void initialize() override;
    void processInput(sf::Event event) override;
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderWindow* targetWindow) override;
    void reset() override;
    const char* getName() const override { return "Snake"; }

private:
    // Game objects
    struct Position {
        int x, y;
        bool operator==(const Position& other) const {
            return x == other.x && y == other.y;
        }
    };

    std::deque<Position> snake;
    Position food;
    Position direction;
    Position nextDirection;

    // Game state
    enum GameState {
        READY,
        PLAYING,
        GAME_OVER
    };
    GameState gameState;

    int score;
    float moveTimer;
    const float MOVE_DELAY = 0.08f;

    // Grid settings
    const int GRID_SIZE = 20;
    const int CELL_SIZE = 30;
    const int GRID_OFFSET_X = 730;
    const int GRID_OFFSET_Y = 240;

    // Rendering
    sf::RectangleShape* cellShape;
    sf::RectangleShape* foodShape;
    sf::RectangleShape* gridBackground;
    sf::Text* scoreText;
    sf::Text* instructionText;
    sf::Text* gameOverText;

    // Helper methods
    void spawnFood();
    bool checkCollision();
    bool isFoodOnSnake(const Position& pos);
    void handleGameOver();
    void setupText();
};