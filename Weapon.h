#pragma once

class Player;

//Weapon types
const int WT_NONE = 0;
const int WT_PISTOL = 1;
const int WT_KNIFE = 2;
const int WT_HMG = 3;
const int WT_ROCKET_LAUNCHER = 4;
const int WT_FLAME_SHOT = 5;
const int WT_LASER_GUN = 6;
const int WT_HAND_GRENADE = 7;
const int WT_FIRE_BOMB_GRENADE = 8;

//Owner types
const int OT_NONE = 0;
const int OT_PLAYER = 1;
const int OT_ENEMY = 2;
const int OT_VEHICLE = 3;

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
    Weapon();
    virtual ~Weapon();
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    virtual void fire(float angle) = 0;
    int getDamage() const;
    void setActive(bool active);
    int getOwnerType() const;
    void applySoundEffect();
    bool getIsActive() const;
    int getWeaponType() const;
    const char* getName() const;
    float getX() const;
    float getY() const;
    void setPosition(float px, float py);
    void setOwnerType(int ot);
    void setMuffled(bool m);
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
    Firearm();
    virtual ~Firearm();
    virtual void reload();
    bool isEmpty() const;
    float getFireRate() const;
    int getAmmo() const;
    void setAmmo(int amount);
    bool canFire();
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    virtual void fire(float angle) = 0;
};

class Pistol : public Firearm {
private:
    bool isDefault;

public:
    Pistol();
    void fire(float angle);
    void update(float dt);
    void render();
    bool getIsDefault() const;
};

class Knife : public Firearm {
private:
    float meleeRange;
    int   meleeDamage;
    float meleeCooldown;
    float meleeTimer;
    bool  isPiercing;

public:
    Knife();
    void fire(float angle);
    void update(float dt);
    void render();
    bool isInRange(float targetX, float targetY) const;
    bool canMelee() const;
    void resetCooldown();
    float getMeleeRange() const;
    int getMeleeDamage() const;
    bool getIsPiercing() const;
    void setPiercing(bool p);
};

class HeavyMachineGun : public Firearm {
private:
    bool  isHeldDown;
    float spinUpTimer;
    float spinUpDuration;
    float currentSpinLevel;

public:
    HeavyMachineGun();
    void fire(float angle);
    void startFire();
    void stopFire();
    void update(float dt);
    void render();
    float getSpinLevel() const;
    bool getIsHeldDown() const;
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
    RocketLauncher();
    void fire(float angle);
    void reload();
    void spawnRocket(float angle);
    void update(float dt);
    void render();
    bool isReloadingNow() const;
    float getBlastRadius() const;
    int getBlastDamage() const;
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
    FlameShot();
    void fire(float angle);
    void startStream();
    void stopStream();
    void update(float dt);
    void render();
    bool isStreamActive() const;
    int getDamagePerSecond() const;
    float getStreamLength() const;
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
    LaserGun();
    void fire(float angle);
    void spawnBeam(float angle);
    void update(float dt);
    void render();
    void deactivateBeam();
    bool isBeamActive() const;
    bool getIsInstantKill() const;
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
    FirePool();
    FirePool(float px, float py, float r, int dps, float dur, int owner);
    void update(float dt);
    void render();
    void applyDamage(float targetX, float targetY, float& dmgOut, float dt);
    bool isExpired() const;
    bool checkCollision(float tx, float ty, float tw, float th) const;
    bool getIsActive() const;
    float getX() const;
    float getY() const;
    float getRadius() const;
    int getDamagePerSecond() const;
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
    Grenade();
    virtual ~Grenade();
    virtual void fire(float launchAngle);
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    virtual void explode() = 0;
    bool checkCollision(float tx, float ty, float tw, float th);
    void applyGravity(float dt);
    void applyBlastDamage(float tx, float ty, int& targetHp);
    bool isInBlastRadius(float tx, float ty) const;
    void launch(float launchAngle, float speed);
    void setThrownBy(Player* p);
    bool getIsExploded() const;
    float getBlastRadius() const;
    float getVelocityX() const;
    float getVelocityY() const;
};

class HandGrenade : public Grenade {
private:
    int   bounceCount;
    int   maxBounce;
    float bounceVelocityDampen;

public:
    HandGrenade();
    void fire(float angle);
    void update(float dt);
    void explode();
    void render();
    void applyBounce();
    int getBounceCount() const;
};

class FireBombGrenade : public Grenade {
private:
    FirePool* firePool;
    float     firePoolDuration;
    float     firePoolRadius;
    int       firePoolDamagePerSecond;

public:
    FireBombGrenade();
    ~FireBombGrenade();
    void fire(float angle);
    void update(float dt);
    void explode();
    void spawnFirePool();
    void render();
    FirePool* getFirePool() const;
};