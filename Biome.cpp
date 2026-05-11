#include "Biome.h"
#include "Block.h"
#include <cmath>

// Biome stuff
Biome::Biome() {
    name = "";
    biomeType = BIOME_PLAINS;
    startX = endX = 0;
    startY = 0; endY = 900;
    blocks = nullptr;
    blockCount = 0;
}

Biome::Biome(const char* name, int biomeType, float startX, float endX) {
    this->name = name;
    this->biomeType = biomeType;
    this->startX = startX;
    this->endX = endX;
    startY = 0; endY = 900;
    blocks = nullptr;
    blockCount = 0;
}

Biome::~Biome() {
    if (blocks) {
        for (int i = 0; i < blockCount; i++)
            delete blocks[i];
        delete[] blocks;
    }
}

bool Biome::checkWaterCollision(float px, float py, float pw, float ph) {
    for (int i = 0; i < blockCount; i++) {
        if (blocks[i] && blocks[i]->getIsWater()) {
            if (blocks[i]->checkCollision(px, py, pw, ph)) {
                return true;
            }
        }
    }
    return false;
}

void Biome::loadTextures(const char* solidPath, const char* waterPath, const char* grassPath, const char* dirtPath) {
    solidTex.loadFromFile(solidPath);
    waterTex.loadFromFile(waterPath);
    grassTex.loadFromFile(grassPath);
    dirtTex.loadFromFile(dirtPath);
}

void Biome::update(float dt) {
    for (int i = 0; i < blockCount; i++) {
        if (blocks[i]) {
            blocks[i]->update(dt);
        }
    }
}

void Biome::render(RenderWindow& window, float camX, float camY) {
    for (int i = 0; i < blockCount; i++) {
        if (blocks[i]) {
            blocks[i]->render(window, camX, camY);
        }
    }
}

bool Biome::isInBiome(float playerX) {
    return (playerX >= startX - 2000 && playerX <= endX + 2000);
}

bool Biome::checkCollision(float px, float py, float pw, float ph) {
    if (!isInBiome(px)) {
        return false;
    }

    for (int i = 0; i < blockCount; i++) {
        if (blocks[i] && blocks[i]->checkCollision(px, py, pw, ph)) {
            return true;
        }
    }

    return false;
}

void Biome::resolveCollisions(float& px, float& py, float pw, float ph,
    float& velX, float& velY, bool& onGround) {
    if (!isInBiome(px)) {
        return;
    }
    for (int i = 0; i < blockCount; i++) {
        if (blocks[i]) {
            blocks[i]->resolveCollision(px, py, pw, ph, velX, velY, onGround);
        }
    }
}

float Biome::getStartX() {
    return startX;
}

float Biome::getEndX() {
    return endX;
}

// PlainsBiome implementations
PlainsBiome::PlainsBiome(float startX, float endX, bool per)
    : Biome("Plains", BIOME_PLAINS, startX, endX), hillSpeed(0.03f), hillSize(4) {
    useperlin = per;
}

void PlainsBiome::generateTerrain(int widthInBlocks, int heightInBlocks) {
    blocks = new Block * [widthInBlocks * heightInBlocks];
    blockCount = 0;

    int surfaceRow = heightInBlocks - 4;

    PerlinNoise perlin(42);

    float speed = 0.03f;
    int hillSize = 4;

    for (int col = 0; col < widthInBlocks; col++) {
        float x = startX + col * 64.0f;
        int sr;
        if (useperlin) {
            float globalCol = (startX / 64.0f) + col;
            float noiseVal = perlin.fractal((globalCol) * 0.005f, 0, 4);
            sr = (int)(heightInBlocks * 0.65f) + (int)(noiseVal * hillSize);
        }
        else {
            sr = surfaceRow + (int)(sin(col * speed) * hillSize);
        }

        if (sr < 1) {
            sr = 1;
        }
        if (sr >= heightInBlocks - 1) {
            sr = heightInBlocks - 2;
        }

        for (int row = sr; row < heightInBlocks; row++) {
            float y = row * 64.0f;
            Block* b;

            if (row == heightInBlocks - 1) {
                b = new IndestructibleBlock(x, y, BIOME_PLAINS, row);
            }
            else if (row == sr) {
                b = new Block(x, y, BLOCK_SOLID, BIOME_PLAINS);
                b->setTexture(grassTex);
            }
            else {
                b = new Block(x, y, BLOCK_SOLID, BIOME_PLAINS);
                b->setTexture(dirtTex);
            }
            if (blockCount < widthInBlocks * heightInBlocks) {
                blocks[blockCount++] = b;
            }
        }
    }
}

// AerialBiome implementations
AerialBiome::AerialBiome(float startX, float endX, bool perlin)
    : Biome("Aerial", BIOME_AERIAL, startX, endX), useperlin(perlin) {
}

void AerialBiome::generateTerrain(int widthInBlocks, int heightInBlocks) {
    blocks = new Block * [widthInBlocks * heightInBlocks];
    blockCount = 0;
    PerlinNoise perlin(42);
    int topRow = 2;

    for (int col = 0; col < widthInBlocks; col++) {
        float x = startX + col * 64.0f;
        int surfaceRow;

        if (useperlin) {
            float globalCol = (startX / 64.0f) + col;

            float n1 = perlin.fractal(globalCol * 0.003f, 0.0f, 5);
            float n2 = perlin.fractal(globalCol * 0.09f, 99.0f, 3) * 0.4f;
            float combined = (n1 + n2) / 1.4f;
            surfaceRow = (int)(heightInBlocks * 0.5f) + (int)(combined * 6.0f);
        }
        else {
            surfaceRow = topRow
                + (int)(sin(col * 0.08f) * 5)
                + (int)(sin(col * 0.20f) * 3);
        }

        if (surfaceRow < 1) {
            surfaceRow = 1;
        }
        if (surfaceRow >= heightInBlocks - 1) {
            surfaceRow = heightInBlocks - 2;
        }

        for (int row = surfaceRow; row < heightInBlocks; row++) {
            float y = row * 64.0f;
            Block* b;

            if (row == heightInBlocks - 1) {
                b = new IndestructibleBlock(x, y, BIOME_AERIAL, row);
            }
            else if (row == surfaceRow) {
                b = new Block(x, y, BLOCK_SOLID, BIOME_AERIAL);
                b->setTexture(grassTex);
            }
            else {
                b = new Block(x, y, BLOCK_SOLID, BIOME_AERIAL);
                b->setTexture(dirtTex);
            }
            if (blockCount < widthInBlocks * heightInBlocks) {
                blocks[blockCount++] = b;
            }
        }
    }
}

// AquaticBiome implementations
AquaticBiome::AquaticBiome(float startX, float endX, bool perlin)
    : Biome("Aquatic", BIOME_AQUATIC, startX, endX), useperlin(perlin) {
}

void AquaticBiome::generateTerrain(int widthInBlocks, int heightInBlocks) {
    blocks = new Block * [widthInBlocks * heightInBlocks];
    blockCount = 0;

    int seaLevelRow = heightInBlocks / 3;
    int floorMidRow = (int)(heightInBlocks * 0.75f);

    PerlinNoise perlin(42);

    for (int col = 0; col < widthInBlocks; col++) {
        float x = startX + col * 64.0f;

        int floorRow;

        if (useperlin) {
            float globalCol = (startX / 64.0f) + col;
            float noiseVal = perlin.fractal(globalCol * 0.004f, 55.0f, 4);
            floorRow = floorMidRow + (int)(noiseVal * 3);
        }
        else {
            floorRow = floorMidRow + (int)(sin(col * 0.03f) * 2);
        }

        if (floorRow < seaLevelRow + 2) {
            floorRow = seaLevelRow + 2;
        }
        if (floorRow >= heightInBlocks - 1) {
            floorRow = heightInBlocks - 2;
        }

        for (int row = seaLevelRow; row < heightInBlocks; row++) {
            float y = row * 64.0f;
            Block* b;

            if (row < floorRow) {
                b = new Block(x, y, BLOCK_WATER, BIOME_AQUATIC);
                b->setTexture(waterTex);
                b->setIsSolid(false);
            }
            else if (row == heightInBlocks - 1) {
                b = new IndestructibleBlock(x, y, BIOME_AQUATIC, row);
            }
            else if (row == floorRow) {
                b = new Block(x, y, BLOCK_SOLID, BIOME_AQUATIC);
                b->setTexture(grassTex);
            }
            else {
                b = new Block(x, y, BLOCK_SOLID, BIOME_AQUATIC);
                b->setTexture(dirtTex);
            }
            if (blockCount < widthInBlocks * heightInBlocks) {
                blocks[blockCount++] = b;
            }
        }
    }
}
