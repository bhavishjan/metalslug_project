#pragma once

class Level;
class Weapon;
class Vehicle;
class Pistol;
class Knife;
class PlayerSoldier;

#include "Block.h"
#include "Animation.h"
#include <SFML/Graphics.hpp>
#include "Entity.h"


class TransformationState {
public:
    virtual ~TransformationState() {}
    virtual void update(float dt) = 0;
    virtual void applyEffects(PlayerSoldier* s) = 0;
    virtual void removeEffects(PlayerSoldier* s) = 0;
    virtual const char* getName() const = 0;
    virtual bool isExpired() const = 0;
};

class NormalState : public TransformationState {
public:
    void update(float dt) override {}
    void applyEffects(PlayerSoldier*) override {}
    void removeEffects(PlayerSoldier*) override {}
    const char* getName() const override { return "Normal"; }
    bool isExpired() const override { return false; }
};

class UndeadState : public TransformationState {
private:
    float timer;
    const float duration;
public:
    UndeadState(float dur = 8.0f) : timer(dur), duration(dur) {}
    void update(float dt) override { timer -= dt; }
    void applyEffects(PlayerSoldier* s) override;
    void removeEffects(PlayerSoldier*) override {}
    const char* getName() const override { return "Undead"; }
    bool isExpired() const override { return timer <= 0.0f; }
};

class MummyState : public TransformationState {
private:
    float timer;
    const float duration;
public:
    MummyState(float dur = 10.0f) : timer(dur), duration(dur) {}
    void update(float dt) override { timer -= dt; }
    void applyEffects(PlayerSoldier* s) override;
    void removeEffects(PlayerSoldier*) override {}
    const char* getName() const override { return "Mummy"; }
    bool isExpired() const override { return timer <= 0.0f; }
};

class PlayerSoldier : public Soldier {
protected:
    const int WALK = 0;
    const int STAND = 1;
    const int SHOOT = 2;
    const int GRENADE = 3;

    const char* name;
    float player_x, player_y;
    float width, height;
    float velocityX, velocityY;
    int hp, maxHp, lives;
    bool isGrounded, isCrouching, isJumping;
    int grenadeCount, grenadeType;

    Weapon* currentWeapon;
    Pistol* pistol;
    Knife* knife;
    Weapon* weaponSlots[2];
    int activeWeaponIndex;
    Vehicle* currentVehicle;

    bool facingRight;
    float aimAngle;
    int damageState;
    float damageTimer;
    float stateTimer;
    float meleeCooldown, meleeTimer;
    bool inVehicle;
    bool isAlive, isImmortal, isFat, isInWater;
    int score, saturation;
    bool specialPowerActive;
    float specialPowerTimer, specialPowerDuration;
    float scale_x, scale_y;
    float moveAcceleration;
    Animation anims[8];
    Sprite sprite;
    int currentAnim;
    TransformationState* transformState;

public:
    PlayerSoldier();
    virtual ~PlayerSoldier();

    void becomeUndead(float duration = 10.0f);
    void becomeMummy(float duration = 10.0f);
    void revertToNormal();
    const char* getTransformStateName() const;
    void changeTransformState(TransformationState* newState);

    void move(float dt) override;
    void attack() override;
    void update(float dt) override;
    virtual void flipToLeft() = 0;
    virtual void flipToRight() = 0;

    void jump();
    void moveLeft();
    void moveRight();
    void applyGravity();
    void checkGrounded();
    void enterWater();
    void exitWater();
    void shoot();
    void meleeAttack();
    void throwGrenade();
    void aim(float angle);
    void takeDamage(int damage);
    void takeMeleeDamage(int damage);
    void die();
    void forceRespawn();
    virtual void onPlayerDied();
    void respawn();
    void updateDamageState();
    void pickupWeapon(Weapon* weapon);
    void switchWeapon();
    void enterVehicle(Vehicle* vehicle);
    void exitVehicle();
    void pickupFood(int saturationValue);
    void crouch();
    void freePrisoner();
    void applyFannumTax();
    float getDistanceFrom(float targetX, float targetY) const;
    bool isNearEnemy() const;
    bool isNearPrisoner() const;
    void updateState();
    void update();
    virtual void updateAnimation(float dt);
    virtual void applyVehicleBonus();
    virtual void removeVehicleBonus();
    void renderHealthBar(RenderWindow& window, float camX = 0, float camY = 0);
    virtual void render(RenderWindow& window, float camX = 0, float camY = 0);

    virtual float getFireRate();
    virtual float getBlastRadius();
    virtual int getGrenadeCount();
    virtual float getMeleeRange();
    virtual int getMeleeDamage();
    virtual bool isShielded();

    bool getIsAlive() const;
    bool isFacingRight() const;
    float getPlayerX() const;
    float getPlayerY() const;
    float getVelocityX() const;
    float getVelocityY() const;
    int getHP() const;
    int getMaxHP() const;
    int getLives() const;
    int getWidth() const;
    int getHeight() const;
    float getFriction() const;
    float getAirFriction() const;
    float getJumpPower() const;
    float getGravity() const;
    float getMaxFallSpeed() const;
    bool getIsGrounded() const;

    void setPlayerX(float x);
    void setPlayerY(float y);
    
    // Developer mode support methods
    void setImmortal(bool immortal);
    void setWeapon(Weapon* weapon);
    void setPlayerPosition(float x, float y);
    void setVelocityX(float vx);
    void setVelocityY(float vy);
    void setVelocity(float vx, float vy);
    void setGrounded(bool grounded);
};

class Marco : public PlayerSoldier {
private:
    bool dualFireActive;
    float dualFireTimer;
    float dualFireDuration;

public:
    Marco();
    void flipToLeft() override;
    void flipToRight() override;
    void render(RenderWindow& window, float camX = 0, float camY = 0) override;
    void shoot();
    void meleeAttack();
    void activateSpecialPower();
    void updateDualFire();
    float getFireRate() override;
    int getGrenadeCount() override;
    void applyVehicleBonus() override;
};

class Tarma : public PlayerSoldier {
private:
    bool immunityActive;
    float immunityTimer, immunityDuration;
    float vehicleFireRateBonus, vehicleDurabilityBonus;

public:
    Tarma();
    void flipToLeft() override;
    void flipToRight() override;
    void render(RenderWindow& window, float camX = 0, float camY = 0) override;
    void takeDamage(int damage);
    void enterVehicle(Vehicle* vehicle);
    void exitVehicle();
    void onVehicleDestroyed();
    void activateSpecialPower();
    void updateImmunity();
    bool isInVehicle() const;
    float getVehicleFireRate() const;
    float getVehicleDurability() const;
};

class Eri : public PlayerSoldier {
private:
    bool doubleGrenadeActive;
    float doubleGrenadeTimer, doubleGrenadeDuration;

public:
    Eri();
    void flipToLeft() override;
    void flipToRight() override;
    void render(RenderWindow& window, float camX = 0, float camY = 0) override;
    void throwGrenade();
    void activateSpecialPower();
    void updateDoubleGrenade();
    void spawnFirePool();
    int getGrenadeCount() override;
    float getBlastRadius() override;
    float getFireRate() override;
    void meleeAttack();
};

class Fiolina : public PlayerSoldier {
private:
    bool superchargedActive;
    float superchargedTimer, superchargedDuration;

public:
    Fiolina();
    void flipToLeft() override;
    void flipToRight() override;
    void render(RenderWindow& window, float camX = 0, float camY = 0) override;
    void pickupWeapon(Weapon* weapon);
    void activateSpecialPower();
    void updateSupercharged();
    float getFireRate() override;
    int getGrenadeCount() override;
    void meleeAttack();
};

class FusionCompanion {
private:
    bool isActive, isSpawned, isImmortal;
    float x, y, width, height;
    int hp, maxHp;
    float speed, fireRate;
    int damage, grenadeCount;
    float blastRadius;
    PlayerSoldier* fusedCharacters[4];
    int fusedCount;
    float spawnDuration, spawnTimer;
    float cooldownTimer, cooldownDuration;
    int spriteID;
    float followDistance;
    float averagedFireRate, averagedSpeed;
    int averagedDamage, averagedGrenadeCount;
    float averagedBlastRadius;

public:
    FusionCompanion();
    ~FusionCompanion();
    void fuseCharacters(PlayerSoldier** chars, int count);
    void averageStats();
    void spawn();
    void despawn();
    bool isExpired() const;
    bool isCooldownExpired() const;
    bool canSpawn() const;
    void startCooldown();
    void resetForNewLevel();
    void followPlayer();
    void attackNearestEnemy();
    void fire();
    void throwGrenade();
    void update();
    void render(RenderWindow& window);
    float getSurvivalCooldown() const;
    float getCampaignCooldown() const;
};

class CharacterManager {
private:
    PlayerSoldier* Player[4];
    int activeIndex;
    int activeCharacterCount;
    FusionCompanion* fusionCompanion;
    float fusionCooldownTimer;

    void switchToIndexKeepingPosition(int index);

public:
    CharacterManager();
    ~CharacterManager();
    void switchCharacter();
    void switchCharacterToIndex(int index);
    PlayerSoldier* getActivePlayer();
    float getX();
    float getY();
    float getVelocityX();
    float getVelocityY();
    int getWidth();
    int getHeight();
    bool getIsGrounded();
    bool isFacingRight();
    bool isCharacterAvailable(int index) const;
    int getNextAvailableIndex() const;
    bool allDead() const;
    void resetAll();
    void spawnFusion();
    void update();
    void forceSetActiveIndex(int index);
    void render(RenderWindow& window);
};