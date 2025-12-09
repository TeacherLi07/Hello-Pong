#pragma once
#include "Ball.h"
#include "Paddle.h"
#include "Platform.h"
#include <vector>

class Game {
public:
    Game();
    void run();

private:
    void processInput();
    void update(float dt);
    void render();

    bool isRunning;
    int termWidth, termHeight; // Actual terminal size
    int vWidth, vHeight;       // Virtual resolution (2x height)
    
    Ball ball;
    Paddle player1;
    Paddle player2;
    int score1, score2;
    int servingPlayer;
    bool isServing;

    // Renderer state
    struct Pixel {
        Platform::Color color;
        bool filled;
    };
    std::vector<Pixel> frameBuffer;
    
    void clearBuffer();
    void drawRect(float x, float y, float w, float h, Platform::Color color);
};
