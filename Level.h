#pragma once

#include "Block.h"
#include "Biome.h"
#include "Enemy.h"


using namespace sf;
using namespace std;

// Forward declarations for enemy classes used in Level implementations
class EnemyManager;
class RebelSoldier;
class ShieldedSoldier;
class BazookaSoldier;
class GrenadeSoldier;
class MummyWarrior;
class Paratrooper;
class Martian;
class Zombie;
class FlyingTara;
class BossLevel;
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
        generateBiomes();
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

    //void spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) override {
    //    // Spawn enemies closer together near the player start position
    //    float levelWidth = levelEnd - levelStart;
    //    float spacing = levelWidth / totalEnemies;
    //    float startX = levelStart + 500.0f;
    //    float startY = 300.0f; // Spawn from height to fall onto terrain
    //    int enemyIndex = 0;
    //    
    //    for (int i = 0; i < rebelBatchCount; i++) {
    //        for (int j = 0; j < 3; j++) {
    //            Enemy* rebel = new RebelSoldier();
    //            float spawnX = startX + enemyIndex * spacing;
    //            rebel->setPosition(spawnX, startY);
    //            rebel->setPlayer(player);
    //            rebel->setPatrolRange(100.f);
    //            enemyManager.addEnemy(rebel);
    //            enemyIndex++;
    //        }
    //    }
    //    
    //    for (int i = 0; i < shieldedBatchCount; i++) {
    //        Enemy* shielded = new ShieldedSoldier();
    //        float spawnX = startX + enemyIndex * spacing;
    //        shielded->setPosition(spawnX, startY);
    //        shielded->setPlayer(player);
    //        shielded->setPatrolRange(100.f);
    //        enemyManager.addEnemy(shielded);
    //        enemyIndex++;
    //    }
    //    
    //    for (int i = 0; i < bazookaBatchCount; i++) {
    //        Enemy* bazooka = new BazookaSoldier();
    //        float spawnX = startX + enemyIndex * spacing;
    //        bazooka->setPosition(spawnX, startY);
    //        bazooka->setPlayer(player);
    //        bazooka->setPatrolRange(100.f);
    //        enemyManager.addEnemy(bazooka);
    //        enemyIndex++;
    //    }
    //    
    //    for (int i = 0; i < grenadierBatchCount; i++) {
    //        Enemy* grenadier = new GrenadeSoldier();
    //        float spawnX = startX + enemyIndex * spacing;
    //        grenadier->setPosition(spawnX, startY);
    //        grenadier->setPlayer(player);
    //        grenadier->setPatrolRange(100.f);
    //        enemyManager.addEnemy(grenadier);
    //        enemyIndex++;
    //    }
    //    
    //    for (int i = 0; i < paratrooperBatchCount; i++) {
    //        for (int j = 0; j < 2; j++) {
    //            Enemy* paratrooper = new Paratrooper();
    //            float spawnX = startX + enemyIndex * spacing;
    //            paratrooper->setPosition(spawnX, startY);
    //            paratrooper->setPlayer(player);
    //            paratrooper->setPatrolRange(100.f);
    //            enemyManager.addEnemy(paratrooper);
    //            enemyIndex++;
    //        }
    //    }
    //    
    //    for (int i = 0; i < martianBatchCount; i++) {
    //        Enemy* martian = new Martian();
    //        float spawnX = startX + enemyIndex * spacing;
    //        martian->setPosition(spawnX, startY);
    //        martian->setPlayer(player);
    //        martian->setPatrolRange(100.f);
    //        enemyManager.addEnemy(martian);
    //        enemyIndex++;
    //    }
    //    
    //    for (int i = 0; i < mummyBatchCount; i++) {
    //        Enemy* mummy = new MummyWarrior();
    //        float spawnX = startX + enemyIndex * spacing;
    //        mummy->setPosition(spawnX, startY);
    //        mummy->setPlayer(player);
    //        mummy->setPatrolRange(100.f);
    //        enemyManager.addEnemy(mummy);
    //        enemyIndex++;
    //    }
    //    
    //    for (int i = 0; i < zombieBatchCountAquatic; i++) {
    //        for (int j = 0; j < 4; j++) {
    //            Enemy* zombie = new Zombie();
    //            float spawnX = startX + enemyIndex * spacing;
    //            zombie->setPosition(spawnX, startY);
    //            zombie->setPlayer(player);
    //            zombie->setPatrolRange(100.f);
    //            enemyManager.addEnemy(zombie);
    //            enemyIndex++;
    //        }
    //    }
    //    
    //    // FlyingTara - commented out, class doesn't exist
    //    /*
    //    for (int i = 0; i < flyingTaraCount; i++) {
    //        for (int j = 0; j < 2; j++) {
    //            Enemy* tara = new FlyingTara();
    //            float spawnX = levelStart + (rebelBatchCount * 3 + shieldedBatchCount + bazookaBatchCount + grenadierBatchCount + paratrooperBatchCount * 2 + martianBatchCount + mummyBatchCount + zombieBatchCountAquatic * 4 + i * 2 + j) * spacing;
    //            tara->setPosition(spawnX, 200.f);
    //            tara->setPlayer(player);
    //            tara->setGroundY(500.f);
    //            tara->setPatrolRange(100.f);
    //            enemyManager.addEnemy(tara);
    //        }
    //    }
    //    */
    //    
    //    // EnemySub - commented out, class doesn't exist
    //    /*
    //    for (int i = 0; i < enemySubCount; i++) {
    //        Enemy* sub = new EnemySub();
    //        float spawnX = levelStart + (rebelBatchCount * 3 + shieldedBatchCount + bazookaBatchCount + grenadierBatchCount + paratrooperBatchCount * 2 + martianBatchCount + mummyBatchCount + zombieBatchCountAquatic * 4 + flyingTaraCount * 2 + i) * spacing;
    //        sub->setPosition(spawnX, 200.f);
    //        sub->setPlayer(player);
    //        sub->setGroundY(500.f);
    //        sub->setPatrolRange(100.f);
    //        enemyManager.addEnemy(sub);
    //    }
    //    */
    //}

    void spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) override {

        float spawnY = 200.0f;

        float plainsStart = levelStart;
        float plainsEnd_ = plainsEnd;
        float aerialStart = plainsEnd;
        float aerialEnd_ = aerialEnd;
        float aquaticStart = aerialEnd;
        float aquaticEnd_ = aquaticEnd;

        float plainsW = plainsEnd_ - plainsStart;
        float aerialW = aerialEnd_ - aerialStart;
        float aquaticW = aquaticEnd_ - aquaticStart;

        // ===== PLAINS — Infantry 2 batches =====
        for (int b = 0; b < 2; b++) {
            float base = plainsStart + (plainsW * 0.25f) + b * (plainsW * 0.45f);

            for (int j = 0; j < 3; j++) {
                RebelSoldier* r = new RebelSoldier();
                r->setPosition(base + j * 150, spawnY);
                r->setPlayer(player); r->setPatrolRange(100.f);
                enemyManager.addEnemy(r);
            }
            ShieldedSoldier* sh = new ShieldedSoldier();
            sh->setPosition(base + 500, spawnY);
            sh->setPlayer(player); sh->setPatrolRange(80.f);
            enemyManager.addEnemy(sh);

            BazookaSoldier* bz = new BazookaSoldier();
            bz->setPosition(base + 700, spawnY);
            bz->setPlayer(player); bz->setPatrolRange(80.f);
            enemyManager.addEnemy(bz);

            GrenadeSoldier* gr = new GrenadeSoldier();
            gr->setPosition(base + 900, spawnY);
            gr->setPlayer(player); gr->setPatrolRange(80.f);
            enemyManager.addEnemy(gr);
        }

        // ===== PLAINS — Undead 2 batches (plains only) =====
        for (int b = 0; b < 2; b++) {
            float base = plainsStart + (plainsW * 0.30f) + b * (plainsW * 0.40f);
            MummyWarrior* m = new MummyWarrior();
            m->setPosition(base, spawnY);
            m->setPlayer(player); m->setPatrolRange(60.f);
            enemyManager.addEnemy(m);
        }

        // ===== AERIAL — Infantry 2 batches =====
        for (int b = 0; b < 2; b++) {
            float base = aerialStart + (aerialW * 0.25f) + b * (aerialW * 0.45f);

            for (int j = 0; j < 3; j++) {
                RebelSoldier* r = new RebelSoldier();
                r->setPosition(base + j * 150, spawnY);
                r->setPlayer(player); r->setPatrolRange(100.f);
                enemyManager.addEnemy(r);
            }
            ShieldedSoldier* sh = new ShieldedSoldier();
            sh->setPosition(base + 500, spawnY);
            sh->setPlayer(player); sh->setPatrolRange(80.f);
            enemyManager.addEnemy(sh);

            BazookaSoldier* bz = new BazookaSoldier();
            bz->setPosition(base + 700, spawnY);
            bz->setPlayer(player); bz->setPatrolRange(80.f);
            enemyManager.addEnemy(bz);

            GrenadeSoldier* gr = new GrenadeSoldier();
            gr->setPosition(base + 900, spawnY);
            gr->setPlayer(player); gr->setPatrolRange(80.f);
            enemyManager.addEnemy(gr);
        }

        // ===== AERIAL — Paratroopers 1 batch =====
        for (int j = 0; j < 2; j++) {
            Paratrooper* p = new Paratrooper();
            p->setPosition(aerialStart + (aerialW * 0.35f) + j * 250, -150.f);
            p->setPlayer(player);
            enemyManager.addEnemy(p);
        }

        // ===== AERIAL — Martian 1 batch =====
        Martian* mar = new Martian();
        mar->setPosition(aerialStart + (aerialW * 0.65f), -50.f);
        mar->setPlayer(player);
        enemyManager.addEnemy(mar);

        // ===== AERIAL — Flying Tara batch 0 =====
        for (int j = 0; j < 2; j++) {
            FlyingTara* ft = new FlyingTara();
            ft->setPosition(aerialStart + (aerialW * 0.40f) + j * 300, 150.f);
            ft->setPlayer(player);
            enemyManager.addEnemy(ft);
        }

        // ===== AQUATIC — Infantry 2 batches =====
        for (int b = 0; b < 2; b++) {
            float base = aquaticStart + (aquaticW * 0.25f) + b * (aquaticW * 0.45f);

            for (int j = 0; j < 3; j++) {
                RebelSoldier* r = new RebelSoldier();
                r->setPosition(base + j * 150, spawnY);
                r->setPlayer(player); r->setPatrolRange(100.f);
                enemyManager.addEnemy(r);
            }
            ShieldedSoldier* sh = new ShieldedSoldier();
            sh->setPosition(base + 500, spawnY);
            sh->setPlayer(player); sh->setPatrolRange(80.f);
            enemyManager.addEnemy(sh);

            BazookaSoldier* bz = new BazookaSoldier();
            bz->setPosition(base + 700, spawnY);
            bz->setPlayer(player); bz->setPatrolRange(80.f);
            enemyManager.addEnemy(bz);

            GrenadeSoldier* gr = new GrenadeSoldier();
            gr->setPosition(base + 900, spawnY);
            gr->setPlayer(player); gr->setPatrolRange(80.f);
            enemyManager.addEnemy(gr);
        }

        // ===== AQUATIC — Zombies 2 batches =====
        for (int b = 0; b < 2; b++) {
            float base = aquaticStart + (aquaticW * 0.25f) + b * (aquaticW * 0.45f);
            for (int j = 0; j < 3; j++) {
                Zombie* z = new Zombie();
                z->setPosition(base + j * 150, spawnY);
                z->setPlayer(player); z->setPatrolRange(80.f);
                enemyManager.addEnemy(z);
            }
        }

        // ===== AQUATIC — Flying Tara batch 1 =====
        for (int j = 0; j < 2; j++) {
            FlyingTara* ft = new FlyingTara();
            ft->setPosition(aquaticStart + (aquaticW * 0.40f) + j * 300, 150.f);
            ft->setPlayer(player);
            enemyManager.addEnemy(ft);
        }
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
        generateBiomes();
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
        float spawnY = -200.0f;

        // IN LEVEL 2: order is Aerial -> Aquatic -> Plains
        float aerialStart = levelStart;
        float aerialEnd_ = plainsEnd;
        float aquaticStart = plainsEnd;
        float aquaticEnd_ = aerialEnd;
        float plainsStart = aerialEnd;
        float plainsEnd_ = aquaticEnd;

        float aerialW = aerialEnd_ - aerialStart;
        float aquaticW = aquaticEnd_ - aquaticStart;
        float plainsW = plainsEnd_ - plainsStart;

        // ===== AERIAL BIOME — Infantry 3 batches =====
        for (int b = 0; b < 3; b++) {
            float base = aerialStart + 300 + b * (aerialW / 3.5f);
            for (int j = 0; j < 3; j++) {
                RebelSoldier* r = new RebelSoldier();
                r->setPosition(base + j * 120, spawnY);
                r->setPlayer(player); r->setPatrolRange(100.f);
                enemyManager.addEnemy(r);
            }
            ShieldedSoldier* sh = new ShieldedSoldier();
            sh->setPosition(base + 400, spawnY);
            sh->setPlayer(player); sh->setPatrolRange(80.f);
            enemyManager.addEnemy(sh);

            BazookaSoldier* bz = new BazookaSoldier();
            bz->setPosition(base + 600, spawnY);
            bz->setPlayer(player); bz->setPatrolRange(80.f);
            enemyManager.addEnemy(bz);

            GrenadeSoldier* gr = new GrenadeSoldier();
            gr->setPosition(base + 800, spawnY);
            gr->setPlayer(player); gr->setPatrolRange(80.f);
            enemyManager.addEnemy(gr);
        }

        // ===== AERIAL BIOME — Paratroopers 2 batches =====
        for (int b = 0; b < 2; b++) {
            for (int j = 0; j < 2; j++) {
                Paratrooper* p = new Paratrooper();
                p->setPosition(aerialStart + 400 + b * (aerialW / 2.5f) + j * 200, -150.f);
                p->setPlayer(player);
                enemyManager.addEnemy(p);
            }
        }

        // ===== AERIAL BIOME — Martian 2 batches =====
        for (int b = 0; b < 2; b++) {
            Martian* mar = new Martian();
            mar->setPosition(aerialStart + 500 + b * (aerialW / 2.5f), 150.f);
            mar->setPlayer(player);
            enemyManager.addEnemy(mar);
        }

        // ===== AQUATIC BIOME — Infantry 3 batches =====
        for (int b = 0; b < 3; b++) {
            float base = aquaticStart + 300 + b * (aquaticW / 3.5f);
            for (int j = 0; j < 3; j++) {
                RebelSoldier* r = new RebelSoldier();
                r->setPosition(base + j * 120, spawnY);
                r->setPlayer(player); r->setPatrolRange(100.f);
                enemyManager.addEnemy(r);
            }
            ShieldedSoldier* sh = new ShieldedSoldier();
            sh->setPosition(base + 400, spawnY);
            sh->setPlayer(player); sh->setPatrolRange(80.f);
            enemyManager.addEnemy(sh);

            BazookaSoldier* bz = new BazookaSoldier();
            bz->setPosition(base + 600, spawnY);
            bz->setPlayer(player); bz->setPatrolRange(80.f);
            enemyManager.addEnemy(bz);

            GrenadeSoldier* gr = new GrenadeSoldier();
            gr->setPosition(base + 800, spawnY);
            gr->setPlayer(player); gr->setPatrolRange(80.f);
            enemyManager.addEnemy(gr);
        }

        // ===== AQUATIC BIOME — Zombies 3 batches =====
        for (int b = 0; b < 3; b++) {
            for (int j = 0; j < 3; j++) {
                Zombie* z = new Zombie();
                z->setPosition(aquaticStart + 200 + b * (aquaticW / 3.5f) + j * 150, spawnY);
                z->setPlayer(player); z->setPatrolRange(80.f);
                enemyManager.addEnemy(z);
            }
        }

        // ===== PLAINS BIOME — Infantry 3 batches =====
        for (int b = 0; b < 3; b++) {
            float base = plainsStart + 300 + b * (plainsW / 3.5f);
            for (int j = 0; j < 3; j++) {
                RebelSoldier* r = new RebelSoldier();
                r->setPosition(base + j * 120, spawnY);
                r->setPlayer(player); r->setPatrolRange(100.f);
                enemyManager.addEnemy(r);
            }
            ShieldedSoldier* sh = new ShieldedSoldier();
            sh->setPosition(base + 400, spawnY);
            sh->setPlayer(player); sh->setPatrolRange(80.f);
            enemyManager.addEnemy(sh);

            BazookaSoldier* bz = new BazookaSoldier();
            bz->setPosition(base + 600, spawnY);
            bz->setPlayer(player); bz->setPatrolRange(80.f);
            enemyManager.addEnemy(bz);

            GrenadeSoldier* gr = new GrenadeSoldier();
            gr->setPosition(base + 800, spawnY);
            gr->setPlayer(player); gr->setPatrolRange(80.f);
            enemyManager.addEnemy(gr);
        }

        // ===== PLAINS BIOME — Undead 3 batches =====
        for (int b = 0; b < 3; b++) {
            MummyWarrior* m = new MummyWarrior();
            m->setPosition(plainsStart + 500 + b * (plainsW / 3.5f), spawnY);
            m->setPlayer(player); m->setPatrolRange(60.f);
            enemyManager.addEnemy(m);
        }

        // ===== Flying Tara 3 batches spread across level =====
        for (int b = 0; b < 3; b++) {
            for (int j = 0; j < 2; j++) {
                FlyingTara* ft = new FlyingTara();
                float ftX = (b == 0) ? aerialStart + 500 + j * 300
                    : (b == 1) ? aquaticStart + 300 + j * 300
                    : plainsStart + 400 + j * 300;
                ft->setPosition(ftX, 150.f);
                ft->setPlayer(player);
                enemyManager.addEnemy(ft);
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
    Level3() : SurvivalLevel("Level 3", 3, 83, 15)
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
        generateBiomes();
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
        float spawnY = -200.0f;

        // IN LEVEL 3: order is Aquatic -> Aerial -> Plains
        float aquaticStart = levelStart;
        float aquaticEnd_ = plainsEnd;
        float aerialStart = plainsEnd;
        float aerialEnd_ = aerialEnd;
        float plainsStart = aerialEnd;
        float plainsEnd_ = aquaticEnd;

        float aquaticW = aquaticEnd_ - aquaticStart;
        float aerialW = aerialEnd_ - aerialStart;
        float plainsW = plainsEnd_ - plainsStart;

        // ===== AQUATIC BIOME — Infantry 3 batches =====
        for (int b = 0; b < 3; b++) {
            float base = aquaticStart + 300 + b * (aquaticW / 3.5f);
            for (int j = 0; j < 3; j++) {
                RebelSoldier* r = new RebelSoldier();
                r->setPosition(base + j * 120, spawnY);
                r->setPlayer(player); r->setPatrolRange(100.f);
                enemyManager.addEnemy(r);
            }
            ShieldedSoldier* sh = new ShieldedSoldier();
            sh->setPosition(base + 400, spawnY);
            sh->setPlayer(player); sh->setPatrolRange(80.f);
            enemyManager.addEnemy(sh);

            BazookaSoldier* bz = new BazookaSoldier();
            bz->setPosition(base + 600, spawnY);
            bz->setPlayer(player); bz->setPatrolRange(80.f);
            enemyManager.addEnemy(bz);

            GrenadeSoldier* gr = new GrenadeSoldier();
            gr->setPosition(base + 800, spawnY);
            gr->setPlayer(player); gr->setPatrolRange(80.f);
            enemyManager.addEnemy(gr);
        }

        // ===== AQUATIC BIOME — Zombies 4 batches =====
        for (int b = 0; b < 4; b++) {
            for (int j = 0; j < 3; j++) {
                Zombie* z = new Zombie();
                z->setPosition(aquaticStart + 200 + b * (aquaticW / 4.5f) + j * 150, spawnY);
                z->setPlayer(player); z->setPatrolRange(80.f);
                enemyManager.addEnemy(z);
            }
        }

        // ===== AERIAL BIOME — Infantry 3 batches =====
        for (int b = 0; b < 3; b++) {
            float base = aerialStart + 300 + b * (aerialW / 3.5f);
            for (int j = 0; j < 3; j++) {
                RebelSoldier* r = new RebelSoldier();
                r->setPosition(base + j * 120, spawnY);
                r->setPlayer(player); r->setPatrolRange(100.f);
                enemyManager.addEnemy(r);
            }
            ShieldedSoldier* sh = new ShieldedSoldier();
            sh->setPosition(base + 400, spawnY);
            sh->setPlayer(player); sh->setPatrolRange(80.f);
            enemyManager.addEnemy(sh);

            BazookaSoldier* bz = new BazookaSoldier();
            bz->setPosition(base + 600, spawnY);
            bz->setPlayer(player); bz->setPatrolRange(80.f);
            enemyManager.addEnemy(bz);

            GrenadeSoldier* gr = new GrenadeSoldier();
            gr->setPosition(base + 800, spawnY);
            gr->setPlayer(player); gr->setPatrolRange(80.f);
            enemyManager.addEnemy(gr);
        }

        // ===== AERIAL BIOME — Paratroopers 3 batches =====
        for (int b = 0; b < 3; b++) {
            for (int j = 0; j < 2; j++) {
                Paratrooper* p = new Paratrooper();
                p->setPosition(aerialStart + 400 + b * (aerialW / 3.5f) + j * 200, -150.f);
                p->setPlayer(player);
                enemyManager.addEnemy(p);
            }
        }

        // ===== AERIAL BIOME — Martian 3 batches =====
        for (int b = 0; b < 3; b++) {
            Martian* mar = new Martian();
            mar->setPosition(aerialStart + 500 + b * (aerialW / 3.5f), 150.f);
            mar->setPlayer(player);
            enemyManager.addEnemy(mar);
        }

        // ===== PLAINS BIOME — Infantry 3 batches =====
        for (int b = 0; b < 3; b++) {
            float base = plainsStart + 300 + b * (plainsW / 3.5f);
            for (int j = 0; j < 3; j++) {
                RebelSoldier* r = new RebelSoldier();
                r->setPosition(base + j * 120, spawnY);
                r->setPlayer(player); r->setPatrolRange(100.f);
                enemyManager.addEnemy(r);
            }
            ShieldedSoldier* sh = new ShieldedSoldier();
            sh->setPosition(base + 400, spawnY);
            sh->setPlayer(player); sh->setPatrolRange(80.f);
            enemyManager.addEnemy(sh);

            BazookaSoldier* bz = new BazookaSoldier();
            bz->setPosition(base + 600, spawnY);
            bz->setPlayer(player); bz->setPatrolRange(80.f);
            enemyManager.addEnemy(bz);

            GrenadeSoldier* gr = new GrenadeSoldier();
            gr->setPosition(base + 800, spawnY);
            gr->setPlayer(player); gr->setPatrolRange(80.f);
            enemyManager.addEnemy(gr);
        }

        // ===== PLAINS BIOME — Undead 4 batches =====
        for (int b = 0; b < 4; b++) {
            MummyWarrior* m = new MummyWarrior();
            m->setPosition(plainsStart + 500 + b * (plainsW / 4.5f), spawnY);
            m->setPlayer(player); m->setPatrolRange(60.f);
            enemyManager.addEnemy(m);
        }

        // ===== Flying Tara 4 batches spread across level =====
        for (int b = 0; b < 4; b++) {
            for (int j = 0; j < 2; j++) {
                FlyingTara* ft = new FlyingTara();
                float ftX = (b == 0) ? aquaticStart + 400 + j * 300
                    : (b == 1) ? aerialStart + 300 + j * 300
                    : (b == 2) ? plainsStart + 300 + j * 300
                    : aerialStart + 700 + j * 300;
                ft->setPosition(ftX, 150.f);
                ft->setPlayer(player);
                enemyManager.addEnemy(ft);
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

        // spawning system initialize karo
        spawnRecordCount = 0;
        lastSpawnCheckX = 0.f;
        for (int i = 0; i < MAX_SPAWN_RECORDS; i++) {
            spawnRecords[i].x = 0.f;
            spawnRecords[i].used = false;
        }
    }

    // -----------------------------------------------------------
    // Destructor — heap memory free karo, memory leak na ho
    // -----------------------------------------------------------
    ~CampaignLevel() {
        delete[] enemiesKilledPerType;
        delete[] vehiclesDestroyedPerType;

        for (int i = 0; i < biomeCount; i++) {
            delete activeBiomes[i];
            activeBiomes[i] = nullptr;
        }
        delete[] activeBiomes;

        delete perlin;
        delete profile;
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

    // ===========================================================
    //  ENEMY SPAWNING SYSTEM
    // ===========================================================

    // -----------------------------------------------------------
    // Check karo ke diye gaye X ke 300px range mein pehle se
    // koi spawn record toh nahi — taake overlap na ho
    // -----------------------------------------------------------
    bool hasSpawnedNear(float x) {
        for (int i = 0; i < spawnRecordCount; i++) {
            if (!spawnRecords[i].used) continue;
            float diff = spawnRecords[i].x - x;
            if (diff < 0.f) diff = -diff;
            if (diff < 300.f) return true;
        }
        return false;
    }

    // -----------------------------------------------------------
    // Spawn hui location note karo — next time duplicate na bane
    // -----------------------------------------------------------
    void recordSpawn(float x) {
        if (spawnRecordCount < MAX_SPAWN_RECORDS) {
            spawnRecords[spawnRecordCount].x = x;
            spawnRecords[spawnRecordCount].used = true;
            spawnRecordCount++;
        }
        // agar array bhar gaya toh purane records recycle karo
        // (yeh infinite world hai, player bahut aage ja sakta hai)
        else {
            // circular overwrite — pehle wala replace karo
            static int overwriteIdx = 0;
            spawnRecords[overwriteIdx].x = x;
            spawnRecords[overwriteIdx].used = true;
            overwriteIdx = (overwriteIdx + 1) % MAX_SPAWN_RECORDS;
        }
    }

    // -----------------------------------------------------------
    // Main spawning function — Game.h ke updateCampaign() se
    // call karo jab bhi player 400px aage badhe
    //
    // Usage (Game.h mein):
    //   if (pX - lastSpawnX > 400.f) {
    //       campaignLevel->spawnWave(pX, enemies, player);
    //       lastSpawnX = pX;
    //   }
    // -----------------------------------------------------------
    void spawnWave(float playerX, EnemyManager& enemyManager, PlayerSoldier* player) {

        // Player ke 700px aage spawn karo taake player ko surprise mile
        float baseX = playerX + 700.f;

        // Agar yahan pehle spawn ho chuka hai toh kuch nahi karo
        if (hasSpawnedNear(baseX)) return;

        // Is location ko record kar lo
        recordSpawn(baseX);

        // Perlin noise se pata karo yahan kaun sa biome hai
        float noiseX = (baseX / 64.f) * 0.03f;
        int   biome = perlin->getBiome(noiseX, 0);

        // Upar se fall karke terrain par land karenge
        float spawnY = -10.f;

        // Thodi variety ke liye position se ek simple seed banao
        int   seed = (int)(baseX / 100.f);

        // ---------------------------------------------------
        // Plains biome — infantry aur kabhi kabhi mummy bhi
        // ---------------------------------------------------
        if (biome == BIOME_PLAINS) {

            // Rebel soldiers har wave mein toh honge hi
            for (int j = 0; j < 2; j++) {
                RebelSoldier* r = new RebelSoldier();
                r->setPosition(baseX + j * 120.f, spawnY);
                r->setPlayer(player);
                r->setPatrolRange(120.f);
                enemyManager.addEnemy(r);
            }

            // Grenade soldier thoda aage
            GrenadeSoldier* g = new GrenadeSoldier();
            g->setPosition(baseX + 420.f, spawnY);
            g->setPlayer(player);
            g->setPatrolRange(80.f);
            enemyManager.addEnemy(g);

            // Har doosri wave mein shielded soldier
            if (seed % 3 == 0) {
                ShieldedSoldier* s = new ShieldedSoldier();
                s->setPosition(baseX + 600.f, spawnY);
                s->setPlayer(player);
                s->setPatrolRange(80.f);
                enemyManager.addEnemy(s);
            }

            // Har teesri wave mein mummy — plains only
            if (seed % 3 == 0) {
                MummyWarrior* m = new MummyWarrior();
                m->setPosition(baseX + 780.f, spawnY);
                m->setPlayer(player);
                m->setPatrolRange(60.f);
                enemyManager.addEnemy(m);
            }
        }

        // ---------------------------------------------------
        // Aerial biome — paratroopers, martians, flying tara
        // ---------------------------------------------------
        else if (biome == BIOME_AERIAL) {

            // Paratroopers upar se aate hain — negative Y
            for (int j = 0; j < 2; j++) {
                Paratrooper* p = new Paratrooper();
                p->setPosition(baseX + j * 250.f, -150.f);
                p->setPlayer(player);
                enemyManager.addEnemy(p);
            }

            // Martian har doosri wave mein
            if (seed % 3 == 0) {
                Martian* mar = new Martian();
                mar->setPosition(baseX + 150.f, -50.f);
                mar->setPlayer(player);
                enemyManager.addEnemy(mar);
            }

            // Ground pe bhi kuch rebels rakh do
            for (int j = 0; j < 2; j++) {
                RebelSoldier* r = new RebelSoldier();
                r->setPosition(baseX + 100.f + j * 150.f, spawnY);
                r->setPlayer(player);
                r->setPatrolRange(100.f);
                enemyManager.addEnemy(r);
            }

            // Flying Tara — aerial ka signature enemy
            FlyingTara* ft = new FlyingTara();
            ft->setPosition(baseX + 350.f, 150.f);
            ft->setPlayer(player);
            enemyManager.addEnemy(ft);
        }

        // ---------------------------------------------------
        // Aquatic biome — zombies aur bazooka wale
        // ---------------------------------------------------
        else {

            // Zombies batch mein aate hain
            for (int j = 0; j < 2; j++) {
                Zombie* z = new Zombie();
                z->setPosition(baseX + j * 150.f, spawnY);
                z->setPlayer(player);
                z->setPatrolRange(80.f);
                enemyManager.addEnemy(z);
            }

            // Bazooka soldier thoda door se fire karta hai
            BazookaSoldier* bz = new BazookaSoldier();
            bz->setPosition(baseX + 520.f, spawnY);
            bz->setPlayer(player);
            bz->setPatrolRange(80.f);
            enemyManager.addEnemy(bz);

            // Har doosri wave mein shielded bhi
            if (seed % 3 == 0) {
                ShieldedSoldier* sh = new ShieldedSoldier();
                sh->setPosition(baseX + 700.f, spawnY);
                sh->setPlayer(player);
                sh->setPatrolRange(80.f);
                enemyManager.addEnemy(sh);
            }
        }
    }

    // -----------------------------------------------------------
    // Kill count update karo jab koi enemy mare
    // checkBulletEnemyCollisions() se call hoga
    // -----------------------------------------------------------
    void recordEnemyKill(const string& name) {

        // String se enemy type ka index nikaalo
        int idx = -1;
        if (name == "Rebel Soldier")  idx = 0;
        else if (name == "Shielded")       idx = 1;
        else if (name == "Bazooka")        idx = 2;
        else if (name == "Grenade")        idx = 3;
        else if (name == "Paratrooper")    idx = 4;
        else if (name == "Mummy")          idx = 5;
        else if (name == "Zombie")         idx = 6;
        else if (name == "Martian")        idx = 7;

        if (idx >= 0) {
            enemiesKilledPerType[idx]++;

            // Check karo kya sab types ka quota pura hua
            checkKillQuota();
        }
    }

    // -----------------------------------------------------------
    // Har type ke liye 5 kills chahiye — tab campaign clear hoga
    // -----------------------------------------------------------
    void checkKillQuota() {
        for (int i = 0; i < 8; i++) {
            if (enemiesKilledPerType[i] < killQuotaPerType)
                return; // abhi bhi koi type incomplete hai
        }
        // Sab types complete — campaign clear!
        isKillQuotaReached = true;
    }

    bool getKillQuotaReached() { return isKillQuotaReached; }

    // Kill progress dekhne ke liye — debug ya UI ke liye useful
    int getKillCount(int typeIdx) {
        if (typeIdx >= 0 && typeIdx < 8)
            return enemiesKilledPerType[typeIdx];
        return 0;
    }
};

//i will made them later but just forward declare them here to use in boss level my goalfirst al levels then other things


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
    void setCurrentLevelByPointer(Level* level) {
        for (int i = 0; i < totalLevels; i++) {
            if (levels[i] == level) {
                currentLevelIndex = i;
                return;
            }
        }
    }
    void spawnCurrentLevelEnemies(EnemyManager& enemyManager, PlayerSoldier* player) {
        Level* level = getCurrentLevel();
        if (level) {
            level->spawnEnemies(enemyManager, player);
        }
    }

    void loadAllLevels();
      

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
