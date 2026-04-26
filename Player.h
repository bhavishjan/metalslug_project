#pragma once

class Player {
protected:
    char name[50];
    float x;
    float y;
    float width;
    float height;
    float velocityX;
    float velocityY;
    int hp;
    int maxHp;
    int lives;
    float speed;
    bool isGrounded;
    bool isCrouching;
    bool isJumping;
    int grenadeCount;
    int grenadeType;
    Weapon* currentWeapon;
    Pistol* pistol;
    Knife* knife;
    bool facingRight;
    float aimAngle;
    int state;
    int state;
    int damageState;
    float damageTimer;
    float stateTimer;
    float meleeCooldown;
    float meleeTimer;
    bool inVehicle;
    Vehicle* currentVehicle;
    bool isAlive;
    bool isImmortal;
    bool isFat;
    bool isInWater;
    int score;
    int saturation;
    bool specialPowerActive;
    float specialPowerTimer;
    float specialPowerDuration;
    Weapon* weaponSlots[2];
    int activeWeaponIndex;

public:
    Player();
    virtual ~Player();
    virtual void move();
    virtual void jump();
    virtual void crouch();
    virtual void shoot();
    virtual void meleeAttack();
    virtual void throwGrenade();
    virtual void takeDamage(int damage);
    void takeMeleeDamage(int damage);
    void die();
    void respawn();
    void pickupWeapon(Weapon* weapon);
    void pickupFood(int saturationValue);
    void enterVehicle(Vehicle* vehicle);
    void exitVehicle();
    void switchWeapon();
    virtual void activateSpecialPower() = 0;
    void updateDamageState();
    void updateState();
    void becomeUndead();
    void becomeMummy();
    void revertState();
    void freePrisoner();
    void aim(float angle);
    void applyFannumTax();
    void applyGravity();
    void checkGrounded();
    void checkCollision();
    void enterWater();
    void exitWater();
    float getDistanceFrom(float targetX, float targetY);
    bool isNearEnemy();
    bool isNearPrisoner();
    void updateInputHistory();
    virtual void update();
    virtual void render();
    virtual float getFireRate();
    virtual float getBlastRadius();
    virtual int getGrenadeCount();
    float getMeleeRange();
    int getMeleeDamage();
    bool isShielded();
    bool getIsAlive() const;
    void applyVehicleBonus();
    void removeVehicleBonus();
    
};

class Marco : public Player {
private:
    bool dualFireActive;
    float dualFireTimer;
    float dualFireDuration;

public:
    Marco();
    void shoot();
    void meleeAttack();
    void activateSpecialPower();
    void updateDualFire();
    float getFireRate();
    int getGrenadeCount();
    void applyVehicleBonus();
};

class Tarma : public Player {
private:
    bool immunityActive;
    float immunityTimer;
    float immunityDuration;
    float vehicleFireRateBonus;
    float vehicleDurabilityBonus;

public:
    Tarma();
    void move();
    void takeDamage(int damage);
    void enterVehicle(Vehicle* vehicle);
    void exitVehicle();
    void onVehicleDestroyed();
    void activateSpecialPower();
    void updateImmunity();
    bool isInVehicle();
    float getVehicleFireRate();
    float getVehicleDurability();
};

class Eri : public Player {
private:
    bool doubleGrenadeActive;
    float doubleGrenadeTimer;
    float doubleGrenadeDuration;

public:
    Eri();
    void throwGrenade();
    void activateSpecialPower();
    void updateDoubleGrenade();
    void spawnFirePool();
    int getGrenadeCount();
    float getBlastRadius();
    void meleeAttack();
    float getFireRate();
};

class Fio : public Player {
private:
    bool superchargedActive;
    float superchargedTimer;
    float superchargedDuration;

public:
    Fio();
    void pickupWeapon(Weapon* weapon);
    void activateSpecialPower();
    void updateSupercharged();
    float getFireRate();
    void meleeAttack();
    int getGrenadeCount();
};

class FusionCompanion {
private:
    bool isActive;
    bool isSpawned;
    bool isImmortal;
    float x;
    float y;
    float width;
    float height;
    int hp;
    int maxHp;
    float speed;
    float fireRate;
    int damage;
    int grenadeCount;
    float blastRadius;
    Player* fusedCharacters[4];
    int fusedCount;
    float spawnDuration;
    float spawnTimer;
    float cooldownTimer;
    float cooldownDuration;
    int spriteID;
    float followDistance;
    float averagedFireRate;
    float averagedSpeed;
    int averagedDamage;
    int averagedGrenadeCount;
    float averagedBlastRadius;

public:
    FusionCompanion();
    ~FusionCompanion();
    void computeAveragedStats();
    void inheritBuffsWeaknesses();
    void followPlayer();
    void attackNearestEnemy();
    void findNearestEnemy();
    void moveTowardsPlayer();
    void moveTowardsEnemy();
    bool isExpired();
    void update();
    void render();
    void spawn();
    void despawn();
    void fuseCharacters(Player** characters, int count);
    void averageStats();
    void inheritBuffs();
    void inheritWeaknesses();
    void fire();
    void throwGrenade();
    bool canSpawn();
    void startCooldown();
    void updateCooldown();
    bool isCooldownExpired();
    bool isSpawnExpired();
    void resetForNewLevel();
    float getSurvivalCooldown();
    float getCampaignCooldown();
    void renderFollowPath();
};

class CharacterManager {
private:
    Player* characters[4];
    int activeIndex;
    FusionCompanion* fusionCompanion;
    float fusionCooldownTimer;
    int activeCharacterCount;

public:
    CharacterManager();
    void switchCharacter();
    Player* getActivePlayer();
    bool isCharacterAvailable(int index);
    int getNextAvailable();
    bool allDead();
    void resetAll();
    void spawnFusion();
    void update();
    void render();
};