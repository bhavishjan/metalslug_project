#pragma once

#include "block.h"
#include "block.h"
using namespace sf;
using namespace std;

class Level {
protected:
  
    string name;
    int    levelNumber;

    //siz
    int   biomeWidth;
    int   biomeHeight;
    float blockSize;

   //bioems
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
    virtual void spawnEnemies() = 0;
    virtual bool checkLevelComplete() = 0;

    // Common Functions

    virtual void loadTextures(string solidPath, string waterPath) {
        if (plains)  plains->loadTextures(solidPath, waterPath);
        if (aerial)  aerial->loadTextures(solidPath, waterPath);
        if (aquatic) aquatic->loadTextures(solidPath, waterPath);
    }

    virtual void update(float dt) {
        if (plains)  plains->update(dt);
        if (aerial)  aerial->update(dt);
        if (aquatic) aquatic->update(dt);
    }

    virtual void render(RenderWindow& window, float camX, float camY) {
        if (plains)  plains->render(window, camX, camY);
        if (aerial)  aerial->render(window, camX, camY);
        if (aquatic) aquatic->render(window, camX, camY);
    }

    virtual void resolveCollisions(float& px, float& py,float pw, float ph,float& velX, float& velY,bool& onGround) {
        if (plains)  plains->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
        if (aerial)  aerial->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
        if (aquatic) aquatic->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
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
        if (plains && plains->checkWaterCollision(px, py, pw, ph))  return true;
        if (aerial && aerial->checkWaterCollision(px, py, pw, ph))  return true;
        if (aquatic && aquatic->checkWaterCollision(px, py, pw, ph)) return true;
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
    virtual void spawnEnemies() = 0;

   
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
            zombieBatchCountAquatic * 4+ flyingTaraCount * 2 + 
            enemySubCount * 1;// zombies batch = 3 to 5

        // Level 1 score multiplier   normal
        scoreMultiplier = 1.0f;

        // player will spawn at start of plains biome
        playerSpawnX = 200;
        playerSpawnY = 100;
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
            "Sprites/blocks/water.png");

        // Terrain generate 
        plains->generateTerrain(biomeWidth, biomeHeight);
        aerial->generateTerrain(biomeWidth, biomeHeight);
        aquatic->generateTerrain(biomeWidth, biomeHeight);

        isLoaded = true;
    }

    
    void spawnEnemies() override {
        
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

        playerSpawnX = 200;
        playerSpawnY = 100;
    }

    ~Level2() {}

    void generateBiomes() override {
       //IN LEVEL 2 FIRST AERIAL THEN AQUATIC THEN PLAINS
        aerial = new AerialBiome(levelStart, plainsEnd);
          
        aquatic = new AquaticBiome(plainsEnd, aerialEnd);

        plains = new PlainsBiome(aerialEnd, aquaticEnd);
        loadTextures("Sprites/blocks/stone.png",
            "Sprites/blocks/water.png");

        // Level 2 HAS MORE HILLS
        aerial->generateTerrain(biomeWidth, biomeHeight);
        aquatic->generateTerrain(biomeWidth, biomeHeight);
        plains->generateTerrain(biomeWidth, biomeHeight);
        

        isLoaded = true;
    }

    void spawnEnemies() override {}

    void update(float dt) override {
        Level::update(dt);
        checkLevelComplete();
    }

    void render(RenderWindow& window, float camX, float camY) override {
        if (aerial)  aerial->render(window, camX, camY);
        if (aquatic) aquatic->render(window, camX, camY);
        if (plains)  plains->render(window, camX, camY);
    
    }

    int getBradleyCount() { return bradleyCount; }
    int getPowPlainsCount() { return powPrisonerPlains; }
    int getPowOtherCount() { return powPrisonerOther; }
};











class Level3 : public SurvivalLevel {
private:
    // Infantry  3 batches each ALL 3 BIOME
    int rebelBatchCount;
    int shieldedBatchCount;
    int bazookaBatchCount;
    int grenadierBatchCount;

    // Aerial  3 batches each
    int paratrooperBatchCount;
    int martianBatchCount;

    // Undead  4 batches each ONLY plains
    int mummyBatchCount;

    // Aquatic  4 batches zombies
    int zombieBatchCountAquatic;

    // Vehicles
    int enemySubCount;      // 2 subs
    int flyingTaraCount;    // 3 batches
    int bradleyCount;       // 2 bradleys

    // POW  2 per biome
    int powPerBiome;

public:
    Level3() : SurvivalLevel("Level 3", 3, 83, 14)
    {
        
        rebelBatchCount = 3;
        shieldedBatchCount = 3;
        bazookaBatchCount = 3;
        grenadierBatchCount = 3;

        paratrooperBatchCount = 3;
        martianBatchCount = 3;

        mummyBatchCount = 4;

        zombieBatchCountAquatic = 4;

        enemySubCount = 2;   // 2 subs IN  aquatic 
        flyingTaraCount = 3;  // 3 batches flying tara
        bradleyCount = 2;   // 2 bradleys different locations

        powPerBiome = 2;      // 2 POW per biome = 6 total

        // Total enemies
        totalEnemies =
            rebelBatchCount * 3 +
            shieldedBatchCount * 1 +
            bazookaBatchCount * 1 +
            grenadierBatchCount * 1 +
            paratrooperBatchCount * 2 +
            martianBatchCount * 1 +
            mummyBatchCount * 1 + flyingTaraCount * 2 +
            zombieBatchCountAquatic * 4  +  enemySubCount * 1 + 
            bradleyCount * 1;

        scoreMultiplier = 2.0f;

        playerSpawnX = 200;
        playerSpawnY = 100;
    }

    ~Level3() {}

    void generateBiomes() override {
        //IN LEVEL 3 FIRST AQUATIC THEN AERIAL THEN PLAINS
        aquatic = new AquaticBiome(levelStart, plainsEnd);

        aerial = new AerialBiome(plainsEnd, aerialEnd);
        plains = new PlainsBiome(aerialEnd, aquaticEnd);
        loadTextures("Sprites/blocks/stone.png",
            "Sprites/blocks/water.png");
        aquatic->generateTerrain(biomeWidth, biomeHeight);
        aerial->generateTerrain(biomeWidth, biomeHeight);
        plains->generateTerrain(biomeWidth, biomeHeight);
       
       
        isLoaded = true;
    }

    void spawnEnemies() override {
     
    }

    void update(float dt) override {
        Level::update(dt);
        checkLevelComplete();
    }

    void render(RenderWindow& window, float camX, float camY) override {
        if (aquatic) aquatic->render(window, camX, camY);
        if (aerial)  aerial->render(window, camX, camY);
        if (plains)  plains->render(window, camX, camY);
    }

    // Getters
    int getRebelBatchCount() { return rebelBatchCount; }
    int getMummyBatchCount() { return mummyBatchCount; }
    int getZombieCount() { return zombieBatchCountAquatic; }
    int getEnemySubCount() { return enemySubCount; }
    int getFlyingTaraCount() { return flyingTaraCount; }
    int getBradleyCount() { return bradleyCount; }
    int getPowPerBiome() { return powPerBiome; }
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
    void spawnEnemies()      override;
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


