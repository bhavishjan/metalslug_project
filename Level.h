#pragma once

#include <SFML/Graphics.hpp>
#include "Block.h"
#include "Biome.h"

using namespace sf;
using namespace std;

class Level {
protected:
    string name;
    int    levelNumber;

    // Size
    int   biomeWidth;
    int   biomeHeight;
    float blockSize;

    // Biomes
    PlainsBiome* plains;
    AerialBiome* aerial;
    AquaticBiome* aquatic;

    // Boundaries
    float levelStart;
    float levelEnd;
    float plainsEnd;
    float aerialEnd;
    float aquaticEnd;

    // State
    bool isComplete;
    bool isLoaded;

public:
    // Constructor
    Level(string name, int levelNumber, int biomeWidth, int biomeHeight);

    // Destructor
    virtual ~Level();

    // Pure virtual functions
    virtual void generateBiomes() = 0;
    virtual void spawnEnemies() = 0;
    virtual bool checkLevelComplete() = 0;

    // Common functions
    virtual void loadTextures(string solidPath, string waterPath);
    virtual void update(float dt);
    virtual void render(RenderWindow& window, float camX, float camY);
    virtual void resolveCollisions(float& px, float& py,
        float pw, float ph,
        float& velX, float& velY,
        bool& onGround);

    // Getters
    string getName();
    int    getLevelNum();
    float  getLevelEnd();
    float  getLevelStart();
    float  getPlainsEnd();
    float  getAerialEnd();
    float  getAquaticEnd();
    bool   getIsComplete();
    bool   getIsLoaded();
};

class SurvivalLevel : public Level {
protected:
    // Enemy tracking
    int totalEnemies;
    int enemiesKilled;

    int infantryBatchCount;
    int aerialBatchCount;
    int undeadBatchCount;
    int zombieBatchCount;
    int flyingTaraBatchCount;
    int powPrisonerCount;

    bool hasHorizontalScroll;
    bool hasVerticalScroll;

    // Player spawn position
    float playerSpawnX;
    float playerSpawnY;

    float scoreMultiplier;

public:
    // Constructor
    SurvivalLevel(string name, int levelNumber, int biomeWidth, int biomeHeight);

    // Destructor
    virtual ~SurvivalLevel();

    // Pure virtual functions (still abstract)
    virtual void generateBiomes() = 0;
    virtual void spawnEnemies() = 0;

    // Implemented virtual function
    bool checkLevelComplete() override;

    // Enemy management
    void enemyKilled();

    // Getters
    float getPlayerSpawnX();
    float getPlayerSpawnY();
    float getScoreMultiplier();
    int   getEnemiesKilled();
    int   getTotalEnemies();
    bool  isHorizontalScroll();
    bool  isVerticalScroll();
};

class Level1 : public SurvivalLevel {
private:
    // Infantry - 2 batches each in 3 biomes
    int rebelBatchCount;
    int shieldedBatchCount;
    int bazookaBatchCount;
    int grenadierBatchCount;

    // Aerial - 1 batch each in aerial biome
    int paratrooperBatchCount;
    int martianBatchCount;

    // Undead - 2 batches each only in plains
    int mummyBatchCount;

    // Aquatic specific
    int zombieBatchCountAquatic;  // 2 batches in aquatic
    int enemySubCount;

    // Flying Tara - 2 batches across entire level
    int flyingTaraCount;

    // POW - 1 per biome
    int powCount;

public:
    // Constructor
    Level1();

    // Destructor
    ~Level1();

    // Override pure virtual functions
    void generateBiomes() override;
    void spawnEnemies() override;

    // Override virtual functions
    void update(float dt) override;
    void render(RenderWindow& window, float camX, float camY) override;

    // Getters
    int getRebelBatchCount();
    int getMummyBatchCount();
    int getZombieCount();
    int getEnemySubCount();
    int getFlyingTaraCount();
    int getPowCount();
};
