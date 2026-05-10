#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include "Weapon.h"
#include "PlayerSoldier.h"
using namespace std;
using namespace sf;

// collectible type constants so we dont have to use random numbers
const int collectibleNone = 0;
const int collectibleFood = 1;
const int collectibleCrate = 2;
const int collectiblePrisoner = 3;

// food type constants
const int foodFruit = 0;
const int foodTurkey = 1;

// where did the crate come from, prisoner dropped it or boss batch or random
const int crateFromPrisoner = 0;
const int crateFromBoss = 1;
const int crateFromDrop = 2;

// biome constants same ones used everywhere in the level design
const int biomePlains = 0;
const int biomeAerial = 1;
const int biomeAquatic = 2;

// per the spec 90 percent chance for heavy weapons and 10 percent for laser
const float heavyWeaponChance = 0.9f;
const float laserGunChance = 0.1f;

// grenade count ranges per spec, 3 to 5 hand grenades, 1 to 3 fire bombs
const int handGrenadeMin = 3;
const int handGrenadeMax = 5;
const int fireBombMin = 1;
const int fireBombMax = 3;


// =============================================
// SATURATION SYSTEM
// basically the hunger system, decreases over time
// turkey gives back 3 units and fruit gives back 2 units per spec
// =============================================
class SaturationSystem {
private:
    int   currentSaturation;
    int   maxSaturation;
    int   minSaturation;
    float depletionRate;
    float depletionTimer;
    bool  isStarving;

public:
    SaturationSystem();
    void update(float dt);
    void addSaturation(int amount);
    void deplete();
    bool isFull()  const;
    bool isEmpty() const;
    int  getCurrentSaturation() const;
    int  getMaxSaturation()     const;
    bool getIsStarving()        const;
};


// =============================================
// DROP MANAGER
// decides what drops when an enemy dies
// small chance food drops, even smaller chance for crate
// =============================================
class DropManager {
private:
    float foodDropChance;
    float turkeyDropChance;
    float fruitDropChance;
    float crateDropChance;

public:
    DropManager();
    int onEnemyKilled();
    bool rollFoodDrop();
    bool rollCrateDrop();
    int determineFoodType();
    void spawnFood(float x, float y, int type);
    void spawnCrate(float x, float y, int source);
};


// =============================================
// COLLECTIBLE (ABSTRACT BASE CLASS)
// all pickupable things in the game inherit from this
// gravity pulls items to ground when they first spawn in
// =============================================
class Collectible {
protected:
    const char* name;
    float x;
    float y;
    float width;
    float height;
    bool  isActive;
    bool  isCollected;
    int   collectibleType;
    float gravity;
    float velocityY;
    bool  isOnGround;
    float spawnChance;
    const char* soundEffect;

    Sprite  sprite;
    Texture texture;
    Sound   collectSound;

public:
    Collectible();
    virtual ~Collectible();
    bool loadTexture(const char* path);
    void applyGravity(float dt);
    bool checkCollision(float px, float py, float pw, float ph) const;
    void activate();
    void deactivate();
    void setPosition(float px, float py);
    void setOnGround(bool g);
    virtual void onCollect(PlayerSoldier* player) = 0;
    virtual void update(float dt);
    virtual void render(RenderWindow& window);
    bool        getIsActive()        const;
    bool        getIsCollected()     const;
    int         getCollectibleType() const;
    float       getX()               const;
    float       getY()               const;
    const char* getName()            const;
};


// =============================================
// FOOD (ABSTRACT)
// turkey and fruit both inherit from here
// both just give back saturation, turkey more than fruit
// =============================================
class Food : public Collectible {
protected:
    int saturationValue;
    int foodType;
    int maxSaturation;

public:
    Food();
    virtual ~Food();
    virtual void applyToPlayer(PlayerSoldier* player);
    virtual void onCollect(PlayerSoldier* player) override;
    int getSaturationValue() const;
    int getFoodType()        const;
};


// =============================================
// FRUIT
// gives 2 saturation units per the spec
// drops more often than turkey from enemies
// =============================================
class Fruit : public Food {
private:
    int   healAmount;
    float dropChance;

public:
    Fruit();
    void onCollect(PlayerSoldier* player) override;
    void update(float dt) override;
    void render(RenderWindow& window) override;
    int   getHealAmount() const;
    float getDropChance() const;
};


// =============================================
// TURKEY
// gives 3 saturation units per spec, more then fruit
// worth it to go out of your way to grab it
// =============================================
class Turkey : public Food {
private:
    int   healAmount;
    float dropChance;

public:
    Turkey();
    void onCollect(PlayerSoldier* player) override;
    void update(float dt) override;
    void render(RenderWindow& window) override;
    int   getHealAmount() const;
    float getDropChance() const;
};


// =============================================
// SUPPLY CRATE
// has a weapon and grenades inside, player opens it nearby
// 90 percent chance for HMG flame or rocket, 10 percent laser
// if player already has a weapon they can just take the ammo instead
// for eri all hand grenades inside become fire bomb grenades
// =============================================
class SupplyCrate : public Collectible {
private:
    Weapon* weaponInside;
    int     weaponType;
    int     handGrenadeCount;
    int     fireBombGrenadeCount;
    bool    isOpened;
    bool    pickupPromptActive;
    float   pickupRange;
    int     pickupKey;
    int     crateSource;
    bool    isFromBossBatch;
    int     ammoForCurrentWeapon;
    bool    hasBeenLooted;

    RectangleShape promptBox;

public:
    SupplyCrate();
    ~SupplyCrate();
    void generateWeapon();
    void generateGrenades();
    int determineGrenadeType();
    void convertGrenadesForEri();
    void spawnFromBossBatch();
    void showPickupPrompt(RenderWindow& window, float playerX, float playerY);
    void openCrate();
    Weapon* pickupWeapon();
    int refillAmmo();
    int getAmmoForCurrentWeapon() const;
    void onCollect(PlayerSoldier* player) override;
    void update(float dt) override;
    void render(RenderWindow& window) override;
    Weapon* getWeaponInside()       const;
    int     getWeaponType()         const;
    int     getHandGrenadeCount()   const;
    int     getFireBombCount()      const;
    bool    getIsOpened()           const;
    bool    getHasBeenLooted()      const;
    bool    getPickupPromptActive() const;
    float   getPickupRange()        const;
    bool    getIsFromBossBatch()    const;
    int     getCrateSource()        const;
};


// =============================================
// POW PRISONER
// poor guy standing around waiting to be rescued
// walk up to him and press E and he will free himself
// as a thank you he drops a supply crate then runs off screen
// per spec level 1 has 1 prisoner per biome, level 2 and 3 have more
// =============================================
class POWPrisoner : public Collectible {
private:
    bool         isFreed;
    float        freeRange;
    int          freeKey;
    float        thankYouTimer;
    float        thankYouDuration;
    bool         isPlayingThankYou;
    int          spawnBiome;
    SupplyCrate* crateToSpawn;
    int          prisonerID;
    int          captivityBiome;

    float walkTimer;
    float walkSpeed;
    bool  hasVanished;

    RectangleShape freePromptBox;

public:
    POWPrisoner();
    ~POWPrisoner();
    bool checkIfPlayerNearby(float playerX, float playerY);
    void freeFromCaptivity(PlayerSoldier* player);
    void spawnSupplyCrate();
    void playThankYouAnim(float dt);
    void walkAfterFreed(float dt);
    void vanish();
    bool isAlreadyFreed() const;
    void onCollect(PlayerSoldier* player) override;
    void update(float dt) override;
    void render(RenderWindow& window) override;
    SupplyCrate* getCreatedCrate()      const;
    bool         getIsFreed()           const;
    float        getFreeRange()         const;
    bool         getHasVanished()       const;
    int          getPrisonerID()        const;
    int          getSpawnBiome()        const;
    bool         getIsPlayingThankYou() const;
    void         setPrisonerID(int id);
    void         setSpawnBiome(int b);
};