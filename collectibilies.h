#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "Weapon.h"
#include "PlayerSoldier.h"

using namespace std;

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
    float depletionRate;   // units lost per second
    float depletionTimer;  // counts down before each depletion
    bool  isStarving;

public:
    SaturationSystem() {
        maxSaturation = 100;
        currentSaturation = maxSaturation;
        minSaturation = 0;
        depletionRate = 1.0f;
        depletionTimer = 1.0f;
        isStarving = false;
    }

    // tick the hunger every frame, deplete once per second
    void update(float dt) {
        depletionTimer -= dt;
        if (depletionTimer <= 0) {
            depletionTimer = 1.0f / depletionRate;
            deplete();
        }
    }

    // food calls this to refill the saturation bar
    void addSaturation(int amount) {
        currentSaturation += amount;
        if (currentSaturation > maxSaturation)
            currentSaturation = maxSaturation;
        isStarving = false; // player ate so they not starving anymore
    }

    // remove one unit of saturation each tick
    void deplete() {
        currentSaturation--;
        if (currentSaturation <= minSaturation) {
            currentSaturation = minSaturation;
            isStarving = true;
        }
    }

    bool isFull()  const { return currentSaturation >= maxSaturation; }
    bool isEmpty() const { return currentSaturation <= minSaturation; }

    int  getCurrentSaturation() const { return currentSaturation; }
    int  getMaxSaturation()     const { return maxSaturation; }
    bool getIsStarving()        const { return isStarving; }
};


// =============================================
// DROP MANAGER
// decides what drops when an enemy dies
// small chance food drops, even smaller chance for crate
// =============================================
class DropManager {
private:
    float foodDropChance;    // probablity of food when enemy dies
    float turkeyDropChance;  // within food drops how often its turkey
    float fruitDropChance;   // within food drops how often its fruit
    float crateDropChance;   // seperate chance for crate to drop

public:
    DropManager() {
        foodDropChance = 0.25f;
        turkeyDropChance = 0.4f;
        fruitDropChance = 0.6f;
        crateDropChance = 0.05f; // crates are pretty rare dont want to break the balance
    }

    // call when enemy dies, returns what type of collectible to spawn
    int onEnemyKilled() {
        if (rollCrateDrop()) return collectibleCrate;
        if (rollFoodDrop())  return collectibleFood;
        return collectibleNone;
    }

    bool rollFoodDrop() {
        float roll = (float)rand() / RAND_MAX;
        return roll <= foodDropChance;
    }

    bool rollCrateDrop() {
        float roll = (float)rand() / RAND_MAX;
        return roll <= crateDropChance;
    }

    // randomly picks turkey or fruit based on their respective chances
    int determineFoodType() {
        float roll = (float)rand() / RAND_MAX;
        return (roll <= turkeyDropChance) ? foodTurkey : foodFruit;
    }

    void spawnFood(float x, float y, int type) {
        // TODO instantiate fruit or turkey object and add to world list
        cout << "Spawning food type " << type << " at (" << x << ", " << y << ")" << endl;
    }

    void spawnCrate(float x, float y, int source) {
        // TODO instantiate supply crate and add to worlds collectible list
        cout << "Spawning crate from source " << source << " at (" << x << ", " << y << ")" << endl;
    }
};


// =============================================
// COLLECTIBLE (ABSTRACT BASE CLASS)
// all pickupable things in the game inherit from this
// gravity pulls items to ground when they first spawn in
// =============================================
class Collectible {
protected:
    char  name[32];
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
    char  soundEffect[32];

    sf::Sprite  sprite;
    sf::Texture texture;
    sf::Sound   collectSound;

public:
    Collectible() {
        name[0] = '\0';
        x = 0;
        y = 0;
        width = 32.0f;
        height = 32.0f;
        isActive = true;
        isCollected = false;
        collectibleType = collectibleNone;
        gravity = 500.0f;
        velocityY = 0;
        isOnGround = false;
        spawnChance = 0.0f;
        soundEffect[0] = '\0';
    }

    virtual ~Collectible() {}

    bool loadTexture(const string& path) {
        if (!texture.loadFromFile(path)) {
            cout << "Warning couldnt load texture for " << name << endl;
            return false;
        }
        sprite.setTexture(texture);
        return true;
    }

    // pulls item downward until it lands on the ground
    void applyGravity(float dt) {
        if (!isOnGround) {
            velocityY += gravity * dt;
            y += velocityY * dt;
        }
    }

    // checks if player rectangle is overlapping with this items rectangle
    bool checkCollision(float px, float py, float pw, float ph) const {
        return (x < px + pw && x + width  > px &&
            y < py + ph && y + height > py);
    }

    void activate() {
        isActive = true;
        isCollected = false;
    }

    void deactivate() {
        isActive = false;
    }

    void setPosition(float px, float py) {
        x = px;
        y = py;
        sprite.setPosition(px, py);
    }

    void setOnGround(bool g) {
        isOnGround = g;
        if (g) velocityY = 0;
    }

    virtual void onCollect(PlayerSoldier* player) = 0;

    virtual void update(float dt) {
        if (!isCollected && isActive)
            applyGravity(dt);
    }

    virtual void render(sf::RenderWindow& window) {
        if (isActive && !isCollected) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }

    bool        getIsActive()        const { return isActive; }
    bool        getIsCollected()     const { return isCollected; }
    int         getCollectibleType() const { return collectibleType; }
    float       getX()               const { return x; }
    float       getY()               const { return y; }
    const char* getName()            const { return name; }
};


// =============================================
// FOOD (ABSTRACT)
// turkey and fruit both inherit from here
// both just give back saturation, turkey more than fruit
// =============================================
class Food : public Collectible {
protected:
    int saturationValue; // how much saturation this food restores
    int foodType;
    int maxSaturation;

public:
    Food() : Collectible() {
        saturationValue = 0;
        foodType = foodFruit;
        maxSaturation = 100;
        collectibleType = collectibleFood;
    }

    virtual ~Food() {}

    // give the saturation back to the player when eaten
    // child classes can override this if they have extra effects
    virtual void applyToPlayer(PlayerSoldier* player) {
        // TODO call player saturation system addSaturation here
        cout << name << " eaten! Gave back " << saturationValue << " saturation points" << endl;
    }

    virtual void onCollect(PlayerSoldier* player) override {
        if (!isCollected) {
            isCollected = true;
            isActive = false;
            applyToPlayer(player);
        }
    }

    int getSaturationValue() const { return saturationValue; }
    int getFoodType()        const { return foodType; }
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
    Fruit() : Food() {
        strncpy_s(name, "Fruit", sizeof(name));
        foodType = foodFruit;
        saturationValue = 2;       // 2 saturation per spec
        healAmount = 2;
        dropChance = 0.15f;
        width = 24.0f;
        height = 24.0f;
    }

    void onCollect(PlayerSoldier* player) override {
        if (!isCollected) {
            isCollected = true;
            isActive = false;
            applyToPlayer(player);
            cout << "Fruit picked up +2 saturation" << endl;
        }
    }

    void update(float dt) override {
        Food::update(dt);
        // fruit just falls to ground and waits for player to grab it
    }

    void render(sf::RenderWindow& window) override {
        if (isActive && !isCollected) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }

    int   getHealAmount() const { return healAmount; }
    float getDropChance() const { return dropChance; }
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
    Turkey() : Food() {
        strncpy_s(name, "Turkey", sizeof(name));
        foodType = foodTurkey;
        saturationValue = 3;       // 3 saturation per spec, more then fruit
        healAmount = 3;
        dropChance = 0.10f;   // bit rarer then fruit
        width = 32.0f;
        height = 32.0f;
    }

    void onCollect(PlayerSoldier* player) override {
        if (!isCollected) {
            isCollected = true;
            isActive = false;
            applyToPlayer(player);
            cout << "Turkey picked up +3 saturation" << endl;
        }
    }

    void update(float dt) override {
        Food::update(dt);
    }

    void render(sf::RenderWindow& window) override {
        if (isActive && !isCollected) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }

    int   getHealAmount() const { return healAmount; }
    float getDropChance() const { return dropChance; }
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

    sf::RectangleShape promptBox;

public:
    SupplyCrate() : Collectible() {
        strncpy_s(name, "Supply Crate", sizeof(name));
        collectibleType = collectibleCrate;
        weaponInside = nullptr;
        weaponType = NONE;
        handGrenadeCount = 0;
        fireBombGrenadeCount = 0;
        isOpened = false;
        pickupPromptActive = false;
        pickupRange = 80.0f;
        pickupKey = 70;         // F key
        crateSource = crateFromDrop;
        isFromBossBatch = false;
        ammoForCurrentWeapon = 0;
        hasBeenLooted = false;
        width = 48.0f;
        height = 48.0f;

        // crate contents are randomised on creation
        generateWeapon();
        generateGrenades();

        promptBox.setSize(sf::Vector2f(120.0f, 24.0f));
        promptBox.setFillColor(sf::Color(0, 0, 0, 180));
    }

    ~SupplyCrate() {
        // weapon ownership passes to player on pickup so dont delete here
        weaponInside = nullptr;
    }

    // randomly picks weapon per the 90 10 split in the spec
    void generateWeapon() {
        float roll = (float)rand() / RAND_MAX;

        if (roll <= laserGunChance) {
            weaponType = LASERGUN;
            ammoForCurrentWeapon = 30;
        }
        else {
            // pick one of the three heavy weapons randomly
            int pick = rand() % 3;
            if (pick == 0) {
                weaponType = HMG;
                ammoForCurrentWeapon = 200;
            }
            else if (pick == 1) {
                weaponType = FLAMESHOT;
                ammoForCurrentWeapon = 100;
            }
            else {
                weaponType = ROCKETLAUNCHER;
                ammoForCurrentWeapon = 10;
            }
        }
        // TODO new up the actual weapon object once we know which character opens it
    }

    // randomly fills the crate with hand grenades or fire bombs
    void generateGrenades() {
        int type = determineGrenadeType();
        if (type == HANDGRENADE) {
            handGrenadeCount = handGrenadeMin + rand() % (handGrenadeMax - handGrenadeMin + 1);
        }
        else {
            fireBombGrenadeCount = fireBombMin + rand() % (fireBombMax - fireBombMin + 1);
        }
    }

    // pick hand grenade or firebomb at random, eri will convert later anyway
    int determineGrenadeType() {
        return (rand() % 2 == 0) ? HANDGRENADE : FIREBOMBGRENADE;
    }

    // eri always gets fire bombs instead of hand grenades per the spec
    // so we convert whatever was in here to fire bombs for her
    void convertGrenadesForEri() {
        if (handGrenadeCount > 0) {
            fireBombGrenadeCount = max(1, handGrenadeCount / 2);
            handGrenadeCount = 0;
            cout << "Grenades converted to fire bombs for Eri count " << fireBombGrenadeCount << endl;
        }
    }

    // boss batch crates are spawned after minion waves, always have extra ammo
    // per spec after every batch kill a crate spawns to replenish the player
    void spawnFromBossBatch() {
        isFromBossBatch = true;
        crateSource = crateFromBoss;
        ammoForCurrentWeapon *= 2; // boss crates give double the ammo
    }

    // show the press F to open prompt if player is close enough
    void showPickupPrompt(sf::RenderWindow& window, float playerX, float playerY) {
        float dx = playerX - x;
        float dy = playerY - y;
        float distance = sqrt(dx * dx + dy * dy);
        pickupPromptActive = (distance <= pickupRange && !isOpened);

        if (pickupPromptActive) {
            promptBox.setPosition(x - 10.0f, y - 30.0f);
            window.draw(promptBox);
            // TODO draw "Press F" text on top of the box once font is loaded
        }
    }

    // player opens the crate, shows whats inside
    void openCrate() {
        if (!isOpened && !hasBeenLooted) {
            isOpened = true;
            cout << "Crate opened! Weapon type " << weaponType << endl;
            cout << "Hand grenades " << handGrenadeCount << " Fire bombs " << fireBombGrenadeCount << endl;
        }
    }

    // player takes the weapon inside, returns pointer and clears it from crate
    Weapon* pickupWeapon() {
        if (isOpened && weaponInside && !hasBeenLooted) {
            hasBeenLooted = true;
            Weapon* taken = weaponInside;
            weaponInside = nullptr;
            return taken;
        }
        return nullptr;
    }

    // player doesnt want the weapon so just refill their current weapon ammo
    int refillAmmo() {
        if (isOpened && !hasBeenLooted) {
            hasBeenLooted = true;
            return ammoForCurrentWeapon;
        }
        return 0;
    }

    int getAmmoForCurrentWeapon() const { return ammoForCurrentWeapon; }

    void onCollect(PlayerSoldier* player) override {
        if (!isCollected && !isOpened) {
            openCrate();
        }
    }

    void update(float dt) override {
        Collectible::update(dt);
        // crates dont move once on the ground, just sit there
    }

    void render(sf::RenderWindow& window) override {
        if (isActive && !hasBeenLooted) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }

    Weapon* getWeaponInside()       const { return weaponInside; }
    int     getWeaponType()         const { return weaponType; }
    int     getHandGrenadeCount()   const { return handGrenadeCount; }
    int     getFireBombCount()      const { return fireBombGrenadeCount; }
    bool    getIsOpened()           const { return isOpened; }
    bool    getHasBeenLooted()      const { return hasBeenLooted; }
    bool    getPickupPromptActive() const { return pickupPromptActive; }
    float   getPickupRange()        const { return pickupRange; }
    bool    getIsFromBossBatch()    const { return isFromBossBatch; }
    int     getCrateSource()        const { return crateSource; }
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
    float        freeRange;        // player must be this close to free him
    int          freeKey;          // key that frees him, E by default
    float        thankYouTimer;
    float        thankYouDuration; // how long the thank you anim plays
    bool         isPlayingThankYou;
    int          spawnBiome;
    SupplyCrate* crateToSpawn;     // the reward crate he drops
    int          prisonerID;
    int          captivityBiome;

    float walkTimer;
    float walkSpeed;   // how fast he runs off screen
    bool  hasVanished;

    sf::RectangleShape freePromptBox;

public:
    POWPrisoner() : Collectible() {
        strncpy_s(name, "POW Prisoner", sizeof(name));
        collectibleType = collectiblePrisoner;
        isFreed = false;
        freeRange = 80.0f;
        freeKey = 69;        // E key ascii 69
        thankYouTimer = 0.0f;
        thankYouDuration = 2.0f;
        isPlayingThankYou = false;
        spawnBiome = biomePlains;
        crateToSpawn = nullptr;
        prisonerID = 0;
        captivityBiome = biomePlains;
        walkTimer = 0.0f;
        walkSpeed = 120.0f;
        hasVanished = false;
        width = 32.0f;
        height = 48.0f;

        freePromptBox.setSize(sf::Vector2f(100.0f, 24.0f));
        freePromptBox.setFillColor(sf::Color(0, 0, 0, 180));
    }

    ~POWPrisoner() {
        // crate is owned by the game world after spawning, dont delete here
        crateToSpawn = nullptr;
    }

    // returns true if player walked close enough to trigger the free prompt
    bool checkIfPlayerNearby(float playerX, float playerY) {
        float dx = playerX - x;
        float dy = playerY - y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance <= freeRange;
    }

    // player pressed the key near the prisoner so free him
    // spawns a crate as a reward and starts the thank you animation
    void freeFromCaptivity(PlayerSoldier* player) {
        if (!isFreed) {
            isFreed = true;
            isPlayingThankYou = true;
            thankYouTimer = thankYouDuration;
            spawnSupplyCrate();
            cout << "POW prisoner freed! Crate is being dropped as reward." << endl;
        }
    }

    // creates the reward crate next to the prisoner when he is freed
    void spawnSupplyCrate() {
        crateToSpawn = new SupplyCrate();
        crateToSpawn->setPosition(x + 20.0f, y);
        cout << "Thank you crate spawned at " << x << " " << y << endl;
        // caller needs to add this crate to the game worlds collectible list
    }

    // play the thank you wave animation for a couple seconds
    // after it finishes the prisoner starts walking off screen
    void playThankYouAnim(float dt) {
        if (isPlayingThankYou) {
            thankYouTimer -= dt;
            if (thankYouTimer <= 0) {
                isPlayingThankYou = false;
                // TODO play a wave animation sprite here
            }
        }
    }

    // prisoner walks to the right and off screen after saying thanks
    void walkAfterFreed(float dt) {
        if (isFreed && !isPlayingThankYou && !hasVanished) {
            x += walkSpeed * dt;
            sprite.setPosition(x, y);
            if (x > 1400.0f) {
                vanish();
            }
        }
    }

    // completely removes the prisoner once he walked far enough
    void vanish() {
        hasVanished = true;
        isActive = false;
        isCollected = true;
        cout << "Prisoner vanished off screen, mission accompleshed" << endl;
    }

    bool isAlreadyFreed() const { return isFreed; }

    void onCollect(PlayerSoldier* player) override {
        // onCollect for prisoner means freeing him not picking him up
        freeFromCaptivity(player);
    }

    void update(float dt) override {
        if (!isFreed) {
            // just stands there looking sad, waiting for player to notice him
        }
        else {
            playThankYouAnim(dt);
            if (!isPlayingThankYou && !hasVanished)
                walkAfterFreed(dt);
        }
    }

    void render(sf::RenderWindow& window) override {
        if (isActive && !hasVanished) {
            sprite.setPosition(x, y);
            window.draw(sprite);

            // draw the fre E prompt above his head if player is nearby
            if (!isFreed) {
                freePromptBox.setPosition(x - 10.0f, y - 30.0f);
                window.draw(freePromptBox);
                // TODO draw "Press E" text on top once font is setup
            }

            // draw the reward crate if it hasnt been picked up yet
            if (crateToSpawn && crateToSpawn->getIsActive()) {
                crateToSpawn->render(window);
            }
        }
    }

    SupplyCrate* getCreatedCrate()      const { return crateToSpawn; }
    bool         getIsFreed()           const { return isFreed; }
    float        getFreeRange()         const { return freeRange; }
    bool         getHasVanished()       const { return hasVanished; }
    int          getPrisonerID()        const { return prisonerID; }
    int          getSpawnBiome()        const { return spawnBiome; }
    bool         getIsPlayingThankYou() const { return isPlayingThankYou; }
    void         setPrisonerID(int id) { prisonerID = id; }
    void         setSpawnBiome(int b) { spawnBiome = b; captivityBiome = b; }
};