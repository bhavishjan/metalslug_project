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

    Texture legsTexture;
    IntRect legsFrames[MAX_FRAMES];
    int legsFrameCount;
    int legsCurrentFrame;
    float legsTimer;
    float legsFrameTime;
    int legsOffsetY;
    int torsoOffsetY;
    int torsoOffsetX;

public:
    Animation()
        : frameCount(0), currentFrame(0), timer(0.0f), frameTime(0.08f),
          legsFrameCount(0), legsCurrentFrame(0), legsTimer(0.0f), legsFrameTime(0.08f),
          legsOffsetY(0), torsoOffsetY(0), torsoOffsetX(0) {}

    void load(const char* path, int x, int y, int frameW, int frameH,
              int count, int stride, float seconds) {
        texture.loadFromFile(path);

        frameTime = seconds;

        if (count > MAX_FRAMES)
            frameCount = MAX_FRAMES;
        else
            frameCount = count;

        for (int i = 0; i < frameCount; i++)
            frames[i] = IntRect(x + i * stride, y, frameW, frameH);
    }

    void loadCustom(const char* path, const int* xs, const int* widths,
                    int y, int frameH, int count, float seconds) {
        texture.loadFromFile(path);

        frameTime = seconds;

        frameCount = count > MAX_FRAMES ? MAX_FRAMES : count;

        for (int i = 0; i < frameCount; i++)
            frames[i] = IntRect(xs[i], y, widths[i], frameH);
    }

    void loadLegs(const char* path, int x, int y, int frameW, int frameH,
                  int count, int stride, float seconds, int offsetY) {
        legsTexture.loadFromFile(path);

        legsFrameTime = seconds;

        if (count > MAX_FRAMES)
            frameCount = MAX_FRAMES;
        else
            frameCount = count;

        legsOffsetY = offsetY;

        for (int i = 0; i < legsFrameCount; i++)
            legsFrames[i] = IntRect(x + i * stride, y, frameW, frameH);
    }

    void loadLegsCustom(const char* path, const int* xs, const int* widths,
                        int y, int frameH, int count, float seconds, int offsetY) {
        legsTexture.loadFromFile(path);

        legsFrameTime = seconds;

        legsFrameCount = count > MAX_FRAMES ? MAX_FRAMES : count;

        legsOffsetY = offsetY;

        for (int i = 0; i < legsFrameCount; i++)
            legsFrames[i] = IntRect(xs[i], y, widths[i], frameH);
    }

    void setTorsoOffset(int offsetY) { 
        torsoOffsetY = offsetY; }
    void setTorsoOffsetX(int offsetX) { 
        torsoOffsetX = offsetX; }


    void update(float dt) {
        if (frameCount <= 0) return;

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
        currentFrame = 0;     timer = 0.0f;
        legsCurrentFrame = 0; legsTimer = 0.0f;
    }

    bool hasFrames() const { return frameCount > 0; }
    bool hasLegs()   const { return legsFrameCount > 0; }

    const Texture& getTexture()     const { return texture; }
    const Texture& getLegsTexture() const { return legsTexture; }

    IntRect currentRect()     const { return frameCount > 0 ? frames[currentFrame] : IntRect(); }
    IntRect currentLegsRect() const { return legsFrameCount > 0 ? legsFrames[legsCurrentFrame] : IntRect(); }

    int getLegsOffsetY()  const { return legsOffsetY;  }
    int getTorsoOffsetY() const { return torsoOffsetY; }
    int getTorsoOffsetX() const { return torsoOffsetX; }
};
