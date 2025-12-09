#pragma once
#include "Ball.h" // For AABB

class Paddle {
public:
    float x, y;
    float width, height;
    float dx, dy;
    float baseSpeed;
    
    // Sprint mechanics
    bool isSprinting;
    float sprintTimer;
    float sprintDuration;
    float cooldownTimer;
    float cooldownMax;
    float speedMultiplier;

    Paddle(float x, float y, float width, float height);
    void update(float dt, float screenWidth, float screenHeight);
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void stopY();
    void stopX();
    void attemptSprint();
    AABB getBox() const;
};
