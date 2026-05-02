#pragma once
#include "Header.h"
#include <iostream>
#include "Player.h"
//Weapon types
const int NONE = 0;
const int PISTOL = 1;
const int KNIFE = 2;
const int HMG = 3;
const int ROCKET_LAUNCHER = 4;
const int FLAME_SHOT = 5;
const int LASER_GUN = 6;
const int HAND_GRENADE = 7;
const int FIRE_BOMB_GRENADE = 8;

//Owner types
const int PLAYER = 1;
const int ENEMY = 2;
const int VEHICLE = 3; 

class Weapon {
protected:
    char  name[32];
    int   damage;
    bool  isActive;
    int   weaponType;
    float x, y;
    int   ownerType;
    char  soundEffect[32];
    bool  isMuffled;

public:
    Weapon() {
        name[0] = '\0';
        damage = 10;
        isActive = false;
        weaponType = NONE;
        x = 0;
        y = 0;
        ownerType = NONE;
        soundEffect[0] = '\0';
        isMuffled = false;
    }

    ~Weapon() {
    }

    int getDamage() const {
        return damage;
    }

    void setActive(bool active) {
        isActive = active;
    }

    int getOwnerType() const {
        return ownerType;
    }

    void applySoundEffect() {
        if (!isMuffled) {
            cout << "Sound: " << soundEffect << endl;
        }
    }

    bool getIsActive() const {
        return isActive;
    }

    int getWeaponType() const {
        return weaponType;
    }

    const char* getName() const {
        return name;
    }

    float getX() const {
        return x;
    }

    float getY() const {
        return y;
    }

    void setPosition(float px, float py) {
        x = px;
        y = py;
    }

    void setOwnerType(int ot) {
        ownerType = ot;
    }

    void setMuffled(bool m) {
        isMuffled = m;
    }
};

class Firearm : public Weapon {
protected:
    float fireRate;
    float fireTimer;
    int   ammo;
    int   maxAmmo;
    bool  isInfiniteAmmo;
    float range;
    float projectileSpeed;

public:
    Firearm() : Weapon() {
        fireRate = 0.2f;
        fireTimer = 0;
        ammo = 100;
        maxAmmo = 100;
        isInfiniteAmmo = false;
        range = 500.0f;
        projectileSpeed = 10.0f;
    }

    ~Firearm() {
    }

    void reload() {
        ammo = maxAmmo;
    }

    bool isEmpty() const {
        return ammo <= 0 && !isInfiniteAmmo;
    }

    float getFireRate() const {
        return fireRate;
    }

    int getAmmo() const {
        return ammo;
    }

    void setAmmo(int amount) {
        ammo = amount;
        if (ammo > maxAmmo) {
            ammo = maxAmmo;
        }
    }

    bool canFire() {
        return fireTimer <= 0 && (ammo > 0 || isInfiniteAmmo);
    }
};
class Pistol : public Firearm {
private:
    bool isDefault;

public:
    Pistol() : Firearm() {
        weaponType = PISTOL;
        isDefault = true;
        damage = 10;
        fireRate = 0.3f;
        ammo = 50;
        maxAmmo = 50;
        isInfiniteAmmo = true;
    }

    void fire(float angle) {
        if (canFire()) {
            cout << "Pistol fires at angle: " << angle << endl;
            if (!isInfiniteAmmo) {
                ammo--;
            }
            fireTimer = fireRate;
        }
    }

    void update(float dt) {
        if (fireTimer > 0) {
            fireTimer -= dt;
        }
    }

    void render() {
        cout << "Rendering Pistol" << endl;
    }

    bool getIsDefault() const {
        return isDefault;
    }
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
        weaponType = KNIFE;
        meleeRange = 30.0f;
        meleeDamage = 50;
        meleeCooldown = 0.5f;
        meleeTimer = 0;
        isPiercing = false;
        damage = 50;
        isInfiniteAmmo = true;
    }

    void fire(float angle) {
        if (canMelee()) {
            cout << "Knife slashes at angle: " << angle << endl;
            meleeTimer = meleeCooldown;
        }
    }

    void update(float dt) {
        if (meleeTimer > 0) {
            meleeTimer -= dt;
        }
    }

    void render() {
        cout << "Rendering Knife" << endl;
    }

    bool isInRange(float targetX, float targetY) const {
        float dx = targetX - x;
        float dy = targetY - y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance <= meleeRange;
    }

    bool canMelee() const {
        return meleeTimer <= 0;
    }

    void resetCooldown() {
        meleeTimer = 0;
    }

    float getMeleeRange() const {
        return meleeRange;
    }

    int getMeleeDamage() const {
        return meleeDamage;
    }

    bool getIsPiercing() const {
        return isPiercing;
    }

    void setPiercing(bool p) {
        isPiercing = p;
    }
};

class HeavyMachineGun : public Firearm {
private:
    bool  isHeldDown;
    float spinUpTimer;
    float spinUpDuration;
    float currentSpinLevel;

public:
    HeavyMachineGun() : Firearm() {
        weaponType = HMG;
        isHeldDown = false;
        spinUpTimer = 0;
        spinUpDuration = 1.0f;
        currentSpinLevel = 0;
        damage = 15;
        fireRate = 0.05f;
        ammo = 200;
        maxAmmo = 200;
    }

    void fire(float angle) {
        if (canFire() && currentSpinLevel >= 1.0f) {
            cout << "HMG fires at angle: " << angle << endl;
            ammo--;
            fireTimer = fireRate;
        }
    }

    void startFire() {
        isHeldDown = true;
    }

    void stopFire() {
        isHeldDown = false;
    }

    void update(float dt) {
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

    void render() {
        cout << "Rendering HMG, spin level: " << currentSpinLevel << endl;
    }

    float getSpinLevel() const {
        return currentSpinLevel;
    }

    bool getIsHeldDown() const {
        return isHeldDown;
    }
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
        weaponType = ROCKET_LAUNCHER;
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

    void fire(float angle) {
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

    void reload() {
        isReloading = true;
        reloadTimer = reloadDuration;
    }

    void spawnRocket(float angle) {
        cout << "Rocket launched at angle: " << angle << endl;
    }

    void update(float dt) {
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

    void render() {
        cout << "Rendering Rocket Launcher" << endl;
    }

    bool isReloadingNow() const {
        return isReloading;
    }

    float getBlastRadius() const {
        return blastRadius;
    }

    int getBlastDamage() const {
        return blastDamage;
    }
};

class FlameShot : public Firearm {
private:
    float streamLength;
    int   damagePerSecond;
    bool  isHeldDown;
    bool  streamActive;
    float streamTimer;
    float maxStreamDuration;

public:
    FlameShot() : Firearm() {
        weaponType = FLAME_SHOT;
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

    void fire(float angle) {
        if (canFire()) {
            startStream();
            cout << "FlameShot fires at angle: " << angle << endl;
        }
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

    void update(float dt) {
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

    void render() {
        cout << "Rendering FlameShot" << endl;
    }

    bool isStreamActive() const {
        return streamActive;
    }

    int getDamagePerSecond() const {
        return damagePerSecond;
    }

    float getStreamLength() const {
        return streamLength;
    }
};

class LaserGun : public Firearm {
private:
    bool  beamActive;
    float beamLength;
    bool  isInstantKill;
    float beamTimer;
    float beamDuration;
    float beamWidth;

public:
    LaserGun() : Firearm() {
        weaponType = LASER_GUN;
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

    void fire(float angle) {
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

    void update(float dt) {
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

    void render() {
        cout << "Rendering Laser Gun" << endl;
    }

    void deactivateBeam() {
        beamActive = false;
    }

    bool isBeamActive() const {
        return beamActive;
    }

    bool getIsInstantKill() const {
        return isInstantKill;
    }

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

public:
    FirePool() {
        x = 0;
        y = 0;
        radius = 50.0f;
        damagePerSecond = 20;
        duration = 5.0f;
        timer = duration;
        isActive = true;
        ownerType = NONE;
    }

    FirePool(float px, float py, float r, int dps, float dur, int owner) {
        x = px;
        y = py;
        radius = r;
        damagePerSecond = dps;
        duration = dur;
        timer = dur;
        isActive = true;
        ownerType = owner;
    }

    void update(float dt) {
        if (isActive) {
            timer -= dt;
            if (timer <= 0) {
                isActive = false;
            }
        }
    }

    void render() {
        if (isActive) {
            cout << "Rendering fire pool at (" << x << ", " << y << ")" << endl;
        }
    }

    void applyDamage(float targetX, float targetY, float& dmgOut, float dt) {
        if (checkCollision(targetX, targetY, 0, 0)) {
            dmgOut = damagePerSecond * dt;
        }
        else {
            dmgOut = 0;
        }
    }

    bool isExpired() const {
        return timer <= 0;
    }

    bool checkCollision(float tx, float ty, float tw, float th) const {
        float dx = tx - x;
        float dy = ty - y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance <= radius;
    }

    bool getIsActive() const {
        return isActive;
    }

    float getX() const {
        return x;
    }

    float getY() const {
        return y;
    }

    float getRadius() const {
        return radius;
    }

    int getDamagePerSecond() const {
        return damagePerSecond;
    }
};

class Grenade : public Weapon {
protected:
    float   width;
    float   height;
    float   velocityX;
    float   velocityY;
    float   blastRadius;
    bool    isExploded;
    Player* thrownByPlayer;
    float   gravity;
    float   fuseTimer;
    float   fuseDuration;
    float   angle;
    float   initialSpeed;

public:
    Grenade() : Weapon() {
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

    ~Grenade() {
    }

    void fire(float launchAngle) {
        launch(launchAngle, initialSpeed);
    }

    bool checkCollision(float tx, float ty, float tw, float th) {
        return (x < tx + tw && x + width > tx && y < ty + th && y + height > ty);
    }

    void applyGravity(float dt) {
        velocityY += gravity * dt;
    }

    void applyBlastDamage(float tx, float ty, int& targetHp) {
        if (isInBlastRadius(tx, ty)) {
            targetHp -= damage;
        }
    }

    bool isInBlastRadius(float tx, float ty) const {
        float dx = tx - x;
        float dy = ty - y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance <= blastRadius;
    }

    void launch(float launchAngle, float speed) {
        angle = launchAngle;
        velocityX = cos(launchAngle) * speed;
        velocityY = sin(launchAngle) * speed;
    }

    void setThrownBy(Player* p) {
        thrownByPlayer = p;
    }

    bool getIsExploded() const {
        return isExploded;
    }

    float getBlastRadius() const {
        return blastRadius;
    }

    float getVelocityX() const {
        return velocityX;
    }

    float getVelocityY() const {
        return velocityY;
    }
};

class HandGrenade : public Grenade {
private:
    int   bounceCount;
    int   maxBounce;
    float bounceVelocityDampen;
public:
    HandGrenade() : Grenade() {
        weaponType = HAND_GRENADE;
        bounceCount = 0;
        maxBounce = 3;
        bounceVelocityDampen = 0.6f;
        blastRadius = 60.0f;
        damage = 80;
    }

    void Handfire(float angle) {
        fire(angle);
    }

    void Handupdate(float dt) {
        if (!isExploded) {
            applyGravity(dt);
            x += velocityX * dt;
            y += velocityY * dt;

            fuseTimer -= dt;
            if (fuseTimer <= 0) {
                Handexplode();
            }

            if (y >= 700) {
                HandapplyBounce();
            }
        }
    }

    void Handexplode() {
        isExploded = true;
        cout << "Hand grenade explodes!" << endl;
    }

    void Handrender() {
        if (!isExploded) {
            cout << "Rendering hand grenade at (" << x << ", " << y << ")" << endl;
        }
    }

    void HandapplyBounce() {
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

    int HandgetBounceCount() const {
        return bounceCount;
    }
};

class FireBombGrenade : public Grenade {
private:
    FirePool* firePool;
    float     firePoolDuration;
    float     firePoolRadius;
    int       firePoolDamagePerSecond;

public:
    FireBombGrenade() : Grenade() {
        weaponType = FIRE_BOMB_GRENADE;
        firePool = nullptr;
        firePoolDuration = 8.0f;
        firePoolRadius = 80.0f;
        firePoolDamagePerSecond = 25;
        blastRadius = 40.0f;
        damage = 60;
    }

    ~FireBombGrenade() {
        if (firePool) {
            delete firePool;
        }
    }

    void FireBombfire(float angle) {
        fire(angle);
    }

    void FireBombupdate(float dt) {
        if (!isExploded) {
            applyGravity(dt);
            x += velocityX * dt;
            y += velocityY * dt;

            fuseTimer -= dt;
            if (fuseTimer <= 0 || y >= 700) {
                FireBombexplode();
            }
        }

        if (firePool) {
            firePool->update(dt);
        }
    }

    void FireBombexplode() {
        isExploded = true;
        spawnFirePool();
        cout << "Fire bomb grenade explodes!" << endl;
    }

    void spawnFirePool() {
        firePool = new FirePool(x, y, firePoolRadius, firePoolDamagePerSecond, firePoolDuration, ownerType);
    }

    void FireBombrender() {
        if (!isExploded) {
            cout << "Rendering fire bomb grenade at (" << x << ", " << y << ")" << endl;
        }

        if (firePool) {
            firePool->render();
        }
    }

    FirePool* FireBombgetFirePool() const {
        return firePool;
    }
};