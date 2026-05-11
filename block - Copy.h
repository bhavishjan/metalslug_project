#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

const int screen_x = 1600;
const int screen_y = 900;

// Biome types
static const int BIOME_AERIAL = 0;
static  const int BIOME_PLAINS = 1;
static const int BIOME_AQUATIC = 2;


// Block types
static const int BLOCK_EMPTY = 0;
static const int BLOCK_SOLID = 1;
static const int BLOCK_WATER = 2;
static const int BLOCK_INDESTRUCTIBLE = 3;



static const int SCREEN_H = 900;
class Block {
protected:
    float x, y;
    float width = 64;
    float height = 64;
    int   blockType;
    int   biomeType;
    bool  isSolid;
    bool  isWater;
    bool  isIndestructible;
    bool  isVisible;
    int   hp;
    Sprite sprite;

public:
    // Default constructor
    Block() {
        x = 0; y = 0;
        blockType = BLOCK_EMPTY;
        biomeType = BIOME_PLAINS;
        isSolid = false;
        isWater = false;
        isIndestructible = false;
        isVisible = true;
        hp = 3;
    }

    // Parameterized constructor
    Block(float x, float y, int blockType, int biomeType) {
        this->x = x;
        this->y = y;
        this->blockType = blockType;
        this->biomeType = biomeType;
        isSolid = (blockType == BLOCK_SOLID);
        isWater = (blockType == BLOCK_WATER);
        isIndestructible = false;
        isVisible = true;
        hp = 3;
    }

    virtual ~Block() {}

    // Getters
    float getX() { return x; }
    float getY() { return y; }
    float getWidth() { return width; }
    float getHeight() { return height; }
    int   getBlockType() { return blockType; }
    bool  getIsSolid() { return isSolid; }
    bool  getIsWater() { return isWater; }
    bool  getIsIndestructible() { return isIndestructible; }

    // Setters
    void setTexture(Texture& tex) { sprite.setTexture(tex); }
    void setIsSolid(bool val) { isSolid = val; }

    // Collision check
    bool checkCollision(float px, float py, float pw, float ph) {
        if (!isSolid) return false;
        if (px + pw <= x) return false;
        if (px >= x + width) return false;
        if (py + ph <= y) return false;
        if (py >= y + height) return false;
        return true;
    }

    void resolveCollision(float& px, float& py, float pw, float ph,
        float& velX, float& velY, bool& onGround) {

        if (!isSolid) return;

        if (px + pw <= x) return;
        if (px >= x + width) return;
        if (py + ph <= y) return;
        if (py >= y + height) return;

        float overlapLeft = (x + width) - px;
        float overlapRight = (px + pw) - x;
        float overlapTop = (y + height) - py;
        float overlapBottom = (py + ph) - y;

        float minX = min(overlapLeft, overlapRight);
        float minY = min(overlapTop, overlapBottom);

        if (minY < minX) {
            // Y collision
            if (velY > 0) {
                // landing on ground
                py = y - ph;
                velY = 0;
                onGround = true;
            }
            else if (velY < 0) {
                // hitting ceiling
                py = y + height;
                velY = 0;
            }
        }
        else {
            // X collision
            if (velX > 0) {
                px = x - pw;
            }
            else if (velX < 0) {
                px = x + width;
            }
            velX = 0;
        }
    }


    virtual void update(float dt) {}

    virtual void render(RenderWindow& window, float camX, float camY) {
        if (!isVisible) return;
        if (blockType == BLOCK_EMPTY) return;
        sprite.setPosition(x - camX, y - camY);
        window.draw(sprite);
    }

    virtual void takeDamage(int amount) {
        if (isIndestructible) return;
        hp -= amount;
        if (hp <= 0) {
            blockType = BLOCK_EMPTY;
            isSolid = false;
            isVisible = false;
        }
    }

    bool isDestroyed() { return blockType == BLOCK_EMPTY; }
};

//childblock
class IndestructibleBlock : public Block {
public:
    IndestructibleBlock(float x, float y, int biomeType, int layerIndex)
        : Block(x, y, BLOCK_SOLID, biomeType)
    {
        isIndestructible = true;
        isSolid = true;
        hp = 99999;
    }

    void takeDamage(int amount) override {}

    void render(RenderWindow& window, float camX, float camY) override {
        sprite.setPosition(x - camX, y - camY);
        window.draw(sprite);
    }
};