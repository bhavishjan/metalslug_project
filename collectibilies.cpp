#include "collectibilies.h"
#include <cstdlib>
#include <cstring>

// SaturationSystem implementations
SaturationSystem::SaturationSystem() {
    maxSaturation = 100;
    currentSaturation = maxSaturation;
    minSaturation = 0;
    depletionRate = 1.0f;
    depletionTimer = 1.0f;
    isStarving = false;
}

void SaturationSystem::update(float dt) {
    depletionTimer -= dt;
    if (depletionTimer <= 0) {
        depletionTimer = 1.0f / depletionRate;
        deplete();
    }
}

void SaturationSystem::addSaturation(int amount) {
    currentSaturation += amount;
    if (currentSaturation > maxSaturation)
        currentSaturation = maxSaturation;
    isStarving = false;
}

void SaturationSystem::deplete() {
    currentSaturation--;
    if (currentSaturation <= minSaturation) {
        currentSaturation = minSaturation;
        isStarving = true;
    }
}

// DropManager implementations
DropManager::DropManager() {
    foodDropChance = 0.25f;
    turkeyDropChance = 0.4f;
    fruitDropChance = 0.6f;
    crateDropChance = 0.05f;
}

int DropManager::onEnemyKilled() {
    if (rollCrateDrop()) return collectibleCrate;
    if (rollFoodDrop())  return collectibleFood;
    return collectibleNone;
}

bool DropManager::rollFoodDrop() {
    float roll = (float)rand() / RAND_MAX;
    return roll <= foodDropChance;
}

bool DropManager::rollCrateDrop() {
    float roll = (float)rand() / RAND_MAX;
    return roll <= crateDropChance;
}

int DropManager::determineFoodType() {
    float roll = (float)rand() / RAND_MAX;
    return (roll <= turkeyDropChance) ? foodTurkey : foodFruit;
}

void DropManager::spawnFood(float x, float y, int type) {
}

void DropManager::spawnCrate(float x, float y, int source) {
}

// Collectible implementations
Collectible::Collectible() {
    name = "";
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
    soundEffect = "";
}

Collectible::~Collectible() {}

bool Collectible::loadTexture(const char* path) {
    if (!texture.loadFromFile(path)) {
        return false;
    }
    sprite.setTexture(texture);
    return true;
}

void Collectible::applyGravity(float dt) {
    if (!isOnGround) {
        velocityY += gravity * dt;
        y += velocityY * dt;
    }
}

bool Collectible::checkCollision(float px, float py, float pw, float ph) const {
    return (x < px + pw && x + width  > px &&
        y < py + ph && y + height > py);
}

void Collectible::activate() {
    isActive = true;
    isCollected = false;
}

void Collectible::deactivate() {
    isActive = false;
}

void Collectible::setPosition(float px, float py) {
    x = px;
    y = py;
    sprite.setPosition(px, py);
}

void Collectible::setOnGround(bool g) {
    isOnGround = g;
    if (g) velocityY = 0;
}

void Collectible::update(float dt) {
    if (!isCollected && isActive)
        applyGravity(dt);
}

void Collectible::render(RenderWindow& window) {
    if (isActive && !isCollected) {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
}

bool Collectible::getIsActive() const {
    return isActive;
}

bool Collectible::getIsCollected() const {
    return isCollected;
}

int Collectible::getCollectibleType() const {
    return collectibleType;
}

float Collectible::getX() const {
    return x;
}

float Collectible::getY() const {
    return y;
}

const char* Collectible::getName() const {
    return name;
}

// Food implementations
Food::Food() : Collectible() {
    saturationValue = 0;
    foodType = foodFruit;
    maxSaturation = 100;
    collectibleType = collectibleFood;
}

Food::~Food() {}

void Food::applyToPlayer(PlayerSoldier* player) {
}

void Food::onCollect(PlayerSoldier* player) {
    if (!isCollected) {
        isCollected = true;
        isActive = false;
        applyToPlayer(player);
    }
}

int Food::getSaturationValue() const {
    return saturationValue;
}

int Food::getFoodType() const {
    return foodType;
}

// Fruit implementations
Fruit::Fruit() : Food() {
    name = "Fruit";
    foodType = foodFruit;
    saturationValue = 2;
    healAmount = 2;
    dropChance = 0.15f;
    width = 24.0f;
    height = 24.0f;
}

void Fruit::onCollect(PlayerSoldier* player) {
    if (!isCollected) {
        isCollected = true;
        isActive = false;
        applyToPlayer(player);
    }
}

void Fruit::update(float dt) {
    Food::update(dt);
}

void Fruit::render(RenderWindow& window) {
    if (isActive && !isCollected) {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
}

int Fruit::getHealAmount() const {
    return healAmount;
}

float Fruit::getDropChance() const {
    return dropChance;
}

// Turkey implementations
Turkey::Turkey() : Food() {
    name = "Turkey";
    foodType = foodTurkey;
    saturationValue = 3;
    healAmount = 3;
    dropChance = 0.10f;
    width = 32.0f;
    height = 32.0f;
}

void Turkey::onCollect(PlayerSoldier* player) {
    if (!isCollected) {
        isCollected = true;
        isActive = false;
        applyToPlayer(player);
    }
}

void Turkey::update(float dt) {
    Food::update(dt);
}

void Turkey::render(RenderWindow& window) {
    if (isActive && !isCollected) {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
}

int Turkey::getHealAmount() const {
    return healAmount;
}

float Turkey::getDropChance() const {
    return dropChance;
}

// SupplyCrate implementations
SupplyCrate::SupplyCrate() : Collectible() {
    name = "Supply Crate";
    collectibleType = collectibleCrate;
    weaponInside = nullptr;
    weaponType = NONE;
    handGrenadeCount = 0;
    fireBombGrenadeCount = 0;
    isOpened = false;
    pickupPromptActive = false;
    pickupRange = 80.0f;
    pickupKey = 70;
    crateSource = crateFromDrop;
    isFromBossBatch = false;
    ammoForCurrentWeapon = 0;
    hasBeenLooted = false;
    width = 48.0f;
    height = 48.0f;

    generateWeapon();
    generateGrenades();

    promptBox.setSize(Vector2f(120.0f, 24.0f));
    promptBox.setFillColor(Color(0, 0, 0, 180));
}

SupplyCrate::~SupplyCrate() {
    weaponInside = nullptr;
}

void SupplyCrate::generateWeapon() {
    float roll = (float)rand() / RAND_MAX;

    if (roll <= laserGunChance) {
        weaponType = LASERGUN;
        ammoForCurrentWeapon = 30;
    }
    else {
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
}

void SupplyCrate::generateGrenades() {
    int type = determineGrenadeType();
    if (type == HANDGRENADE) {
        handGrenadeCount = handGrenadeMin + rand() % (handGrenadeMax - handGrenadeMin + 1);
    }
    else {
        fireBombGrenadeCount = fireBombMin + rand() % (fireBombMax - fireBombMin + 1);
    }
}

int SupplyCrate::determineGrenadeType() {
    return (rand() % 2 == 0) ? HANDGRENADE : FIREBOMBGRENADE;
}

void SupplyCrate::convertGrenadesForEri() {
    if (handGrenadeCount > 0) {
        fireBombGrenadeCount = max(1, handGrenadeCount / 2);
        handGrenadeCount = 0;
    }
}

void SupplyCrate::spawnFromBossBatch() {
    isFromBossBatch = true;
    crateSource = crateFromBoss;
    ammoForCurrentWeapon *= 2;
}

void SupplyCrate::showPickupPrompt(RenderWindow& window, float playerX, float playerY) {
    float dx = playerX - x;
    float dy = playerY - y;
    float distance = sqrt(dx * dx + dy * dy);
    pickupPromptActive = (distance <= pickupRange && !isOpened);

    if (pickupPromptActive) {
        promptBox.setPosition(x - 10.0f, y - 30.0f);
        window.draw(promptBox);
    }
}

void SupplyCrate::openCrate() {
    if (!isOpened && !hasBeenLooted) {
        isOpened = true;
    }
}

Weapon* SupplyCrate::pickupWeapon() {
    if (isOpened && weaponInside && !hasBeenLooted) {
        hasBeenLooted = true;
        Weapon* taken = weaponInside;
        weaponInside = nullptr;
        return taken;
    }
    return nullptr;
}

int SupplyCrate::refillAmmo() {
    if (isOpened && !hasBeenLooted) {
        hasBeenLooted = true;
        return ammoForCurrentWeapon;
    }
    return 0;
}

void SupplyCrate::onCollect(PlayerSoldier* player) {
    if (!isCollected && !isOpened) {
        openCrate();
    }
}

void SupplyCrate::update(float dt) {
    Collectible::update(dt);
}

void SupplyCrate::render(RenderWindow& window) {
    if (isActive && !hasBeenLooted) {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
}

int SupplyCrate::getAmmoForCurrentWeapon() const {
    return ammoForCurrentWeapon;
}

Weapon* SupplyCrate::getWeaponInside() const {
    return weaponInside;
}

int SupplyCrate::getWeaponType() const {
    return weaponType;
}

int SupplyCrate::getHandGrenadeCount() const {
    return handGrenadeCount;
}

int SupplyCrate::getFireBombCount() const {
    return fireBombGrenadeCount;
}

bool SupplyCrate::getIsOpened() const {
    return isOpened;
}

bool SupplyCrate::getHasBeenLooted() const {
    return hasBeenLooted;
}

bool SupplyCrate::getPickupPromptActive() const {
    return pickupPromptActive;
}

float SupplyCrate::getPickupRange() const {
    return pickupRange;
}

bool SupplyCrate::getIsFromBossBatch() const {
    return isFromBossBatch;
}

int SupplyCrate::getCrateSource() const {
    return crateSource;
}

// POWPrisoner implementations
POWPrisoner::POWPrisoner() : Collectible() {
    name = "POW Prisoner";
    collectibleType = collectiblePrisoner;
    isFreed = false;
    freeRange = 80.0f;
    freeKey = 69;
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

    freePromptBox.setSize(Vector2f(100.0f, 24.0f));
    freePromptBox.setFillColor(Color(0, 0, 0, 180));
}

POWPrisoner::~POWPrisoner() {
    crateToSpawn = nullptr;
}

bool POWPrisoner::checkIfPlayerNearby(float playerX, float playerY) {
    float dx = playerX - x;
    float dy = playerY - y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance <= freeRange;
}

void POWPrisoner::freeFromCaptivity(PlayerSoldier* player) {
    if (!isFreed) {
        isFreed = true;
        isPlayingThankYou = true;
        thankYouTimer = thankYouDuration;
        spawnSupplyCrate();
    }
}

void POWPrisoner::spawnSupplyCrate() {
    crateToSpawn = new SupplyCrate();
    crateToSpawn->setPosition(x + 20.0f, y);
}

void POWPrisoner::playThankYouAnim(float dt) {
    if (isPlayingThankYou) {
        thankYouTimer -= dt;
        if (thankYouTimer <= 0) {
            isPlayingThankYou = false;
        }
    }
}

void POWPrisoner::walkAfterFreed(float dt) {
    if (isFreed && !isPlayingThankYou && !hasVanished) {
        x += walkSpeed * dt;
        sprite.setPosition(x, y);
        if (x > 1400.0f) {
            vanish();
        }
    }
}

void POWPrisoner::vanish() {
    hasVanished = true;
    isActive = false;
    isCollected = true;
}

void POWPrisoner::onCollect(PlayerSoldier* player) {
    freeFromCaptivity(player);
}

void POWPrisoner::update(float dt) {
    if (!isFreed) {
    }
    else {
        playThankYouAnim(dt);
        if (!isPlayingThankYou && !hasVanished)
            walkAfterFreed(dt);
    }
}

void POWPrisoner::render(RenderWindow& window) {
    if (isActive && !hasVanished) {
        sprite.setPosition(x, y);
        window.draw(sprite);

        if (!isFreed) {
            freePromptBox.setPosition(x - 10.0f, y - 30.0f);
            window.draw(freePromptBox);
        }

        if (crateToSpawn && crateToSpawn->getIsActive()) {
            crateToSpawn->render(window);
        }
    }
}

void POWPrisoner::setPrisonerID(int id) {
    prisonerID = id;
}

void POWPrisoner::setSpawnBiome(int b) {
    spawnBiome = b;
    captivityBiome = b;
}

SupplyCrate* POWPrisoner::getCreatedCrate() const {
    return crateToSpawn;
}

bool POWPrisoner::getIsFreed() const {
    return isFreed;
}

float POWPrisoner::getFreeRange() const {
    return freeRange;
}

bool POWPrisoner::getHasVanished() const {
    return hasVanished;
}

int POWPrisoner::getPrisonerID() const {
    return prisonerID;
}

int POWPrisoner::getSpawnBiome() const {
    return spawnBiome;
}

bool POWPrisoner::getIsPlayingThankYou() const {
    return isPlayingThankYou;
}

bool POWPrisoner::isAlreadyFreed() const {
    return isFreed;
}
