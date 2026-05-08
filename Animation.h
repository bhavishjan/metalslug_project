#pragma once
#include "Level.h"
#include <SFML/Graphics.hpp>

class Animation {
private:
    static const int MAX_FRAMES = 32;

    Texture texture;
    Texture legsTexture;
    IntRect frames[MAX_FRAMES];
    IntRect legsFrames[MAX_FRAMES];
    int frameCount;
    int legsFrameCount;
    int currentFrame;
    int legsCurrentFrame;
    float timer;
    float legsTimer;
    float frameTime;
    float legsFrameTime;
    int legsOffsetY;
    int headOffsetY;
    int headOffsetX;

public:
    Animation()
        : frameCount(0), legsFrameCount(0), currentFrame(0), legsCurrentFrame(0),
          timer(0.0f), legsTimer(0.0f), frameTime(0.08f), legsFrameTime(0.08f),
          legsOffsetY(0), headOffsetY(0), headOffsetX(0) {}

    void load(const char* path, const int* xs, const int* ys,
              const int* widths, const int* heights, int count, float seconds) {
        if (!texture.loadFromFile(path)) {
            return;
        }

        frameTime = seconds;

        if (count > MAX_FRAMES) {
            frameCount = MAX_FRAMES;
        }
        else {
            frameCount = count;
        }

        for (int i = 0; i < frameCount; i++)
            frames[i] = IntRect(xs[i], ys[i], widths[i], heights[i]);
    }

    void loadLegs(const char* path, const int* xs, const int* ys,
                   const int* widths, const int* heights, int count, float seconds, int offsetY) {
        if (!legsTexture.loadFromFile(path)) {
            return;
        }

        legsFrameTime = seconds;

        if (count > MAX_FRAMES) {
            legsFrameCount = MAX_FRAMES;
        }
        else {
            legsFrameCount = count;
        }

        legsOffsetY = offsetY;

        for (int i = 0; i < legsFrameCount; i++)
            legsFrames[i] = IntRect(xs[i], ys[i], widths[i], heights[i]);
    }

    void setHeadOffset(int offsetY) { 
        headOffsetY = offsetY; 
    }
    void setHeadOffsetX(int offsetX) { 
        headOffsetX = offsetX; 
    }

    void update(float dt) {
        if (frameCount <= 0) {
            return;
        }

        timer += dt;

        while (timer >= frameTime) {
            timer -= frameTime;
            currentFrame = (currentFrame + 1) % frameCount;
        }

        if (legsFrameCount > 0) {
            legsTimer += dt;
            while (legsTimer >= legsFrameTime) {
                legsTimer -= legsFrameTime;
                legsCurrentFrame = (legsCurrentFrame + 1) % legsFrameCount;
            }
        }
    }

    void reset() {
        currentFrame = 0;
        legsCurrentFrame = 0;
        timer = 0.0f;
        legsTimer = 0.0f;
    }

    const Texture& getTexture() const { 
        return texture; 
    }

    const Texture& getLegsTexture() const {
        return legsTexture;
    }

    IntRect currentRect() const {
        if (frameCount > 0) {
            return frames[currentFrame];
        }
        else {
            return IntRect(0, 0, 0, 0);
        }
    }

    IntRect currentLegsRect() const {
        if (legsFrameCount > 0) {
            return legsFrames[legsCurrentFrame];
        }
        else {
            return IntRect(0, 0, 0, 0);
        }
    }

    bool hasLegs() const { return legsFrameCount > 0; }
    int getLegsOffsetY() const { return legsOffsetY; }
    int getHeadOffsetY() const { return headOffsetY; }
    int getHeadOffsetX() const { return headOffsetX; }
};
