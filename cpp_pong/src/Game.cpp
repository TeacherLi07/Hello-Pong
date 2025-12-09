#include "Game.h"
#include "Platform.h"
#include <string>
#include <vector>
#include <cmath>
#include <ctime>

Game::Game()
    : isRunning(true), termWidth(80), termHeight(24),
      vWidth(80), vHeight(48), // Double vertical resolution
      ball(vWidth / 2 - 1, vHeight / 2 - 1, 2, 2), // Ball 2x2 virtual pixels
      player1(2, vHeight / 2 - 4, 2, 8),           // Paddle 2x8 virtual pixels
      player2(vWidth - 4, vHeight / 2 - 4, 2, 8),
      score1(0), score2(0), servingPlayer(1), isServing(true) {
    
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    Platform::init();
    Platform::hideCursor();
    
    // Try to get actual terminal size
    // Platform::getTerminalSize(termWidth, termHeight); // Optional: dynamic size
    // For now, stick to fixed size but larger if possible, or just map to 80x24
    
    vWidth = termWidth;
    vHeight = termHeight * 2;
    
    // Re-init objects with new resolution
    ball.width = 2; ball.height = 2;
    player1.width = 2; player1.height = 8;
    player2.width = 2; player2.height = 8;
    
    ball.reset(vWidth, vHeight);
    player1.x = 2; player1.y = vHeight / 2 - 4;
    player2.x = vWidth - 4; player2.y = vHeight / 2 - 4;

    frameBuffer.resize(vWidth * vHeight);
}

void Game::run() {
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (isRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        processInput();
        update(dt);
        render();

        // Platform::sleep(16); // Remove sleep for max FPS, or keep small sleep
        // To improve smoothness, we can sleep less, e.g. 8ms (~120fps)
        Platform::sleep(8); 
    }

    Platform::cleanup();
}

void Game::processInput() {
#ifdef PLATFORM_WINDOWS
    player1.stopX(); player1.stopY();
    player2.stopX(); player2.stopY();

    // Player 1
    if (Platform::isKeyDown('W')) player1.moveUp();
    if (Platform::isKeyDown('S')) player1.moveDown();
    if (Platform::isKeyDown('A')) player1.moveLeft();
    if (Platform::isKeyDown('D')) player1.moveRight();
    if (Platform::isKeyDown(VK_LSHIFT)) player1.attemptSprint();

    // Player 2
    if (Platform::isKeyDown('I')) player2.moveUp();
    if (Platform::isKeyDown('K')) player2.moveDown();
    if (Platform::isKeyDown('J')) player2.moveLeft();
    if (Platform::isKeyDown('L')) player2.moveRight();
    if (Platform::isKeyDown(VK_RSHIFT)) player2.attemptSprint(); // Right shift for P2?

    if (Platform::isKeyDown(VK_SPACE)) {
         if (isServing) {
            isServing = false;
            ball.dx = (servingPlayer == 1) ? 30.0f : -30.0f;
            ball.dy = (rand() % 20 - 10) * 1.0f;
        }
    }
    if (Platform::isKeyDown('Q')) isRunning = false;

#else
    // Linux fallback (simplified)
    if (Platform::kbhit()) {
        char c = Platform::getch();
        switch (c) {
            case 'q': isRunning = false; break;
            case 'w': player1.moveUp(); break;
            case 's': player1.moveDown(); break;
            case 'i': player2.moveUp(); break;
            case 'k': player2.moveDown(); break;
            case ' ': 
                if (isServing) {
                    isServing = false;
                    ball.dx = (servingPlayer == 1) ? 30.0f : -30.0f;
                    ball.dy = (rand() % 20 - 10) * 1.0f;
                }
                break;
        }
    }
#endif
}

void Game::update(float dt) {
    if (isServing) {
        ball.y = vHeight / 2 - 1;
        ball.x = vWidth / 2 - 1;
        return;
    }

    player1.update(dt, vWidth, vHeight);
    player2.update(dt, vWidth, vHeight);
    ball.update(dt);

    // Collision with paddles
    if (ball.collides(player1.getBox())) {
        ball.dx = -ball.dx * 1.05f;
        ball.x = player1.x + player1.width; // Push out
        
        // Add paddle velocity to ball for spin effect
        ball.dy += player1.dy * 5.0f;
    }
    if (ball.collides(player2.getBox())) {
        ball.dx = -ball.dx * 1.05f;
        ball.x = player2.x - ball.width; // Push out
        
        ball.dy += player2.dy * 5.0f;
    }

    // Collision with walls
    if (ball.y <= 0) {
        ball.y = 0;
        ball.dy = -ball.dy;
    }
    if (ball.y >= vHeight - ball.height) {
        ball.y = vHeight - ball.height;
        ball.dy = -ball.dy;
    }

    // Scoring
    if (ball.x < 0) {
        score2++;
        servingPlayer = 1;
        isServing = true;
        ball.reset(vWidth, vHeight);
    }
    if (ball.x > vWidth) {
        score1++;
        servingPlayer = 2;
        isServing = true;
        ball.reset(vWidth, vHeight);
    }
}

void Game::clearBuffer() {
    std::fill(frameBuffer.begin(), frameBuffer.end(), Pixel{Platform::BLACK, false});
}

void Game::drawRect(float x, float y, float w, float h, Platform::Color color) {
    int ix = (int)x;
    int iy = (int)y;
    int iw = (int)w;
    int ih = (int)h;

    for (int j = iy; j < iy + ih; ++j) {
        for (int i = ix; i < ix + iw; ++i) {
            if (i >= 0 && i < vWidth && j >= 0 && j < vHeight) {
                frameBuffer[j * vWidth + i] = {color, true};
            }
        }
    }
}

void Game::render() {
    clearBuffer();

    // Draw entities to virtual buffer
    drawRect(player1.x, player1.y, player1.width, player1.height, Platform::RED);
    drawRect(player2.x, player2.y, player2.width, player2.height, Platform::BLUE);
    drawRect(ball.x, ball.y, ball.width, ball.height, Platform::WHITE);

    // Render to string
    std::string buffer;
    buffer.reserve(termWidth * termHeight * 20);

    Platform::setCursorPosition(0, 0);
    buffer += Platform::getFgColorCode(Platform::CYAN) + "╭";
    for (int i = 0; i < termWidth; ++i) buffer += "─";
    buffer += "╮" + Platform::getFgColorCode(Platform::RESET) + "\n";

    for (int y = 0; y < termHeight; ++y) {
        buffer += Platform::getFgColorCode(Platform::CYAN) + "│" + Platform::getFgColorCode(Platform::RESET);
        
        for (int x = 0; x < termWidth; ++x) {
            // Get two vertical pixels
            int vY1 = y * 2;
            int vY2 = y * 2 + 1;
            
            Pixel p1 = frameBuffer[vY1 * vWidth + x];
            Pixel p2 = frameBuffer[vY2 * vWidth + x];

            // Logic for half-block rendering
            // ▀ (Upper Half Block)
            // FG color = p1.color
            // BG color = p2.color
            
            if (!p1.filled && !p2.filled) {
                if (x == termWidth / 2) buffer += Platform::getFgColorCode(Platform::CYAN) + "│";
                else buffer += " ";
            } else {
                if (p1.filled && p2.filled) {
                    if (p1.color == p2.color) {
                        buffer += Platform::getFgColorCode(p1.color) + "█";
                    } else {
                        buffer += Platform::getFgColorCode(p1.color) + Platform::getBgColorCode(p2.color) + "▀" + Platform::getBgColorCode(Platform::RESET);
                    }
                } else if (p1.filled) {
                    buffer += Platform::getFgColorCode(p1.color) + "▀";
                } else if (p2.filled) {
                    buffer += Platform::getFgColorCode(p2.color) + "▄";
                }
            }
            buffer += Platform::getFgColorCode(Platform::RESET); // Reset after each char to be safe/simple
        }
        buffer += Platform::getFgColorCode(Platform::CYAN) + "│" + Platform::getFgColorCode(Platform::RESET) + "\n";
    }

    buffer += Platform::getFgColorCode(Platform::CYAN) + "╰";
    for (int i = 0; i < termWidth; ++i) buffer += "─";
    buffer += "╯" + Platform::getFgColorCode(Platform::RESET) + "\n";

    // UI
    buffer += Platform::getFgColorCode(Platform::RED) + "P1: " + std::to_string(score1);
    if (player1.isSprinting) buffer += " (DASH!)";
    buffer += Platform::getFgColorCode(Platform::RESET) + "  ";
    
    buffer += Platform::getFgColorCode(Platform::BLUE) + "P2: " + std::to_string(score2);
    if (player2.isSprinting) buffer += " (DASH!)";
    buffer += Platform::getFgColorCode(Platform::RESET) + "\n";

    std::cout << buffer << std::flush;
}
