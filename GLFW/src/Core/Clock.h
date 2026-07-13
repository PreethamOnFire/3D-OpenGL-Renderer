#pragma once

class Clock {
public:
    void tick();
    float getTime() const { return currentTime; }
    float getDeltaTime() const { return deltaTime; }

private:
    float currentTime = 0.0f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
};
