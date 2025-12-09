#include "Paddle.h"
#include <algorithm>

Paddle::Paddle(float x, float y, float width, float height)
    : x(x), y(y), width(width), height(height), dx(0), dy(0), baseSpeed(30.0f),
      isSprinting(false), sprintTimer(0), sprintDuration(0.3f),
      cooldownTimer(0), cooldownMax(3.0f), speedMultiplier(1.0f) {
}

void Paddle::attemptSprint() {
    if (cooldownTimer <= 0 && !isSprinting) {
        isSprinting = true;
        sprintTimer = sprintDuration;
        cooldownTimer = cooldownMax;
        speedMultiplier = 3.0f; // 5x might be too fast for CLI
    }
}

void Paddle::update(float dt, float screenWidth, float screenHeight) {
    if (isSprinting) {
        sprintTimer -= dt;
        if (sprintTimer <= 0) {
            isSprinting = false;
            speedMultiplier = 1.0f;
        }
    }

    if (cooldownTimer > 0) {
        cooldownTimer -= dt;
    }

    float currentSpeed = baseSpeed * speedMultiplier;

    // Apply movement
    x += dx * currentSpeed * dt;
    y += dy * currentSpeed * dt;

    // Clamp to screen
    // Assuming screenWidth/Height are the boundaries
    // We might want to limit horizontal movement to a zone (e.g. 1/3 of screen)
    // For now, just clamp to screen
    x = std::max(0.0f, std::min(screenWidth - width, x));
    y = std::max(0.0f, std::min(screenHeight - height, y));
}

void Paddle::moveUp() {
    dy = -1.0f;
}

void Paddle::moveDown() {
    dy = 1.0f;
}

void Paddle::moveLeft() {
    dx = -1.0f;
}

void Paddle::moveRight() {
    dx = 1.0f;
}

void Paddle::stopY() {
    dy = 0;
}

void Paddle::stopX() {
    dx = 0;
}

AABB Paddle::getBox() const {
    return { x, y, width, height };
}
