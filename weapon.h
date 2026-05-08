#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include "PlayerSoldier.h"

using namespace std;

const float BLOCK_SIZE = 32.0f;

const int NONE = 0;
const int PISTOL = 1;
const int KNIFE = 2;
const int HMG = 3;
const int ROCKETLAUNCHER = 4;
const int FLAMESHOT = 5;
const int LASERGUN = 6;
const int HANDGRENADE = 7;
const int FIREBOMBGRENADE = 8;

const int NOowner = 0;
const int PLAYER = 1;
const int ENEMY = 2;
const int VEHICLE = 3;

class Weapon {
protected:
    const char* name;
    int   damage;
    bool  isActive;
    int   weaponType;
    float x, y;
    int   ownerType;
    char  soundEffect[32];
    bool  isMuffled;
    sf::Sprite  sprite;
    sf::Texture texture;

public:
    Weapon() {
        damage = 0;
        isActive = false;
        weaponType = NONE;
        x = 0;
        y = 0;
        ownerType = NOowner;
        soundEffect[0] = '\0';
        isMuffled = false;
    }

    virtual ~Weapon() {}

    bool loadTexture(const string& path) {
        if (!texture.loadFromFile(path)) {
            cout << "Warning couldnt load texture: " << path << endl;
            return false;
        }
        sprite.setTexture(texture);
        return true;
    }

    void setPosition(float px, float py) {
        x = px;
        y = py;
        sprite.setPosition(px, py);
    }

    void applySoundEffect() {
        if (!isMuffled)
            cout << "Sound: " << soundEffect << endl;
    }

    void setMuffled(bool m) { isMuffled = m; }
    void setActive(bool active) { isActive = active; }
    void setOwnerType(int ot) { ownerType = ot; }

    int         getDamage()     const { return damage; }
    bool        getIsActive()   const { return isActive; }
    int         getWeaponType() const { return weaponType; }
    const char* getName()       const { return name; }
    float       getX()          const { return x; }
    float       getY()          const { return y; }
    int         getOwnerType()  const { return ownerType; }
    sf::Sprite& getSprite() { return sprite; }

    virtual void render(sf::RenderWindow& window) {
        if (isActive) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }
};

// UML requirement
class ProjectileWeapon : public Weapon {
protected:
    float projectileSpeed;
    float range;
    int   ammo;
    int   maxAmmo;
    bool  isInfiniteAmmo;
    float fireRate;
    float fireTimer;

public:
    ProjectileWeapon() : Weapon() {
        projectileSpeed = 10.0f;
        range = 500.0f;
        ammo = 0;
        maxAmmo = 0;
        isInfiniteAmmo = false;
        fireRate = 0.25f;
        fireTimer = 0.0f;
    }

    virtual ~ProjectileWeapon() {}
    virtual void fire(float angle) = 0;

    bool canFire() {
        return fireTimer <= 0 && (ammo > 0 || isInfiniteAmmo);
    }

    virtual void update(float dt) {
        if (fireTimer > 0)
            fireTimer -= dt;
    }

    bool isEmpty()  const { return ammo <= 0 && !isInfiniteAmmo; }
    float getFireRate() const { return fireRate; }
    int   getAmmo()     const { return ammo; }

    void setAmmo(int amount) {
        ammo = amount;
        if (ammo > maxAmmo) {
            ammo = maxAmmo;
        }
    }

    void reload() { ammo = maxAmmo; }
};

class Firearm : public ProjectileWeapon {
protected:
    float range;
    float projectileSpeed;

public:
    Firearm() : ProjectileWeapon() {
        range = 500.0f;
        projectileSpeed = 10.0f;
    }

    virtual ~Firearm() {}

    virtual void fire(float angle) = 0;

    virtual void update(float dt) override {
        ProjectileWeapon::update(dt);
    }
};

class Pistol : public Firearm {
private:
    bool isDefault;
    bool isDualFiring;
    float dualFireTimer;

public:
    Pistol() : Firearm() {
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

    void fire(float angle) override {
        if (canFire()) {
            cout << "Pistol fires at angle: " << angle << endl;
            fireTimer = fireRate;
        }
    }

    void activateDualFire() {
        isDualFiring = true;
        dualFireTimer = 10.0f;
    }

    void fireDual(float angle) {
        if (isDualFiring && canFire()) {
            fire(angle);
            fire(180.0f - angle);
        }
    }

    void update(float dt) override {
        Firearm::update(dt);
        if (isDualFiring) {
            dualFireTimer -= dt;
            if (dualFireTimer <= 0) {
                isDualFiring = false;
                dualFireTimer = 0.0f;
            }
        }
    }

    void render(sf::RenderWindow& window) override {
        if (isActive) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }

    bool getIsDefault()    const { return isDefault; }
    bool getIsDualFiring() const { return isDualFiring; }
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

    void fire(float angle) override {
        if (canMelee()) {
            cout << "Knife slashes at angle: " << angle << endl;
            meleeTimer = meleeCooldown;
        }
    }

    void update(float dt) override {
        if (meleeTimer > 0)
            meleeTimer -= dt;
    }

    void render(sf::RenderWindow& window) override {
        if (isActive) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }

    bool isInRange(float targetX, float targetY) const {
        float dx = targetX - x;
        float dy = targetY - y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance <= meleeRange;
    }

    bool canMelee()        const { return meleeTimer <= 0; }
    void resetCooldown() { meleeTimer = 0; }
    void setPiercing(bool p) { isPiercing = p; }
    float getMeleeRange()  const { return meleeRange; }
    int   getMeleeDamage() const { return meleeDamage; }
    bool  getIsPiercing()  const { return isPiercing; }
};

class HeavyMachineGun : public Firearm {
private:
    bool  isHeldDown;
    float spinUpTimer;
    float spinUpDuration;
    float currentSpinLevel;

public:
    HeavyMachineGun() : Firearm() {
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

    void fire(float angle) override {
        if (canFire() && currentSpinLevel >= 1.0f) {
            cout << "HMG fires at angle: " << angle << endl;
            ammo--;
            fireTimer = fireRate;
        }
    }

    void startFire() { isHeldDown = true; }
    void stopFire() { isHeldDown = false; }

    void update(float dt) override {
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

    void render(sf::RenderWindow& window) override {
        if (isActive) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }

    float getSpinLevel()  const { return currentSpinLevel; }
    bool  getIsHeldDown() const { return isHeldDown; }
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

    void fire(float angle) override {
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

    void spawnRocket(float angle) {
        cout << "Rocket launched at angle: " << angle << endl;
    }

    void update(float dt) override {
        Firearm::update(dt);
        if (isReloading) {
            reloadTimer -= dt;
            if (reloadTimer <= 0) {
                isReloading = false;
                ammo = maxAmmo;
            }
        }
    }

    void render(sf::RenderWindow& window) override {
        if (isActive) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }

    bool  isReloadingNow() const { return isReloading; }
    float getBlastRadius() const { return blastRadius; }
    int   getBlastDamage() const { return blastDamage; }
};

class FlameShot : public Firearm {
private:
    float streamLength;
    int   damagePerSecond;
    bool  isHeldDown;
    bool  streamActive;
    float streamTimer;
    float maxStreamDuration;
    sf::RectangleShape flameRect;

public:
    FlameShot() : Firearm() {
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
        flameRect.setSize(sf::Vector2f(streamLength, BLOCK_SIZE * 0.5f));
        flameRect.setFillColor(sf::Color(255, 100, 0, 180));
    }

    void fire(float angle) override {
        if (canFire())
            startStream();
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

    void update(float dt) override {
        Firearm::update(dt);
        if (streamActive) {
            streamTimer -= dt;
            ammo -= dt * 5;
            if (streamTimer <= 0 || ammo <= 0)
                stopStream();
        }
    }

    void render(sf::RenderWindow& window) override {
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

    bool isStreamActive()     const { return streamActive; }
    int  getDamagePerSecond() const { return damagePerSecond; }
    float getStreamLength()   const { return streamLength; }
};

class LaserGun : public Firearm {
private:
    bool  beamActive;
    float beamLength;
    bool  isInstantKill;
    float beamTimer;
    float beamDuration;
    float beamWidth;
    sf::RectangleShape beamRect;

public:
    LaserGun() : Firearm() {
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
        beamRect.setSize(sf::Vector2f(beamLength, beamWidth));
        beamRect.setFillColor(sf::Color(0, 255, 100, 220));
    }

    void fire(float angle) override {
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

    void update(float dt) override {
        Firearm::update(dt);
        if (beamActive) {
            beamTimer -= dt;
            if (beamTimer <= 0)
                deactivateBeam();
        }
    }

    void render(sf::RenderWindow& window) override {
        if (isActive) {
            sprite.setPosition(x, y);
            window.draw(sprite);
            if (beamActive) {
                beamRect.setPosition(x, y);
                window.draw(beamRect);
            }
        }
    }

    void deactivateBeam() { beamActive = false; }
    bool isBeamActive()      const { return beamActive; }
    bool getIsInstantKill()  const { return isInstantKill; }
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
    sf::CircleShape poolCircle;

public:
    FirePool() {
        x = 0; y = 0;
        radius = 3.0f * BLOCK_SIZE;
        damagePerSecond = 2;
        duration = 10.0f;
        timer = duration;
        isActive = true;
        ownerType = NOowner;
        setupVisual();
    }

    FirePool(float px, float py, float r, int dps, float dur, int owner) {
        x = px; y = py;
        radius = r;
        damagePerSecond = dps;
        duration = dur;
        timer = dur;
        isActive = true;
        ownerType = owner;
        setupVisual();
    }

    void setupVisual() {
        poolCircle.setRadius(radius);
        poolCircle.setOrigin(radius, radius);
        poolCircle.setFillColor(sf::Color(255, 60, 0, 130));
    }

    void update(float dt) {
        if (isActive) {
            timer -= dt;
            if (timer <= 0) {
                isActive = false;
            }
            float alpha = (timer / duration) * 130;
            poolCircle.setFillColor(sf::Color(255, 60, 0, (sf::Uint8)alpha));
        }
    }

    void render(sf::RenderWindow& window) {
        if (isActive) {
            poolCircle.setPosition(x, y);
            window.draw(poolCircle);
        }
    }

    void applyDamage(float targetX, float targetY, float& dmgOut, float dt) {
        if (checkCollision(targetX, targetY, 0, 0))
            dmgOut = damagePerSecond * dt;
        else
            dmgOut = 0;
    }

    bool checkCollision(float tx, float ty, float tw, float th) const {
        float dx = tx - x;
        float dy = ty - y;
        return sqrt(dx * dx + dy * dy) <= radius;
    }

    bool  isExpired()          const { return timer <= 0; }
    bool  getIsActive()        const { return isActive; }
    float getX()               const { return x; }
    float getY()               const { return y; }
    float getRadius()          const { return radius; }
    int   getDamagePerSecond() const { return damagePerSecond; }
};

class Grenade : public ProjectileWeapon {
protected:
    float  width, height;
    float  velocityX, velocityY;
    float  blastRadius;
    bool   isExploded;
    PlayerSoldier* thrownByPlayer;
    float  gravity;
    float  fuseTimer;
    float  fuseDuration;
    float  angle;
    float  initialSpeed;

public:
    Grenade() : ProjectileWeapon() {
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

    virtual ~Grenade() {}

    void fire(float launchAngle) override {
        launch(launchAngle, initialSpeed);
    }

    bool checkCollision(float tx, float ty, float tw, float th) {
        return (x < tx + tw && x + width > tx &&
            y < ty + th && y + height > ty);
    }

    void applyGravity(float dt) { velocityY += gravity * dt; }

    void applyBlastDamage(float tx, float ty, int& targetHp) {
        if (isInBlastRadius(tx, ty))
            targetHp -= damage;
    }

    bool isInBlastRadius(float tx, float ty) const {
        float dx = tx - x;
        float dy = ty - y;
        return sqrt(dx * dx + dy * dy) <= blastRadius;
    }

    void launch(float launchAngle, float speed) {
        angle = launchAngle;
        velocityX = cos(launchAngle) * speed;
        velocityY = sin(launchAngle) * speed;
    }

    void setThrownBy(PlayerSoldier* p) { thrownByPlayer = p; }

    bool  getIsExploded()  const { return isExploded; }
    float getBlastRadius() const { return blastRadius; }
    float getVelocityX()   const { return velocityX; }
    float getVelocityY()   const { return velocityY; }
};

class HandGrenade : public Grenade {
private:
    int   bounceCount;
    int   maxBounce;
    float bounceVelocityDampen;
    bool  isGrenadeActive;
    float groundLevel;

public:
    HandGrenade() : Grenade() {
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

    void throwGrenade(float angle) {
        fire(angle);
        isGrenadeActive = true;
        fuseTimer = fuseDuration;
        bounceCount = 0;
    }

    void update(float dt) override {
        if (!isExploded && isGrenadeActive) {
            applyGravity(dt);
            x += velocityX * dt;
            y += velocityY * dt;
            fuseTimer -= dt;
            if (fuseTimer <= 0) {
                explode();
            }
            if (y >= groundLevel) {
                applyBounce();
            }
        }
    }

    void reset(float startX, float startY, float vx, float vy) {
        x = startX; y = startY;
        velocityX = vx; velocityY = vy;
        isExploded = false;
        fuseTimer = fuseDuration;
        isGrenadeActive = true;
        bounceCount = 0;
    }

    void setVelocity(float vx, float vy) { velocityX = vx; velocityY = vy; }
    void setGroundLevel(float gl) { groundLevel = gl; }

    void explode() {
        isExploded = true;
        isGrenadeActive = false;
        cout << "Hand grenade explodes!" << endl;
    }

    void render(sf::RenderWindow& window) override {
        if (!isExploded && isGrenadeActive) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }

    void applyBounce() {
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

    int  getBounceCount() const { return bounceCount; }
    bool getIsActive()    const { return isGrenadeActive; }
};

class FireBombGrenade : public Grenade {
private:
    FirePool* firePool;
    float     firePoolDuration;
    float     firePoolRadius;
    int       firePoolDamagePerSec;
    bool      isBombActive;

public:
    FireBombGrenade() : Grenade() {
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

    ~FireBombGrenade() {
        if (firePool) {
            delete firePool;
            firePool = nullptr;
        }
    }

    void throwBomb(float angle) {
        fire(angle);
        isBombActive = true;
        fuseTimer = fuseDuration;
    }

    void update(float dt) override {
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

    void bombExplode() {
        isExploded = true;
        isBombActive = false;
        spawnFirePool();
        cout << "Fire bomb grenade explodes!" << endl;
    }

    void spawnFirePool() {
        firePool = new FirePool(x, y, firePoolRadius,
            firePoolDamagePerSec,
            firePoolDuration, ownerType);
    }

    void render(sf::RenderWindow& window) override {
        if (!isExploded && isBombActive) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
        if (firePool)
            firePool->render(window);
    }

    FirePool* getFirePool()  const { return firePool; }
    bool      isBombFlying() const { return isBombActive; }
};