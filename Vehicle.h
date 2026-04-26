#pragma once
#include "Player.h"

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
    Vehicle();
    virtual ~Vehicle();
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void move();
    virtual void takeDamage(int damage);
    void destroy();
    bool isAlive();
    int getHp();
    float getSpeed();
    void mountPilot(Player* Player);
    void ejectPilot();
    bool hasPilotInside();
    void applySoundEffect();
    void checkCollision();
    int getBiomeType();
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
    PlayerVehicle();
    virtual ~PlayerVehicle();
    virtual void fire() = 0;
    bool canFire();
    void reload();
    void takeDamage(int damage);
    int getDamageState();
    void updateDamageState();
    void renderDamageHue();
    void activateImmunity();
    void deactivateImmunity();
    bool isImmune();
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
    EnemyVehicle();
    virtual ~EnemyVehicle();
    virtual void attack() = 0;
    void detectPlayer();
    void moveTowardsPlayer();
    void spawnProjectile();
    int getScoreValue();
    void setGrudgeful();
    void applyPowerBoost();
    void onPlayerPassed();
    void returnWithBoost();
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
    MetalSlug();
    void fire();
    void fireCannon();
    void tilt();
    void jump();
    void crouch();
    void throwGrenade();
    void update();
    void render();
    void suicideAttack();
    void triggerWarning();
    void eject();
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
    SlugFlyer();
    void fire();
    void fireMissile();
    void ascend();
    void descend();
    void hover();
    void update();
    void render();
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
    SlugMariner();
    void fire();
    void fireHorizontalMissile();
    void fireVerticalMissile();
    void fireReverseMissile();
    void dive();
    void surface();
    void update();
    void render();
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
    AmphibiousSlug();
    void fire();
    void detectTerrainMode();
    void switchToMariner();
    void switchToFlyer();
    void switchToSlug();
    int getCurrentMode();
    void onWaterContact();
    void onGroundContact();
    void onAirborne();
    void update();
    void render();
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
    FlyingTara();
    void attack();
    void dropGrenade();
    void hover();
    void checkIfOnTopOfPlayer();
    void patrol();
    void update();
    void render();
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
    M15ABradley();
    void attack();
    void fireMissile();
    void moveHorizontally();
    void checkFlatTerrain();
    void update();
    void render();
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
    EnemySub();
    void attack();
    void fireRocket();
    void dive();
    void surface();
    void update();
    void render();
};