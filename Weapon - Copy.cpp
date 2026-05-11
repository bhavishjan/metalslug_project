#define _CRT_SECURE_NO_WARNINGS
#include "Weapon.h"
#include <cmath>

// Bullet class stuff
Bullet::Bullet() : x(0), y(0), velX(0), velY(0),
    active(false), damage(3), ownerType(NOowner),
    range(400.f), distanceTravelled(0) {
    shape.setSize({ 6.f, 3.f });
    shape.setFillColor(Color::Yellow);
    shape.setOrigin(3.f, 1.5f);
}

void Bullet::spawn(float startX, float startY, float angle,
    int dmg, int owner, float speed, float maxRange,
    const Color& bulletColor)
{
    x = startX;
    y = startY;
    velX = cos(angle) * speed;
    velY = sin(angle) * speed;
    damage = dmg;
    ownerType = owner;
    range = maxRange;
    distanceTravelled = 0;
    shape.setFillColor(bulletColor);
    active = true;
}

void Bullet::update(float dt) {
    if (!active) return;
    float dx = velX * dt;
    float dy = velY * dt;
    x += dx;
    y += dy;
    distanceTravelled += sqrt(dx * dx + dy * dy);
    if (distanceTravelled >= range)
        active = false;
}

bool Bullet::checkHit(float tx, float ty, float tw, float th) const {
    if (!active) return false;
    return (x > tx && x < tx + tw && y > ty && y < ty + th);
}

void Bullet::render(RenderWindow& window, float camX, float camY) {
    if (!active) return;
    shape.setPosition(x - camX, y - camY);
    shape.setRotation(atan2(velY, velX) * 180.f / 3.14159f);
    window.draw(shape);
}

void Bullet::setActive(bool a) { active = a; }
bool Bullet::isActive() const { return active; }
void Bullet::deactivate() { active = false; }
int Bullet::getDamage() const { return damage; }
int Bullet::getOwner() const { return ownerType; }
float Bullet::getX() const { return x; }
float Bullet::getY() const { return y; }
float Bullet::getRadius() const { return 3.f; }

// Rocket class stuff
Rocket::Rocket() {
    x = y = velX = velY = 0;
    active = false;
}

void Rocket::setPosition(float px, float py) { x = px; y = py; }
void Rocket::setVelocity(float vx, float vy) { velX = vx; velY = vy; }
void Rocket::setActive(bool s) { active = s; }

bool Rocket::isActive() const { return active; }
float Rocket::getX() const { return x; }
float Rocket::getY() const { return y; }
bool Rocket::getIsExploded() const { return false; }

void Rocket::update(float dt) {
    if (!active) return;
    velY += 980.f * dt;
    x += velX * dt;
    y += velY * dt;
    if (y > 1600.f) active = false;
}

void Rocket::render(RenderWindow& window, float camX, float camY) {
    if (!active) return;
    RectangleShape s({ 12.f, 5.f });
    s.setFillColor(Color(200, 100, 0));
    s.setOrigin(6.f, 2.5f);
    s.setPosition(x - camX, y - camY);
    s.setRotation(atan2(velY, velX) * 180.f / 3.14159f);
    window.draw(s);
}

// Weapon class stuff
Weapon::Weapon() {
    name = "";
    damage = 0;
    isActive = false;
    weaponType = NONE;
    x = 0;
    y = 0;
    ownerType = NOowner;
    soundEffect = "";
    isMuffled = false;
}

Weapon::~Weapon() {}

bool Weapon::loadTexture(const char* path) {
    if (!texture.loadFromFile(path)) {
        return false;
    }
    sprite.setTexture(texture);
    return true;
}

void Weapon::setPosition(float px, float py) {
    x = px;
    y = py;
    sprite.setPosition(px, py);
}

void Weapon::applySoundEffect() {
}

void Weapon::setMuffled(bool m) { isMuffled = m; }
void Weapon::setActive(bool active) { isActive = active; }
void Weapon::setOwnerType(int ot) { ownerType = ot; }

int Weapon::getDamage() const { return damage; }
bool Weapon::getIsActive() const { return isActive; }
int Weapon::getWeaponType() const { return weaponType; }
const char* Weapon::getName() const { return name; }
float Weapon::getX() const { return x; }
float Weapon::getY() const { return y; }
int Weapon::getOwnerType() const { return ownerType; }
Sprite& Weapon::getSprite() { return sprite; }

void Weapon::render(RenderWindow& window) {
    if (isActive) {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
}

// ProjectileWeapon class stuff
ProjectileWeapon::ProjectileWeapon() : Weapon() {
    projectileSpeed = 10.0f;
    range = 500.0f;
    ammo = 0;
    maxAmmo = 0;
    isInfiniteAmmo = false;
    fireRate = 0.25f;
    fireTimer = 0.0f;
}

ProjectileWeapon::~ProjectileWeapon() {}

bool ProjectileWeapon::canFire() {
    return fireTimer <= 0 && (ammo > 0 || isInfiniteAmmo);
}

void ProjectileWeapon::update(float dt) {
    if (fireTimer > 0)
        fireTimer -= dt;
}

bool ProjectileWeapon::isEmpty() const { return ammo <= 0 && !isInfiniteAmmo; }
float ProjectileWeapon::getFireRate() const { return fireRate; }
int ProjectileWeapon::getAmmo() const { return ammo; }

void ProjectileWeapon::setAmmo(int amount) {
    ammo = amount;
    if (ammo > maxAmmo) ammo = maxAmmo;
}

void ProjectileWeapon::reload() { ammo = maxAmmo; }

// Firearm class stuff
Firearm::Firearm() : ProjectileWeapon() {
    range = 500.0f;
    projectileSpeed = 10.0f;
}

Firearm::~Firearm() {}

void Firearm::update(float dt) {
    ProjectileWeapon::update(dt);
}

// Pistol class stuff
Pistol::Pistol() : Firearm() {
    name = "Pistol";
    weaponType = PISTOL;
    isDefault = true;
    damage = 3;
    fireRate = 0.25f;
    ammo = 50;
    maxAmmo = 50;
    isInfiniteAmmo = true;
    isDualFiring = false;
    dualFireTimer = 0.0f;
}

void Pistol::fire(float angle) {
    if (canFire()) {
        fireTimer = fireRate;
    }
}

void Pistol::activateDualFire() {
    isDualFiring = true;
    dualFireTimer = 10.0f;
}

void Pistol::fireDual(float angle) {
    if (isDualFiring && canFire()) {
        fire(angle);
        fire(180.0f - angle);
    }
}

void Pistol::update(float dt) {
    Firearm::update(dt);
    if (isDualFiring) {
        dualFireTimer -= dt;
        if (dualFireTimer <= 0) {
            isDualFiring = false;
            dualFireTimer = 0.0f;
        }
    }
}

void Pistol::render(RenderWindow& window) {
    if (isActive) {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
}

bool Pistol::getIsDefault() const { return isDefault; }
bool Pistol::getIsDualFiring() const { return isDualFiring; }

// Knife class stuff
Knife::Knife() : Firearm() {
    name = "Knife";
    weaponType = KNIFE;
    meleeRange = BLOCK_SIZE;
    meleeDamage = 2;
    meleeCooldown = 0.5f;
    meleeTimer = 0;
    isPiercing = false;
    damage = 2;
    isInfiniteAmmo = true;
}

void Knife::fire(float angle) {
    if (canMelee()) {
        meleeTimer = meleeCooldown;
    }
}

void Knife::update(float dt) {
    if (meleeTimer > 0)
        meleeTimer -= dt;
}

void Knife::render(RenderWindow& window) {
    if (isActive) {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
}

bool Knife::isInRange(float targetX, float targetY) const {
    float dx = targetX - x;
    float dy = targetY - y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance <= meleeRange;
}

bool Knife::canMelee() const { return meleeTimer <= 0; }
void Knife::resetCooldown() { meleeTimer = 0; }
void Knife::setPiercing(bool p) { isPiercing = p; }
float Knife::getMeleeRange() const { return meleeRange; }
int Knife::getMeleeDamage() const { return meleeDamage; }
bool Knife::getIsPiercing() const { return isPiercing; }

// HeavyMachineGun class stuff
HeavyMachineGun::HeavyMachineGun() : Firearm() {
    name = "HMG";
    weaponType = HMG;
    isHeldDown = false;
    spinUpTimer = 0;
    spinUpDuration = 1.0f;
    currentSpinLevel = 0;
    damage = 3;
    fireRate = 0.125f;
    ammo = 200;
    maxAmmo = 200;
}

void HeavyMachineGun::fire(float angle) {
    if (canFire() && currentSpinLevel >= 1.0f) {
        ammo--;
        fireTimer = fireRate;
    }
}

void HeavyMachineGun::startFire() { isHeldDown = true; }
void HeavyMachineGun::stopFire() { isHeldDown = false; }

void HeavyMachineGun::update(float dt) {
    Firearm::update(dt);
    if (isHeldDown) {
        if (currentSpinLevel < 1.0f) {
            spinUpTimer += dt;
            currentSpinLevel = spinUpTimer / spinUpDuration;
            if (currentSpinLevel > 1.0f)
                currentSpinLevel = 1.0f;
        }
    }
    else {
        spinUpTimer = 0;
        currentSpinLevel = 0;
    }
}

void HeavyMachineGun::render(RenderWindow& window) {
    if (isActive) {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
}

float HeavyMachineGun::getSpinLevel() const { return currentSpinLevel; }
bool HeavyMachineGun::getIsHeldDown() const { return isHeldDown; }

// RocketLauncher class stuff
RocketLauncher::RocketLauncher() : Firearm() {
    name = "Rocket Launcher";
    weaponType = ROCKETLAUNCHER;
    reloadTimer = 0;
    reloadDuration = 2.0f;
    blastRadius = 3.0f * BLOCK_SIZE;
    blastDamage = 5;
    isReloading = false;
    rocketSpeed = 8.0f;
    damage = 5;
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

void RocketLauncher::spawnRocket(float angle) {
}

void RocketLauncher::update(float dt) {
    Firearm::update(dt);
    if (isReloading) {
        reloadTimer -= dt;
        if (reloadTimer <= 0) {
            isReloading = false;
            ammo = maxAmmo;
        }
    }
}

void RocketLauncher::render(RenderWindow& window) {
    if (isActive) {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
}

bool RocketLauncher::isReloadingNow() const { return isReloading; }
float RocketLauncher::getBlastRadius() const { return blastRadius; }
int RocketLauncher::getBlastDamage() const { return blastDamage; }

// FlameShot class stuff
FlameShot::FlameShot() : Firearm() {
    name = "Flame Shot";
    weaponType = FLAMESHOT;
    streamLength = 5.0f * BLOCK_SIZE;
    damagePerSecond = 2;
    isHeldDown = false;
    streamActive = false;
    streamTimer = 0;
    maxStreamDuration = 5.0f;
    damage = 2;
    fireRate = 0.1f;
    ammo = 100;
    maxAmmo = 100;
    flameRect.setSize(Vector2f(streamLength, BLOCK_SIZE * 0.5f));
    flameRect.setFillColor(Color(255, 100, 0, 180));
}

void FlameShot::fire(float angle) {
    if (canFire())
        startStream();
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
    Firearm::update(dt);
    if (streamActive) {
        streamTimer -= dt;
        ammo -= dt * 5;
        if (streamTimer <= 0 || ammo <= 0)
            stopStream();
    }
}

void FlameShot::render(RenderWindow& window) {
    if (isActive && streamActive) {
        sprite.setPosition(x, y);
        window.draw(sprite);
        flameRect.setPosition(x, y);
        window.draw(flameRect);
    }
    else if (isActive) {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
}

bool FlameShot::isStreamActive() const { return streamActive; }
int FlameShot::getDamagePerSecond() const { return damagePerSecond; }
float FlameShot::getStreamLength() const { return streamLength; }

// LaserGun class stuff
LaserGun::LaserGun() : Firearm() {
    name = "Laser Gun";
    weaponType = LASERGUN;
    beamActive = false;
    beamLength = 1200.0f;
    isInstantKill = true;
    beamTimer = 0;
    beamDuration = 0.5f;
    beamWidth = 5.0f;
    damage = 9999;
    fireRate = 1.0f;
    ammo = 30;
    maxAmmo = 30;
    beamRect.setSize(Vector2f(beamLength, beamWidth));
    beamRect.setFillColor(Color(0, 255, 100, 220));
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
}

void LaserGun::update(float dt) {
    Firearm::update(dt);
    if (beamActive) {
        beamTimer -= dt;
        if (beamTimer <= 0)
            deactivateBeam();
    }
}

void LaserGun::render(RenderWindow& window) {
    if (isActive) {
        sprite.setPosition(x, y);
        window.draw(sprite);
        if (beamActive) {
            beamRect.setPosition(x, y);
            window.draw(beamRect);
        }
    }
}

void LaserGun::deactivateBeam() { beamActive = false; }
bool LaserGun::isBeamActive() const { return beamActive; }
bool LaserGun::getIsInstantKill() const { return isInstantKill; }

// FirePool class stuff
FirePool::FirePool() {
    x = 0; y = 0;
    radius = 3.0f * BLOCK_SIZE;
    damagePerSecond = 2;
    duration = 10.0f;
    timer = duration;
    isActive = true;
    ownerType = NOowner;
    setupVisual();
}

FirePool::FirePool(float px, float py, float r, int dps, float dur, int owner) {
    x = px; y = py;
    radius = r;
    damagePerSecond = dps;
    duration = dur;
    timer = dur;
    isActive = true;
    ownerType = owner;
    setupVisual();
}

void FirePool::setupVisual() {
    poolCircle.setRadius(radius);
    poolCircle.setOrigin(radius, radius);
    poolCircle.setFillColor(Color(255, 60, 0, 130));
}

void FirePool::update(float dt) {
    if (isActive) {
        timer -= dt;
        if (timer <= 0) isActive = false;
        float alpha = (timer / duration) * 130;
        poolCircle.setFillColor(Color(255, 60, 0, (Uint8)alpha));
    }
}

void FirePool::render(RenderWindow& window) {
    if (isActive) {
        poolCircle.setPosition(x, y);
        window.draw(poolCircle);
    }
}

void FirePool::applyDamage(float targetX, float targetY, float& dmgOut, float dt) {
    if (checkCollision(targetX, targetY, 0, 0))
        dmgOut = damagePerSecond * dt;
    else
        dmgOut = 0;
}

bool FirePool::checkCollision(float tx, float ty, float tw, float th) const {
    float dx = tx - x;
    float dy = ty - y;
    return sqrt(dx * dx + dy * dy) <= radius;
}

bool FirePool::isExpired() const { return timer <= 0; }
bool FirePool::getIsActive() const { return isActive; }
float FirePool::getX() const { return x; }
float FirePool::getY() const { return y; }
float FirePool::getRadius() const { return radius; }
int FirePool::getDamagePerSecond() const { return damagePerSecond; }

// Grenade class stuff
Grenade::Grenade() : ProjectileWeapon() {
    width = 10.0f;
    height = 10.0f;
    velocityX = 0;
    velocityY = 0;
    blastRadius = 3.0f * BLOCK_SIZE;
    isExploded = false;
    thrownByPlayer = nullptr;
    gravity = 500.0f;
    fuseTimer = 3.0f;
    fuseDuration = 3.0f;
    angle = 0;
    initialSpeed = 400.0f;
    damage = 5;
}

Grenade::~Grenade() {}

void Grenade::fire(float launchAngle) {
    launch(launchAngle, initialSpeed);
}

bool Grenade::checkCollision(float tx, float ty, float tw, float th) {
    return (x < tx + tw && x + width > tx &&
        y < ty + th && y + height > ty);
}

void Grenade::applyGravity(float dt) { velocityY += gravity * dt; }

void Grenade::applyBlastDamage(float tx, float ty, int& targetHp) {
    if (isInBlastRadius(tx, ty))
        targetHp -= damage;
}

bool Grenade::isInBlastRadius(float tx, float ty) const {
    float dx = tx - x;
    float dy = ty - y;
    return sqrt(dx * dx + dy * dy) <= blastRadius;
}

void Grenade::launch(float launchAngle, float speed) {
    angle = launchAngle;
    velocityX = cos(launchAngle) * speed;
    velocityY = sin(launchAngle) * speed;
}

void Grenade::setThrownBy(PlayerSoldier* p) { thrownByPlayer = p; }

bool Grenade::getIsExploded() const { return isExploded; }
float Grenade::getBlastRadius() const { return blastRadius; }
float Grenade::getVelocityX() const { return velocityX; }
float Grenade::getVelocityY() const { return velocityY; }

void Grenade::setIsExploded(bool exploded) { isExploded = exploded; }

// HandGrenade class stuff
HandGrenade::HandGrenade() : Grenade() {
    name = "Hand Grenade";
    weaponType = HANDGRENADE;
    bounceCount = 0;
    maxBounce = 3;
    bounceVelocityDampen = 0.6f;
    blastRadius = 3.0f * BLOCK_SIZE;
    damage = 5;
    isGrenadeActive = false;
    groundLevel = 700.0f;
}

void HandGrenade::throwGrenade(float angle) {
    fire(angle);
    isGrenadeActive = true;
    fuseTimer = fuseDuration;
    bounceCount = 0;
}

void HandGrenade::update(float dt) {
    if (!isExploded && isGrenadeActive) {
        applyGravity(dt);
        x += velocityX * dt;
        y += velocityY * dt;
        fuseTimer -= dt;
        if (fuseTimer <= 0) explode();
        if (y >= groundLevel) applyBounce();
    }
}

void HandGrenade::reset(float startX, float startY, float vx, float vy) {
    x = startX; y = startY;
    velocityX = vx; velocityY = vy;
    isExploded = false;
    fuseTimer = fuseDuration;
    isGrenadeActive = true;
    bounceCount = 0;
}

void HandGrenade::setVelocity(float vx, float vy) { velocityX = vx; velocityY = vy; }
void HandGrenade::setGroundLevel(float gl) { groundLevel = gl; }

void HandGrenade::explode() {
    isExploded = true;
    isGrenadeActive = false;
}

void HandGrenade::render(RenderWindow& window) {
    if (!isExploded && isGrenadeActive) {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
}

void HandGrenade::applyBounce() {
    if (bounceCount < maxBounce) {
        y = groundLevel;
        velocityY = -velocityY * bounceVelocityDampen;
        velocityX *= bounceVelocityDampen;
        bounceCount++;
    }
    else {
        velocityY = 0;
        velocityX = 0;
    }
}

int HandGrenade::getBounceCount() const { return bounceCount; }
bool HandGrenade::getIsActive() const { return isGrenadeActive; }

// FireBombGrenade class stuff
FireBombGrenade::FireBombGrenade() : Grenade() {
    name = "Fire Bomb Grenade";
    weaponType = FIREBOMBGRENADE;
    firePool = nullptr;
    firePoolDuration = 10.0f;
    firePoolRadius = 3.0f * BLOCK_SIZE;
    firePoolDamagePerSec = 2;
    blastRadius = 3.0f * BLOCK_SIZE;
    damage = 5;
    isBombActive = false;
}

FireBombGrenade::~FireBombGrenade() {
    if (firePool) {
        delete firePool;
        firePool = nullptr;
    }
}

void FireBombGrenade::throwBomb(float angle) {
    fire(angle);
    isBombActive = true;
    fuseTimer = fuseDuration;
}

void FireBombGrenade::update(float dt) {
    if (!isExploded && isBombActive) {
        applyGravity(dt);
        x += velocityX * dt;
        y += velocityY * dt;
        fuseTimer -= dt;
        if (fuseTimer <= 0 || y >= 700)
            bombExplode();
    }
    if (firePool) {
        firePool->update(dt);
        if (firePool->isExpired()) {
            delete firePool;
            firePool = nullptr;
        }
    }
}

void FireBombGrenade::bombExplode() {
    isExploded = true;
    isBombActive = false;
    spawnFirePool();
}

void FireBombGrenade::spawnFirePool() {
    firePool = new FirePool(x, y, firePoolRadius,
        firePoolDamagePerSec,
        firePoolDuration, ownerType);
}

void FireBombGrenade::render(RenderWindow& window) {
    if (!isExploded && isBombActive) {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
    if (firePool)
        firePool->render(window);
}

FirePool* FireBombGrenade::getFirePool() const { return firePool; }
bool FireBombGrenade::isBombFlying() const { return isBombActive; }

// BulletManager class stuff
BulletManager::BulletManager() : multiKillCount(0) {}

void BulletManager::spawnBullet(float x, float y, float angle,
    int damage, int ownerType,
    float speed, float range,
    const Color& bulletColor)
{
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].isActive()) {
            bullets[i].spawn(x, y, angle, damage, ownerType, speed, range, bulletColor);
            return;
        }
    }
}

void BulletManager::spawnGrenade(float x, float y, float vx, float vy) {
    for (int i = 0; i < MAX_GRENADES; i++) {
        if (!grenades[i].getIsActive()) {
            grenades[i].reset(x, y, vx, vy);
            return;
        }
    }
}

void BulletManager::spawnRocket(float x, float y, float vx, float vy) {
    for (int i = 0; i < MAX_ROCKETS; i++) {
        if (!rockets[i].isActive()) {
            rockets[i].setPosition(x, y);
            rockets[i].setVelocity(vx, vy);
            rockets[i].setActive(true);
            return;
        }
    }
}

void BulletManager::update(float dt) {
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].update(dt);
    for (int i = 0; i < MAX_GRENADES; i++) grenades[i].update(dt);
    for (int i = 0; i < MAX_ROCKETS; i++) rockets[i].update(dt);
}

void BulletManager::render(RenderWindow& window, float camX, float camY) {
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].render(window, camX, camY);
    for (int i = 0; i < MAX_GRENADES; i++) grenades[i].render(window);
    for (int i = 0; i < MAX_ROCKETS; i++) rockets[i].render(window, camX, camY);
}

void BulletManager::clearAll() {
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].deactivate();
    for (int i = 0; i < MAX_GRENADES; i++) grenades[i].setActive(false);
    for (int i = 0; i < MAX_ROCKETS; i++) rockets[i].setActive(false);
    multiKillCount = 0;
}

void BulletManager::checkMultiKill(float x, float y, ScoreSystem* scoreSystem) {
}

void BulletManager::checkPlayerCollisions(float px, float py, float pw, float ph,
    int& totalDamage)
{
    totalDamage = 0;

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].isActive()) continue;
        if (bullets[i].getOwner() != ENEMY) continue;
        if (bullets[i].checkHit(px, py, pw, ph)) {
            totalDamage += bullets[i].getDamage();
            bullets[i].setActive(false);
        }
    }

    for (int i = 0; i < MAX_GRENADES; i++) {
        if (!grenades[i].getIsActive()) continue;
        if (grenades[i].getIsExploded()) {
            int hp = 0;
            grenades[i].applyBlastDamage(px + pw / 2, py + ph / 2, hp);
            totalDamage += (-hp);
        }
    }

    for (int i = 0; i < MAX_ROCKETS; i++) {
        if (!rockets[i].isActive()) continue;
        float dx = rockets[i].getX() - (px + pw / 2);
        float dy = rockets[i].getY() - (py + ph / 2);
        if (sqrt(dx * dx + dy * dy) < 60.f) {
            totalDamage += 15;
            rockets[i].setActive(false);
        }
    }
}

bool BulletManager::popPlayerBulletHit(float tx, float ty, float tw, float th,
    int& outDamage, float& hitX, float& hitY)
{
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].isActive()) continue;
        if (bullets[i].getOwner() != PLAYER) continue;
        if (!bullets[i].checkHit(tx, ty, tw, th)) continue;

        outDamage = bullets[i].getDamage();
        hitX = bullets[i].getX();
        hitY = bullets[i].getY();
        bullets[i].setActive(false);
        return true;
    }
    return false;
}

int BulletManager::getBulletCount() const {
    return MAX_BULLETS;
}

Bullet* BulletManager::getBullet(int index) {
    if (index >= 0 && index < MAX_BULLETS) {
        return &bullets[index];
    }
    return nullptr;
}
