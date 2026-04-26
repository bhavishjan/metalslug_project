#include "Weapon.h"
#include "Player.h"
#include <iostream>
#include <cmath>

using namespace std;

Weapon::Weapon() {
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

Weapon::~Weapon() {
}

int Weapon::getDamage() const {
    return damage;
}

void Weapon::setActive(bool active) {
    isActive = active;
}

int Weapon::getOwnerType() const {
    return ownerType;
}

void Weapon::applySoundEffect() {
    if (!isMuffled) {
        cout << "Sound: " << soundEffect << endl;
    }
}

bool Weapon::getIsActive() const {
    return isActive;
}

int Weapon::getWeaponType() const {
    return weaponType;
}

const char* Weapon::getName() const {
    return name;
}

float Weapon::getX() const {
    return x;
}

float Weapon::getY() const {
    return y;
}

void Weapon::setPosition(float px, float py) {
    x = px;
    y = py;
}

void Weapon::setOwnerType(int ot) {
    ownerType = ot;
}

void Weapon::setMuffled(bool m) {
    isMuffled = m;
}

Firearm::Firearm() : Weapon() {
    fireRate = 0.2f;
    fireTimer = 0;
    ammo = 100;
    maxAmmo = 100;
    isInfiniteAmmo = false;
    range = 500.0f;
    projectileSpeed = 10.0f;
}

Firearm::~Firearm() {
}

void Firearm::reload() {
    ammo = maxAmmo;
}

bool Firearm::isEmpty() const {
    return ammo <= 0 && !isInfiniteAmmo;
}

float Firearm::getFireRate() const {
    return fireRate;
}

int Firearm::getAmmo() const {
    return ammo;
}

void Firearm::setAmmo(int amount) {
    ammo = amount;
    if (ammo > maxAmmo) {
        ammo = maxAmmo;
    }
}

bool Firearm::canFire() {
    return fireTimer <= 0 && (ammo > 0 || isInfiniteAmmo);
}

Pistol::Pistol() : Firearm() {
    weaponType = WT_PISTOL;
    isDefault = true;
    damage = 10;
    fireRate = 0.3f;
    ammo = 50;
    maxAmmo = 50;
    isInfiniteAmmo = true;
}

void Pistol::fire(float angle) {
    if (canFire()) {
        cout << "Pistol fires at angle: " << angle << endl;
        if (!isInfiniteAmmo) {
            ammo--;
        }
        fireTimer = fireRate;
    }
}

void Pistol::update(float dt) {
    if (fireTimer > 0) {
        fireTimer -= dt;
    }
}

void Pistol::render() {
    cout << "Rendering Pistol" << endl;
}

bool Pistol::getIsDefault() const {
    return isDefault;
}

Knife::Knife() : Firearm() {
    weaponType = WT_KNIFE;
    meleeRange = 30.0f;
    meleeDamage = 50;
    meleeCooldown = 0.5f;
    meleeTimer = 0;
    isPiercing = false;
    damage = 50;
    isInfiniteAmmo = true;
}

void Knife::fire(float angle) {
    if (canMelee()) {
        cout << "Knife slashes at angle: " << angle << endl;
        meleeTimer = meleeCooldown;
    }
}

void Knife::update(float dt) {
    if (meleeTimer > 0) {
        meleeTimer -= dt;
    }
}

void Knife::render() {
    cout << "Rendering Knife" << endl;
}

bool Knife::isInRange(float targetX, float targetY) const {
    float dx = targetX - x;
    float dy = targetY - y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance <= meleeRange;
}

bool Knife::canMelee() const {
    return meleeTimer <= 0;
}

void Knife::resetCooldown() {
    meleeTimer = 0;
}

float Knife::getMeleeRange() const {
    return meleeRange;
}

int Knife::getMeleeDamage() const {
    return meleeDamage;
}

bool Knife::getIsPiercing() const {
    return isPiercing;
}

void Knife::setPiercing(bool p) {
    isPiercing = p;
}

HeavyMachineGun::HeavyMachineGun() : Firearm() {
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

void HeavyMachineGun::fire(float angle) {
    if (canFire() && currentSpinLevel >= 1.0f) {
        cout << "HMG fires at angle: " << angle << endl;
        ammo--;
        fireTimer = fireRate;
    }
}

void HeavyMachineGun::startFire() {
    isHeldDown = true;
}

void HeavyMachineGun::stopFire() {
    isHeldDown = false;
}

void HeavyMachineGun::update(float dt) {
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

void HeavyMachineGun::render() {
    cout << "Rendering HMG, spin level: " << currentSpinLevel << endl;
}

float HeavyMachineGun::getSpinLevel() const {
    return currentSpinLevel;
}

bool HeavyMachineGun::getIsHeldDown() const {
    return isHeldDown;
}

RocketLauncher::RocketLauncher() : Firearm() {
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

void RocketLauncher::fire(float angle) {
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

void RocketLauncher::reload() {
    isReloading = true;
    reloadTimer = reloadDuration;
}

void RocketLauncher::spawnRocket(float angle) {
    cout << "Rocket launched at angle: " << angle << endl;
}

void RocketLauncher::update(float dt) {
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

void RocketLauncher::render() {
    cout << "Rendering Rocket Launcher" << endl;
}

bool RocketLauncher::isReloadingNow() const {
    return isReloading;
}

float RocketLauncher::getBlastRadius() const {
    return blastRadius;
}

int RocketLauncher::getBlastDamage() const {
    return blastDamage;
}

FlameShot::FlameShot() : Firearm() {
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

void FlameShot::fire(float angle) {
    if (canFire()) {
        startStream();
        cout << "FlameShot fires at angle: " << angle << endl;
    }
}

void FlameShot::startStream() {
    streamActive = true;
    isHeldDown = true;
    streamTimer = maxStreamDuration;
}

void FlameShot::stopStream() {
    streamActive = false;
    isHeldDown = false;
}

void FlameShot::update(float dt) {
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

void FlameShot::render() {
    cout << "Rendering FlameShot" << endl;
}

bool FlameShot::isStreamActive() const {
    return streamActive;
}

int FlameShot::getDamagePerSecond() const {
    return damagePerSecond;
}

float FlameShot::getStreamLength() const {
    return streamLength;
}

LaserGun::LaserGun() : Firearm() {
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

void LaserGun::fire(float angle) {
    if (canFire()) {
        spawnBeam(angle);
        ammo--;
        fireTimer = fireRate;
    }
}

void LaserGun::spawnBeam(float angle) {
    beamActive = true;
    beamTimer = beamDuration;
    cout << "Laser beam fired at angle: " << angle << endl;
}

void LaserGun::update(float dt) {
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

void LaserGun::render() {
    cout << "Rendering Laser Gun" << endl;
}

void LaserGun::deactivateBeam() {
    beamActive = false;
}

bool LaserGun::isBeamActive() const {
    return beamActive;
}

bool LaserGun::getIsInstantKill() const {
    return isInstantKill;
}

FirePool::FirePool() {
    x = 0;
    y = 0;
    radius = 50.0f;
    damagePerSecond = 20;
    duration = 5.0f;
    timer = duration;
    isActive = true;
    ownerType = OT_NONE;
}

FirePool::FirePool(float px, float py, float r, int dps, float dur, int owner) {
    x = px;
    y = py;
    radius = r;
    damagePerSecond = dps;
    duration = dur;
    timer = dur;
    isActive = true;
    ownerType = owner;
}

void FirePool::update(float dt) {
    if (isActive) {
        timer -= dt;
        if (timer <= 0) {
            isActive = false;
        }
    }
}

void FirePool::render() {
    if (isActive) {
        cout << "Rendering fire pool at (" << x << ", " << y << ")" << endl;
    }
}

void FirePool::applyDamage(float targetX, float targetY, float& dmgOut, float dt) {
    if (checkCollision(targetX, targetY, 0, 0)) {
        dmgOut = damagePerSecond * dt;
    }
    else {
        dmgOut = 0;
    }
}

bool FirePool::isExpired() const {
    return timer <= 0;
}

bool FirePool::checkCollision(float tx, float ty, float tw, float th) const {
    float dx = tx - x;
    float dy = ty - y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance <= radius;
}

bool FirePool::getIsActive() const {
    return isActive;
}

float FirePool::getX() const {
    return x;
}

float FirePool::getY() const {
    return y;
}

float FirePool::getRadius() const {
    return radius;
}

int FirePool::getDamagePerSecond() const {
    return damagePerSecond;
}

Grenade::Grenade() : Weapon() {
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

Grenade::~Grenade() {
}

void Grenade::fire(float launchAngle) {
    launch(launchAngle, initialSpeed);
}

bool Grenade::checkCollision(float tx, float ty, float tw, float th) {
    return (x < tx + tw && x + width > tx && y < ty + th && y + height > ty);
}

void Grenade::applyGravity(float dt) {
    velocityY += gravity * dt;
}

void Grenade::applyBlastDamage(float tx, float ty, int& targetHp) {
    if (isInBlastRadius(tx, ty)) {
        targetHp -= damage;
    }
}

bool Grenade::isInBlastRadius(float tx, float ty) const {
    float dx = tx - x;
    float dy = ty - y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance <= blastRadius;
}

void Grenade::launch(float launchAngle, float speed) {
    angle = launchAngle;
    velocityX = cos(launchAngle) * speed;
    velocityY = sin(launchAngle) * speed;
}

void Grenade::setThrownBy(Player* p) {
    thrownByPlayer = p;
}

bool Grenade::getIsExploded() const {
    return isExploded;
}

float Grenade::getBlastRadius() const {
    return blastRadius;
}

float Grenade::getVelocityX() const {
    return velocityX;
}

float Grenade::getVelocityY() const {
    return velocityY;
}

HandGrenade::HandGrenade() : Grenade() {
    weaponType = WT_HAND_GRENADE;
    bounceCount = 0;
    maxBounce = 3;
    bounceVelocityDampen = 0.6f;
    blastRadius = 60.0f;
    damage = 80;
}

void HandGrenade::fire(float angle) {
    Grenade::fire(angle);
}

void HandGrenade::update(float dt) {
    if (!isExploded) {
        applyGravity(dt);
        x += velocityX * dt;
        y += velocityY * dt;

        fuseTimer -= dt;
        if (fuseTimer <= 0) {
            explode();
        }

        if (y >= 700) {
            applyBounce();
        }
    }
}

void HandGrenade::explode() {
    isExploded = true;
    cout << "Hand grenade explodes!" << endl;
}

void HandGrenade::render() {
    if (!isExploded) {
        cout << "Rendering hand grenade at (" << x << ", " << y << ")" << endl;
    }
}

void HandGrenade::applyBounce() {
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

int HandGrenade::getBounceCount() const {
    return bounceCount;
}

FireBombGrenade::FireBombGrenade() : Grenade() {
    weaponType = WT_FIRE_BOMB_GRENADE;
    firePool = nullptr;
    firePoolDuration = 8.0f;
    firePoolRadius = 80.0f;
    firePoolDamagePerSecond = 25;
    blastRadius = 40.0f;
    damage = 60;
}

FireBombGrenade::~FireBombGrenade() {
    if (firePool) {
        delete firePool;
    }
}

void FireBombGrenade::fire(float angle) {
    Grenade::fire(angle);
}

void FireBombGrenade::update(float dt) {
    if (!isExploded) {
        applyGravity(dt);
        x += velocityX * dt;
        y += velocityY * dt;

        fuseTimer -= dt;
        if (fuseTimer <= 0 || y >= 700) {
            explode();
        }
    }

    if (firePool) {
        firePool->update(dt);
    }
}

void FireBombGrenade::explode() {
    isExploded = true;
    spawnFirePool();
    cout << "Fire bomb grenade explodes!" << endl;
}

void FireBombGrenade::spawnFirePool() {
    firePool = new FirePool(x, y, firePoolRadius, firePoolDamagePerSecond, firePoolDuration, ownerType);
}

void FireBombGrenade::render() {
    if (!isExploded) {
        cout << "Rendering fire bomb grenade at (" << x << ", " << y << ")" << endl;
    }

    if (firePool) {
        firePool->render();
    }
}

FirePool* FireBombGrenade::getFirePool() const {
    return firePool;
}