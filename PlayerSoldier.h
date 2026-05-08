#pragma once
#include "Header.h"
#include "Block.h"
#include "Animation.h"
#include <SFML/Graphics.hpp>
#include "entity.h"
class Weapon;
class Vehicle;
class Pistol;
class Knife;

class TransformationState {
public:
    virtual ~TransformationState() {}
    virtual void        update(float dt) = 0;
    virtual void        applyEffects(class PlayerSoldier* s) = 0;
    virtual void        removeEffects(class PlayerSoldier* s) = 0;
    virtual const char* getName()  const = 0;
    virtual bool        isExpired()const = 0;
};

class NormalState : public TransformationState {
public:
    void        update(float dt)               override {}
    void        applyEffects(PlayerSoldier*)   override {}
    void        removeEffects(PlayerSoldier*)  override {}
    const char* getName()  const               override { return "Normal"; }
    bool        isExpired()const               override { return false; }
};

class UndeadState : public TransformationState {
private:
    float timer;
    const float duration;
public:
    UndeadState(float dur = 10.0f) : timer(dur), duration(dur) {}
    void        update(float dt)               override { timer -= dt; }
    void        applyEffects(PlayerSoldier* s) override;
    void        removeEffects(PlayerSoldier*)  override {}
    const char* getName()  const               override { return "Undead"; }
    bool        isExpired()const               override { return timer <= 0.0f; }
};

class MummyState : public TransformationState {
private:
    float timer;
    const float duration;
public:
    MummyState(float dur = 10.0f) : timer(dur), duration(dur) {}
    void        update(float dt)               override { timer -= dt; }
    void        applyEffects(PlayerSoldier* s) override;
    void        removeEffects(PlayerSoldier*)  override {}
    const char* getName()  const               override { return "Mummy"; }
    bool        isExpired()const               override { return timer <= 0.0f; }
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
    int   hp, maxHp, lives;
    bool  isGrounded, isCrouching, isJumping;
    int   grenadeCount, grenadeType;

    Weapon* currentWeapon;
    Pistol* pistol;
    Knife* knife;
    Weapon* weaponSlots[2];
    int      activeWeaponIndex;
    Vehicle* currentVehicle;

    bool  facingRight;
    float aimAngle;
    int   damageState;
    float damageTimer;
    float stateTimer;
    float meleeCooldown, meleeTimer;
    bool  inVehicle;
    bool  isAlive, isImmortal, isFat, isInWater;
    int   score, saturation;
    bool  specialPowerActive;
    float specialPowerTimer, specialPowerDuration;
    float scale_x, scale_y;
    float moveAcceleration;
    Animation anims[8];
    Sprite    sprite;
    int       currentAnim;

    TransformationState* transformState;

    void changeTransformState(TransformationState* newState) {
        if (transformState) {
            transformState->removeEffects(this);
            delete transformState;
        }
        transformState = newState;
        if (transformState) {
            transformState->applyEffects(this);
        }
    }

public:
    PlayerSoldier() : Soldier() {
        player_x = 100.0f; player_y = 500.0f;
        scale_x = 0.5f;   scale_y = 0.5f;
        width = (int)(115 * scale_x);
        height = (int)(158 * scale_y);
        velocityX = velocityY = 0.0f;
        hp = maxHp = 100;
        lives = 3;
        isGrounded = isCrouching = isJumping = false;
        grenadeCount = 10; grenadeType = 0;

        currentWeapon = nullptr;
        pistol = nullptr;
        knife = nullptr;
        weaponSlots[0] = weaponSlots[1] = nullptr;
        activeWeaponIndex = 0;
        currentVehicle = nullptr;

        facingRight = true;
        aimAngle = 0.0f;
        damageState = 0;
        damageTimer = 0.0f;
        stateTimer = 0.0f;
        meleeCooldown = 0.5f;
        meleeTimer = 0.0f;
        inVehicle = false;
        isAlive = true;
        isImmortal = false;
        isFat = false;
        isInWater = false;
        score = 0;
        saturation = 100;
        specialPowerActive = false;
        specialPowerTimer = 0.0f;
        specialPowerDuration = 10.0f;
        moveAcceleration = 0.6f;
        currentAnim = STAND;
        name = "PlayerSoldier";

        transformState = new NormalState();
    }

    virtual ~PlayerSoldier() {
        delete transformState;
        transformState = nullptr;
    }

    void becomeUndead(float duration = 10.0f) {
        changeTransformState(new UndeadState(duration));
    }

    void becomeMummy(float duration = 10.0f) {
        changeTransformState(new MummyState(duration));
    }

    void revertToNormal() {
        changeTransformState(new NormalState());
    }

    const char* getTransformStateName() const {
        return transformState ? transformState->getName() : "None";
    }

    void move(float dt) override {
        player_x += velocityX;
        player_y += velocityY;
    }

    void attack() override { shoot(); }

    void update(float dt) override {
        if (transformState) {
            transformState->update(dt);
            if (transformState->isExpired()) {
                revertToNormal();
            }
        }
        updateAnimation(dt);
    }

    virtual void flipToLeft() = 0;
    virtual void flipToRight() = 0;

    void jump() {
        if (isGrounded && !inVehicle) {
            velocityY = -12.0f;
            isJumping = true;
            isGrounded = false;
        }
    }

    void moveLeft() { velocityX -= moveAcceleration; }
    void moveRight() { velocityX += moveAcceleration; }

    void applyGravity() {
        if (!isGrounded && !inVehicle)
            velocityY += 0.5f;
    }

    void checkGrounded() {
        if (player_y >= 700) {
            player_y = 700;
            velocityY = 0;
            isGrounded = true;
            isJumping = false;
        }
        else {
            isGrounded = false;
        }
    }

    void enterWater() {
        isInWater = true;
        velocityX *= 0.5f;
        velocityY *= 0.5f;
    }

    void exitWater() {
        isInWater = false;
        velocityX *= 2.0f;
        velocityY *= 2.0f;
    }

    void shoot() {
        if (currentWeapon && !inVehicle) {
            currentAnim = SHOOT;
        }
    }

    void meleeAttack() {
        if (meleeTimer <= 0 && !inVehicle) {
            meleeTimer = meleeCooldown;
        }
    }

    void throwGrenade() {
        if (grenadeCount > 0 && !inVehicle) {
            grenadeCount--;
            currentAnim = GRENADE;
        }
    }

    void aim(float angle) { aimAngle = angle; }

    void takeDamage(int damage) {
        if (!isImmortal && isAlive) {
            hp -= damage;
            if (hp <= 0) {
                hp = 0;
                die();
            }
            updateDamageState();
        }
    }

    void takeMeleeDamage(int damage) { takeDamage(damage); }

    void die() {
        isAlive = false;
        lives--;
        if (lives > 0) {
            respawn();
        }
    }

    void respawn() {
        hp = maxHp;
        isAlive = true;
        player_x = 100.0f;
        player_y = 500.0f;
        velocityX = velocityY = 0.0f;
        revertToNormal();
    }

    void updateDamageState() {
        float pct = (float)hp / maxHp;
        damageState = (pct > 0.6f) ? 0 : (pct > 0.3f) ? 1 : 2;
    }

    void pickupWeapon(Weapon* weapon) {
        currentWeapon = weapon;
    }

    void switchWeapon() {
        activeWeaponIndex = (activeWeaponIndex + 1) % 2;
        currentWeapon = weaponSlots[activeWeaponIndex];
    }

    void enterVehicle(Vehicle* vehicle) {
        currentVehicle = vehicle;
        inVehicle = true;
    }

    void exitVehicle() {
        currentVehicle = nullptr;
        inVehicle = false;
    }

    void pickupFood(int saturationValue) {
        saturation += saturationValue;
        if (saturation > 100) {
            saturation = 100;
        }
    }

    void crouch() {
        if (!inVehicle) {
            isCrouching = true;
            height = 50.0f;
        }
    }

    void freePrisoner() {}

    void applyFannumTax() {
        if (isFat) {
            width *= 1.5f;
            height *= 1.5f;
        }
    }

    float getDistanceFrom(float targetX, float targetY) const {
        float dx = player_x - targetX, dy = player_y - targetY;
        return sqrt(dx * dx + dy * dy);
    }

    bool isNearEnemy()    const { return false; }
    bool isNearPrisoner() const { return false; }

    void updateState() {
        if (stateTimer > 0) {
            stateTimer -= 0.016f;
        }
    }

    void update() {
        applyGravity();
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

    virtual void updateAnimation(float dt) { anims[currentAnim].update(dt); }

    virtual void applyVehicleBonus() {}
    virtual void removeVehicleBonus() {}

    virtual void render(RenderWindow& window, float camX = 0, float camY = 0) {
        currentAnim = (velocityX > 0.1f || velocityX < -0.1f) ? WALK : STAND;
        Animation& a = anims[currentAnim];
        if (a.hasLegs()) {
            IntRect lr = a.currentLegsRect();
            sprite.setTexture(a.getLegsTexture());
            sprite.setTextureRect(lr);
            sprite.setOrigin(lr.width / 2.0f, (float)lr.height);
            sprite.setPosition(player_x + width / 2.0f - camX,
                player_y + height - camY + a.getLegsOffsetY() * scale_y);
            sprite.setScale(facingRight ? scale_x : -scale_x, scale_y);
            window.draw(sprite);
        }
        IntRect r = a.currentRect();
        sprite.setTexture(a.getTexture());
        sprite.setTextureRect(r);
        sprite.setOrigin(r.width / 2.0f, (float)r.height);
        float hx = player_x + width / 2.0f - camX
            + (facingRight ? a.getHeadOffsetX() : -a.getHeadOffsetX()) * scale_x;
        sprite.setPosition(hx, player_y + height - camY - a.getHeadOffsetY() * scale_y);
        sprite.setScale(facingRight ? scale_x : -scale_x, scale_y);
        window.draw(sprite);
    }

    virtual float getFireRate() { return 0.2f; }
    virtual float getBlastRadius() { return 50.0f; }
    virtual int   getGrenadeCount() { return grenadeCount; }
    virtual float getMeleeRange() { return 30.0f; }
    virtual int   getMeleeDamage() { return 20; }
    virtual bool  isShielded() { return false; }

    bool  getIsAlive()   const { return isAlive; }
    bool  isFacingRight()const { return facingRight; }
    float getPlayerX()   const { return player_x; }
    float getPlayerY()   const { return player_y; }
    float getVelocityX() const { return velocityX; }
    float getVelocityY() const { return velocityY; }
    int   getHP()        const { return hp; }
    int   getMaxHP()     const { return maxHp; }
    int   getLives()     const { return lives; }
    int   getWidth()     const { return (int)width; }
    int   getHeight()    const { return (int)height; }

    void setPlayerX(float x) { player_x = x; }
    void setPlayerY(float y) { player_y = y; }
    void setPlayerPosition(float x, float y) { player_x = x; player_y = y; }
    void setVelocityX(float vx) { velocityX = vx; }
    void setVelocityY(float vy) { velocityY = vy; }
    void setVelocity(float vx, float vy) { velocityX = vx; velocityY = vy; }
};


inline void UndeadState::applyEffects(PlayerSoldier* s) {
    s->setVelocity(s->getVelocityX() * 0.7f, s->getVelocityY());
}

inline void MummyState::applyEffects(PlayerSoldier* s) {
    s->setVelocity(s->getVelocityX() * 0.5f, s->getVelocityY());
    s->pickupWeapon(nullptr);
}


class Marco : public PlayerSoldier {
private:
    bool  dualFireActive;
    float dualFireTimer;
    float dualFireDuration;

public:
    Marco() : PlayerSoldier() {
        name = "Marco";
        dualFireActive = false;
        dualFireTimer = 0;
        dualFireDuration = 10.0f;
        scale_x = scale_y = 2.0f;
        width = 29.0f; height = 37.0f;

        static const int walkHeadXs[12] = { 10,47,82,115,147,181,216,254,291,328,364,400 };
        static const int walkHeadYs[12] = { 477,477,477,477,477,477,477,477,477,477,477,477 };
        static const int walkHeadWs[12] = { 32,30,28,27,29,30,32,32,32,31,31,31 };
        static const int walkHeadHs[12] = { 29,29,29,29,29,29,29,29,29,29,29,29 };
        static const int walkLegsXs[12] = { 10,36,69,105,129,149,170,196,227,263,288,308 };
        static const int walkLegsYs[12] = { 511,511,511,511,511,511,511,511,511,511,511,511 };
        static const int walkLegsWs[12] = { 21,28,31,19,15,16,21,26,31,20,15,16 };
        static const int walkLegsHs[12] = { 20,20,20,20,20,20,20,20,20,20,20,20 };

        anims[WALK].load("Sprites/Marco Rossi 1.png",
            walkHeadXs, walkHeadYs, walkHeadWs, walkHeadHs, 12, 0.08f);
        anims[WALK].loadLegs("Sprites/Marco Rossi 1.png",
            walkLegsXs, walkLegsYs, walkLegsWs, walkLegsHs, 12, 0.08f, 0);
        anims[WALK].setHeadOffset(13);
        anims[WALK].setHeadOffsetX(6);

        static const int standXs[] = { 10 }; static const int standYs[] = { 693 };
        static const int standWs[] = { 29 }; static const int standHs[] = { 37 };
        anims[STAND].load("Sprites/Marco Rossi 1.png", standXs, standYs, standWs, standHs, 1, 0.18f);
    }

    void flipToLeft()  override { facingRight = false; }
    void flipToRight() override { facingRight = true; }

    void render(RenderWindow& window, float camX = 0, float camY = 0) override {
        PlayerSoldier::render(window, camX, camY);
    }

    void shoot() {
        if (dualFireActive) {
            currentAnim = SHOOT;
        }
        else {
            PlayerSoldier::shoot();
        }
    }

    void meleeAttack() {
        PlayerSoldier::meleeAttack();
    }

    void activateSpecialPower() {
        dualFireActive = true;
        dualFireTimer = dualFireDuration;
        specialPowerActive = true;
        specialPowerTimer = dualFireDuration;
    }

    void updateDualFire() {
        if (dualFireTimer > 0) {
            dualFireTimer -= 0.016f;
            if (dualFireTimer <= 0) {
                dualFireActive = false;
            }
        }
    }

    float getFireRate()     override { return dualFireActive ? 0.1f : 0.2f; }
    int   getGrenadeCount() override { return grenadeCount * 2; }

    void applyVehicleBonus() override { velocityX += 2.0f; }
};


class Tarma : public PlayerSoldier {
private:
    bool  immunityActive;
    float immunityTimer, immunityDuration;
    float vehicleFireRateBonus, vehicleDurabilityBonus;

public:
    Tarma() : PlayerSoldier() {
        name = "Tarma";
        immunityActive = false;
        immunityTimer = 0;
        immunityDuration = 5.0f;
        vehicleFireRateBonus = 1.5f;
        vehicleDurabilityBonus = 1.3f;
        scale_x = scale_y = 2.0f;
        width = 29.0f; height = 37.0f;

        static const int walkHeadXs[12] = { 6,48,87,124,159,197,237,278,318,358,397,435 };
        static const int walkHeadYs[12] = { 582,581,581,581,581,581,582,581,581,582,582,582 };
        static const int walkHeadWs[12] = { 32,30,28,27,29,30,32,32,32,31,31,31 };
        static const int walkHeadHs[12] = { 28,29,28,27,28,28,29,29,28,27,28,28 };
        static const int walkLegsXs[12] = { 10,46,84,126,163,200,238,274,316,359,400,436 };
        static const int walkLegsYs[12] = { 618,618,618,618,618,618,618,618,618,618,618,618 };
        static const int walkLegsWs[12] = { 21,28,31,19,15,16,21,26,31,20,15,18 };
        static const int walkLegsHs[12] = { 20,20,16,20,20,20,20,20,16,20,20,20 };

        anims[WALK].load("Sprites/Tarma Roving.png",
            walkHeadXs, walkHeadYs, walkHeadWs, walkHeadHs, 12, 0.08f);
        anims[WALK].loadLegs("Sprites/Tarma Roving.png",
            walkLegsXs, walkLegsYs, walkLegsWs, walkLegsHs, 12, 0.08f, 0);
        anims[WALK].setHeadOffset(11);
        anims[WALK].setHeadOffsetX(4);

        static const int standXs[] = { 8 }; static const int standYs[] = { 644 };
        static const int standWs[] = { 29 }; static const int standHs[] = { 37 };
        anims[STAND].load("Sprites/Tarma Roving.png", standXs, standYs, standWs, standHs, 1, 0.18f);
    }

    void flipToLeft()  override { facingRight = false; }
    void flipToRight() override { facingRight = true; }

    void render(RenderWindow& window, float camX = 0, float camY = 0) override {
        PlayerSoldier::render(window, camX, camY);
    }

    void takeDamage(int damage) {
        if (immunityActive) {
            return;
        }
        PlayerSoldier::takeDamage(damage);
    }

    void enterVehicle(Vehicle* vehicle) {
        PlayerSoldier::enterVehicle(vehicle);
        applyVehicleBonus();
    }

    void exitVehicle() {
        removeVehicleBonus();
        PlayerSoldier::exitVehicle();
    }

    void onVehicleDestroyed() { activateSpecialPower(); }

    void activateSpecialPower() {
        immunityActive = true;
        immunityTimer = immunityDuration;
        specialPowerActive = true;
        specialPowerTimer = immunityDuration;
    }

    void updateImmunity() {
        if (immunityTimer > 0) {
            immunityTimer -= 0.016f;
            if (immunityTimer <= 0) {
                immunityActive = false;
            }
        }
    }

    bool  isInVehicle()          const { return inVehicle; }
    float getVehicleFireRate()   const { return 0.2f * vehicleFireRateBonus; }
    float getVehicleDurability() const { return vehicleDurabilityBonus; }
};


class Eri : public PlayerSoldier {
private:
    bool  doubleGrenadeActive;
    float doubleGrenadeTimer, doubleGrenadeDuration;

public:
    Eri() : PlayerSoldier() {
        name = "Eri";
        doubleGrenadeActive = false;
        doubleGrenadeTimer = 0;
        doubleGrenadeDuration = 10.0f;
        scale_x = scale_y = 2.0f;
        width = 30.0f; height = 36.0f;

        static const int walkHeadXs[12] = { 26,61,96,130,164,200,236,271,306,343,379,413 };
        static const int walkHeadYs[12] = { 537,538,539,539,539,539,540,540,540,540,540,540 };
        static const int walkHeadWs[12] = { 32,32,31,31,31,31,31,31,32,32,30,30 };
        static const int walkHeadHs[12] = { 28,28,28,28,28,28,28,28,28,28,29,29 };
        static const int walkLegsXs[12] = { 127,155,178,203,232,265,297,326,349,373,400,431 };
        static const int walkLegsYs[12] = { 573,572,572,572,572,573,572,572,573,573,574,574 };
        static const int walkLegsWs[12] = { 24,18,20,22,25,27,25,17,18,21,26,30 };
        static const int walkLegsHs[12] = { 25,26,26,26,25,23,26,26,25,25,23,25 };

        anims[WALK].load("Sprites/Eri Kasamoto.png",
            walkHeadXs, walkHeadYs, walkHeadWs, walkHeadHs, 12, 0.08f);
        anims[WALK].loadLegs("Sprites/Eri Kasamoto.png",
            walkLegsXs, walkLegsYs, walkLegsWs, walkLegsHs, 12, 0.08f, 0);
        anims[WALK].setHeadOffset(11);
        anims[WALK].setHeadOffsetX(4);

        static const int standXs[] = { 6 }; static const int standYs[] = { 606 };
        static const int standWs[] = { 30 }; static const int standHs[] = { 36 };
        anims[STAND].load("Sprites/Eri Kasamoto.png", standXs, standYs, standWs, standHs, 1, 0.18f);
    }

    void flipToLeft()  override { facingRight = false; }
    void flipToRight() override { facingRight = true; }

    void render(RenderWindow& window, float camX = 0, float camY = 0) override {
        PlayerSoldier::render(window, camX, camY);
    }

    void throwGrenade() {
        if (doubleGrenadeActive && grenadeCount >= 2) {
            grenadeCount -= 2;
            currentAnim = GRENADE;
            spawnFirePool();
        }
        else {
            PlayerSoldier::throwGrenade();
        }
    }

    void activateSpecialPower() {
        doubleGrenadeActive = true;
        doubleGrenadeTimer = doubleGrenadeDuration;
        specialPowerActive = true;
        specialPowerTimer = doubleGrenadeDuration;
    }

    void updateDoubleGrenade() {
        if (doubleGrenadeTimer > 0) {
            doubleGrenadeTimer -= 0.016f;
            if (doubleGrenadeTimer <= 0) {
                doubleGrenadeActive = false;
            }
        }
    }

    void spawnFirePool() {}

    int   getGrenadeCount() override { return doubleGrenadeActive ? grenadeCount * 2 : grenadeCount; }
    float getBlastRadius()  override { return doubleGrenadeActive ? 80.0f : 50.0f; }
    float getFireRate()     override { return 0.25f; }

    void meleeAttack() {
        PlayerSoldier::meleeAttack();
    }
};


class Fiolina : public PlayerSoldier {
private:
    bool  superchargedActive;
    float superchargedTimer, superchargedDuration;

public:
    Fiolina() : PlayerSoldier() {
        name = "Fiolina";
        superchargedActive = false;
        superchargedTimer = 0;
        superchargedDuration = 12.0f;
        scale_x = scale_y = 2.0f;
        width = 26.0f; height = 38.0f;

        static const int walkHeadXs[12] = { 10,42,74,106,138,170,203,236,269,303,336,367 };
        static const int walkHeadYs[12] = { 506,507,508,507,506,506,507,508,508,509,507,507 };
        static const int walkHeadWs[12] = { 27,27,27,27,27,28,28,28,29,28,26,27 };
        static const int walkHeadHs[12] = { 26,26,26,26,26,26,26,26,26,26,27,27 };
        static const int walkLegsXs[12] = { 10,37,67,99,129,151,174,200,231,266,295,318 };
        static const int walkLegsYs[12] = { 540,540,541,540,540,541,541,541,541,541,540,540 };
        static const int walkLegsWs[12] = { 22,25,27,25,17,18,21,26,30,24,18,20 };
        static const int walkLegsHs[12] = { 26,25,23,26,26,25,25,23,25,25,26,26 };

        anims[WALK].load("Sprites/Fiolina Germi 1.png",
            walkHeadXs, walkHeadYs, walkHeadWs, walkHeadHs, 12, 0.08f);
        anims[WALK].loadLegs("Sprites/Fiolina Germi 1.png",
            walkLegsXs, walkLegsYs, walkLegsWs, walkLegsHs, 12, 0.08f, 0);
        anims[WALK].setHeadOffset(11);
        anims[WALK].setHeadOffsetX(6);

        static const int standXs[] = { 10 }; static const int standYs[] = { 663 };
        static const int standWs[] = { 29 }; static const int standHs[] = { 34 };
        anims[STAND].load("Sprites/Fiolina Germi 1.png", standXs, standYs, standWs, standHs, 1, 0.18f);
    }

    void flipToLeft()  override { facingRight = false; }
    void flipToRight() override { facingRight = true; }

    void render(RenderWindow& window, float camX = 0, float camY = 0) override {
        PlayerSoldier::render(window, camX, camY);
    }

    void pickupWeapon(Weapon* weapon) {
        PlayerSoldier::pickupWeapon(weapon);
    }

    void activateSpecialPower() {
        superchargedActive = true;
        superchargedTimer = superchargedDuration;
        specialPowerActive = true;
        specialPowerTimer = superchargedDuration;
    }

    void updateSupercharged() {
        if (superchargedTimer > 0) {
            superchargedTimer -= 0.016f;
            if (superchargedTimer <= 0) {
                superchargedActive = false;
            }
        }
    }

    float getFireRate()     override { return superchargedActive ? 0.1f : 0.2f; }
    int   getGrenadeCount() override { return superchargedActive ? grenadeCount + 5 : grenadeCount; }

    void meleeAttack() {
        PlayerSoldier::meleeAttack();
    }
};


class FusionCompanion {
private:
    bool  isActive, isSpawned, isImmortal;
    float x, y, width, height;
    int   hp, maxHp;
    float speed, fireRate;
    int   damage, grenadeCount;
    float blastRadius;

    PlayerSoldier* fusedCharacters[4];
    int   fusedCount;

    float spawnDuration, spawnTimer;
    float cooldownTimer, cooldownDuration;
    int   spriteID;
    float followDistance;
    float averagedFireRate, averagedSpeed;
    int   averagedDamage, averagedGrenadeCount;
    float averagedBlastRadius;

public:
    FusionCompanion() {
        isActive = isSpawned = false;
        isImmortal = true;
        x = y = 0;
        width = 60.0f; height = 90.0f;
        hp = maxHp = 200;
        speed = 4.0f; fireRate = 0.15f;
        damage = 25; grenadeCount = 20; blastRadius = 60.0f;
        fusedCount = 0;
        spawnDuration = 30.0f; spawnTimer = 0;
        cooldownTimer = 0; cooldownDuration = 60.0f;
        spriteID = 0; followDistance = 100.0f;
        averagedFireRate = averagedSpeed = 0;
        averagedDamage = averagedGrenadeCount = 0;
        averagedBlastRadius = 0;
        for (int i = 0; i < 4; i++) fusedCharacters[i] = nullptr;
    }

    ~FusionCompanion() {}

    void fuseCharacters(PlayerSoldier** characters, int count) {
        fusedCount = count;
        for (int i = 0; i < count && i < 4; i++) {
            fusedCharacters[i] = characters[i];
        }
        averageStats();
    }

    void averageStats() {
        if (fusedCount == 0) {
            return;
        }
        float totalFireRate = 0, totalBlast = 0;
        int   totalGrenades = 0;
        for (int i = 0; i < fusedCount; i++) {
            if (fusedCharacters[i]) {
                totalFireRate += fusedCharacters[i]->getFireRate();
                totalGrenades += fusedCharacters[i]->getGrenadeCount();
                totalBlast += fusedCharacters[i]->getBlastRadius();
            }
        }
        averagedFireRate = totalFireRate / fusedCount;
        averagedGrenadeCount = totalGrenades / fusedCount;
        averagedBlastRadius = totalBlast / fusedCount;
    }

    void spawn() { isActive = isSpawned = true;  spawnTimer = spawnDuration; }
    void despawn() { isActive = isSpawned = false; startCooldown(); }

    bool isExpired()        const { return spawnTimer <= 0; }
    bool isCooldownExpired()const { return cooldownTimer <= 0; }
    bool canSpawn()         const { return !isActive && cooldownTimer <= 0; }

    void startCooldown() { cooldownTimer = cooldownDuration; }
    void resetForNewLevel() { despawn(); cooldownTimer = 0; }

    void followPlayer() {}
    void attackNearestEnemy() {}
    void fire() {}

    void throwGrenade() {
        if (grenadeCount > 0) {
            grenadeCount--;
        }
    }

    void update() {
        if (isActive && isSpawned) {
            spawnTimer -= 0.016f;
            if (isExpired()) {
                despawn();
                return;
            }
            followPlayer();
            attackNearestEnemy();
        }
        if (cooldownTimer > 0) {
            cooldownTimer -= 0.016f;
        }
    }

    void render(RenderWindow& window) {}

    float getSurvivalCooldown() const { return 60.0f; }
    float getCampaignCooldown() const { return 120.0f; }
};


class CharacterManager {
private:
    PlayerSoldier* characters[4];
    int activeIndex;
    int activeCharacterCount;

    FusionCompanion* fusionCompanion;
    float fusionCooldownTimer;

    void switchToIndexKeepingPosition(int index) {
        if (!isCharacterAvailable(index) || index == activeIndex) {
            return;
        }
        PlayerSoldier* oldP = characters[activeIndex];
        PlayerSoldier* newP = characters[index];
        if (oldP && newP) {
            newP->setPlayerPosition(oldP->getPlayerX(), oldP->getPlayerY() - oldP->getHeight());
            newP->setVelocity(oldP->getVelocityX(), oldP->getVelocityY());
            oldP->isFacingRight() ? newP->flipToRight() : newP->flipToLeft();
        }
        activeIndex = index;
    }

public:
    CharacterManager() {
        characters[0] = new Marco();
        characters[1] = new Tarma();
        characters[2] = new Eri();
        characters[3] = new Fiolina();
        activeIndex = 0;
        activeCharacterCount = 4;
        fusionCompanion = new FusionCompanion();
        fusionCooldownTimer = 0;
    }

    ~CharacterManager() {
        for (int i = 0; i < 4; i++) {
            delete characters[i];
            characters[i] = nullptr;
        }
        delete fusionCompanion;
        fusionCompanion = nullptr;
    }

    void switchCharacter() {
        int next = getNextAvailableIndex();
        if (next != -1) {
            switchToIndexKeepingPosition(next);
        }
    }

    void switchCharacterToIndex(int index) {
        if (index < 0 || index >= 4 || !isCharacterAvailable(index)) {
            return;
        }
        PlayerSoldier* oldP = characters[activeIndex];
        PlayerSoldier* newP = characters[index];
        if (oldP && newP && index != activeIndex) {
            newP->setPlayerPosition(oldP->getPlayerX(), oldP->getPlayerY());
            newP->setVelocity(oldP->getVelocityX(), oldP->getVelocityY());
        }
        activeIndex = index;
    }

    PlayerSoldier* getActivePlayer() { return characters[activeIndex]; }

    bool isCharacterAvailable(int index) const {
        return (index >= 0 && index < 4)
            && characters[index]
            && characters[index]->getIsAlive();
    }

    int getNextAvailableIndex() const {
        for (int i = 0; i < 4; i++) {
            int next = (activeIndex + i + 1) % 4;
            if (isCharacterAvailable(next)) {
                return next;
            }
        }
        return -1;
    }

    bool allDead() const {
        for (int i = 0; i < 4; i++) {
            if (characters[i] && characters[i]->getIsAlive()) {
                return false;
            }
        }
        return true;
    }

    void resetAll() {
        for (int i = 0; i < 4; i++) {
            if (characters[i]) {
                characters[i]->respawn();
            }
        }
        activeIndex = 0;
    }

    void spawnFusion() {
        if (!fusionCompanion->canSpawn()) {
            return;
        }
        PlayerSoldier* alive[4];
        int count = 0;
        for (int i = 0; i < 4; i++) {
            if (characters[i] && characters[i]->getIsAlive()) {
                alive[count++] = characters[i];
            }
        }
        fusionCompanion->fuseCharacters(alive, count);
        fusionCompanion->spawn();
    }

    void update() {
        if (characters[activeIndex]) {
            characters[activeIndex]->update();
        }
        if (fusionCompanion) {
            fusionCompanion->update();
        }
    }

    void forceSetActiveIndex(int index) {
        if (index >= 0 && index < 4) {
            activeIndex = index;
        }
    }

    void render(RenderWindow& window, float camX = 0, float camY = 0) {
        if (characters[activeIndex]) {
            characters[activeIndex]->render(window, camX, camY);
        }
        if (fusionCompanion) {
            fusionCompanion->render(window);
        }
    }
};