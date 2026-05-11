#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "ScoreSystem.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
using namespace sf;
using namespace std;

// Forward declaration
class PlayerSoldier;

// bullets
const int NOowner = 0;
const int PLAYER = 1;
const int ENEMY = 2;
const int VEHICLE = 3;

class Bullet {
private:
    float x, y;
    float velX, velY;
    bool  active;
    int   damage;
    int   ownerType;
    float range;
    float distanceTravelled;
    RectangleShape shape;

public:
    Bullet();
    void spawn(float startX, float startY, float angle,
        int dmg, int owner, float speed = 500.f, float maxRange = 400.f,
        const Color& bulletColor = Color::Yellow);
    void update(float dt);
    bool checkHit(float tx, float ty, float tw, float th) const;
    void render(RenderWindow& window, float camX = 0, float camY = 0);
    void     setActive(bool a);
    bool     isActive() const;
    void     deactivate();
    int      getDamage() const;
    int      getOwner() const;
    float    getX() const;
    float    getY() const;
    float    getRadius() const;
};

class Rocket {
private:
    float x, y;
    float velX, velY;
    bool  active;

public:
    Rocket();
    void setPosition(float px, float py);
    void setVelocity(float vx, float vy);
    void setActive(bool s);
    bool  isActive() const;
    float getX() const;
    float getY() const;
    bool  getIsExploded() const;
    void update(float dt);
    void render(RenderWindow& window, float camX = 0, float camY = 0);
};

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

class Weapon {
protected:
    const char* name;
    int   damage;
    bool  isActive;
    int   weaponType;
    float x, y;
    int   ownerType;
    const char* soundEffect;
    bool  isMuffled;
    Sprite  sprite;
    Texture texture;

public:
    Weapon();
    virtual ~Weapon();
    bool loadTexture(const char* path);
    void setPosition(float px, float py);
    void applySoundEffect();
    void setMuffled(bool m);
    void setActive(bool active);
    void setOwnerType(int ot);
    int         getDamage() const;
    bool        getIsActive() const;
    int         getWeaponType() const;
    const char* getName() const;
    float       getX() const;
    float       getY() const;
    int         getOwnerType() const;
    Sprite& getSprite();
    virtual void render(RenderWindow& window);
};

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
    ProjectileWeapon();
    virtual ~ProjectileWeapon();
    virtual void fire(float angle) = 0;
    bool canFire();
    virtual void update(float dt);
    bool isEmpty() const;
    float getFireRate() const;
    int   getAmmo() const;
    void setAmmo(int amount);
    void reload();
};

class Firearm : public ProjectileWeapon {
protected:
    float range;
    float projectileSpeed;

public:
    Firearm();
    virtual ~Firearm();
    virtual void fire(float angle) = 0;
    virtual void update(float dt) override;
};

class Pistol : public Firearm {
private:
    bool isDefault;
    bool isDualFiring;
    float dualFireTimer;

public:
    Pistol();
    void fire(float angle) override;
    void activateDualFire();
    void fireDual(float angle);
    void update(float dt) override;
    void render(RenderWindow& window) override;
    bool getIsDefault() const;
    bool getIsDualFiring() const;
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
    void fire(float angle) override;
    void update(float dt) override;
    void render(RenderWindow& window) override;
    bool isInRange(float targetX, float targetY) const;
    bool canMelee() const;
    void resetCooldown();
    void setPiercing(bool p);
    float getMeleeRange() const;
    int   getMeleeDamage() const;
    bool  getIsPiercing() const;
};

class HeavyMachineGun : public Firearm {
private:
    bool  isHeldDown;
    float spinUpTimer;
    float spinUpDuration;
    float currentSpinLevel;

public:
    HeavyMachineGun();
    void fire(float angle) override;
    void startFire();
    void stopFire();
    void update(float dt) override;
    void render(RenderWindow& window) override;
    float getSpinLevel() const;
    bool  getIsHeldDown() const;
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
    void fire(float angle) override;
    void spawnRocket(float angle);
    void update(float dt) override;
    void render(RenderWindow& window) override;
    bool  isReloadingNow() const;
    float getBlastRadius() const;
    int   getBlastDamage() const;
};

class FlameShot : public Firearm {
private:
    float streamLength;
    int   damagePerSecond;
    bool  isHeldDown;
    bool  streamActive;
    float streamTimer;
    float maxStreamDuration;
    RectangleShape flameRect;

public:
    FlameShot();
    void fire(float angle) override;
    void startStream();
    void stopStream();
    void update(float dt) override;
    void render(RenderWindow& window) override;
    bool isStreamActive() const;
    int  getDamagePerSecond() const;
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
    RectangleShape beamRect;

public:
    LaserGun();
    void fire(float angle) override;
    void spawnBeam(float angle);
    void update(float dt) override;
    void render(RenderWindow& window) override;
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
    CircleShape poolCircle;

public:
    FirePool();
    FirePool(float px, float py, float r, int dps, float dur, int owner);
    void setupVisual();
    void update(float dt);
    void render(RenderWindow& window);
    void applyDamage(float targetX, float targetY, float& dmgOut, float dt);
    bool checkCollision(float tx, float ty, float tw, float th) const;
    bool  isExpired() const;
    bool  getIsActive() const;
    float getX() const;
    float getY() const;
    float getRadius() const;
    int   getDamagePerSecond() const;
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
    Grenade();
    virtual ~Grenade();
    void fire(float launchAngle) override;
    bool checkCollision(float tx, float ty, float tw, float th);
    void applyGravity(float dt);
    void applyBlastDamage(float tx, float ty, int& targetHp);
    bool isInBlastRadius(float tx, float ty) const;
    void launch(float launchAngle, float speed);
    void setThrownBy(PlayerSoldier* p);
    bool  getIsExploded() const;
    float getBlastRadius() const;
    float getVelocityX() const;
    float getVelocityY() const;
    void setIsExploded(bool exploded);
};

class HandGrenade : public Grenade {
private:
    int   bounceCount;
    int   maxBounce;
    float bounceVelocityDampen;
    bool  isGrenadeActive;
    float groundLevel;

public:
    HandGrenade();
    void throwGrenade(float angle);
    void update(float dt) override;
    void reset(float startX, float startY, float vx, float vy);
    void setVelocity(float vx, float vy);
    void setGroundLevel(float gl);
    void explode();
    void render(RenderWindow& window) override;
    void applyBounce();
    int  getBounceCount() const;
    bool getIsActive() const;
};

class FireBombGrenade : public Grenade {
private:
    FirePool* firePool;
    float     firePoolDuration;
    float     firePoolRadius;
    int       firePoolDamagePerSec;
    bool      isBombActive;

public:
    FireBombGrenade();
    ~FireBombGrenade();
    void throwBomb(float angle);
    void update(float dt) override;
    void bombExplode();
    void spawnFirePool();
    void render(RenderWindow& window) override;
    FirePool* getFirePool() const;
    bool      isBombFlying() const;
};

class BulletManager {
private:
    static const int MAX_BULLETS = 200;
    static const int MAX_GRENADES = 20;
    static const int MAX_ROCKETS = 20;

    Bullet      bullets[MAX_BULLETS];
    HandGrenade grenades[MAX_GRENADES];
    Rocket      rockets[MAX_ROCKETS];
    int multiKillCount;

public:
    BulletManager();
    void spawnBullet(float x, float y, float angle,
        int damage, int ownerType,
        float speed = 500.f, float range = 400.f,
        const Color& bulletColor = Color::Yellow);
    void spawnGrenade(float x, float y, float vx, float vy);
    void spawnRocket(float x, float y, float vx, float vy);
    void update(float dt);
    void render(RenderWindow& window, float camX = 0, float camY = 0);
    void clearAll();
    void checkMultiKill(float x, float y, ScoreSystem* scoreSystem);
    void checkPlayerCollisions(float px, float py, float pw, float ph,
        int& totalDamage);
    bool popPlayerBulletHit(float tx, float ty, float tw, float th,
        int& outDamage, float& hitX, float& hitY);
    int getBulletCount() const;
    Bullet* getBullet(int index);
};
