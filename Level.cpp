#pragma once
#include "Biome.h"
#include "Level.h"

using namespace sf;
using namespace std;
    
    // Constructor
    Level::Level(string name, int levelNumber, int biomeWidth, int biomeHeight) {
        this->name = name;
        this->levelNumber = levelNumber;
        this->biomeWidth = biomeWidth;
        this->biomeHeight = biomeHeight;
        this->blockSize = 64.0f;
        this->isComplete = false;
        this->isLoaded = false;

        // Biomes null 
        plains = nullptr;
        aerial = nullptr;
        aquatic = nullptr;

        // Boundaries calculate 
        // Plains then Aerial then Aquatic left te right
        levelStart = 0;
        plainsEnd = biomeWidth * blockSize;
        aerialEnd = plainsEnd + biomeWidth * blockSize;
        aquaticEnd = aerialEnd + biomeWidth * blockSize;
        levelEnd = aquaticEnd;
    }


    Level::~Level() {
        delete plains;
        delete aerial;
        delete aquatic;
        plains = nullptr;
        aerial = nullptr;
        aquatic = nullptr;
    }
    // Common Functions

    void Level::loadTextures(string solidPath, string waterPath) {
        if (plains)  plains->loadTextures(solidPath, waterPath);
        if (aerial)  aerial->loadTextures(solidPath, waterPath);
        if (aquatic) aquatic->loadTextures(solidPath, waterPath);
    }

    void Level::update(float dt) {
        if (plains)  plains->update(dt);
        if (aerial)  aerial->update(dt);
        if (aquatic) aquatic->update(dt);
    }

    void Level::render(RenderWindow& window, float camX, float camY) {
        if (plains)  plains->render(window, camX, camY);
        if (aerial)  aerial->render(window, camX, camY);
        if (aquatic) aquatic->render(window, camX, camY);
    }

    void Level::resolveCollisions(float& px, float& py,
        float pw, float ph,
        float& velX, float& velY,
        bool& onGround) {
        if (plains)  plains->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
        if (aerial)  aerial->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
        if (aquatic) aquatic->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
    }


    string Level::getName() { return name; }
    int    Level::getLevelNum() { return levelNumber; }
    float  Level::getLevelEnd() { return levelEnd; }
    float  Level::getLevelStart() { return levelStart; }
    float  Level::getPlainsEnd() { return plainsEnd; }
    float  Level::getAerialEnd() { return aerialEnd; }
    float  Level::getAquaticEnd() { return aquaticEnd; }
    bool   Level::getIsComplete() { return isComplete; }
    bool   Level::getIsLoaded() { return isLoaded; }


    SurvivalLevel::SurvivalLevel(string name, int levelNumber, int biomeWidth, int biomeHeight)
        : Level(name, levelNumber, biomeWidth, biomeHeight)
    {
        // Enemy counts
        totalEnemies = 0;
        enemiesKilled = 0;

        // Batch counts //child will override
        infantryBatchCount = 0;
        aerialBatchCount = 0;
        undeadBatchCount = 0;
        zombieBatchCount = 0;
        flyingTaraBatchCount = 0;
        powPrisonerCount = 0;

        // Scrolling always on in survival level
        hasHorizontalScroll = true;
        hasVerticalScroll = true;

        // Default spawn start of plains 
        playerSpawnX = 200;
        playerSpawnY = 100;

        // Default multiplier
        scoreMultiplier = 1.0f;
    }

    SurvivalLevel::~SurvivalLevel() {}


    bool SurvivalLevel::checkLevelComplete() {
        if (enemiesKilled >= totalEnemies && totalEnemies > 0) {
            isComplete = true;
        }
        return isComplete;
    }


    void SurvivalLevel::enemyKilled() {
        enemiesKilled++;
        checkLevelComplete();
    }


    float SurvivalLevel::getPlayerSpawnX() { return playerSpawnX; }
    float SurvivalLevel::getPlayerSpawnY() { return playerSpawnY; }
    float SurvivalLevel::getScoreMultiplier() { return scoreMultiplier; }
    int   SurvivalLevel::getEnemiesKilled() { return enemiesKilled; }
    int   SurvivalLevel::getTotalEnemies() { return totalEnemies; }
    bool  SurvivalLevel::isHorizontalScroll() { return hasHorizontalScroll; }
    bool  SurvivalLevel::isVerticalScroll() { return hasVerticalScroll; }

    
    Level1::Level1() : SurvivalLevel("Level 1", 1, 83, 14)
    {
        //for level 1
        rebelBatchCount = 2;
        shieldedBatchCount = 2;
        bazookaBatchCount = 2;
        grenadierBatchCount = 2;

        paratrooperBatchCount = 1;
        martianBatchCount = 1;

        mummyBatchCount = 2;

        zombieBatchCountAquatic = 2;
        enemySubCount = 1;

        flyingTaraCount = 2;
        powCount = 3; // 1 per biome

        totalEnemies =
            rebelBatchCount * 3 +   // rebel
            shieldedBatchCount * 1 +   // shielded batch = 1 2
            bazookaBatchCount * 1 +   // bazooka batch  = 1 2
            grenadierBatchCount * 1 +   // grenade batch  = 1 2
            paratrooperBatchCount * 2 + // paratroopers
            martianBatchCount * 1 +   // martians
            mummyBatchCount * 1 +   // mummies
            zombieBatchCountAquatic * 4;// zombies batch = 3 to 5

        // Level 1 score multiplier   normal
        scoreMultiplier = 1.0f;

        // player will spawn at start of plains biome
        playerSpawnX = 200;
        playerSpawnY = 100;
    }

    Level1::~Level1() {}


    void Level1::generateBiomes() {
        // Plains biome
        plains = new PlainsBiome(levelStart, plainsEnd);

        // Aerial biome after plains
        aerial = new AerialBiome(plainsEnd, aerialEnd);

        // Aquatic biome after aerial
        aquatic = new AquaticBiome(aerialEnd, aquaticEnd);

        // Textures load k
        loadTextures("Sprites/blocks/stone.png",
            "Sprites/blocks/water.png");

        // Terrain generate 
        plains->generateTerrain(biomeWidth, biomeHeight);
        aerial->generateTerrain(biomeWidth, biomeHeight);
        aquatic->generateTerrain(biomeWidth, biomeHeight);

        isLoaded = true;
    }


    void Level1::spawnEnemies() {

    }


    void Level1::update(float dt) {
        //base class called
        Level::update(dt);

        // Level 1 specific update

        checkLevelComplete();
    }


    void Level1::render(RenderWindow& window, float camX, float camY) {

        Level::render(window, camX, camY);

    }


    int Level1::getRebelBatchCount() { return rebelBatchCount; }
    int Level1::getMummyBatchCount() { return mummyBatchCount; }
    int Level1::getZombieCount() { return zombieBatchCountAquatic; }
    int Level1::getEnemySubCount() { return enemySubCount; }
    int Level1::getFlyingTaraCount() { return flyingTaraCount; }
    int Level1::getPowCount() { return powCount; }