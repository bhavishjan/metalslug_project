#pragma once

#include <SFML/Graphics.hpp>
#include "Block.h"
#include <cmath>

using namespace sf;
using namespace std;

class Biome {
protected:
    string name;
    int    biomeType;
    float  startX, endX;
    float  startY, endY;
    Block** blocks;  // Aggregation with Block
    int    blockCount;
    Texture solidTex;
    Texture waterTex;

public:
    // Constructors
    Biome();
    Biome(string name, int biomeType, float startX, float endX);

    // Destructor
    virtual ~Biome();

    // Pure virtual function - must be implemented by derived classes
    virtual void generateTerrain(int widthInBlocks, int heightInBlocks) = 0;

    // Texture loading
    void loadTextures(string solidPath, string waterPath);

    // Update and render
    void update(float dt);
    void render(RenderWindow& window, float camX, float camY);

    // Collision methods
    bool isInBiome(float playerX);
    bool checkCollision(float px, float py, float pw, float ph);
    void resolveCollisions(float& px, float& py, float pw, float ph,
        float& velX, float& velY, bool& onGround);

    // Getters
    float getStartX();
    float getEndX();
};

// Derived class: PlainsBiome
class PlainsBiome : public Biome {
public:
    PlainsBiome(float startX, float endX);

    void generateTerrain(int widthInBlocks, int heightInBlocks) override;
};

// Derived class: AerialBiome
class AerialBiome : public Biome {
public:
    AerialBiome(float startX, float endX);

    void generateTerrain(int widthInBlocks, int heightInBlocks) override;
};

// Derived class: AquaticBiome
class AquaticBiome : public Biome {
public:
    AquaticBiome(float startX, float endX);

    void generateTerrain(int widthInBlocks, int heightInBlocks) override;
};