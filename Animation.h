#pragma once
#include "Header.h"
#include <SFML/Graphics.hpp>

class Animation {
private:
    static const int MAX_FRAMES = 32;

    Texture texture;
    IntRect frames[MAX_FRAMES];
    int frameCount;
    int currentFrame;
    float timer;
    float frameTime;

public:
    Animation()
        : frameCount(0), currentFrame(0), timer(0.0f), frameTime(0.08f) {}

    void load(const char* path, const int* xs, const int* ys,
              const int* widths, const int* heights, int count, float seconds) {
        texture.loadFromFile(path);

        frameTime = seconds;

        if (count > MAX_FRAMES)
            frameCount = MAX_FRAMES;
        else
            frameCount = count;

        for (int i = 0; i < frameCount; i++)
            frames[i] = IntRect(xs[i], ys[i], widths[i], heights[i]);
    }

    void update(float dt) {
        if (frameCount <= 0) return;

        timer += dt;
        while (timer >= frameTime) {
            timer -= frameTime;
            currentFrame = (currentFrame + 1) % frameCount;
        }
    }

    void reset() {
        currentFrame = 0;
        timer = 0.0f;
    }

    const Texture& getTexture() const { return texture; }
    IntRect currentRect() const { return frameCount > 0 ? frames[currentFrame] : IntRect(); }
};
