#pragma once

#include "Block.h"
#include "Biome.h"
#include "Enemy.h"
#include "PlayerSoldier.h"

using namespace sf;
using namespace std;

class Level {
protected:

    string name;
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

    // Constructor




    float getPlayerSpawnX() { return playerSpawnX; }
    float getPlayerSpawnY() { return playerSpawnY; }

    void playerReachedEnd() { isComplete = true; }

    virtual void spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) {
        // Default empty implementation - derived classes can override
    }





    Level(string name, int levelNumber, int biomeWidth, int biomeHeight) {
        this->name = name;
        this->levelNumber = levelNumber;
        this->biomeWidth = biomeWidth;
        this->biomeHeight = biomeHeight;
        this->blockSize = 64.0f;
        this->isComplete = false;
        this->isLoaded = false;

        playerSpawnX = 0;
        playerSpawnY = 0;
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


    virtual ~Level() {
        delete plains;
        delete aerial;
        delete aquatic;
        plains = nullptr;
        aerial = nullptr;
        aquatic = nullptr;
    }


    virtual void generateBiomes() = 0;
    virtual bool checkLevelComplete() = 0;

    // Common Functions

    virtual void loadTextures(string solidPath, string waterPath, string grassPath, string dirtPath) {
        if (plains) {
            plains->loadTextures(solidPath, waterPath, grassPath, dirtPath);
        }
        if (aerial) {
            aerial->loadTextures(solidPath, waterPath, grassPath, dirtPath);
        }
        if (aquatic) {
            aquatic->loadTextures(solidPath, waterPath, grassPath, dirtPath);
        }
    }

    virtual void update(float dt) {
        if (plains) {
            plains->update(dt);
        }
        if (aerial) {
            aerial->update(dt);
        }
        if (aquatic) {
            aquatic->update(dt);
        }
    }

    virtual void render(RenderWindow& window, float camX, float camY) {
        if (plains) {
            plains->render(window, camX, camY);
        }
        if (aerial) {
            aerial->render(window, camX, camY);
        }
        if (aquatic) {
            aquatic->render(window, camX, camY);
        }
    }

    virtual void resolveCollisions(float& px, float& py, float pw, float ph, float& velX, float& velY, bool& onGround) {
        if (plains) {
            plains->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
        }
        if (aerial) {
            aerial->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
        }
        if (aquatic) {
            aquatic->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
        }
    }
    bool checkCollision(float px, float py, float pw, float ph) {
        if (plains && plains->checkCollision(px, py, pw, ph))
            return true;

        if (aerial && aerial->checkCollision(px, py, pw, ph))
            return true;

        if (aquatic && aquatic->checkCollision(px, py, pw, ph))
            return true;

        return false;
    }






    string getName() { return name; }
    int    getLevelNum() { return levelNumber; }
    float  getLevelEnd() { return levelEnd; }
    float  getLevelStart() { return levelStart; }
    float  getPlainsEnd() { return plainsEnd; }
    float  getAerialEnd() { return aerialEnd; }
    float  getAquaticEnd() { return aquaticEnd; }
    bool   getIsComplete() { return isComplete; }
    bool   getIsLoaded() { return isLoaded; }



    bool checkWaterAt(float px, float py, float pw, float ph) {
        if (plains && plains->checkWaterCollision(px, py, pw, ph)) {
            return true;
        }
        if (aerial && aerial->checkWaterCollision(px, py, pw, ph)) {
            return true;
        }
        if (aquatic && aquatic->checkWaterCollision(px, py, pw, ph)) {
            return true;
        }
        return false;
    }
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

    SurvivalLevel(string name, int levelNumber, int biomeWidth, int biomeHeight)
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



        // Default multiplier
        scoreMultiplier = 1.0f;
    }

    virtual ~SurvivalLevel() {}

    virtual void generateBiomes() = 0;

    bool checkLevelComplete() override {
        if (enemiesKilled >= totalEnemies && totalEnemies > 0) {
            isComplete = true;
        }
        return isComplete;
    }
    void playerReachedEnd() {
        isComplete = true;
    }

    void enemyKilled() {
        enemiesKilled++;
        checkLevelComplete();
    }


    float getScoreMultiplier() { return scoreMultiplier; }
    int   getEnemiesKilled() { return enemiesKilled; }
    int   getTotalEnemies() { return totalEnemies; }
    bool  isHorizontalScroll() { return hasHorizontalScroll; }
    bool  isVerticalScroll() { return hasVerticalScroll; }
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


    Level1() : SurvivalLevel("Level 1", 1, 83, 15)
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
            shieldedBatchCount * 1 +   // shielded batch = 1TO 2
            bazookaBatchCount * 1 +   // bazooka batch  = 1 TO2
            grenadierBatchCount * 1 +   // grenade batch  = 1TO 2
            paratrooperBatchCount * 2 + // paratroopers
            martianBatchCount * 1 +   // martians
            mummyBatchCount * 1 +   // mummies
            zombieBatchCountAquatic * 4 + flyingTaraCount * 2 +
            enemySubCount * 1;// zombies batch = 3 to 5

        // Level 1 score multiplier   normal
        scoreMultiplier = 1.0f;

        // player will spawn at top of screen to guarantee spawn above terrain
        playerSpawnX = 400;
        playerSpawnY = 0;
    }

    ~Level1() {}


    void generateBiomes() override {
        // Plains biome
        plains = new PlainsBiome(levelStart, plainsEnd);

        // Aerial biome after plains
        aerial = new AerialBiome(plainsEnd, aerialEnd);

        // Aquatic biome after aerial
        aquatic = new AquaticBiome(aerialEnd, aquaticEnd);

        // Textures load k
        loadTextures("Sprites/blocks/stone.png",
            "Sprites/blocks/water.png",
            "Sprites/blocks/grass.png",
            "Sprites/blocks/dirt.png");

        // Terrain generate 
        plains->generateTerrain(biomeWidth, biomeHeight);
        aerial->generateTerrain(biomeWidth, biomeHeight);
        aquatic->generateTerrain(biomeWidth, biomeHeight);

        isLoaded = true;
    }

    void spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) override {
        // Spawn enemies closer together near the player start position
        float levelWidth = levelEnd - levelStart;
        float spacing = levelWidth / totalEnemies;
        float startX = levelStart + 500.0f;
        float startY = 300.0f; // Spawn from height to fall onto terrain
        int enemyIndex = 0;
        
        for (int i = 0; i < rebelBatchCount; i++) {
            for (int j = 0; j < 3; j++) {
                Enemy* rebel = new RebelSoldier();
                float spawnX = startX + enemyIndex * spacing;
                rebel->setPosition(spawnX, startY);
                rebel->setPlayer(player);
                rebel->setPatrolRange(100.f);
                enemyManager.addEnemy(rebel);
                enemyIndex++;
            }
        }
        
        for (int i = 0; i < shieldedBatchCount; i++) {
            Enemy* shielded = new ShieldedSoldier();
            float spawnX = startX + enemyIndex * spacing;
            shielded->setPosition(spawnX, startY);
            shielded->setPlayer(player);
            shielded->setPatrolRange(100.f);
            enemyManager.addEnemy(shielded);
            enemyIndex++;
        }
        
        for (int i = 0; i < bazookaBatchCount; i++) {
            Enemy* bazooka = new BazookaSoldier();
            float spawnX = startX + enemyIndex * spacing;
            bazooka->setPosition(spawnX, startY);
            bazooka->setPlayer(player);
            bazooka->setPatrolRange(100.f);
            enemyManager.addEnemy(bazooka);
            enemyIndex++;
        }
        
        for (int i = 0; i < grenadierBatchCount; i++) {
            Enemy* grenadier = new GrenadeSoldier();
            float spawnX = startX + enemyIndex * spacing;
            grenadier->setPosition(spawnX, startY);
            grenadier->setPlayer(player);
            grenadier->setPatrolRange(100.f);
            enemyManager.addEnemy(grenadier);
            enemyIndex++;
        }
        
        for (int i = 0; i < paratrooperBatchCount; i++) {
            for (int j = 0; j < 2; j++) {
                Enemy* paratrooper = new Paratrooper();
                float spawnX = startX + enemyIndex * spacing;
                paratrooper->setPosition(spawnX, startY);
                paratrooper->setPlayer(player);
                paratrooper->setPatrolRange(100.f);
                enemyManager.addEnemy(paratrooper);
                enemyIndex++;
            }
        }
        
        for (int i = 0; i < martianBatchCount; i++) {
            Enemy* martian = new Martian();
            float spawnX = startX + enemyIndex * spacing;
            martian->setPosition(spawnX, startY);
            martian->setPlayer(player);
            martian->setPatrolRange(100.f);
            enemyManager.addEnemy(martian);
            enemyIndex++;
        }
        
        for (int i = 0; i < mummyBatchCount; i++) {
            Enemy* mummy = new MummyWarrior();
            float spawnX = startX + enemyIndex * spacing;
            mummy->setPosition(spawnX, startY);
            mummy->setPlayer(player);
            mummy->setPatrolRange(100.f);
            enemyManager.addEnemy(mummy);
            enemyIndex++;
        }
        
        for (int i = 0; i < zombieBatchCountAquatic; i++) {
            for (int j = 0; j < 4; j++) {
                Enemy* zombie = new Zombie();
                float spawnX = startX + enemyIndex * spacing;
                zombie->setPosition(spawnX, startY);
                zombie->setPlayer(player);
                zombie->setPatrolRange(100.f);
                enemyManager.addEnemy(zombie);
                enemyIndex++;
            }
        }
        
        // FlyingTara - commented out, class doesn't exist
        /*
        for (int i = 0; i < flyingTaraCount; i++) {
            for (int j = 0; j < 2; j++) {
                Enemy* tara = new FlyingTara();
                float spawnX = levelStart + (rebelBatchCount * 3 + shieldedBatchCount + bazookaBatchCount + grenadierBatchCount + paratrooperBatchCount * 2 + martianBatchCount + mummyBatchCount + zombieBatchCountAquatic * 4 + i * 2 + j) * spacing;
                tara->setPosition(spawnX, 200.f);
                tara->setPlayer(player);
                tara->setGroundY(500.f);
                tara->setPatrolRange(100.f);
                enemyManager.addEnemy(tara);
            }
        }
        */
        
        // EnemySub - commented out, class doesn't exist
        /*
        for (int i = 0; i < enemySubCount; i++) {
            Enemy* sub = new EnemySub();
            float spawnX = levelStart + (rebelBatchCount * 3 + shieldedBatchCount + bazookaBatchCount + grenadierBatchCount + paratrooperBatchCount * 2 + martianBatchCount + mummyBatchCount + zombieBatchCountAquatic * 4 + flyingTaraCount * 2 + i) * spacing;
            sub->setPosition(spawnX, 200.f);
            sub->setPlayer(player);
            sub->setGroundY(500.f);
            sub->setPatrolRange(100.f);
            enemyManager.addEnemy(sub);
        }
        */
    }


    void update(float dt) override {
        //base class called
        Level::update(dt);

        // Level 1 specific update

        checkLevelComplete();
    }


    void render(RenderWindow& window, float camX, float camY) override {

        Level::render(window, camX, camY);

    }


    int getRebelBatchCount() { return rebelBatchCount; }
    int getMummyBatchCount() { return mummyBatchCount; }
    int getZombieCount() { return zombieBatchCountAquatic; }
    int getEnemySubCount() { return enemySubCount; }
    int getFlyingTaraCount() { return flyingTaraCount; }
    int getPowCount() { return powCount; }
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
    Level2() : SurvivalLevel("Level 2", 2, 83, 14)//250/3
    {
        rebelBatchCount = 3;
        shieldedBatchCount = 3;
        bazookaBatchCount = 3;
        grenadierBatchCount = 3;

        paratrooperBatchCount = 2;
        martianBatchCount = 2;

        mummyBatchCount = 3;

        zombieBatchCountAquatic = 3;
        enemySubCount = 1;

        flyingTaraCount = 3;
        bradleyCount = 2;

        powPrisonerPlains = 2;
        powPrisonerOther = 1;

        totalEnemies =
            rebelBatchCount * 3 +
            shieldedBatchCount * 1 +
            bazookaBatchCount * 1 +
            grenadierBatchCount * 1 +
            paratrooperBatchCount * 2 +
            martianBatchCount * 1 +
            mummyBatchCount * 1 +
            zombieBatchCountAquatic * 4 + flyingTaraCount * 2 +
            enemySubCount * 1;

        scoreMultiplier = 1.5f;

        playerSpawnX = 400;
        playerSpawnY = 0;
    }

    ~Level2() {}

    void generateBiomes() override {
        //IN LEVEL 2 FIRST AERIAL THEN AQUATIC THEN PLAINS
        aerial = new AerialBiome(levelStart, plainsEnd);

        aquatic = new AquaticBiome(plainsEnd, aerialEnd);

        plains = new PlainsBiome(aerialEnd, aquaticEnd);
        loadTextures("Sprites/blocks/stone.png",
            "Sprites/blocks/water.png",
            "Sprites/blocks/grass.png",
            "Sprites/blocks/dirt.png");

        // Level 2 HAS MORE HILLS
        aerial->generateTerrain(biomeWidth, biomeHeight);
        aquatic->generateTerrain(biomeWidth, biomeHeight);
        plains->generateTerrain(biomeWidth, biomeHeight);


        isLoaded = true;
    }

    void spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) override {
        // Spawn enemies closer together near the player start position
        float levelWidth = levelEnd - levelStart;
        float spacing = levelWidth / totalEnemies;
        float startX = levelStart + 500.0f;
        float startY = 300.0f; // Spawn from height to fall onto terrain
        int enemyIndex = 0;
        
        for (int i = 0; i < rebelBatchCount; i++) {
            for (int j = 0; j < 3; j++) {
                Enemy* rebel = new RebelSoldier();
                float spawnX = startX + enemyIndex * spacing;
                rebel->setPosition(spawnX, startY);
                rebel->setPlayer(player);
                rebel->setPatrolRange(100.f);
                enemyManager.addEnemy(rebel);
                enemyIndex++;
            }
        }
        
        for (int i = 0; i < shieldedBatchCount; i++) {
            Enemy* shielded = new ShieldedSoldier();
            float spawnX = startX + enemyIndex * spacing;
            shielded->setPosition(spawnX, startY);
            shielded->setPlayer(player);
            shielded->setPatrolRange(100.f);
            enemyManager.addEnemy(shielded);
            enemyIndex++;
        }
        
        for (int i = 0; i < bazookaBatchCount; i++) {
            Enemy* bazooka = new BazookaSoldier();
            float spawnX = startX + enemyIndex * spacing;
            bazooka->setPosition(spawnX, startY);
            bazooka->setPlayer(player);
            bazooka->setPatrolRange(100.f);
            enemyManager.addEnemy(bazooka);
            enemyIndex++;
        }
        
        for (int i = 0; i < grenadierBatchCount; i++) {
            Enemy* grenadier = new GrenadeSoldier();
            float spawnX = startX + enemyIndex * spacing;
            grenadier->setPosition(spawnX, startY);
            grenadier->setPlayer(player);
            grenadier->setPatrolRange(100.f);
            enemyManager.addEnemy(grenadier);
            enemyIndex++;
        }
        
        for (int i = 0; i < paratrooperBatchCount; i++) {
            for (int j = 0; j < 2; j++) {
                Enemy* paratrooper = new Paratrooper();
                float spawnX = startX + enemyIndex * spacing;
                paratrooper->setPosition(spawnX, startY);
                paratrooper->setPlayer(player);
                paratrooper->setPatrolRange(100.f);
                enemyManager.addEnemy(paratrooper);
                enemyIndex++;
            }
        }
        
        for (int i = 0; i < martianBatchCount; i++) {
            Enemy* martian = new Martian();
            float spawnX = startX + enemyIndex * spacing;
            martian->setPosition(spawnX, startY);
            martian->setPlayer(player);
            martian->setPatrolRange(100.f);
            enemyManager.addEnemy(martian);
            enemyIndex++;
        }
        
        for (int i = 0; i < mummyBatchCount; i++) {
            Enemy* mummy = new MummyWarrior();
            float spawnX = startX + enemyIndex * spacing;
            mummy->setPosition(spawnX, startY);
            mummy->setPlayer(player);
            mummy->setPatrolRange(100.f);
            enemyManager.addEnemy(mummy);
            enemyIndex++;
        }
        
        for (int i = 0; i < zombieBatchCountAquatic; i++) {
            for (int j = 0; j < 4; j++) {
                Enemy* zombie = new Zombie();
                float spawnX = startX + enemyIndex * spacing;
                zombie->setPosition(spawnX, startY);
                zombie->setPlayer(player);
                zombie->setPatrolRange(100.f);
                enemyManager.addEnemy(zombie);
                enemyIndex++;
            }
        }
    }

    void update(float dt) override {
        Level::update(dt);
        checkLevelComplete();
    }

    void render(RenderWindow& window, float camX, float camY) override {
        if (aquatic) {
            aquatic->render(window, camX, camY);
        }
        if (aerial) {
            aerial->render(window, camX, camY);
        }
        if (plains) {
            plains->render(window, camX, camY);
        }
    }

    // Getters
    int getRebelBatchCount() { return rebelBatchCount; }
    int getMummyBatchCount() { return mummyBatchCount; }
    int getZombieCount() { return zombieBatchCountAquatic; }
    int getEnemySubCount() { return enemySubCount; }
    int getFlyingTaraCount() { return flyingTaraCount; }
    int getBradleyCount() { return bradleyCount; }
    int getPowPrisonerPlains() { return powPrisonerPlains; }
    int getPowPrisonerOther() { return powPrisonerOther; }
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
    Level3() : SurvivalLevel("Level 3", 3, 83, 16)
    {
        rebelBatchCount = 4;
        shieldedBatchCount = 4;
        bazookaBatchCount = 4;
        grenadierBatchCount = 4;

        paratrooperBatchCount = 3;
        martianBatchCount = 3;

        mummyBatchCount = 4;

        zombieBatchCountAquatic = 4;
        enemySubCount = 2;

        flyingTaraCount = 4;
        bradleyCount = 3;

        powPrisonerPlains = 3;
        powPrisonerOther = 2;

        totalEnemies =
            rebelBatchCount * 3 +
            shieldedBatchCount * 1 +
            bazookaBatchCount * 1 +
            grenadierBatchCount * 1 +
            paratrooperBatchCount * 2 +
            martianBatchCount * 1 +
            mummyBatchCount * 1 + flyingTaraCount * 2 +
            zombieBatchCountAquatic * 4 + enemySubCount * 1 +
            bradleyCount * 1;

        scoreMultiplier = 2.0f;

        playerSpawnX = 400;
        playerSpawnY = 0;
    }

    ~Level3() {}

    void generateBiomes() override {
        //IN LEVEL 3 FIRST AQUATIC THEN AERIAL THEN PLAINS
        aquatic = new AquaticBiome(levelStart, plainsEnd);

        aerial = new AerialBiome(plainsEnd, aerialEnd);

        plains = new PlainsBiome(aerialEnd, aquaticEnd);
        loadTextures("Sprites/blocks/stone.png",
            "Sprites/blocks/water.png",
            "Sprites/blocks/grass.png",
            "Sprites/blocks/dirt.png");

        aquatic->generateTerrain(biomeWidth, biomeHeight);
        aerial->generateTerrain(biomeWidth, biomeHeight);
        plains->generateTerrain(biomeWidth, biomeHeight);

        isLoaded = true;
    }

    void spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) override {
        // Spawn enemies closer together near the player start position
        float levelWidth = levelEnd - levelStart;
        float spacing = levelWidth / totalEnemies;
        float startX = levelStart + 500.0f;
        float startY = 300.0f; // Spawn from height to fall onto terrain
        int enemyIndex = 0;
        
        for (int i = 0; i < rebelBatchCount; i++) {
            for (int j = 0; j < 3; j++) {
                Enemy* rebel = new RebelSoldier();
                float spawnX = startX + enemyIndex * spacing;
                rebel->setPosition(spawnX, startY);
                rebel->setPlayer(player);
                rebel->setPatrolRange(100.f);
                enemyManager.addEnemy(rebel);
                enemyIndex++;
            }
        }
        
        for (int i = 0; i < shieldedBatchCount; i++) {
            Enemy* shielded = new ShieldedSoldier();
            float spawnX = startX + enemyIndex * spacing;
            shielded->setPosition(spawnX, startY);
            shielded->setPlayer(player);
            shielded->setPatrolRange(100.f);
            enemyManager.addEnemy(shielded);
            enemyIndex++;
        }
        
        for (int i = 0; i < bazookaBatchCount; i++) {
            Enemy* bazooka = new BazookaSoldier();
            float spawnX = startX + enemyIndex * spacing;
            bazooka->setPosition(spawnX, startY);
            bazooka->setPlayer(player);
            bazooka->setPatrolRange(100.f);
            enemyManager.addEnemy(bazooka);
            enemyIndex++;
        }
        
        for (int i = 0; i < grenadierBatchCount; i++) {
            Enemy* grenadier = new GrenadeSoldier();
            float spawnX = startX + enemyIndex * spacing;
            grenadier->setPosition(spawnX, startY);
            grenadier->setPlayer(player);
            grenadier->setPatrolRange(100.f);
            enemyManager.addEnemy(grenadier);
            enemyIndex++;
        }
        
        for (int i = 0; i < paratrooperBatchCount; i++) {
            for (int j = 0; j < 2; j++) {
                Enemy* paratrooper = new Paratrooper();
                float spawnX = startX + enemyIndex * spacing;
                paratrooper->setPosition(spawnX, startY);
                paratrooper->setPlayer(player);
                paratrooper->setPatrolRange(100.f);
                enemyManager.addEnemy(paratrooper);
                enemyIndex++;
            }
        }
        
        for (int i = 0; i < martianBatchCount; i++) {
            Enemy* martian = new Martian();
            float spawnX = startX + enemyIndex * spacing;
            martian->setPosition(spawnX, startY);
            martian->setPlayer(player);
            martian->setPatrolRange(100.f);
            enemyManager.addEnemy(martian);
            enemyIndex++;
        }
        
        for (int i = 0; i < mummyBatchCount; i++) {
            Enemy* mummy = new MummyWarrior();
            float spawnX = startX + enemyIndex * spacing;
            mummy->setPosition(spawnX, startY);
            mummy->setPlayer(player);
            mummy->setPatrolRange(100.f);
            enemyManager.addEnemy(mummy);
            enemyIndex++;
        }
        
        for (int i = 0; i < zombieBatchCountAquatic; i++) {
            for (int j = 0; j < 4; j++) {
                Enemy* zombie = new Zombie();
                float spawnX = startX + enemyIndex * spacing;
                zombie->setPosition(spawnX, startY);
                zombie->setPlayer(player);
                zombie->setPatrolRange(100.f);
                enemyManager.addEnemy(zombie);
                enemyIndex++;
            }
        }
    }

    void update(float dt) override {
        Level::update(dt);
        checkLevelComplete();
    }

    void render(RenderWindow& window, float camX, float camY) override {
        if (aquatic) {
            aquatic->render(window, camX, camY);
        }
        if (aerial) {
            aerial->render(window, camX, camY);
        }
        if (plains) {
            plains->render(window, camX, camY);
        }
    }

    // Getters
    int getRebelBatchCount() { return rebelBatchCount; }
    int getMummyBatchCount() { return mummyBatchCount; }
    int getZombieCount() { return zombieBatchCountAquatic; }
    int getEnemySubCount() { return enemySubCount; }
    int getFlyingTaraCount() { return flyingTaraCount; }
    int getBradleyCount() { return bradleyCount; }
    int getPowPrisonerPlains() { return powPrisonerPlains; }
    int getPowPrisonerOther() { return powPrisonerOther; }
};

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
public:




    CampaignLevel(int profileChoice) {

        // Basic settings
        isInfinite = true;
        generationChunkSize = 16;
        killQuotaPerType = 5;
        vehicleDestroyQuota = 3;
        fusionCooldownDuration = 180.0f;
        fusionCooldownTimer = 0.0f;
        spawnRadius = 500.0f;

        // Chunk tracking
        currentChunkX = 0;
        generatedLeft = false;
        generatedRight = false;
        currentGeneratedWidth = 0;
        maxGeneratedWidth = 1000000;

        // Quota tracking
        isKillQuotaReached = false;
        isDynamicSpawning = true;

        // Arrays - 8 enemy types, 3 vehicle types
        enemiesKilledPerType = new int[8];
        vehiclesDestroyedPerType = new int[3];

        for (int i = 0; i < 8; i++)
            enemiesKilledPerType[i] = 0;

        for (int i = 0; i < 3; i++)
            vehiclesDestroyedPerType[i] = 0;

        // Perlin setup
        NoiseProfileFactory factory;
        profile = factory.createProfile(profileChoice);
        perlin = new PerlinNoise(42);


        maxBiomes = 5000000;
        biomeCount = 0;
        activeBiomes = new Biome * [maxBiomes];

    }



    bool checkCollision(float px, float py, float pw, float ph) {
        for (int i = 0; i < biomeCount; i++)
            if (activeBiomes[i]->checkCollision(px, py, pw, ph))
                return true;
        return false;
    }

    void resolveCollisions(float& px, float& py, float pw, float ph,
        float& velX, float& velY, bool& onGround) {
        for (int i = 0; i < biomeCount; i++)
            activeBiomes[i]->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
    }

    // Chunk is 16 blocks wide - generates terrain from perlin noise


    void generateChunk(int startCol) {
        float scale = profile->getScale();
        int octaves = profile->getOctaves();

        // Decide biome from noise
        float noiseX = startCol * 0.03f;




        int biomeType = perlin->getBiome(noiseX, 0);

        float startX = startCol * 64.0f;
        float endX = (startCol + generationChunkSize) * 64.0f;

        // Polymorphism - create child class from Biome pointer
        Biome* b = nullptr;
        if (biomeType == BIOME_AERIAL) {
            b = new AerialBiome(startX, endX, true);
        }
        else if (biomeType == BIOME_AQUATIC) {
            b = new AquaticBiome(startX, endX, true);
        }
        else {
            b = new PlainsBiome(startX, endX, true);
        }

        // Load textures and generate terrain
        b->loadTextures("Sprites/blocks/stone.png", "Sprites/blocks/water.png", "Sprites/blocks/grass.png", "Sprites/blocks/dirt.png");
        b->generateTerrain(generationChunkSize, 20);

        if (biomeCount < maxBiomes) {
            activeBiomes[biomeCount++] = b;
        }
    }
    // Draw blocks stored in chunk array
    void render(RenderWindow& window, float camX, float camY) {
        for (int i = 0; i < biomeCount; i++)
            activeBiomes[i]->render(window, camX, camY);
    }


    void update(float playerX) {
        int playerChunk = (int)(playerX / (generationChunkSize * 64.0f));
        if (playerChunk + 5 > currentChunkX) {
            generateChunk(currentChunkX * generationChunkSize);
            currentChunkX++;
            generatedRight = true;
        }
        if (playerChunk - 2 < 0) {
            generateChunk((currentChunkX - 1) * generationChunkSize);
            generatedLeft = true;
        }
    }




    bool checkWaterAt(float px, float py, float pw, float ph) {
        for (int i = 0; i < biomeCount; i++) {
            if (activeBiomes[i]->checkWaterCollision(px, py, pw, ph)) {
                return true;
            }
        }
        return false;
    }
};

//i will made them later but just forward declare them here to use in boss level my goalfirst al levels then other things
class IronNokana;    // forward declaration
class HairbusterRiberts;
class SeaSatan;
class UltimateBoss;
class SupplyCrate;
class Enemy;
class SlugMariner;


//boss
class BossLevel : public SurvivalLevel {
private:
    // Phase tracking 4 phases
    int  currentPhase;      // 1,2,3,4
    bool isPhase1Complete;
    bool isPhase2Complete;
    bool isPhase3Complete;
    bool isPhase4Complete;

    // pointers for plymor
    IronNokana* groundBoss;
    HairbusterRiberts* aerialBoss;
    SeaSatan* aquaticBoss;
    UltimateBoss* ultimateBoss;

    // Minion system
    Enemy** bossMinions;
    int     minionCount;
    int     minionBatchSize;

    // Crate after batch kill
    SupplyCrate* crateAfterBatch;

    // Phase 4 have blended biome
    bool blendedBiomeActive;

    // Slug Mariner fr  phase 3
    SlugMariner* slugMarinerOnCoast;

public:
    // Constructor
    BossLevel();
    ~BossLevel();

    // ooverride to decide in chils class
    void generateBiomes()    override;
    void spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) override;
    bool checkLevelComplete()override;
    void update(float dt)    override;
    void render(RenderWindow& window,
        float camX, float camY) override;

    // Phase functions
    void startPhase1();
    void startPhase2();
    void startPhase3();
    void startPhase4();
    void checkPhaseComplete();

    // Boss functions  
    void retreatBoss();
    void fuseBosses();      //in 4 all fuses

    // Minion functions
    void spawnMinions();
    void checkMinionBatchKilled();
    void spawnCrateAfterBatch();

    // Blended biome
    void activateBlendedBiome();
};

class LevelManager {
private:
    Level* levels[10];
    int currentLevelIndex;
    int totalLevels;

public:
    LevelManager() {
        currentLevelIndex = 0;
        totalLevels = 0;
        for (int i = 0; i < 10; i++) {
            levels[i] = nullptr;
        }
    }

    ~LevelManager() {
        // Don't delete levels as they might be owned by other classes like SurvivalGame and  CampaignGame
        for (int i = 0; i < totalLevels; i++) {
            levels[i] = nullptr;
        }
    }

    void addLevel(Level* level) {
        if (totalLevels < 10) {
            levels[totalLevels] = level;
            totalLevels++;
        }
    }

    Level* getCurrentLevel() {
        if (currentLevelIndex >= 0 && currentLevelIndex < totalLevels) {
            return levels[currentLevelIndex];
        }
        return nullptr;
    }

    Level* getLevel(int index) {
        if (index >= 0 && index < totalLevels) {
            return levels[index];
        }
        return nullptr;
    }

    void setCurrentLevel(int index) {
        if (index >= 0 && index < totalLevels) {
            currentLevelIndex = index;
        }
    }

    int getCurrentLevelIndex() {
        return currentLevelIndex;
    }

    int getTotalLevels() {
        return totalLevels;
    }

    void nextLevel() {
        if (currentLevelIndex < totalLevels - 1) {
            currentLevelIndex++;
        }
    }

    void previousLevel() {
        if (currentLevelIndex > 0) {
            currentLevelIndex--;
        }
    }

    void spawnCurrentLevelEnemies(EnemyManager& enemyManager, PlayerSoldier* player) {
        Level* level = getCurrentLevel();
        if (level) {
            level->spawnEnemies(enemyManager, player);
        }
    }

    void loadAllLevels() {
        levels[0] = new Level1();
        levels[1] = new Level2();
        levels[2] = new Level3();
        totalLevels = 3;
    }

    void switchToLevel(int index) {
        if (index >= 0 && index < totalLevels) {
            currentLevelIndex = index;
        }
    }

    void update(float dt) {
        Level* level = getCurrentLevel();
        if (level) {
            level->update(dt);
        }
    }
};

