#pragma once
class Camera {
private:
    float x;
    float y;
    float targetX;
    float targetY;
    float smoothing;
    float minX;
    float maxX;
    float minY;
    float maxY;
    int screenWidth;
    int screenHeight;

public:
    Camera()
        : x(0.0f),
          y(0.0f),
          targetX(0.0f),
          targetY(0.0f),
          smoothing(0.2f),
          minX(0.0f),
          maxX(0.0f),
          minY(0.0f),
          maxY(0.0f),
          screenWidth(1600),
          screenHeight(900) {
    }

    Camera(int width, int height)
        : x(0.0f),
          y(0.0f),
          targetX(0.0f),
          targetY(0.0f),
          smoothing(0.2f),
          minX(0.0f),
          maxX(0.0f),
          minY(0.0f),
          maxY(0.0f),
          screenWidth(width),
          screenHeight(height) {
    }

    void setSmoothing(float value) {
        smoothing = value;
    }

    void setBounds(float minXVal, float maxXVal, float minYVal, float maxYVal) {
        minX = minXVal;
        maxX = maxXVal;
        minY = minYVal;
        maxY = maxYVal;
    }

    void setScreenSize(int width, int height) {
        screenWidth = width;
        screenHeight = height;
    }

    void setPosition(float posX, float posY) {
        x = posX;
        y = posY;
    }

    void setTarget(float targetPosX, float targetPosY) {
        targetX = targetPosX;
        targetY = targetPosY;
    }

    void follow(float playerX, float playerY) {
        targetX = playerX - screenWidth / 2.0f;
        targetY = playerY - screenHeight / 2.0f;
    }

    void update() {
        x += (targetX - x) * smoothing;
        y += (targetY - y) * smoothing;

        if (x < minX) {
            x = minX;
        }

        if (x > maxX) {
            x = maxX;
        }

        if (y < minY) {
            y = minY;
        }

        if (y > maxY) {
            y = maxY;
        }
    }

    void reset() {
        x = 0.0f;
        y = 0.0f;
        targetX = 0.0f;
        targetY = 0.0f;
    }

    float getX() const {
        return x;
    }

    float getY() const {
        return y;
    }

    float getTargetX() const {
        return targetX;
    }

    float getTargetY() const {
        return targetY;
    }
};