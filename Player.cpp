    #include "Player.h"
#include "Weapon.h"
#include "Vehicle.h"
#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace std;
using namespace sf;

Player::Player() {
    x = 100.0f;
    y = 500.0f;
    width = 50.0f;
    height = 80.0f;
    velocityX = 0;
    velocityY = 0;
    hp = 100;
    maxHp = 100;
    lives = 3;
    speed = 5.0f;
    isGrounded = false;
    isCrouching = false;
    isJumping = false;
    grenadeCount = 10;
    grenadeType = 0;
    currentWeapon = nullptr;
    pistol = nullptr;
    knife = nullptr;
    facingRight = true;
    aimAngle = 0;
    state = 0;
    damageState = 0;
    damageTimer = 0;
    stateTimer = 0;
    meleeCooldown = 0.5f;
    meleeTimer = 0;
    inVehicle = false;
    currentVehicle = nullptr;
    isAlive = true;
    isImmortal = false;
    isFat = false;
    isInWater = false;
    score = 0;
    saturation = 100;
    specialPowerActive = false;
    specialPowerTimer = 0;
    specialPowerDuration = 10.0f;
    weaponSlots[0] = nullptr;
    weaponSlots[1] = nullptr;
    activeWeaponIndex = 0;
}

Player::~Player() {
}

void Player::move() {
    if (!inVehicle) {
        x += velocityX;
        y += velocityY;
    }
}

void Player::jump() {
    if (isGrounded && !inVehicle) {
        velocityY = -12.0f;
        isJumping = true;
        isGrounded = false;
    }
}

void Player::crouch() {
    if (!inVehicle) {
        isCrouching = true;
        height = 50.0f;
    }
}

void Player::shoot() {
    if (currentWeapon && !inVehicle) {
        cout << "Player shoots" << endl;
    }
}

void Player::meleeAttack() {
    if (meleeTimer <= 0 && !inVehicle) {
        cout << "Player melee attack" << endl;
        meleeTimer = meleeCooldown;
    }
}

void Player::throwGrenade() {
    if (grenadeCount > 0 && !inVehicle) {
        cout << "Player throws grenade" << endl;
        grenadeCount--;
    }
}

void Player::takeDamage(int damage) {
    if (!isImmortal && isAlive) {
        hp -= damage;
        if (hp <= 0) {
            hp = 0;
            die();
        }
        updateDamageState();
    }
}

void Player::takeMeleeDamage(int damage) {
    takeDamage(damage);
}

void Player::die() {
    isAlive = false;
    lives--;
    if (lives > 0) {
        respawn();
    }
}

void Player::respawn() {
    hp = maxHp;
    isAlive = true;
    x = 100.0f;
    y = 500.0f;
    velocityX = 0;
    velocityY = 0;
}

void Player::pickupWeapon(Weapon* weapon) {
    currentWeapon = weapon;
}

void Player::pickupFood(int saturationValue) {
    saturation += saturationValue;
    if (saturation > 100) {
        saturation = 100;
    }
}

void Player::enterVehicle(Vehicle* vehicle) {
    currentVehicle = vehicle;
    inVehicle = true;
}

void Player::exitVehicle() {
    currentVehicle = nullptr;
    inVehicle = false;
}

void Player::switchWeapon() {
    activeWeaponIndex = (activeWeaponIndex + 1) % 2;
    currentWeapon = weaponSlots[activeWeaponIndex];
}

void Player::updateDamageState() {
    float healthPercent = (float)hp / maxHp;
    if (healthPercent > 0.6f) {
        damageState = 0;
    }
    else if (healthPercent > 0.3f) {
        damageState = 1;
    }
    else {
        damageState = 2;
    }
}

void Player::updateState() {
    if (stateTimer > 0) {
        stateTimer -= 0.016f;
    }
}

void Player::becomeUndead() {
    state = 1;
    stateTimer = 10.0f;
    speed *= 0.7f;
}

void Player::becomeMummy() {
    state = 2;
    stateTimer = 10.0f;
    currentWeapon = nullptr;
}

void Player::revertState() {
    state = 0;
    stateTimer = 0;
}

void Player::freePrisoner() {
    cout << "Player frees prisoner" << endl;
}

void Player::aim(float angle) {
    aimAngle = angle;
}

void Player::applyFannumTax() {
    if (isFat) {
        width *= 1.5f;
        height *= 1.5f;
    }
}

void Player::applyGravity() {
    if (!isGrounded && !inVehicle) {
        velocityY += 0.5f;
    }
}

void Player::checkGrounded() {
    if (y >= 700) {
        y = 700;
        velocityY = 0;
        isGrounded = true;
        isJumping = false;
    }
    else {
        isGrounded = false;
    }
}

void Player::checkCollision() {
}

void Player::enterWater() {
    isInWater = true;
    speed *= 0.5f;
}

void Player::exitWater() {
    isInWater = false;
    speed *= 2.0f;
}

float Player::getDistanceFrom(float targetX, float targetY) {
    float dx = x - targetX;
    float dy = y - targetY;
    return sqrt(dx * dx + dy * dy);
}

bool Player::isNearEnemy() {
    return false;
}

bool Player::isNearPrisoner() {
    return false;
}

void Player::updateInputHistory() {
}

void Player::update() {
    applyGravity();
    move();
    checkGrounded();
    updateState();

    if (meleeTimer > 0) {
        meleeTimer -= 0.016f;
    }

    if (specialPowerTimer > 0) {
        specialPowerTimer -= 0.016f;
        if (specialPowerTimer <= 0) {
            specialPowerActive = false;
        }
    }
}

void Player::render() {
	cout << "Render player at (" << x << ", " << y << ")" << endl;
}

float Player::getFireRate() {
    return 0.2f;
}

float Player::getBlastRadius() {
    return 50.0f;
}

int Player::getGrenadeCount() {
    return grenadeCount;
}

float Player::getMeleeRange() {
    return 30.0f;
}

int Player::getMeleeDamage() {
    return 20;
}

bool Player::isShielded() {
    return false;
}

void Player::applyVehicleBonus() {
}

void Player::removeVehicleBonus() {
}

bool Player::getIsAlive() const {
    return isAlive;
}

Marco::Marco() : Player() {
    name[0] = 'M'; name[1] = 'a'; name[2] = 'r'; name[3] = 'c'; name[4] = 'o'; name[5] = '\0';
    dualFireActive = false;
    dualFireTimer = 0;
    dualFireDuration = 10.0f;
}

void Marco::shoot() {
    if (dualFireActive) {
        cout << "Marco dual fires!" << endl;
    }
    else {
        Player::shoot();
    }
}

void Marco::meleeAttack() {
    cout << "Marco melee attack" << endl;
    Player::meleeAttack();
}

void Marco::activateSpecialPower() {
    dualFireActive = true;
    dualFireTimer = dualFireDuration;
    specialPowerActive = true;
    specialPowerTimer = dualFireDuration;
}

void Marco::updateDualFire() {
    if (dualFireTimer > 0) {
        dualFireTimer -= 0.016f;
        if (dualFireTimer <= 0) {
            dualFireActive = false;
        }
    }
}

float Marco::getFireRate() {
    return dualFireActive ? 0.1f : 0.2f;
}

int Marco::getGrenadeCount() {
    return grenadeCount * 2;
}

void Marco::applyVehicleBonus() {
    speed += 2.0f;
}

Tarma::Tarma() : Player() {
    name[0] = 'T'; name[1] = 'a'; name[2] = 'r'; name[3] = 'm'; name[4] = 'a'; name[5] = '\0';
    immunityActive = false;
    immunityTimer = 0;
    immunityDuration = 5.0f;
    vehicleFireRateBonus = 1.5f;
    vehicleDurabilityBonus = 1.3f;
}

void Tarma::move() {
    Player::move();
}

void Tarma::takeDamage(int damage) {
    if (immunityActive) {
        cout << "Tarma is immune!" << endl;
        return;
    }
    Player::takeDamage(damage);
}

void Tarma::enterVehicle(Vehicle* vehicle) {
    Player::enterVehicle(vehicle);
    applyVehicleBonus();
}

void Tarma::exitVehicle() {
    removeVehicleBonus();
    Player::exitVehicle();
}

void Tarma::onVehicleDestroyed() {
    activateSpecialPower();
}

void Tarma::activateSpecialPower() {
    immunityActive = true;
    immunityTimer = immunityDuration;
    specialPowerActive = true;
    specialPowerTimer = immunityDuration;
}

void Tarma::updateImmunity() {
    if (immunityTimer > 0) {
        immunityTimer -= 0.016f;
        if (immunityTimer <= 0) {
            immunityActive = false;
        }
    }
}

bool Tarma::isInVehicle() {
    return inVehicle;
}

float Tarma::getVehicleFireRate() {
    return 0.2f * vehicleFireRateBonus;
}

float Tarma::getVehicleDurability() {
    return vehicleDurabilityBonus;
}

Eri::Eri() : Player() {
    name[0] = 'E'; name[1] = 'r'; name[2] = 'i'; name[3] = '\0';
    doubleGrenadeActive = false;
    doubleGrenadeTimer = 0;
    doubleGrenadeDuration = 10.0f;
}

void Eri::throwGrenade() {
    if (doubleGrenadeActive && grenadeCount >= 2) {
        cout << "Eri throws double grenades!" << endl;
        grenadeCount -= 2;
        spawnFirePool();
    }
    else {
        Player::throwGrenade();
    }
}

void Eri::activateSpecialPower() {
    doubleGrenadeActive = true;
    doubleGrenadeTimer = doubleGrenadeDuration;
    specialPowerActive = true;
    specialPowerTimer = doubleGrenadeDuration;
}

void Eri::updateDoubleGrenade() {
    if (doubleGrenadeTimer > 0) {
        doubleGrenadeTimer -= 0.016f;
        if (doubleGrenadeTimer <= 0) {
            doubleGrenadeActive = false;
        }
    }
}

void Eri::spawnFirePool() {
    cout << "Fire pool created!" << endl;
}

int Eri::getGrenadeCount() {
    return doubleGrenadeActive ? grenadeCount * 2 : grenadeCount;
}

float Eri::getBlastRadius() {
    return doubleGrenadeActive ? 80.0f : 50.0f;
}

void Eri::meleeAttack() {
    cout << "Eri melee attack" << endl;
    Player::meleeAttack();
}

float Eri::getFireRate() {
    return 0.25f;
}

Fio::Fio() : Player() {
    name[0] = 'F'; name[1] = 'i'; name[2] = 'o'; name[3] = '\0';
    superchargedActive = false;
    superchargedTimer = 0;
    superchargedDuration = 12.0f;
}

void Fio::pickupWeapon(Weapon* weapon) {
    Player::pickupWeapon(weapon);
    if (superchargedActive) {
        cout << "Fio gets supercharged ammo!" << endl;
    }
}

void Fio::activateSpecialPower() {
    superchargedActive = true;
    superchargedTimer = superchargedDuration;
    specialPowerActive = true;
    specialPowerTimer = superchargedDuration;
}

void Fio::updateSupercharged() {
    if (superchargedTimer > 0) {
        superchargedTimer -= 0.016f;
        if (superchargedTimer <= 0) {
            superchargedActive = false;
        }
    }
}

float Fio::getFireRate() {
    return superchargedActive ? 0.1f : 0.2f;
}

void Fio::meleeAttack() {
    cout << "Fio melee attack" << endl;
    Player::meleeAttack();
}

int Fio::getGrenadeCount() {
    return superchargedActive ? grenadeCount + 5 : grenadeCount;
}

FusionCompanion::FusionCompanion() {
    isActive = false;
    isSpawned = false;
    isImmortal = true;
    x = 0;
    y = 0;
    width = 60.0f;
    height = 90.0f;
    hp = 200;
    maxHp = 200;
    speed = 4.0f;
    fireRate = 0.15f;
    damage = 25;
    grenadeCount = 20;
    blastRadius = 60.0f;
    fusedCount = 0;
    spawnDuration = 30.0f;
    spawnTimer = 0;
    cooldownTimer = 0;
    cooldownDuration = 60.0f;
    spriteID = 0;
    followDistance = 100.0f;
    averagedFireRate = 0;
    averagedSpeed = 0;
    averagedDamage = 0;
    averagedGrenadeCount = 0;
    averagedBlastRadius = 0;

    for (int i = 0; i < 4; i++) {
        fusedCharacters[i] = nullptr;
    }
}

FusionCompanion::~FusionCompanion() {
}

void FusionCompanion::computeAveragedStats() {
    averageStats();
}

void FusionCompanion::inheritBuffsWeaknesses() {
}

void FusionCompanion::followPlayer() {
    cout << "Fusion companion follows player" << endl;
}

void FusionCompanion::attackNearestEnemy() {
    cout << "Fusion companion attacks enemy" << endl;
}

void FusionCompanion::findNearestEnemy() {
}

void FusionCompanion::moveTowardsPlayer() {
    followPlayer();
}

void FusionCompanion::moveTowardsEnemy() {
}

bool FusionCompanion::isExpired() {
    return spawnTimer <= 0;
}

void FusionCompanion::update() {
    if (isActive && isSpawned) {
        spawnTimer -= 0.016f;
        if (isExpired()) {
            despawn();
        }
        followPlayer();
        attackNearestEnemy();
    }

    if (cooldownTimer > 0) {
        cooldownTimer -= 0.016f;
    }
}

void FusionCompanion::render() {
    if (isSpawned) {
        cout << "Rendering Fusion Companion at (" << x << ", " << y << ")" << endl;
    }
}

void FusionCompanion::spawn() {
    isActive = true;
    isSpawned = true;
    spawnTimer = spawnDuration;
}

void FusionCompanion::despawn() {
    isActive = false;
    isSpawned = false;
    startCooldown();
}

void FusionCompanion::fuseCharacters(Player** characters, int count) {
    fusedCount = count;
    for (int i = 0; i < count && i < 4; i++) {
        fusedCharacters[i] = characters[i];
    }
    averageStats();
}

void FusionCompanion::averageStats() {
    if (fusedCount == 0) return;

    float totalFireRate = 0;
    float totalSpeed = 0;
    int totalDamage = 0;
    int totalGrenades = 0;
    float totalBlastRadius = 0;

    for (int i = 0; i < fusedCount; i++) {
        if (fusedCharacters[i]) {
            totalFireRate += fusedCharacters[i]->getFireRate();
            totalGrenades += fusedCharacters[i]->getGrenadeCount();
            totalBlastRadius += fusedCharacters[i]->getBlastRadius();
        }
    }

    averagedFireRate = totalFireRate / fusedCount;
    averagedGrenadeCount = totalGrenades / fusedCount;
    averagedBlastRadius = totalBlastRadius / fusedCount;
}

void FusionCompanion::inheritBuffs() {
}

void FusionCompanion::inheritWeaknesses() {
}

void FusionCompanion::fire() {
    cout << "Fusion companion fires" << endl;
}

void FusionCompanion::throwGrenade() {
    if (grenadeCount > 0) {
        cout << "Fusion companion throws grenade" << endl;
        grenadeCount--;
    }
}

bool FusionCompanion::canSpawn() {
    return !isActive && cooldownTimer <= 0;
}

void FusionCompanion::startCooldown() {
    cooldownTimer = cooldownDuration;
}

void FusionCompanion::updateCooldown() {
    if (cooldownTimer > 0) {
        cooldownTimer -= 0.016f;
    }
}

bool FusionCompanion::isCooldownExpired() {
    return cooldownTimer <= 0;
}

bool FusionCompanion::isSpawnExpired() {
    return spawnTimer <= 0;
}

void FusionCompanion::resetForNewLevel() {
    despawn();
    cooldownTimer = 0;
}

float FusionCompanion::getSurvivalCooldown() {
    return 60.0f;
}

float FusionCompanion::getCampaignCooldown() {
    return 120.0f;
}

void FusionCompanion::renderFollowPath() {
}

CharacterManager::CharacterManager() {
    characters[0] = new Marco();
    characters[1] = new Tarma();
    characters[2] = new Eri();
    characters[3] = new Fio();
    activeIndex = 0;
    fusionCompanion = new FusionCompanion();
    fusionCooldownTimer = 0;
    activeCharacterCount = 4;
}

void CharacterManager::switchCharacter() {
    int nextIndex = getNextAvailable();
    if (nextIndex != -1) {
        activeIndex = nextIndex;
    }
}

Player* CharacterManager::getActivePlayer() {
    return characters[activeIndex];
}

bool CharacterManager::isCharacterAvailable(int index) {
    if (index < 0 || index >= 4) return false;
    return characters[index] && characters[index]->getIsAlive();
}

int CharacterManager::getNextAvailable() {
    for (int i = 0; i < 4; i++) {
        int nextIndex = (activeIndex + i + 1) % 4;
        if (isCharacterAvailable(nextIndex)) {
            return nextIndex;
        }
    }
    return -1;
}

bool CharacterManager::allDead() {
    for (int i = 0; i < 4; i++) {
        if (characters[i] && characters[i]->getIsAlive()) {
            return false;
        }
    }
    return true;
}

void CharacterManager::resetAll() {
    for (int i = 0; i < 4; i++) {
        if (characters[i]) {
            characters[i]->respawn();
        }
    }
    activeIndex = 0;
}

void CharacterManager::spawnFusion() {
    if (fusionCompanion->canSpawn()) {
        Player* aliveCharacters[4];
        int count = 0;
        for (int i = 0; i < 4; i++) {
            if (characters[i] && characters[i]->getIsAlive()) {
                aliveCharacters[count++] = characters[i];
            }
        }
        fusionCompanion->fuseCharacters(aliveCharacters, count);
        fusionCompanion->spawn();
    }
}

void CharacterManager::update() {
    if (characters[activeIndex]) {
        characters[activeIndex]->update();
    }

    if (fusionCompanion) {
        fusionCompanion->update();
    }
}

void CharacterManager::render() {
    if (characters[activeIndex]) {
        characters[activeIndex]->render();
    }

    if (fusionCompanion) {
        fusionCompanion->render();
    }
}