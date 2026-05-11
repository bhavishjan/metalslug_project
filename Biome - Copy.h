#pragma once
#include <SFML/Graphics.hpp>
#include "PerlinNoise.h"
#include "Block.h"
using namespace sf;


// Speed affects wave steepness, size affects hill height
class Biome {
protected:
    const char*  name;
    int    biomeType;
    float  startX, endX;
    float  startY, endY;
    Block** blocks;
    int    blockCount;
    Texture solidTex;
    Texture waterTex;
    Texture grassTex;
    Texture dirtTex;

public:
    Biome();
    Biome(const char* name, int biomeType, float startX, float endX);
    virtual ~Biome();
    bool checkWaterCollision(float px, float py, float pw, float ph);
    virtual void generateTerrain(int widthInBlocks, int heightInBlocks) = 0;
    void loadTextures(const char* solidPath, const char* waterPath, const char* grassPath, const char* dirtPath);
    void update(float dt);
    void render(RenderWindow& window, float camX, float camY);
    bool isInBiome(float playerX);
    bool checkCollision(float px, float py, float pw, float ph);
    void resolveCollisions(float& px, float& py, float pw, float ph,
        float& velX, float& velY, bool& onGround);
    float getStartX();
    float getEndX();
};


class PlainsBiome : public Biome {
protected:
    bool useperlin;
private:
    float hillSpeed;
    int   hillSize;
public:
    PlainsBiome(float startX, float endX, bool per = false);
    void generateTerrain(int widthInBlocks, int heightInBlocks) override;
};


class AerialBiome : public Biome {
protected:
    bool useperlin;
public:
    AerialBiome(float startX, float endX, bool perlin = false);
    void generateTerrain(int widthInBlocks, int heightInBlocks) override;
};


class AquaticBiome : public Biome {
protected:
    bool useperlin;
public:
    AquaticBiome(float startX, float endX, bool perlin = false);
    void generateTerrain(int widthInBlocks, int heightInBlocks) override;
};
