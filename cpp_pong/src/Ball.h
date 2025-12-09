#pragma once

struct AABB {
    float x, y, width, height;
};

class Ball {
public:
    float x, y;
    float width, height;
    float dx, dy;

    Ball(float x, float y, float width, float height);
    void reset(float screenWidth, float screenHeight);
    void update(float dt);
    bool collides(const AABB& box);
    AABB getBox() const;
};
