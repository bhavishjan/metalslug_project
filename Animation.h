#pragma once
#include "Header.h"
#include <SFML/Graphics.hpp>

// One sprite-sheet animation. Holds a sequence of frame rectangles + a texture,
// and steps through them on a fixed timer. Optionally a second "legs" layer can
// be loaded so a character can be drawn as torso (top) + legs (bottom).
class Animation {
private:
    static const int MAX_FRAMES = 32;

    // ----- Main (torso / full-body) layer -----
    Texture texture;                 // Sprite sheet for this layer
    IntRect frames[MAX_FRAMES];      // Source rectangle for each frame
    int frameCount;                  // How many frames are actually used
    int currentFrame;                // Index of the frame to draw right now
    float timer;                     // Seconds accumulated toward next frame
    float frameTime;                 // Seconds per frame

    // ----- Optional second layer (legs) drawn below the main layer -----
    Texture legsTexture;
    IntRect legsFrames[MAX_FRAMES];
    int legsFrameCount;
    int legsCurrentFrame;
    float legsTimer;
    float legsFrameTime;
    int legsOffsetY;                 // Extra vertical shift for legs (source-px)
    int torsoOffsetY;                // How far above the feet the torso sits (source-px)
    int torsoOffsetX;                // Horizontal nudge for torso (source-px), aligns torso with legs

public:
    Animation()
        : frameCount(0), currentFrame(0), timer(0.0f), frameTime(0.08f),
          legsFrameCount(0), legsCurrentFrame(0), legsTimer(0.0f), legsFrameTime(0.08f),
          legsOffsetY(0), torsoOffsetY(0), torsoOffsetX(0) {}

    // Load a strip of equally-spaced frames in a single horizontal row.
    // Use this when every frame has the same width and stride.
    void load(const char* path, int x, int y, int frameW, int frameH,
              int count, int stride, float seconds) {
        texture.loadFromFile(path);
        frameTime = seconds;
        frameCount = count > MAX_FRAMES ? MAX_FRAMES : count;
        for (int i = 0; i < frameCount; i++)
            frames[i] = IntRect(x + i * stride, y, frameW, frameH);
    }

    // Load frames with explicit per-frame X positions and widths.
    // Use this when the frames on the sheet have varying widths/spacing.
    void loadCustom(const char* path, const int* xs, const int* widths,
                    int y, int frameH, int count, float seconds) {
        texture.loadFromFile(path);
        frameTime = seconds;
        frameCount = count > MAX_FRAMES ? MAX_FRAMES : count;
        for (int i = 0; i < frameCount; i++)
            frames[i] = IntRect(xs[i], y, widths[i], frameH);
    }

    // Load the legs layer as an equally-spaced strip.
    void loadLegs(const char* path, int x, int y, int frameW, int frameH,
                  int count, int stride, float seconds, int offsetY) {
        legsTexture.loadFromFile(path);
        legsFrameTime = seconds;
        legsFrameCount = count > MAX_FRAMES ? MAX_FRAMES : count;
        legsOffsetY = offsetY;
        for (int i = 0; i < legsFrameCount; i++)
            legsFrames[i] = IntRect(x + i * stride, y, frameW, frameH);
    }

    // Load the legs layer with explicit per-frame X positions and widths.
    void loadLegsCustom(const char* path, const int* xs, const int* widths,
                        int y, int frameH, int count, float seconds, int offsetY) {
        legsTexture.loadFromFile(path);
        legsFrameTime = seconds;
        legsFrameCount = count > MAX_FRAMES ? MAX_FRAMES : count;
        legsOffsetY = offsetY;
        for (int i = 0; i < legsFrameCount; i++)
            legsFrames[i] = IntRect(xs[i], y, widths[i], frameH);
    }

    // How far the torso layer should be lifted above the feet, in source-pixels.
    void setTorsoOffset(int offsetY) { torsoOffsetY = offsetY; }
    // Horizontal nudge for the torso layer to align it with the legs.
    void setTorsoOffsetX(int offsetX) { torsoOffsetX = offsetX; }

    // Advance both layers by dt seconds, wrapping around when a frame elapses.
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

    // Restart at frame 0 (used when the player stops moving, etc.).
    void reset() {
        currentFrame = 0;     timer = 0.0f;
        legsCurrentFrame = 0; legsTimer = 0.0f;
    }

    // ----- Queries -----
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
