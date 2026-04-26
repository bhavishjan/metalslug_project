#include <iostream>
#include "Vehicle.h"
#include "Player.h"

using namespace std;

Vehicle::Vehicle() {
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

Vehicle::~Vehicle() {
    delete[] name;
    delete[] soundEffect;
}

void Vehicle::move() {
    x += velocityX;
    y += velocityY;

    if (isAffectedByGravity) {
        velocityY += gravity;
    }
}

void Vehicle::takeDamage(int damage) {
    if (!isDestroyed) {
        hp -= damage;
        if (hp <= 0) {
            hp = 0;
            destroy();
        }
    }
}

void Vehicle::destroy() {
    isDestroyed = true;
    isActive = false;
    if (hasPilot) {
        ejectPilot();
    }
}

bool Vehicle::isAlive() {
    return !isDestroyed && hp > 0;
}

int Vehicle::getHp() {
    return hp;
}

float Vehicle::getSpeed() {
    return speed;
}

void Vehicle::mountPilot(Player* Player) {
    pilot = Player;
    hasPilot = true;
}

void Vehicle::ejectPilot() {
    pilot = nullptr;
    hasPilot = false;
}

bool Vehicle::hasPilotInside() {
    return hasPilot;
}

void Vehicle::applySoundEffect() {
    if (!isMuffled) {
        cout << "Playing sound: " << soundEffect << endl;
    }
}

void Vehicle::checkCollision() {
    // Basic collision detection
}

int Vehicle::getBiomeType() {
    return biomeType;
}

PlayerVehicle::PlayerVehicle() : Vehicle() {
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

PlayerVehicle::~PlayerVehicle() {
}

bool PlayerVehicle::canFire() {
    return fireTimer <= 0 && ammo > 0 && !isDestroyed;
}

void PlayerVehicle::reload() {
    ammo = maxAmmo;
}

void PlayerVehicle::takeDamage(int damage) {
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

int PlayerVehicle::getDamageState() {
    return damageState;
}

void PlayerVehicle::updateDamageState() {
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

void PlayerVehicle::renderDamageHue() {
    if (screenRedHue > 0) {
        cout << "Red hue: " << screenRedHue << endl;
    }
}

void PlayerVehicle::activateImmunity() {
    isImmuneToExplosions = true;
    immunityTimer = immunityDuration;
}

void PlayerVehicle::deactivateImmunity() {
    isImmuneToExplosions = false;
    immunityTimer = 0;
}

bool PlayerVehicle::isImmune() {
    return isImmuneToExplosions;
}

EnemyVehicle::EnemyVehicle() : Vehicle() {
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

EnemyVehicle::~EnemyVehicle() {
}

void EnemyVehicle::detectPlayer() {
    // player detection logic
}

void EnemyVehicle::moveTowardsPlayer() {
    if (targetX > x)
        velocityX = speed;
    else
        velocityX = -speed;
}

void EnemyVehicle::spawnProjectile() {
    cout << "Enemy vehicle fires projectile" << endl;
}

int EnemyVehicle::getScoreValue() {
    return scoreValue;
}

void EnemyVehicle::setGrudgeful() {
    isGrudgeful = true;
    grudgeTimer = 10.0f;
}

void EnemyVehicle::applyPowerBoost() {
    powerMultiplier = 1.5f;
    speed *= powerMultiplier;
}

void EnemyVehicle::onPlayerPassed() {
    hasBeenPassed = true;
    setGrudgeful();
}

void EnemyVehicle::returnWithBoost() {
    applyPowerBoost();
    velocityX = -velocityX;
}

MetalSlug::MetalSlug() : PlayerVehicle() {
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

void MetalSlug::fire() {
    if (canFire()) {
        cout << "Metal Slug fires bullet" << endl;
        ammo--;
        fireTimer = bulletFireRate;
    }
}

void MetalSlug::fireCannon() {
    if (cannonAmmo > 0) {
        cout << "Metal Slug fires cannon" << endl;
        cannonAmmo--;
    }
}

void MetalSlug::tilt() {
    if (canTilt && tiltAngle < maxTiltAngle) {
        tiltAngle += 5.0f;
    }
}

void MetalSlug::jump() {
    if (isGrounded) {
        velocityY = -10.0f;
        isGrounded = false;
    }
}

void MetalSlug::crouch() {
    height = 60;
}

void MetalSlug::throwGrenade() {
    cout << "Metal Slug throws grenade" << endl;
}

void MetalSlug::update() {
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

void MetalSlug::render() {
    cout << "Rendering Metal Slug at (" << x << ", " << y << ")" << endl;
}

void MetalSlug::suicideAttack() {
    triggerWarning();
    cout << "Metal Slug performs suicide attack!" << endl;
}

void MetalSlug::triggerWarning() {
    isWarning = true;
    warningTimer = warningDuration;
}

void MetalSlug::eject() {
    ejectPilot();
    cout << "Pilot ejected from Metal Slug" << endl;
}

SlugFlyer::SlugFlyer() : PlayerVehicle() {
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

void SlugFlyer::fire() {
    if (canFire()) {
        cout << "Slug Flyer fires bullets" << endl;
        bulletAmmo--;
        fireTimer = bulletFireRate;
    }
}

void SlugFlyer::fireMissile() {
    if (missileAmmo > 0) {
        cout << "Slug Flyer fires missile" << endl;
        missileAmmo--;
    }
}

void SlugFlyer::ascend() {
    if (altitude < maxAltitude) {
        altitude += 5.0f;
        y -= 5.0f;
    }
}

void SlugFlyer::descend() {
    if (altitude > minAltitude) {
        altitude -= 5.0f;
        y += 5.0f;
    }
}

void SlugFlyer::hover() {
    velocityY = 0;
    velocityX = 0;
}

void SlugFlyer::update() {
    if (!hoverCapability) {
        move();
    }

    if (fireTimer > 0) {
        fireTimer -= 0.016f;
    }
}

void SlugFlyer::render() {
    cout << "Rendering Slug Flyer at (" << x << ", " << y << ") altitude: " << altitude << endl;
}

SlugMariner::SlugMariner() : PlayerVehicle() {
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

void SlugMariner::fire() {
    fireHorizontalMissile();
}

void SlugMariner::fireHorizontalMissile() {
    if (horizontalMissileAmmo > 0) {
        cout << "Slug Mariner fires horizontal missile" << endl;
        horizontalMissileAmmo--;
    }
}

void SlugMariner::fireVerticalMissile() {
    if (verticalMissileAmmo > 0) {
        cout << "Slug Mariner fires vertical missile" << endl;
        verticalMissileAmmo--;
    }
}

void SlugMariner::fireReverseMissile() {
    if (reverseMissileAmmo > 0) {
        cout << "Slug Mariner fires reverse missile" << endl;
        reverseMissileAmmo--;
    }
}

void SlugMariner::dive() {
    if (depth < maxDepth) {
        depth += 10.0f;
        y += 10.0f;
        isSubmerged = true;
    }
}

void SlugMariner::surface() {
    if (depth > 0) {
        depth -= 10.0f;
        y -= 10.0f;
        if (depth <= 0) {
            isSubmerged = false;
        }
    }
}

void SlugMariner::update() {
    move();

    if (fireTimer > 0) {
        fireTimer -= 0.016f;
    }
}

void SlugMariner::render() {
    cout << "Rendering Slug Mariner at (" << x << ", " << y << ") depth: " << depth << endl;
}

AmphibiousSlug::AmphibiousSlug() : PlayerVehicle() {
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

void AmphibiousSlug::fire() {
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

void AmphibiousSlug::detectTerrainMode() {
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

void AmphibiousSlug::switchToMariner() {
    if (currentMode != 2) {
        currentMode = 2;
        isTransitioning = true;
        transitionTimer = 1.0f;
    }
}

void AmphibiousSlug::switchToFlyer() {
    if (currentMode != 1) {
        currentMode = 1;
        isTransitioning = true;
        transitionTimer = 1.0f;
    }
}

void AmphibiousSlug::switchToSlug() {
    if (currentMode != 0) {
        currentMode = 0;
        isTransitioning = true;
        transitionTimer = 1.0f;
    }
}

int AmphibiousSlug::getCurrentMode() {
    return currentMode;
}

void AmphibiousSlug::onWaterContact() {
    isInWater = true;
    isGrounded = false;
    isFlying = false;
}

void AmphibiousSlug::onGroundContact() {
    isGrounded = true;
    isInWater = false;
    isFlying = false;
}

void AmphibiousSlug::onAirborne() {
    isFlying = true;
    isGrounded = false;
    isInWater = false;
}

void AmphibiousSlug::update() {
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

void AmphibiousSlug::render() {
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

FlyingTara::FlyingTara() : EnemyVehicle() {
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

void FlyingTara::attack() {
    if (isOnTopOfPlayer) {
        dropGrenade();
    }
}

void FlyingTara::dropGrenade() {
    if (grenadeTimer <= 0) {
        cout << "Flying Tara drops grenade" << endl;
        grenadeTimer = grenadeRate;
    }
}

void FlyingTara::hover() {
    y = hoverHeight;
    velocityY = 0;
}

void FlyingTara::checkIfOnTopOfPlayer() {
    // player detection logic
    isOnTopOfPlayer = false;
}

void FlyingTara::patrol() {
    velocityX = horizontalSpeed * patrolDirection;

    if (x > 1500) {
        patrolDirection = -1;
    }
    else if (x < 100) {
        patrolDirection = 1;
    }
}

void FlyingTara::update() {
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

void FlyingTara::render() {
    cout << "Rendering Flying Tara at (" << x << ", " << y << ")" << endl;
}

M15ABradley::M15ABradley() : EnemyVehicle() {
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

void M15ABradley::attack() {
    fireMissile();
}

void M15ABradley::fireMissile() {
    if (missileTimer <= 0) {
        cout << "M-15A Bradley fires missile" << endl;
        missileTimer = missileRate;
    }
}

void M15ABradley::moveHorizontally() {
    if (canMoveHorizontally && !isStationary) {
        velocityX = approachSpeed;
    }
}

void M15ABradley::checkFlatTerrain() {
    // terrain detection logic
    isOnFlatTerrain = true;
}

void M15ABradley::update() {
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

void M15ABradley::render() {
    cout << "Rendering M-15A Bradley at (" << x << ", " << y << ")" << endl;
}

EnemySub::EnemySub() : EnemyVehicle() {
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

void EnemySub::attack() {
    fireRocket();
}

void EnemySub::fireRocket() {
    if (rocketTimer <= 0) {
        cout << "Enemy Sub fires rocket" << endl;
        rocketTimer = rocketRate;
    }
}

void EnemySub::dive() {
    if (depth < maxDepth) {
        depth += 10.0f;
        y += 10.0f;
        isSubmerged = true;
    }
}

void EnemySub::surface() {
    if (depth > 0) {
        depth -= 10.0f;
        y -= 10.0f;
        if (depth <= 0) {
            isSubmerged = false;
        }
    }
}

void EnemySub::update() {
    move();

    if (rocketTimer > 0) {
        rocketTimer -= 0.016f;
    }
    else {
        attack();
    }
}

void EnemySub::render() {
    cout << "Rendering Enemy Sub at (" << x << ", " << y << ") depth: " << depth << endl;
}