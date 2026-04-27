#include <SFML/Graphics.hpp>
#include "Block.h"
#include "Biome.h"

//if speed is less = gentle wave
//if speed is more = more peaks
//if size is less = smaller hill
//if size is more = bigger hill
    Biome::Biome() {
        name = "";
        biomeType = BIOME_PLAINS;
        startX = endX = 0;
        startY = 0; endY = 900;
        blocks = nullptr;
        blockCount = 0;
    }

    Biome::Biome(string name, int biomeType, float startX, float endX) {
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

    void Biome::loadTextures(string solidPath, string waterPath) {
        solidTex.loadFromFile(solidPath);
        waterTex.loadFromFile(waterPath);
    }

    void Biome::update(float dt) {
        for (int i = 0; i < blockCount; i++)
            if (blocks[i]) blocks[i]->update(dt);
    }

    void Biome::render(RenderWindow& window, float camX, float camY) {
        for (int i = 0; i < blockCount; i++)
            if (blocks[i]) 
                blocks[i]->render(window, camX, camY);
    }

    bool Biome::isInBiome(float playerX) {
        // Biome ka center
        float biomeCenter = (startX + endX) / 2;
        // Biome ki half width
        float halfWidth = (endX - startX) / 2;
        // Player aur center ki distance
        float distance = abs(playerX - biomeCenter);
        // Agar distance half width se kam hai → player andar hai
        return (distance < halfWidth);
    }

    bool Biome::checkCollision(float px, float py, float pw, float ph) {

        // check is playerin  biome 
        if (!isInBiome(px)) return false;

        // then  block check 
        for (int i = 0; i < blockCount; i++)
            if (blocks[i] && blocks[i]->checkCollision(px, py, pw, ph))
                return true;

        return false;
    }
    void Biome::resolveCollisions(float& px, float& py, float pw, float ph,
        float& velX, float& velY, bool& onGround) {
        if (!isInBiome(px)) return;
        for (int i = 0; i < blockCount; i++)
            if (blocks[i])
                blocks[i]->resolveCollision(px, py, pw, ph, velX, velY, onGround);
    }
    float Biome::getStartX() { return startX; }
    float Biome::getEndX() { return endX; }


    PlainsBiome::PlainsBiome(float startX, float endX)
        : Biome("Plains", BIOME_PLAINS, startX, endX) {
    }

    void PlainsBiome::generateTerrain(int widthInBlocks, int heightInBlocks) {
        blocks = new Block * [widthInBlocks * heightInBlocks];
        blockCount = 0;

        // Surface bilkul upar rakho — sirf 1-2 rows
        int surfaceRow = heightInBlocks - 3; // bottom se 3 rows upar
        float speed = 0.03f;
        int hillSize = 1;

        for (int col = 0; col < widthInBlocks; col++) {
            float x = startX + col * 64.0f;

            // Gentle wave
            int sr = surfaceRow + (int)(sin(col * speed) * hillSize);
            if (sr < 1) sr = 1;
            if (sr >= heightInBlocks - 1) sr = heightInBlocks - 2;

            for (int row = sr; row < heightInBlocks; row++) {
                // Blocks screen ke bilkul bottom par
                float y = SCREEN_H - (heightInBlocks - row - 1) * 64.0f;
                Block* b;

                if (row == heightInBlocks - 1)
                    b = new IndestructibleBlock(x, y, BIOME_PLAINS, row);
                else {
                    b = new Block(x, y, BLOCK_SOLID, BIOME_PLAINS);
                    b->setTexture(solidTex);
                }
                blocks[blockCount++] = b;
            }
        }
    }

//ariel biome use two sin waves to create more peak variant mountains
    AerialBiome::AerialBiome(float startX, float endX)
        : Biome("Aerial", BIOME_AERIAL, startX, endX) {
    }

    void AerialBiome::generateTerrain(int widthInBlocks, int heightInBlocks) {
        blocks = new Block * [widthInBlocks * heightInBlocks];
        blockCount = 0;

        // Mountains are above so divide by 3 to get highh peak
        int topRow = heightInBlocks / 3;

        for (int col = 0; col < widthInBlocks; col++) {
            float x = startX + col * 64.0f;

            // 2 sin waves
            // 1st wave =  slow wave 
            // 2md wave =  fast wave 
            int surfaceRow = topRow
                + (int)(sin(col * 0.08f) * 4)   // slow
                + (int)(sin(col * 0.20f) * 2);  // jagged wave

            if (surfaceRow < 1)                  surfaceRow = 1;
            if (surfaceRow >= heightInBlocks - 1) surfaceRow = heightInBlocks - 2;

            for (int row = surfaceRow; row < heightInBlocks; row++) {
                float y = SCREEN_H - (heightInBlocks - row - 1) * 64.0f;
                Block* b;

                if (row == heightInBlocks - 1) {
                    b = new IndestructibleBlock(x, y, BIOME_AERIAL, row);
                }
                else {
                    b = new Block(x, y, BLOCK_SOLID, BIOME_AERIAL);
                    b->setTexture(solidTex);
                }
                blocks[blockCount++] = b;
            }
        }
    }
//water on top and floor at bottom with very gentle wave
    AquaticBiome::AquaticBiome(float startX, float endX)
        : Biome("Aquatic", BIOME_AQUATIC, startX, endX) {
    }

    void AquaticBiome::generateTerrain(int widthInBlocks, int heightInBlocks) {
        blocks = new Block * [widthInBlocks * heightInBlocks];
        blockCount = 0;

        // Sea level water start
        int seaLevelRow = heightInBlocks / 3;
        // Sea floor solid ground start
        int floorMidRow = (int)(heightInBlocks * 0.75f);

        for (int col = 0; col < widthInBlocks; col++) {
            float x = startX + col * 64.0f;

            // Sea floor mein bahut gentle wave
            // speed = 0.05 (bahut slow) = almost flat sea floor
            int floorRow = floorMidRow + (int)(sin(col * 0.05f) * 1);

            if (floorRow < seaLevelRow + 2)       floorRow = seaLevelRow + 2;
            if (floorRow >= heightInBlocks - 1)    floorRow = heightInBlocks - 2;

            for (int row = seaLevelRow; row < heightInBlocks; row++) {
                float y = SCREEN_H - (heightInBlocks - row - 1) * 64.0f;
                Block* b;

                if (row < floorRow) {
                    // water blocks
                    b = new Block(x, y, BLOCK_WATER, BIOME_AQUATIC);
                    b->setTexture(waterTex);
                    b->setIsSolid(false);
                }
                else if (row == heightInBlocks - 1) {
                    // bottom most indestructible
                    b = new IndestructibleBlock(x, y, BIOME_AQUATIC, row);
                }
                else {
                    // Sea floor solid
                    b = new Block(x, y, BLOCK_SOLID, BIOME_AQUATIC);
                    b->setTexture(solidTex);
                }
                blocks[blockCount++] = b;
            }
        }
    }