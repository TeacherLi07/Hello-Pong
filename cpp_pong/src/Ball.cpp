#include "Ball.h"
#include <cstdlib>
#include <cmath>

Ball::Ball(float x, float y, float width, float height)
    : x(x), y(y), width(width), height(height), dx(0), dy(0) {
    reset(80, 24); // Default reset, will be overridden by game logic usually
}

void Ball::reset(float screenWidth, float screenHeight) {
    x = screenWidth / 2 - width / 2;
    y = screenHeight / 2 - height / 2;

    // Random direction
    dx = (rand() % 2 == 0) ? -20.0f : 20.0f; // Slower speed for CLI
    dy = (float)(rand() % 10 - 5);
}

void Ball::update(float dt) {
    x += dx * dt;
    y += dy * dt;
}

bool Ball::collides(const AABB& box) {
    if (x > box.x + box.width || x + width < box.x) return false;
    if (y > box.y + box.height || y + height < box.y) return false;
    return true;
}

AABB Ball::getBox() const {
    return { x, y, width, height };
}
