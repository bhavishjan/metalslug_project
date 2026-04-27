#pragma once

#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

// Block types
const int BLOCK_EMPTY = 0;
const int BLOCK_SOLID = 1;
const int BLOCK_WATER = 2;
const int BLOCK_INDESTRUCTIBLE = 3;

// Biome types
const int BIOME_AERIAL = 0;
const int BIOME_PLAINS = 1;
const int BIOME_AQUATIC = 2;

const int SCREEN_H = 900;

class Block {
protected:
    float x, y;
    float width;
    float height;
    int   blockType;
    int   biomeType;
    bool  isSolid;
    bool  isWater;
    bool  isIndestructible;
    bool  isVisible;
    int   hp;
    Sprite sprite;

public:
    // Constructors
    Block();
    Block(float x, float y, int blockType, int biomeType);

    // Destructor
    virtual ~Block();

    // Getters
    float getX();
    float getY();
    float getWidth();
    float getHeight();
    int   getBlockType();
    bool  getIsSolid();
    bool  getIsWater();
    bool  getIsIndestructible();

    // Setters
    void setTexture(Texture& tex);
    void setIsSolid(bool val);

    // Collision methods
    bool checkCollision(float px, float py, float pw, float ph);
    void resolveCollision(float& px, float& py, float pw, float ph,
        float& velX, float& velY, bool& onGround);

    // Virtual methods
    virtual void update(float dt);
    virtual void render(RenderWindow& window, float camX, float camY);
    virtual void takeDamage(int amount);

    // Utility
    bool isDestroyed();
};

// Derived class: IndestructibleBlock
class IndestructibleBlock : public Block {
public:
    IndestructibleBlock(float x, float y, int biomeType, int layerIndex);

    void takeDamage(int amount) override;
    void render(RenderWindow& window, float camX, float camY) override;
};