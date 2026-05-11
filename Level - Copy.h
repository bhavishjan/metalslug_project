#pragma once

#include "Block.h"
#include "Biome.h"
#include "Enemy.h"
#include <cstring>

using namespace sf;

// Forward declaration
class BossLevel;


class Level {
protected:

    const char*  name;
    int    levelNumber;

    int   biomeWidth;
    int   biomeHeight;
    float blockSize;

    PlainsBiome* plains;
    AerialBiome* aerial;
    AquaticBiome* aquatic;


    // Boundaries

    float levelStart;
    float levelEnd;
    float plainsEnd;
    float aerialEnd;
    float aquaticEnd;

    float playerSpawnX;
    float playerSpawnY;
    // State

    bool isComplete;
    bool isLoaded;

public:
    Level(const char* name, int levelNumber, int biomeWidth, int biomeHeight);
    virtual ~Level();

    virtual void generateBiomes() = 0;
    virtual bool checkLevelComplete() = 0;
    virtual void spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player);

    virtual void loadTextures(const char* solidPath, const char* waterPath, const char* grassPath, const char* dirtPath);
    virtual void update(float dt);
    virtual void render(RenderWindow& window, float camX, float camY);
    virtual void resolveCollisions(float& px, float& py, float pw, float ph, float& velX, float& velY, bool& onGround);
    bool checkCollision(float px, float py, float pw, float ph);
    bool checkWaterAt(float px, float py, float pw, float ph);

    float getPlayerSpawnX();
    float getPlayerSpawnY();
    void playerReachedEnd();
    const char* getName();
    int getLevelNum();
    float getLevelEnd();
    float getLevelStart();
    float getPlainsEnd();
    float getAerialEnd();
    float getAquaticEnd();
    bool getIsComplete();
    bool getIsLoaded();
};

class SurvivalLevel : public Level {
protected:
    //enemie tracking
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




    float scoreMultiplier;

public:
    SurvivalLevel(const char* name, int levelNumber, int biomeWidth, int biomeHeight);
    virtual ~SurvivalLevel();

    virtual void generateBiomes() = 0;
    bool checkLevelComplete() override;
    void playerReachedEnd();
    void enemyKilled();

    float getScoreMultiplier();
    int getEnemiesKilled();
    int getTotalEnemies();
    bool isHorizontalScroll();
    bool isVerticalScroll();
};

class Level1 : public SurvivalLevel {
private:


    // Infantry 2 batches each in 3 biomes 
    int rebelBatchCount;
    int shieldedBatchCount;
    int bazookaBatchCount;
    int grenadierBatchCount;

    // Aerial 1 batch each aerial biome
    int paratrooperBatchCount;
    int martianBatchCount;

    // Undead  2 batches each only in plains
    int mummyBatchCount;

    // Aquatic specific
    int zombieBatchCountAquatic;  // 2 batches in aquatic 
    int enemySubCount;

    // Flying Tara  2 batches poore level mein
    int flyingTaraCount;

    // POW  1 per biome
    int powCount;

public:
    Level1();
    ~Level1();

    void generateBiomes() override;
    void spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) override;
    void update(float dt) override;
    void render(RenderWindow& window, float camX, float camY) override;

    int getRebelBatchCount();
    int getMummyBatchCount();
    int getZombieCount();
    int getEnemySubCount();
    int getFlyingTaraCount();
    int getPowCount();
};

class Level2 : public SurvivalLevel {
private:
    int rebelBatchCount;
    int shieldedBatchCount;
    int bazookaBatchCount;
    int grenadierBatchCount;
    int paratrooperBatchCount;
    int martianBatchCount;
    int mummyBatchCount;
    int zombieBatchCountAquatic;
    int enemySubCount;
    int flyingTaraCount;
    int bradleyCount;
    int powPrisonerPlains;
    int powPrisonerOther;

public:
    Level2();
    ~Level2();

    void generateBiomes() override;
    void spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) override;
    void update(float dt) override;
    void render(RenderWindow& window, float camX, float camY) override;

    int getRebelBatchCount();
    int getMummyBatchCount();
    int getZombieCount();
    int getEnemySubCount();
    int getFlyingTaraCount();
    int getBradleyCount();
    int getPowPrisonerPlains();
    int getPowPrisonerOther();
};

class Level3 : public SurvivalLevel {
private:
    int rebelBatchCount;
    int shieldedBatchCount;
    int bazookaBatchCount;
    int grenadierBatchCount;
    int paratrooperBatchCount;
    int martianBatchCount;
    int mummyBatchCount;
    int zombieBatchCountAquatic;
    int enemySubCount;
    int flyingTaraCount;
    int bradleyCount;
    int powPrisonerPlains;
    int powPrisonerOther;

public:
    Level3();
    ~Level3();

    void generateBiomes() override;
    void spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) override;
    void update(float dt) override;
    void render(RenderWindow& window, float camX, float camY) override;

    int getRebelBatchCount();
    int getMummyBatchCount();
    int getZombieCount();
    int getEnemySubCount();
    int getFlyingTaraCount();
    int getBradleyCount();
    int getPowPrisonerPlains();
    int getPowPrisonerOther();
};

// Infinite terrain
// Infinite terrain
class CampaignLevel {
private:
    bool isInfinite;
    int maxGeneratedWidth;
    int currentGeneratedWidth;
    int generationChunkSize;

    int* enemiesKilledPerType;
    int* vehiclesDestroyedPerType;
    int killQuotaPerType;
    int vehicleDestroyQuota;

    bool isKillQuotaReached;
    bool isDynamicSpawning;
    float spawnRadius;

    float fusionCooldownTimer;
    float fusionCooldownDuration;

    int currentChunkX;
    bool generatedLeft;
    bool generatedRight;

    PerlinNoise* perlin;
    NoiseProfile* profile;


    Texture solidTex;
    Texture waterTex;


    Biome** activeBiomes;
    int biomeCount;
    int maxBiomes;

    // -----------------------------------------------------------
    // Spawning track karne ke liye — ek jagah dobara spawn na ho
    // -----------------------------------------------------------
    struct SpawnRecord {
        float x;
        bool  used;
    };

    static const int MAX_SPAWN_RECORDS = 2048;
    SpawnRecord spawnRecords[MAX_SPAWN_RECORDS];
    int         spawnRecordCount;

    // player kitna aage gaya tab se last spawn hua
    float lastSpawnCheckX;

public:
    CampaignLevel(int profileChoice);
    ~CampaignLevel();

    bool checkCollision(float px, float py, float pw, float ph);
    void resolveCollisions(float& px, float& py, float pw, float ph,
        float& velX, float& velY, bool& onGround);
    void generateChunk(int startCol);
    void render(RenderWindow& window, float camX, float camY);
    void update(float playerX);
    bool checkWaterAt(float px, float py, float pw, float ph);

    bool hasSpawnedNear(float x);
    void recordSpawn(float x);
    void spawnWave(float playerX, EnemyManager& enemyManager, PlayerSoldier* player);
    void recordEnemyKill(const char* name);
    void checkKillQuota();
    bool getKillQuotaReached();
    int getKillCount(int typeIdx);
};

//i will made them later but just forward declare them here to use in boss level my goalfirst al levels then other things


class LevelManager {
private:
    Level* levels[10];
    int currentLevelIndex;
    int totalLevels;

public:
    LevelManager();
    ~LevelManager();

    void addLevel(Level* level);
    Level* getCurrentLevel();
    Level* getLevel(int index);
    void setCurrentLevel(int index);
    int getCurrentLevelIndex();
    int getTotalLevels();
    void nextLevel();
    void previousLevel();
    void setCurrentLevelByPointer(Level* level);
    void spawnCurrentLevelEnemies(EnemyManager& enemyManager, PlayerSoldier* player);
    void loadAllLevels();
    void switchToLevel(int index);
    void update(float dt);
};


//class EnemyManager {
//private:
//    Enemy* enemies[50];
//    int enemyCount;
//    int maxEnemies;
//    BulletManager* bulletMgr;
//
//public:
//    EnemyManager() {
//        enemyCount = 0;
//        maxEnemies = 50;
//        bulletMgr = nullptr;
//        for (int i = 0; i < maxEnemies; i++) {
//            enemies[i] = nullptr;
//        }
//    }
//
//    void setBulletManager(BulletManager* bm) {
//        bulletMgr = bm;
//    }
//
//    ~EnemyManager() {
//        clearAll();
//    }
//
//    void clearAll() {
//        for (int i = 0; i < maxEnemies; i++) {
//            delete enemies[i];
//            enemies[i] = nullptr;
//        }
//        enemyCount = 0;
//    }
//
//    void addEnemy(Enemy* enemy) {
//        if (enemyCount < maxEnemies) {
//            enemies[enemyCount] = enemy;
//            enemyCount++;
//        }
//    }
//
//    Enemy* getEnemyAt(int index) {
//        if (index >= 0 && index < enemyCount) {
//            return enemies[index];
//        }
//        return nullptr;
//    }
//
//    void updateAll(float dt, PlayerSoldier* player) {
//        for (int i = 0; i < enemyCount; i++) {
//            if (!enemies[i]) {
//                continue;
//            }
//            if (!enemies[i]->getIsAlive()) {
//                continue;
//            }
//
//            enemies[i]->setPlayer(player);
//            enemies[i]->setBulletManager(bulletMgr);
//            enemies[i]->update(dt);
//        }
//
//        // Enemy-to-enemy separation
//        for (int i = 0; i < enemyCount; i++) {
//            if (!enemies[i] || !enemies[i]->getIsAlive()) continue;
//            for (int j = i + 1; j < enemyCount; j++) {
//                if (!enemies[j] || !enemies[j]->getIsAlive()) continue;
//                enemies[i]->checkEnemyCollision(enemies[j]);
//                enemies[j]->checkEnemyCollision(enemies[i]);
//            }
//        }
//    }
//
//    void renderAll(RenderWindow& window, float camX, float camY) {
//        for (int i = 0; i < enemyCount; i++) {
//            if (enemies[i]) {
//                enemies[i]->render(window, camX, camY);
//            }
//        }
//    }
//
//    int getEnemyCount() const {
//        return enemyCount;
//    }
//};
