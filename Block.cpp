#include <SFML/Graphics.hpp>
#include "Block.h"
using namespace sf;
using namespace std;

// Default constructor
Block::Block() {
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
Block::Block(float x, float y, int blockType, int biomeType) {
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

Block::~Block() {}

// Getters
float Block::getX() { return x; }
float Block::getY() { return y; }
float Block::getWidth() { return width; }
float Block::getHeight() { return height; }
int   Block::getBlockType() { return blockType; }
bool  Block::getIsSolid() { return isSolid; }
bool  Block::getIsWater() { return isWater; }
bool  Block::getIsIndestructible() { return isIndestructible; }

// Setters
void Block::setTexture(Texture& tex) { sprite.setTexture(tex); }
void Block::setIsSolid(bool val) { isSolid = val; }
// Collision check
bool Block::checkCollision(float px, float py, float pw, float ph) {
    if (!isSolid) return false;

    // Player  center
    float playerCX = px + pw / 2;
    float playerCY = py + ph / 2;
    float nearestX = max(x, min(playerCX, x + width));
    float nearestY = max(y, min(playerCY, y + height));

    // Distance formula
    float dx = playerCX - nearestX;
    float dy = playerCY - nearestY;
    float distance = sqrt(dx * dx + dy * dy);

    // Collision threshold
    // Player  half + block  half
    float threshold = min(pw, ph) / 2;

    if (distance < threshold) {
        return true;
    }
    return false;
}

void Block::resolveCollision(float& px, float& py, float pw, float ph,
    float& velX, float& velY, bool& onGround) {
    if (!isSolid) return;

    float playerCX = px + pw / 2;
    float playerCY = py + ph / 2;

    float nearestX = max(x, min(playerCX, x + width));
    float nearestY = max(y, min(playerCY, y + height));

    float dx = playerCX - nearestX;
    float dy = playerCY - nearestY;
    float distance = sqrt(dx * dx + dy * dy);

    float playerRadius = min(pw, ph) / 2;

    if (distance < playerRadius) {
        float overlap = playerRadius - distance;

        if (distance == 0) {
            py -= overlap;
            velY = 0;
            onGround = true;
        }
        else {
            float nx = dx / distance;
            float ny = dy / distance;

            px += nx * overlap;
            py += ny * overlap;

            float dot = velX * nx + velY * ny;
            if (dot < 0) {
                velX -= dot * nx;
                velY -= dot * ny;
            }

            // Agar upar se push hua = ground par hai
            if (ny < -0.5f) {
                onGround = true;
            }
        }
    }
}




void Block::update(float dt) {}

void Block::render(RenderWindow& window, float camX, float camY) {
    if (!isVisible) return;
    if (blockType == BLOCK_EMPTY) return;
    sprite.setPosition(x - camX, y - camY);
    window.draw(sprite);
}

void Block::takeDamage(int amount) {
    if (isIndestructible) return;
    hp -= amount;
    if (hp <= 0) {
        blockType = BLOCK_EMPTY;
        isSolid = false;
        isVisible = false;
    }
}

bool Block::isDestroyed() { return blockType == BLOCK_EMPTY; }

IndestructibleBlock::IndestructibleBlock(float x, float y, int biomeType, int layerIndex)
    : Block(x, y, BLOCK_SOLID, biomeType)
{
    isIndestructible = true;
    isSolid = true;
    hp = 99999;
}

void IndestructibleBlock::takeDamage(int amount) {}

void IndestructibleBlock::render(RenderWindow& window, float camX, float camY) {
    sprite.setPosition(x - camX, y - camY);
    window.draw(sprite);
}