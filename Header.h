#pragma once
#include "Block.h"
#include "Player.h"
using namespace sf;
using namespace std;


class Level {
protected:
    string name;
    int    levelNumber;

    //size
    int   biomeWidth;
    int   biomeHeight;
    float blockSize;

    //biomes
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

    virtual void resolveCollisions(float& px, float& py, float pw, float ph, float& velX, float& velY, bool& onGround) {
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


    Level1() : SurvivalLevel("Level 1", 1, 83, 14)
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
            zombieBatchCountAquatic * 4 + enemySubCount * 1 +
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


class Vehicle {
protected:
    char* name;
    float x;
    float y;
    float width;
    float height;
    int hp;
    int maxHp;
    float speed;
    float maxSpeed;
    bool isActive;
    bool isDestroyed;
    int vehicleType;
    int ownerType;
    float velocityX;
    float velocityY;
    char* soundEffect;
    bool isMuffled;
    Player* pilot;
    bool hasPilot;
    float gravity;
    bool isAffectedByGravity;
    int biomeType;
    float collisionBox;

public:
    Vehicle() {
        name = new char[50];
        x = 0;
        y = 0;
        width = 100;
        height = 80;
        hp = 100;
        maxHp = 100;
        speed = 3.0f;
        maxSpeed = 5.0f;
        isActive = false;
        isDestroyed = false;
        vehicleType = 0;
        ownerType = 0;
        velocityX = 0;
        velocityY = 0;
        soundEffect = new char[50];
        isMuffled = false;
        pilot = nullptr;
        hasPilot = false;
        gravity = 0.5f;
        isAffectedByGravity = true;
        biomeType = 0;
        collisionBox = 100;
    }

    ~Vehicle() {
        delete[] name;
        delete[] soundEffect;
    }

    void move() {
        x += velocityX;
        y += velocityY;

        if (isAffectedByGravity) {
            velocityY += gravity;
        }
    }

    void takeDamage(int damage) {
        if (!isDestroyed) {
            hp -= damage;
            if (hp <= 0) {
                hp = 0;
                destroy();
            }
        }
    }

    void destroy() {
        isDestroyed = true;
        isActive = false;
        if (hasPilot) {
            ejectPilot();
        }
    }

    bool isAlive() {
        return !isDestroyed && hp > 0;
    }

    int getHp() {
        return hp;
    }

    float getSpeed() {
        return speed;
    }

    void mountPilot(Player* Player) {
        pilot = Player;
        hasPilot = true;
    }

    void ejectPilot() {
        pilot = nullptr;
        hasPilot = false;
    }

    bool hasPilotInside() {
        return hasPilot;
    }

    void applySoundEffect() {
        if (!isMuffled) {
            cout << "Playing sound: " << soundEffect << endl;
        }
    }

    void checkCollision() {
        // Basic collision detection
    }

    int getBiomeType() {
        return biomeType;
    }
};

class PlayerVehicle : public Vehicle {
protected:
    int durability;
    int maxDurability;
    float injuryTimer;
    float injuryStateDuration;
    float criticalStateDuration;
    int damageState;
    bool isMounted;
    float fireRate;
    float fireTimer;
    int ammo;
    int maxAmmo;
    float screenRedHue;
    bool isImmuneToExplosions;
    float immunityTimer;
    float immunityDuration;

public:
    PlayerVehicle() : Vehicle() {
        durability = 100;
        maxDurability = 100;
        injuryTimer = 0;
        injuryStateDuration = 1.0f;
        criticalStateDuration = 1.0f;
        damageState = 0;
        isMounted = false;
        fireRate = 0.2f;
        fireTimer = 0;
        ammo = 100;
        maxAmmo = 100;
        screenRedHue = 0;
        isImmuneToExplosions = false;
        immunityTimer = 0;
        immunityDuration = 3.0f;
    }

    ~PlayerVehicle() {
    }

    bool canFire() {
        return fireTimer <= 0 && ammo > 0 && !isDestroyed;
    }

    void reload() {
        ammo = maxAmmo;
    }

    void takeDamage(int damage) {
        if (isImmuneToExplosions) {
            return;
        }

        durability -= damage;
        hp -= damage;

        if (durability <= 0) {
            durability = 0;
            destroy();
        }

        updateDamageState();
    }

    int getDamageState() {
        return damageState;
    }

    void updateDamageState() {
        float healthPercent = (float)hp / maxHp;

        if (healthPercent > 0.6f) {
            damageState = 0;
            screenRedHue = 0;
        }
        else if (healthPercent > 0.3f) {
            damageState = 1;
            screenRedHue = 0.3f;
            injuryTimer = injuryStateDuration;
        }
        else {
            damageState = 2;
            screenRedHue = 0.6f;
            injuryTimer = criticalStateDuration;
        }
    }

    void renderDamageHue() {
        if (screenRedHue > 0) {
            cout << "Red hue: " << screenRedHue << endl;
        }
    }

    void activateImmunity() {
        isImmuneToExplosions = true;
        immunityTimer = immunityDuration;
    }

    void deactivateImmunity() {
        isImmuneToExplosions = false;
        immunityTimer = 0;
    }

    bool isImmune() {
        return isImmuneToExplosions;
    }
};

class EnemyVehicle : public Vehicle {
protected:
    float detectionRange;
    float attackRange;
    float attackTimer;
    float attackRate;
    float targetX;
    float targetY;
    bool isAggressive;
    int scoreValue;
    bool isGrudgeful;
    float powerMultiplier;
    int spawnBatchSize;
    int currentBatch;
    float grudgeTimer;
    bool hasBeenPassed;

public:
    EnemyVehicle() : Vehicle() {
        detectionRange = 300.0f;
        attackRange = 200.0f;
        attackTimer = 0;
        attackRate = 1.0f;
        targetX = 0;
        targetY = 0;
        isAggressive = true;
        scoreValue = 500;
        isGrudgeful = false;
        powerMultiplier = 1.0f;
        spawnBatchSize = 1;
        currentBatch = 0;
        grudgeTimer = 0;
        hasBeenPassed = false;
    }

    ~EnemyVehicle() {
    }

    void detectPlayer() {
        // player detection logic
    }

    void moveTowardsPlayer() {
        if (targetX > x)
            velocityX = speed;
        else
            velocityX = -speed;
    }

    void spawnProjectile() {
        cout << "Enemy vehicle fires projectile" << endl;
    }

    int getScoreValue() {
        return scoreValue;
    }

    void setGrudgeful() {
        isGrudgeful = true;
        grudgeTimer = 10.0f;
    }

    void applyPowerBoost() {
        powerMultiplier = 1.5f;
        speed *= powerMultiplier;
    }

    void onPlayerPassed() {
        hasBeenPassed = true;
        setGrudgeful();
    }

    void returnWithBoost() {
        applyPowerBoost();
        velocityX = -velocityX;
    }
};
class MetalSlug : public PlayerVehicle {
private:
    bool canTilt;
    float tiltAngle;
    float maxTiltAngle;
    bool canTraverseBlock;
    int maxBlockTraversal;
    float bulletFireRate;
    int bulletDamage;
    int cannonAmmo;
    int maxCannonAmmo;
    float cannonBlastRadius;
    int cannonDamage;
    bool isGrounded;
    float warningTimer;
    float warningDuration;
    bool isWarning;
    bool hasShield;
    int shieldHp;

public:
MetalSlug() : PlayerVehicle() {
    canTilt = true;
    tiltAngle = 0;
    maxTiltAngle = 45.0f;
    canTraverseBlock = true;
    maxBlockTraversal = 2;
    bulletFireRate = 0.1f;
    bulletDamage = 10;
    cannonAmmo = 10;
    maxCannonAmmo = 10;
    cannonBlastRadius = 50.0f;
    cannonDamage = 50;
    isGrounded = true;
    warningTimer = 0;
    warningDuration = 2.0f;
    isWarning = false;
    hasShield = false;
    shieldHp = 0;
    width = 120;
    height = 90;
    maxHp = 150;
    hp = 150;
}

void fire() {
    if (canFire()) {
        cout << "Metal Slug fires bullet" << endl;
        ammo--;
        fireTimer = bulletFireRate;
    }
}

void fireCannon() {
    if (cannonAmmo > 0) {
        cout << "Metal Slug fires cannon" << endl;
        cannonAmmo--;
    }
}

void tilt() {
    if (canTilt && tiltAngle < maxTiltAngle) {
        tiltAngle += 5.0f;
    }
}

void jump() {
    if (isGrounded) {
        velocityY = -10.0f;
        isGrounded = false;
    }
}

void crouch() {
    height = 60;
}

void throwGrenade() {
    cout << "Metal Slug throws grenade" << endl;
}

void update() {
    move();

    if (fireTimer > 0) {
        fireTimer -= 0.016f;
    }

    if (warningTimer > 0) {
        warningTimer -= 0.016f;
        if (warningTimer <= 0) {
            isWarning = false;
        }
    }

    if (y >= 700) {
        y = 700;
        velocityY = 0;
        isGrounded = true;
    }
}

void render() {
    cout << "Rendering Metal Slug at (" << x << ", " << y << ")" << endl;
}

void suicideAttack() {
    triggerWarning();
    cout << "Metal Slug performs suicide attack!" << endl;
}

void triggerWarning() {
    isWarning = true;
    warningTimer = warningDuration;
}

void eject() {
    ejectPilot();
    cout << "Pilot ejected from Metal Slug" << endl;
}
};

class SlugFlyer : public PlayerVehicle {
private:
    int missileAmmo;
    int maxMissileAmmo;
    int missileDamage;
    float altitude;
    float maxAltitude;
    float minAltitude;
    float bulletFireRate;
    int bulletAmmo;
    int bulletDamage;
    bool isFlying;
    bool hoverCapability;

public:
SlugFlyer() : PlayerVehicle() {
    missileAmmo = 20;
    maxMissileAmmo = 20;
    missileDamage = 30;
    altitude = 200.0f;
    maxAltitude = 500.0f;
    minAltitude = 100.0f;
    bulletFireRate = 0.15f;
    bulletAmmo = 200;
    bulletDamage = 8;
    isFlying = true;
    hoverCapability = true;
    isAffectedByGravity = false;
    width = 140;
    height = 80;
}

void fire() {
    if (canFire()) {
        cout << "Slug Flyer fires bullets" << endl;
        bulletAmmo--;
        fireTimer = bulletFireRate;
    }
}

void fireMissile() {
    if (missileAmmo > 0) {
        cout << "Slug Flyer fires missile" << endl;
        missileAmmo--;
    }
}

void ascend() {
    if (altitude < maxAltitude) {
        altitude += 5.0f;
        y -= 5.0f;
    }
}

void descend() {
    if (altitude > minAltitude) {
        altitude -= 5.0f;
        y += 5.0f;
    }
}

void hover() {
    velocityY = 0;
    velocityX = 0;
}

void update() {
    if (!hoverCapability) {
        move();
    }

    if (fireTimer > 0) {
        fireTimer -= 0.016f;
    }
}

void render() {
    cout << "Rendering Slug Flyer at (" << x << ", " << y << ") altitude: " << altitude << endl;
}
};

class SlugMariner : public PlayerVehicle {
private:
    int horizontalMissileAmmo;
    int verticalMissileAmmo;
    int reverseMissileAmmo;
    int maxMissileAmmoEach;
    bool isSubmerged;
    float depth;
    float maxDepth;
    float underwaterVisibility;
    float surfaceLevel;

public:
SlugMariner() : PlayerVehicle() {
    horizontalMissileAmmo = 10;
    verticalMissileAmmo = 10;
    reverseMissileAmmo = 10;
    maxMissileAmmoEach = 10;
    isSubmerged = false;
    depth = 0;
    maxDepth = 300.0f;
    underwaterVisibility = 200.0f;
    surfaceLevel = 400.0f;
    width = 130;
    height = 70;
}

void fire() {
    fireHorizontalMissile();
}

void fireHorizontalMissile() {
    if (horizontalMissileAmmo > 0) {
        cout << "Slug Mariner fires horizontal missile" << endl;
        horizontalMissileAmmo--;
    }
}

void fireVerticalMissile() {
    if (verticalMissileAmmo > 0) {
        cout << "Slug Mariner fires vertical missile" << endl;
        verticalMissileAmmo--;
    }
}

void fireReverseMissile() {
    if (reverseMissileAmmo > 0) {
        cout << "Slug Mariner fires reverse missile" << endl;
        reverseMissileAmmo--;
    }
}

void dive() {
    if (depth < maxDepth) {
        depth += 10.0f;
        y += 10.0f;
        isSubmerged = true;
    }
}

void surface() {
    if (depth > 0) {
        depth -= 10.0f;
        y -= 10.0f;
        if (depth <= 0) {
            isSubmerged = false;
        }
    }
}

void update() {
    move();

    if (fireTimer > 0) {
        fireTimer -= 0.016f;
    }
}

void render() {
    cout << "Rendering Slug Mariner at (" << x << ", " << y << ") depth: " << depth << endl;
}
};

class AmphibiousSlug : public PlayerVehicle {
private:
    int currentMode;
    bool isInWater;
    bool isGrounded;
    bool isFlying;
    MetalSlug* slugRef;
    SlugFlyer* flyerRef;
    SlugMariner* marinerRef;
    float transitionTimer;
    bool isTransitioning;

public:
AmphibiousSlug() : PlayerVehicle() {
    currentMode = 0;
    isInWater = false;
    isGrounded = true;
    isFlying = false;
    slugRef = new MetalSlug();
    flyerRef = new SlugFlyer();
    marinerRef = new SlugMariner();
    transitionTimer = 0;
    isTransitioning = false;
}

void fire() {
    if (currentMode == 0) {
        slugRef->fire();
    }
    else if (currentMode == 1) {
        flyerRef->fire();
    }
    else if (currentMode == 2) {
        marinerRef->fire();
    }
}

void detectTerrainMode() {
    if (isInWater) {
        switchToMariner();
    }
    else if (isFlying) {
        switchToFlyer();
    }
    else if (isGrounded) {
        switchToSlug();
    }
}

void switchToMariner() {
    if (currentMode != 2) {
        currentMode = 2;
        isTransitioning = true;
        transitionTimer = 1.0f;
    }
}

void switchToFlyer() {
    if (currentMode != 1) {
        currentMode = 1;
        isTransitioning = true;
        transitionTimer = 1.0f;
    }
}

void switchToSlug() {
    if (currentMode != 0) {
        currentMode = 0;
        isTransitioning = true;
        transitionTimer = 1.0f;
    }
}

int getCurrentMode() {
    return currentMode;
}

void onWaterContact() {
    isInWater = true;
    isGrounded = false;
    isFlying = false;
}

void onGroundContact() {
    isGrounded = true;
    isInWater = false;
    isFlying = false;
}

void onAirborne() {
    isFlying = true;
    isGrounded = false;
    isInWater = false;
}

void update() {
    detectTerrainMode();

    if (isTransitioning) {
        transitionTimer -= 0.016f;
        if (transitionTimer <= 0) {
            isTransitioning = false;
        }
    }

    if (currentMode == 0) {
        slugRef->update();
    }
    else if (currentMode == 1) {
        flyerRef->update();
    }
    else if (currentMode == 2) {
        marinerRef->update();
    }
}

void render() {
    if (currentMode == 0) {
        slugRef->render();
    }
    else if (currentMode == 1) {
        flyerRef->render();
    }
    else if (currentMode == 2) {
        marinerRef->render();
    }
}
};

class FlyingTara : public EnemyVehicle {
private:
    float hoverHeight;
    int grenadeDamage;
    float grenadeBlastRadius;
    float grenadeTimer;
    float grenadeRate;
    bool isOnTopOfPlayer;
    float horizontalSpeed;
    int spawnCount;
    int patrolDirection;

public:
FlyingTara() : EnemyVehicle() {
    hoverHeight = 150.0f;
    grenadeDamage = 25;
    grenadeBlastRadius = 40.0f;
    grenadeTimer = 0;
    grenadeRate = 2.0f;
    isOnTopOfPlayer = false;
    horizontalSpeed = 2.0f;
    spawnCount = 1;
    patrolDirection = 1;
    scoreValue = 300;
    width = 100;
    height = 60;
    isAffectedByGravity = false;
}

void attack() {
    if (isOnTopOfPlayer) {
        dropGrenade();
    }
}

void dropGrenade() {
    if (grenadeTimer <= 0) {
        cout << "Flying Tara drops grenade" << endl;
        grenadeTimer = grenadeRate;
    }
}

void hover() {
    y = hoverHeight;
    velocityY = 0;
}

void checkIfOnTopOfPlayer() {
    // player detection logic
    isOnTopOfPlayer = false;
}

void patrol() {
    velocityX = horizontalSpeed * patrolDirection;

    if (x > 1500) {
        patrolDirection = -1;
    }
    else if (x < 100) {
        patrolDirection = 1;
    }
}

void update() {
    hover();
    patrol();
    checkIfOnTopOfPlayer();
    move();

    if (grenadeTimer > 0) {
        grenadeTimer -= 0.016f;
    }

    if (attackTimer > 0) {
        attackTimer -= 0.016f;
    }
    else {
        attack();
        attackTimer = attackRate;
    }
}

void render() {
    cout << "Rendering Flying Tara at (" << x << ", " << y << ")" << endl;
}
};

class M15ABradley : public EnemyVehicle {
private:
    float missileBlastRadius;
    int missileDamage;
    float missileTimer;
    float missileRate;
    bool isStationary;
    bool canMoveHorizontally;
    bool isOnFlatTerrain;
    float missileArcAngle;
    float missileArcHeight;
    float approachSpeed;

public:
M15ABradley() : EnemyVehicle() {
    missileBlastRadius = 60.0f;
    missileDamage = 40;
    missileTimer = 0;
    missileRate = 3.0f;
    isStationary = false;
    canMoveHorizontally = true;
    isOnFlatTerrain = false;
    missileArcAngle = 45.0f;
    missileArcHeight = 100.0f;
    approachSpeed = 2.0f;
    scoreValue = 600;
    width = 150;
    height = 100;
}

void attack() {
    fireMissile();
}

void fireMissile() {
    if (missileTimer <= 0) {
        cout << "M-15A Bradley fires missile" << endl;
        missileTimer = missileRate;
    }
}

void moveHorizontally() {
    if (canMoveHorizontally && !isStationary) {
        velocityX = approachSpeed;
    }
}

void checkFlatTerrain() {
    // terrain detection logic
    isOnFlatTerrain = true;
}

void update() {
    checkFlatTerrain();

    if (isOnFlatTerrain) {
        moveHorizontally();
    }

    move();

    if (missileTimer > 0) {
        missileTimer -= 0.016f;
    }
    else {
        attack();
    }
}

void render() {
    cout << "Rendering M-15A Bradley at (" << x << ", " << y << ")" << endl;
}
};

class EnemySub : public EnemyVehicle {
private:
    int rocketDamage;
    float rocketBlastRadius;
    float rocketTimer;
    float rocketRate;
    bool isSubmerged;
    float depth;
    float maxDepth;
    int spawnBiome;
    float rocketArcAngle;

public:
    EnemySub() : EnemyVehicle() {
        rocketDamage = 35;
        rocketBlastRadius = 50.0f;
        rocketTimer = 0;
        rocketRate = 4.0f;
        isSubmerged = true;
        depth = 200.0f;
        maxDepth = 400.0f;
        spawnBiome = 2;
        rocketArcAngle = 60.0f;
        scoreValue = 700;
        width = 160;
        height = 70;
    }

    void attack() {
        fireRocket();
    }

    void fireRocket() {
        if (rocketTimer <= 0) {
            cout << "Enemy Sub fires rocket" << endl;
            rocketTimer = rocketRate;
        }
    }

    void dive() {
        if (depth < maxDepth) {
            depth += 10.0f;
            y += 10.0f;
            isSubmerged = true;
        }
    }

    void surface() {
        if (depth > 0) {
            depth -= 10.0f;
            y -= 10.0f;
            if (depth <= 0) {
                isSubmerged = false;
            }
        }
    }

    void update() {
        move();

        if (rocketTimer > 0) {
            rocketTimer -= 0.016f;
        }
        else {
            attack();
        }
    }

    void render() {
        cout << "Rendering Enemy Sub at (" << x << ", " << y << ") depth: " << depth << endl;
    }
};

//Weapon types
const int WT_NONE = 0;
const int WT_PISTOL = 1;
const int WT_KNIFE = 2;
const int WT_HMG = 3;
const int WT_ROCKET_LAUNCHER = 4;
const int WT_FLAME_SHOT = 5;
const int WT_LASER_GUN = 6;
const int WT_HAND_GRENADE = 7;
const int WT_FIRE_BOMB_GRENADE = 8;

//Owner types
const int OT_NONE = 0;
const int OT_PLAYER = 1;
const int OT_ENEMY = 2;
const int OT_VEHICLE = 3;

class Weapon {
protected:
    char  name[32];
    int   damage;
    bool  isActive;
    int   weaponType;
    float x, y;
    int   ownerType;
    char  soundEffect[32];
    bool  isMuffled;

public:
    Weapon() {
        name[0] = '\0';
        damage = 10;
        isActive = false;
        weaponType = WT_NONE;
        x = 0;
        y = 0;
        ownerType = OT_NONE;
        soundEffect[0] = '\0';
        isMuffled = false;
    }

    ~Weapon() {
    }

    int getDamage() const {
        return damage;
    }

    void setActive(bool active) {
        isActive = active;
    }

    int getOwnerType() const {
        return ownerType;
    }

    void applySoundEffect() {
        if (!isMuffled) {
            cout << "Sound: " << soundEffect << endl;
        }
    }

    bool getIsActive() const {
        return isActive;
    }

    int getWeaponType() const {
        return weaponType;
    }

    const char* getName() const {
        return name;
    }

    float getX() const {
        return x;
    }

    float getY() const {
        return y;
    }

    void setPosition(float px, float py) {
        x = px;
        y = py;
    }

    void setOwnerType(int ot) {
        ownerType = ot;
    }

    void setMuffled(bool m) {
        isMuffled = m;
    }
};

class Firearm : public Weapon {
protected:
    float fireRate;
    float fireTimer;
    int   ammo;
    int   maxAmmo;
    bool  isInfiniteAmmo;
    float range;
    float projectileSpeed;

public:
    Firearm() : Weapon() {
        fireRate = 0.2f;
        fireTimer = 0;
        ammo = 100;
        maxAmmo = 100;
        isInfiniteAmmo = false;
        range = 500.0f;
        projectileSpeed = 10.0f;
    }

    ~Firearm() {
    }

    void reload() {
        ammo = maxAmmo;
    }

    bool isEmpty() const {
        return ammo <= 0 && !isInfiniteAmmo;
    }

    float getFireRate() const {
        return fireRate;
    }

    int getAmmo() const {
        return ammo;
    }

    void setAmmo(int amount) {
        ammo = amount;
        if (ammo > maxAmmo) {
            ammo = maxAmmo;
        }
    }

    bool canFire() {
        return fireTimer <= 0 && (ammo > 0 || isInfiniteAmmo);
    }
};
class Pistol : public Firearm {
private:
    bool isDefault;

public:
    Pistol() : Firearm() {
        weaponType = WT_PISTOL;
        isDefault = true;
        damage = 10;
        fireRate = 0.3f;
        ammo = 50;
        maxAmmo = 50;
        isInfiniteAmmo = true;
    }

    void fire(float angle) {
        if (canFire()) {
            cout << "Pistol fires at angle: " << angle << endl;
            if (!isInfiniteAmmo) {
                ammo--;
            }
            fireTimer = fireRate;
        }
    }

    void update(float dt) {
        if (fireTimer > 0) {
            fireTimer -= dt;
        }
    }

    void render() {
        cout << "Rendering Pistol" << endl;
    }

    bool getIsDefault() const {
        return isDefault;
    }
};

class Knife : public Firearm {
private:
    float meleeRange;
    int   meleeDamage;
    float meleeCooldown;
    float meleeTimer;
    bool  isPiercing;

public:
    Knife() : Firearm() {
        weaponType = WT_KNIFE;
        meleeRange = 30.0f;
        meleeDamage = 50;
        meleeCooldown = 0.5f;
        meleeTimer = 0;
        isPiercing = false;
        damage = 50;
        isInfiniteAmmo = true;
    }

    void fire(float angle) {
        if (canMelee()) {
            cout << "Knife slashes at angle: " << angle << endl;
            meleeTimer = meleeCooldown;
        }
    }

    void update(float dt) {
        if (meleeTimer > 0) {
            meleeTimer -= dt;
        }
    }

    void render() {
        cout << "Rendering Knife" << endl;
    }

    bool isInRange(float targetX, float targetY) const {
        float dx = targetX - x;
        float dy = targetY - y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance <= meleeRange;
    }

    bool canMelee() const {
        return meleeTimer <= 0;
    }

    void resetCooldown() {
        meleeTimer = 0;
    }

    float getMeleeRange() const {
        return meleeRange;
    }

    int getMeleeDamage() const {
        return meleeDamage;
    }

    bool getIsPiercing() const {
        return isPiercing;
    }

    void setPiercing(bool p) {
        isPiercing = p;
    }
};

class HeavyMachineGun : public Firearm {
private:
    bool  isHeldDown;
    float spinUpTimer;
    float spinUpDuration;
    float currentSpinLevel;

public:
    HeavyMachineGun() : Firearm() {
        weaponType = WT_HMG;
        isHeldDown = false;
        spinUpTimer = 0;
        spinUpDuration = 1.0f;
        currentSpinLevel = 0;
        damage = 15;
        fireRate = 0.05f;
        ammo = 200;
        maxAmmo = 200;
    }

    void fire(float angle) {
        if (canFire() && currentSpinLevel >= 1.0f) {
            cout << "HMG fires at angle: " << angle << endl;
            ammo--;
            fireTimer = fireRate;
        }
    }

    void startFire() {
        isHeldDown = true;
    }

    void stopFire() {
        isHeldDown = false;
    }

    void update(float dt) {
        if (fireTimer > 0) {
            fireTimer -= dt;
        }

        if (isHeldDown) {
            if (currentSpinLevel < 1.0f) {
                spinUpTimer += dt;
                currentSpinLevel = spinUpTimer / spinUpDuration;
                if (currentSpinLevel > 1.0f) {
                    currentSpinLevel = 1.0f;
                }
            }
        }
        else {
            spinUpTimer = 0;
            currentSpinLevel = 0;
        }
    }

    void render() {
        cout << "Rendering HMG, spin level: " << currentSpinLevel << endl;
    }

    float getSpinLevel() const {
        return currentSpinLevel;
    }

    bool getIsHeldDown() const {
        return isHeldDown;
    }
};

class RocketLauncher : public Firearm {
private:
    float reloadTimer;
    float reloadDuration;
    float blastRadius;
    int   blastDamage;
    bool  isReloading;
    float rocketSpeed;

public:
    RocketLauncher() : Firearm() {
        weaponType = WT_ROCKET_LAUNCHER;
        reloadTimer = 0;
        reloadDuration = 2.0f;
        blastRadius = 80.0f;
        blastDamage = 100;
        isReloading = false;
        rocketSpeed = 8.0f;
        damage = 100;
        fireRate = 2.0f;
        ammo = 10;
        maxAmmo = 10;
    }

    void fire(float angle) {
        if (canFire() && !isReloading) {
            spawnRocket(angle);
            ammo--;
            fireTimer = fireRate;
            if (ammo <= 0) {
                isReloading = true;
                reloadTimer = reloadDuration;
            }
        }
    }

    void reload() {
        isReloading = true;
        reloadTimer = reloadDuration;
    }

    void spawnRocket(float angle) {
        cout << "Rocket launched at angle: " << angle << endl;
    }

    void update(float dt) {
        if (fireTimer > 0) {
            fireTimer -= dt;
        }

        if (isReloading) {
            reloadTimer -= dt;
            if (reloadTimer <= 0) {
                isReloading = false;
                ammo = maxAmmo;
            }
        }
    }

    void render() {
        cout << "Rendering Rocket Launcher" << endl;
    }

    bool isReloadingNow() const {
        return isReloading;
    }

    float getBlastRadius() const {
        return blastRadius;
    }

    int getBlastDamage() const {
        return blastDamage;
    }
};

class FlameShot : public Firearm {
private:
    float streamLength;
    int   damagePerSecond;
    bool  isHeldDown;
    bool  streamActive;
    float streamTimer;
    float maxStreamDuration;

public:
    FlameShot() : Firearm() {
        weaponType = WT_FLAME_SHOT;
        streamLength = 150.0f;
        damagePerSecond = 30;
        isHeldDown = false;
        streamActive = false;
        streamTimer = 0;
        maxStreamDuration = 5.0f;
        damage = 30;
        fireRate = 0.1f;
        ammo = 100;
        maxAmmo = 100;
    }

    void fire(float angle) {
        if (canFire()) {
            startStream();
            cout << "FlameShot fires at angle: " << angle << endl;
        }
    }

    void startStream() {
        streamActive = true;
        isHeldDown = true;
        streamTimer = maxStreamDuration;
    }

    void stopStream() {
        streamActive = false;
        isHeldDown = false;
    }

    void update(float dt) {
        if (fireTimer > 0) {
            fireTimer -= dt;
        }

        if (streamActive) {
            streamTimer -= dt;
            ammo -= dt * 5;
            if (streamTimer <= 0 || ammo <= 0) {
                stopStream();
            }
        }
    }

    void render() {
        cout << "Rendering FlameShot" << endl;
    }

    bool isStreamActive() const {
        return streamActive;
    }

    int getDamagePerSecond() const {
        return damagePerSecond;
    }

    float getStreamLength() const {
        return streamLength;
    }
};

class LaserGun : public Firearm {
private:
    bool  beamActive;
    float beamLength;
    bool  isInstantKill;
    float beamTimer;
    float beamDuration;
    float beamWidth;

public:
    LaserGun() : Firearm() {
        weaponType = WT_LASER_GUN;
        beamActive = false;
        beamLength = 500.0f;
        isInstantKill = true;
        beamTimer = 0;
        beamDuration = 0.5f;
        beamWidth = 5.0f;
        damage = 999;
        fireRate = 1.0f;
        ammo = 30;
        maxAmmo = 30;
    }

    void fire(float angle) {
        if (canFire()) {
            spawnBeam(angle);
            ammo--;
            fireTimer = fireRate;
        }
    }

    void spawnBeam(float angle) {
        beamActive = true;
        beamTimer = beamDuration;
        cout << "Laser beam fired at angle: " << angle << endl;
    }

    void update(float dt) {
        if (fireTimer > 0) {
            fireTimer -= dt;
        }

        if (beamActive) {
            beamTimer -= dt;
            if (beamTimer <= 0) {
                deactivateBeam();
            }
        }
    }

    void render() {
        cout << "Rendering Laser Gun" << endl;
    }

    void deactivateBeam() {
        beamActive = false;
    }

    bool isBeamActive() const {
        return beamActive;
    }

    bool getIsInstantKill() const {
        return isInstantKill;
    }

};

class FirePool {
private:
    float x, y;
    float radius;
    int   damagePerSecond;
    float duration;
    float timer;
    bool  isActive;
    int   ownerType;

public:
    FirePool() {
        x = 0;
        y = 0;
        radius = 50.0f;
        damagePerSecond = 20;
        duration = 5.0f;
        timer = duration;
        isActive = true;
        ownerType = OT_NONE;
    }

    FirePool(float px, float py, float r, int dps, float dur, int owner) {
        x = px;
        y = py;
        radius = r;
        damagePerSecond = dps;
        duration = dur;
        timer = dur;
        isActive = true;
        ownerType = owner;
    }

    void update(float dt) {
        if (isActive) {
            timer -= dt;
            if (timer <= 0) {
                isActive = false;
            }
        }
    }

    void render() {
        if (isActive) {
            cout << "Rendering fire pool at (" << x << ", " << y << ")" << endl;
        }
    }

    void applyDamage(float targetX, float targetY, float& dmgOut, float dt) {
        if (checkCollision(targetX, targetY, 0, 0)) {
            dmgOut = damagePerSecond * dt;
        }
        else {
            dmgOut = 0;
        }
    }

    bool isExpired() const {
        return timer <= 0;
    }

    bool checkCollision(float tx, float ty, float tw, float th) const {
        float dx = tx - x;
        float dy = ty - y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance <= radius;
    }

    bool getIsActive() const {
        return isActive;
    }

    float getX() const {
        return x;
    }

    float getY() const {
        return y;
    }

    float getRadius() const {
        return radius;
    }

    int getDamagePerSecond() const {
        return damagePerSecond;
    }
};

class Grenade : public Weapon {
protected:
    float   width;
    float   height;
    float   velocityX;
    float   velocityY;
    float   blastRadius;
    bool    isExploded;
    Player* thrownByPlayer;
    float   gravity;
    float   fuseTimer;
    float   fuseDuration;
    float   angle;
    float   initialSpeed;

public:
    Grenade() : Weapon() {
        width = 10.0f;
        height = 10.0f;
        velocityX = 0;
        velocityY = 0;
        blastRadius = 50.0f;
        isExploded = false;
        thrownByPlayer = nullptr;
        gravity = 0.5f;
        fuseTimer = 3.0f;
        fuseDuration = 3.0f;
        angle = 0;
        initialSpeed = 10.0f;
        damage = 50;
    }

    ~Grenade() {
    }

    void fire(float launchAngle) {
        launch(launchAngle, initialSpeed);
    }

    bool checkCollision(float tx, float ty, float tw, float th) {
        return (x < tx + tw && x + width > tx && y < ty + th && y + height > ty);
    }

    void applyGravity(float dt) {
        velocityY += gravity * dt;
    }

    void applyBlastDamage(float tx, float ty, int& targetHp) {
        if (isInBlastRadius(tx, ty)) {
            targetHp -= damage;
        }
    }

    bool isInBlastRadius(float tx, float ty) const {
        float dx = tx - x;
        float dy = ty - y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance <= blastRadius;
    }

    void launch(float launchAngle, float speed) {
        angle = launchAngle;
        velocityX = cos(launchAngle) * speed;
        velocityY = sin(launchAngle) * speed;
    }

    void setThrownBy(Player* p) {
        thrownByPlayer = p;
    }

    bool getIsExploded() const {
        return isExploded;
    }

    float getBlastRadius() const {
        return blastRadius;
    }

    float getVelocityX() const {
        return velocityX;
    }

    float getVelocityY() const {
        return velocityY;
    }
};

class HandGrenade : public Grenade {
private:
    int   bounceCount;
    int   maxBounce;
    float bounceVelocityDampen;
public:
    HandGrenade() : Grenade() {
        weaponType = WT_HAND_GRENADE;
        bounceCount = 0;
        maxBounce = 3;
        bounceVelocityDampen = 0.6f;
        blastRadius = 60.0f;
        damage = 80;
    }

    void Handfire(float angle) {
        fire(angle);
    }

    void Handupdate(float dt) {
        if (!isExploded) {
            applyGravity(dt);
            x += velocityX * dt;
            y += velocityY * dt;

            fuseTimer -= dt;
            if (fuseTimer <= 0) {
                Handexplode();
            }

            if (y >= 700) {
                HandapplyBounce();
            }
        }
    }

    void Handexplode() {
        isExploded = true;
        cout << "Hand grenade explodes!" << endl;
    }

    void Handrender() {
        if (!isExploded) {
            cout << "Rendering hand grenade at (" << x << ", " << y << ")" << endl;
        }
    }

    void HandapplyBounce() {
        if (bounceCount < maxBounce) {
            y = 700;
            velocityY = -velocityY * bounceVelocityDampen;
            velocityX *= bounceVelocityDampen;
            bounceCount++;
        }
        else {
            velocityY = 0;
            velocityX = 0;
        }
    }

    int HandgetBounceCount() const {
        return bounceCount;
    }
};

class FireBombGrenade : public Grenade {
private:
    FirePool* firePool;
    float     firePoolDuration;
    float     firePoolRadius;
    int       firePoolDamagePerSecond;

public:
    FireBombGrenade() : Grenade() {
        weaponType = WT_FIRE_BOMB_GRENADE;
        firePool = nullptr;
        firePoolDuration = 8.0f;
        firePoolRadius = 80.0f;
        firePoolDamagePerSecond = 25;
        blastRadius = 40.0f;
        damage = 60;
    }

    ~FireBombGrenade() {
        if (firePool) {
            delete firePool;
        }
    }

    void FireBombfire(float angle) {
        fire(angle);
    }

    void FireBombupdate(float dt) {
        if (!isExploded) {
            applyGravity(dt);
            x += velocityX * dt;
            y += velocityY * dt;

            fuseTimer -= dt;
            if (fuseTimer <= 0 || y >= 700) {
                FireBombexplode();
            }
        }

        if (firePool) {
            firePool->update(dt);
        }
    }

    void FireBombexplode() {
        isExploded = true;
        spawnFirePool();
        cout << "Fire bomb grenade explodes!" << endl;
    }

    void spawnFirePool() {
        firePool = new FirePool(x, y, firePoolRadius, firePoolDamagePerSecond, firePoolDuration, ownerType);
    }

    void FireBombrender() {
        if (!isExploded) {
            cout << "Rendering fire bomb grenade at (" << x << ", " << y << ")" << endl;
        }

        if (firePool) {
            firePool->render();
        }
    }

    FirePool* FireBombgetFirePool() const {
        return firePool;
    }
};
