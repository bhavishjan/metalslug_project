#define _CRT_SECURE_NO_WARNINGS
#include "PlayerSoldier.h"
#include <cmath>

// UndeadState implementation
void UndeadState::applyEffects(PlayerSoldier* s) {
    if (!s) return;
    // Apply undead effects
}

// MummyState implementation
void MummyState::applyEffects(PlayerSoldier* s) {
    if (!s) return;
    // Apply mummy effects
}

// PlayerSoldier implementation
PlayerSoldier::PlayerSoldier() : Soldier() {
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
    isImmortal = isFat = isInWater = false;
    score = saturation = 0;
    specialPowerActive = false;
    specialPowerTimer = 0.0f;
    specialPowerDuration = 0.0f;
    moveAcceleration = 0.6f;
    currentAnim = STAND;
    name = "PlayerSoldier";
    transformState = new NormalState();
}

PlayerSoldier::~PlayerSoldier() {
    delete transformState;
    transformState = nullptr;
}

void PlayerSoldier::becomeUndead(float duration) {
    changeTransformState(new UndeadState(duration));
}

void PlayerSoldier::becomeMummy(float duration) {
    changeTransformState(new MummyState(duration));
}

void PlayerSoldier::revertToNormal() {
    changeTransformState(new NormalState());
}

const char* PlayerSoldier::getTransformStateName() const {
    return transformState ? transformState->getName() : "None";
}

void PlayerSoldier::changeTransformState(TransformationState* newState) {
    if (transformState) {
        transformState->removeEffects(this);
        delete transformState;
    }
    transformState = newState;
    if (transformState) {
        transformState->applyEffects(this);
    }
}

void PlayerSoldier::move(float dt) {
    player_x += velocityX;
    player_y += velocityY;
}

void PlayerSoldier::attack() { shoot(); }

void PlayerSoldier::update(float dt) {
    if (transformState) {
        transformState->update(dt);
        if (transformState->isExpired()) {
            revertToNormal();
        }
    }
    updateAnimation(dt);
}

void PlayerSoldier::jump() {
    if (isGrounded && !inVehicle) {
        velocityY = getJumpPower();
        isJumping = true;
        isGrounded = false;
    }
}

void PlayerSoldier::moveLeft() { velocityX -= moveAcceleration; } // move left with acceleration
void PlayerSoldier::moveRight() { velocityX += moveAcceleration; } // move right with acceleration

void PlayerSoldier::applyGravity() {
    if (!isGrounded && !inVehicle)
        velocityY += 0.5f; // apply gravity when not grounded
}

void PlayerSoldier::checkGrounded() {
    if (player_y >= 700) {
        player_y = 700;
        velocityY = 0;
        isGrounded = true;
        isJumping = false;
    }
    else {
        isGrounded = false;
    }
} // check if player is on ground

void PlayerSoldier::enterWater() {
    isInWater = true;
    velocityX *= 0.5f; // slow down in water
    velocityY *= 0.5f;
}

void PlayerSoldier::exitWater() {
    isInWater = false;
    velocityX *= 2.0f; // restore speed when exiting water
    velocityY *= 2.0f;
}

void PlayerSoldier::shoot() {
    if (currentWeapon && !inVehicle) {
        currentAnim = SHOOT;
    }
}

void PlayerSoldier::meleeAttack() {
    if (meleeTimer <= 0 && !inVehicle) {
        meleeTimer = meleeCooldown;
    }
}

void PlayerSoldier::throwGrenade() {
    if (grenadeCount > 0 && !inVehicle) {
        grenadeCount--;
        currentAnim = GRENADE;
    }
}

void PlayerSoldier::aim(float angle) { aimAngle = angle; }

void PlayerSoldier::takeDamage(int damage) {
    if (!isImmortal && isAlive) {
        hp -= damage;
        if (hp <= 0) {
            hp = 0;
            die();
        }
        updateDamageState();
    }
}

void PlayerSoldier::takeMeleeDamage(int damage) { takeDamage(damage); }

void PlayerSoldier::die() {
    isAlive = false;
    lives--;
    onPlayerDied();
}

void PlayerSoldier::forceRespawn() {
    if (lives >= 0) {
        respawn();
    }
}

void PlayerSoldier::onPlayerDied() {
    // Override in subclasses if needed
}

void PlayerSoldier::respawn() {
    hp = maxHp;
    isAlive = true;
    player_x = 100.0f;
    player_y = 500.0f;
    velocityX = velocityY = 0.0f;
    revertToNormal();
}

void PlayerSoldier::updateDamageState() {
    float pct = (float)hp / maxHp;
    damageState = (pct > 0.6f) ? 0 : (pct > 0.3f) ? 1 : 2;
}

void PlayerSoldier::pickupWeapon(Weapon* weapon) {
    currentWeapon = weapon;
}

void PlayerSoldier::switchWeapon() {
    activeWeaponIndex = (activeWeaponIndex + 1) % 2;
    currentWeapon = weaponSlots[activeWeaponIndex];
}

void PlayerSoldier::enterVehicle(Vehicle* vehicle) {
    currentVehicle = vehicle;
    inVehicle = true;
}

void PlayerSoldier::exitVehicle() {
    currentVehicle = nullptr;
    inVehicle = false;
}

void PlayerSoldier::pickupFood(int saturationValue) {
    saturation += saturationValue;
    if (saturation > 100) {
        saturation = 100;
    }
}

void PlayerSoldier::crouch() {
    if (!inVehicle) {
        isCrouching = true;
        height = 50.0f;
    }
}

void PlayerSoldier::freePrisoner() {}

void PlayerSoldier::applyFannumTax() {
    if (isFat) {
        width *= 1.5f;
        height *= 1.5f;
    }
}

float PlayerSoldier::getDistanceFrom(float targetX, float targetY) const {
    float dx = player_x - targetX, dy = player_y - targetY;
    return sqrt(dx * dx + dy * dy);
}

bool PlayerSoldier::isNearEnemy() const { return false; }
bool PlayerSoldier::isNearPrisoner() const { return false; }

void PlayerSoldier::updateState() {
    if (stateTimer > 0) {
        stateTimer -= 0.016f;
    }
}

void PlayerSoldier::update() {
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

void PlayerSoldier::updateAnimation(float dt) { anims[currentAnim].update(dt); }

void PlayerSoldier::applyVehicleBonus() {}
void PlayerSoldier::removeVehicleBonus() {}

void PlayerSoldier::renderHealthBar(RenderWindow& window, float camX, float camY) {
    const float barWidth = 50.0f;
    const float barHeight = 8.0f;
    const float barOffsetY = -20.0f;

    float healthPercent = (float)hp / maxHp;
    if (healthPercent < 0) healthPercent = 0;
    if (healthPercent > 1) healthPercent = 1;

    RectangleShape bgBar(Vector2f(barWidth, barHeight));
    bgBar.setPosition(player_x + width / 2.0f - barWidth / 2.0f - camX, player_y + barOffsetY - camY);
    bgBar.setFillColor(Color(100, 0, 0));
    window.draw(bgBar);

    RectangleShape healthBar(Vector2f(barWidth * healthPercent, barHeight));
    healthBar.setPosition(player_x + width / 2.0f - barWidth / 2.0f - camX, player_y + barOffsetY - camY);
    healthBar.setFillColor(Color(0, 255, 0));
    window.draw(healthBar);

    RectangleShape border(Vector2f(barWidth, barHeight));
    border.setPosition(player_x + width / 2.0f - barWidth / 2.0f - camX, player_y + barOffsetY - camY);
    border.setFillColor(Color::Transparent);
    border.setOutlineThickness(2);
    border.setOutlineColor(Color::White);
    window.draw(border);
}

void PlayerSoldier::render(RenderWindow& window, float camX, float camY) {
    currentAnim = (velocityX > 0.1f || velocityX < -0.1f) ? WALK : STAND;
    Animation& a = anims[currentAnim];

    if (a.hasLegs()) {
        IntRect lr = a.currentLegsRect();
        if (lr.width > 0 && lr.height > 0) {
            sprite.setTexture(a.getLegsTexture(), true);
            sprite.setTextureRect(lr);
            sprite.setOrigin(lr.width / 2.0f, (float)lr.height);
            sprite.setPosition(player_x + width / 2.0f - camX, player_y + height - camY + a.getLegsOffsetY() * 2.0f);
            float scX = facingRight ? 2.0f : -2.0f;
            sprite.setScale(scX, 2.0f);
            window.draw(sprite);
        }
    }

    IntRect r = a.currentRect();
    if (r.width > 0 && r.height > 0) {
        sprite.setTexture(a.getTexture(), true);
        sprite.setTextureRect(r);
        sprite.setOrigin(r.width / 2.0f, (float)r.height);
        float hx = player_x + width / 2.0f - camX + (facingRight ? a.getHeadOffsetX() : -a.getHeadOffsetX()) * 2.0f;
        sprite.setPosition(hx, player_y + height - camY - a.getHeadOffsetY() * 2.0f);
        float scX = facingRight ? 2.0f : -2.0f;
        sprite.setScale(scX, 2.0f);
        window.draw(sprite);
    }

    renderHealthBar(window, camX, camY);
}

float PlayerSoldier::getFireRate() { return 0.2f; }
float PlayerSoldier::getBlastRadius() { return 50.0f; }
int PlayerSoldier::getGrenadeCount() { return grenadeCount; }
float PlayerSoldier::getMeleeRange() { return 30.0f; }
int PlayerSoldier::getMeleeDamage() { return 20; }
bool PlayerSoldier::isShielded() { return false; }

void PlayerSoldier::setPlayerX(float x) { player_x = x; }
void PlayerSoldier::setPlayerY(float y) { player_y = y; }
void PlayerSoldier::setPlayerPosition(float x, float y) { player_x = x; player_y = y; }
void PlayerSoldier::setVelocityX(float vx) { velocityX = vx; }
void PlayerSoldier::setVelocityY(float vy) { velocityY = vy; }
void PlayerSoldier::setVelocity(float vx, float vy) { velocityX = vx; velocityY = vy; }
void PlayerSoldier::setGrounded(bool grounded) { isGrounded = grounded; }

bool PlayerSoldier::getIsAlive() const { return isAlive; }
bool PlayerSoldier::isFacingRight() const { return facingRight; }
float PlayerSoldier::getPlayerX() const { return player_x; }
float PlayerSoldier::getPlayerY() const { return player_y; }
float PlayerSoldier::getVelocityX() const { return velocityX; }
float PlayerSoldier::getVelocityY() const { return velocityY; }
int PlayerSoldier::getHP() const { return hp; }
int PlayerSoldier::getMaxHP() const { return maxHp; }
int PlayerSoldier::getLives() const { return lives; }
int PlayerSoldier::getWidth() const { return width; }
int PlayerSoldier::getHeight() const { return height; }
float PlayerSoldier::getFriction() const { return 0.8f; }
float PlayerSoldier::getAirFriction() const { return 0.95f; }
float PlayerSoldier::getJumpPower() const { return -40.0f; }
float PlayerSoldier::getGravity() const { return 4.0f; }
float PlayerSoldier::getMaxFallSpeed() const { return 15.0f; }
bool PlayerSoldier::getIsGrounded() const { return isGrounded; }



// Marco implementations
Marco::Marco() : PlayerSoldier() {
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
    static const int standXs[] = { 10 }; static const int standYs[] = { 693 };
    static const int standWs[] = { 29 }; static const int standHs[] = { 37 };

    anims[WALK].load("Sprites/Marco Rossi 1.png", walkHeadXs, walkHeadYs, walkHeadWs, walkHeadHs, 12, 0.08f);
    anims[WALK].loadLegs("Sprites/Marco Rossi 1.png", walkLegsXs, walkLegsYs, walkLegsWs, walkLegsHs, 12, 0.08f, 0);
    anims[WALK].setHeadOffset(13);
    anims[WALK].setHeadOffsetX(6);
    anims[STAND].load("Sprites/Marco Rossi 1.png", standXs, standYs, standWs, standHs, 1, 0.18f);
}

void Marco::flipToLeft() { facingRight = false; }
void Marco::flipToRight() { facingRight = true; }

void Marco::render(RenderWindow& window, float camX, float camY) {
    PlayerSoldier::render(window, camX, camY);
}

void Marco::shoot() {
    if (dualFireActive) {
        currentAnim = SHOOT;
    }
    else {
        PlayerSoldier::shoot();
    }
}

void Marco::meleeAttack() {
    PlayerSoldier::meleeAttack();
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

float Marco::getFireRate() { return dualFireActive ? 0.1f : 0.2f; }
int Marco::getGrenadeCount() { return grenadeCount * 2; }
void Marco::applyVehicleBonus() { velocityX += 2.0f; }





// Tarma implementations
Tarma::Tarma() : PlayerSoldier() {
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
    static const int standXs[] = { 8 }; static const int standYs[] = { 644 };
    static const int standWs[] = { 29 }; static const int standHs[] = { 37 };

    anims[WALK].load("Sprites/Tarma Roving.png", walkHeadXs, walkHeadYs, walkHeadWs, walkHeadHs, 12, 0.08f);
    anims[WALK].loadLegs("Sprites/Tarma Roving.png", walkLegsXs, walkLegsYs, walkLegsWs, walkLegsHs, 12, 0.08f, 0);
    anims[WALK].setHeadOffset(11);
    anims[WALK].setHeadOffsetX(4);
    anims[STAND].load("Sprites/Tarma Roving.png", standXs, standYs, standWs, standHs, 1, 0.18f);
}

void Tarma::flipToLeft() { facingRight = false; }
void Tarma::flipToRight() { facingRight = true; }

void Tarma::render(RenderWindow& window, float camX, float camY) {
    PlayerSoldier::render(window, camX, camY);
}

void Tarma::takeDamage(int damage) {
    if (immunityActive) {
        return;
    }
    PlayerSoldier::takeDamage(damage);
}

void Tarma::enterVehicle(Vehicle* vehicle) {
    PlayerSoldier::enterVehicle(vehicle);
    applyVehicleBonus();
}

void Tarma::exitVehicle() {
    removeVehicleBonus();
    PlayerSoldier::exitVehicle();
}

void Tarma::onVehicleDestroyed() { activateSpecialPower(); }

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

bool Tarma::isInVehicle() const { return inVehicle; }
float Tarma::getVehicleFireRate() const { return 0.2f * vehicleFireRateBonus; }
float Tarma::getVehicleDurability() const { return vehicleDurabilityBonus; }





// Eri implementations
Eri::Eri() : PlayerSoldier() {
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
    static const int standXs[] = { 6 }; static const int standYs[] = { 606 };
    static const int standWs[] = { 30 }; static const int standHs[] = { 36 };

    anims[WALK].load("Sprites/Eri Kasamoto.png", walkHeadXs, walkHeadYs, walkHeadWs, walkHeadHs, 12, 0.08f);
    anims[WALK].loadLegs("Sprites/Eri Kasamoto.png", walkLegsXs, walkLegsYs, walkLegsWs, walkLegsHs, 12, 0.08f, 0);
    anims[WALK].setHeadOffset(11);
    anims[WALK].setHeadOffsetX(4);
    anims[STAND].load("Sprites/Eri Kasamoto.png", standXs, standYs, standWs, standHs, 1, 0.18f);
}

void Eri::flipToLeft() { facingRight = false; }
void Eri::flipToRight() { facingRight = true; }

void Eri::render(RenderWindow& window, float camX, float camY) {
    PlayerSoldier::render(window, camX, camY);
}

void Eri::throwGrenade() {
    if (doubleGrenadeActive && grenadeCount >= 2) {
        grenadeCount -= 2;
        currentAnim = GRENADE;
        spawnFirePool();
    }
    else {
        PlayerSoldier::throwGrenade();
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

void Eri::spawnFirePool() {}

int Eri::getGrenadeCount() { return doubleGrenadeActive ? grenadeCount * 2 : grenadeCount; }
float Eri::getBlastRadius() { return doubleGrenadeActive ? 80.0f : 50.0f; }
float Eri::getFireRate() { return 0.25f; }

void Eri::meleeAttack() {
    PlayerSoldier::meleeAttack();
}





// Fiolina implementations
Fiolina::Fiolina() : PlayerSoldier() {
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
    static const int standXs[] = { 10 }; static const int standYs[] = { 663 };
    static const int standWs[] = { 29 }; static const int standHs[] = { 34 };

    anims[WALK].load("Sprites/Fiolina Germi 1.png", walkHeadXs, walkHeadYs, walkHeadWs, walkHeadHs, 12, 0.08f);
    anims[WALK].loadLegs("Sprites/Fiolina Germi 1.png", walkLegsXs, walkLegsYs, walkLegsWs, walkLegsHs, 12, 0.08f, 0);
    anims[WALK].setHeadOffset(11);
    anims[WALK].setHeadOffsetX(6);
    anims[STAND].load("Sprites/Fiolina Germi 1.png", standXs, standYs, standWs, standHs, 1, 0.18f);
}

void Fiolina::flipToLeft() { facingRight = false; }
void Fiolina::flipToRight() { facingRight = true; }

void Fiolina::render(RenderWindow& window, float camX, float camY) {
    PlayerSoldier::render(window, camX, camY);
}

void Fiolina::pickupWeapon(Weapon* weapon) {
    PlayerSoldier::pickupWeapon(weapon);
}

void Fiolina::activateSpecialPower() {
    superchargedActive = true;
    superchargedTimer = superchargedDuration;
    specialPowerActive = true;
    specialPowerTimer = superchargedDuration;
}

void Fiolina::updateSupercharged() {
    if (superchargedTimer > 0) {
        superchargedTimer -= 0.016f;
        if (superchargedTimer <= 0) {
            superchargedActive = false;
        }
    }
}

float Fiolina::getFireRate() { return superchargedActive ? 0.1f : 0.2f; }
int Fiolina::getGrenadeCount() { return superchargedActive ? grenadeCount + 5 : grenadeCount; }

void Fiolina::meleeAttack() {
    PlayerSoldier::meleeAttack();
}





// FusionCompanion implementations
FusionCompanion::FusionCompanion() {
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

FusionCompanion::~FusionCompanion() {}

void FusionCompanion::fuseCharacters(PlayerSoldier** chars, int count) {
    fusedCount = count;
    for (int i = 0; i < count && i < 4; i++) {
        fusedCharacters[i] = chars[i];
    }
    averageStats();
}

void FusionCompanion::averageStats() {
    if (fusedCount == 0) {
        return;
    }
    float totalFireRate = 0, totalBlast = 0;
    int totalGrenades = 0;
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

void FusionCompanion::spawn() { isActive = isSpawned = true; spawnTimer = spawnDuration; }
void FusionCompanion::despawn() { isActive = isSpawned = false; startCooldown(); }

bool FusionCompanion::isExpired() const { return spawnTimer <= 0; }
bool FusionCompanion::isCooldownExpired() const { return cooldownTimer <= 0; }
bool FusionCompanion::canSpawn() const { return !isActive && cooldownTimer <= 0; }

void FusionCompanion::startCooldown() { cooldownTimer = cooldownDuration; }
void FusionCompanion::resetForNewLevel() { despawn(); cooldownTimer = 0; }

void FusionCompanion::followPlayer() {}
void FusionCompanion::attackNearestEnemy() {}
void FusionCompanion::fire() {}

void FusionCompanion::throwGrenade() {
    if (grenadeCount > 0) {
        grenadeCount--;
    }
}

void FusionCompanion::update() {
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

void FusionCompanion::render(RenderWindow& window) {}

float FusionCompanion::getSurvivalCooldown() const { return 60.0f; }
float FusionCompanion::getCampaignCooldown() const { return 120.0f; }





// CharacterManager implementations
CharacterManager::CharacterManager() {
    Player[0] = new Marco();
    Player[1] = new Tarma();
    Player[2] = new Eri();
    Player[3] = new Fiolina();
    activeIndex = 0;
    activeCharacterCount = 4;
    fusionCompanion = new FusionCompanion();
    fusionCooldownTimer = 0;
}

CharacterManager::~CharacterManager() {
    for (int i = 0; i < 4; i++) {
        delete Player[i];
        Player[i] = nullptr;
    }
    delete fusionCompanion;
    fusionCompanion = nullptr;
}

void CharacterManager::switchToIndexKeepingPosition(int index) {
    if (!isCharacterAvailable(index) || index == activeIndex) {
        return;
    }
    PlayerSoldier* oldP = Player[activeIndex];
    PlayerSoldier* newP = Player[index];
    if (oldP && newP) {
        newP->setPlayerPosition(oldP->getPlayerX(), oldP->getPlayerY() - oldP->getHeight());
        newP->setVelocity(oldP->getVelocityX(), oldP->getVelocityY());
        oldP->isFacingRight() ? newP->flipToRight() : newP->flipToLeft();
    }
    activeIndex = index;
}

void CharacterManager::switchCharacter() {
    int next = getNextAvailableIndex();
    if (next != -1) {
        switchToIndexKeepingPosition(next);
    }
}

void CharacterManager::switchCharacterToIndex(int index) {
    if (index < 0 || index >= 4 || !isCharacterAvailable(index)) {
        return;
    }
    PlayerSoldier* oldP = Player[activeIndex];
    PlayerSoldier* newP = Player[index];
    if (oldP && newP && index != activeIndex) {
        newP->setPlayerPosition(oldP->getPlayerX(), oldP->getPlayerY());
        newP->setVelocity(oldP->getVelocityX(), oldP->getVelocityY());
    }
    activeIndex = index;
}

PlayerSoldier* CharacterManager::getActivePlayer() { return Player[activeIndex]; }

float CharacterManager::getX() { return Player[activeIndex]->getPlayerX(); }
float CharacterManager::getY() { return Player[activeIndex]->getPlayerY(); }
float CharacterManager::getVelocityX() { return Player[activeIndex]->getVelocityX(); }
float CharacterManager::getVelocityY() { return Player[activeIndex]->getVelocityY(); }
int CharacterManager::getWidth() { return Player[activeIndex]->getWidth(); }
int CharacterManager::getHeight() { return Player[activeIndex]->getHeight(); }
bool CharacterManager::getIsGrounded() { return Player[activeIndex]->getIsGrounded(); }
bool CharacterManager::isFacingRight() { return Player[activeIndex]->isFacingRight(); }

bool CharacterManager::isCharacterAvailable(int index) const {
    return (index >= 0 && index < 4)
        && Player[index]
        && Player[index]->getIsAlive();
}

int CharacterManager::getNextAvailableIndex() const {
    for (int i = 0; i < 4; i++) {
        int next = (activeIndex + i + 1) % 4;
        if (isCharacterAvailable(next)) {
            return next;
        }
    }
    return -1;
}

bool CharacterManager::allDead() const {
    for (int i = 0; i < 4; i++) {
        if (Player[i] && Player[i]->getIsAlive()) {
            return false;
        }
    }
    return true;
}

void CharacterManager::resetAll() {
    for (int i = 0; i < 4; i++) {
        if (Player[i]) {
            Player[i]->respawn();
        }
    }
    activeIndex = 0;
}

void CharacterManager::spawnFusion() {
    if (!fusionCompanion->canSpawn()) {
        return;
    }
    PlayerSoldier* alive[4];
    int count = 0;
    for (int i = 0; i < 4; i++) {
        if (Player[i] && Player[i]->getIsAlive()) {
            alive[count++] = Player[i];
        }
    }
    fusionCompanion->fuseCharacters(alive, count);
    fusionCompanion->spawn();
}

void CharacterManager::update() {
    if (Player[activeIndex]) {
        Player[activeIndex]->update();
    }
    if (fusionCompanion) {
        fusionCompanion->update();
    }
}

void CharacterManager::forceSetActiveIndex(int index) {
    if (index >= 0 && index < 4) {
        activeIndex = index;
    }
}

// Developer mode support methods implementation
void PlayerSoldier::setImmortal(bool immortal) {
    isImmortal = immortal;
}

void PlayerSoldier::setWeapon(Weapon* weapon) {
    currentWeapon = weapon;
}

void CharacterManager::render(RenderWindow& window) {
    if (Player[activeIndex]) {
        Player[activeIndex]->render(window, 0, 0);
    }
    if (fusionCompanion) {
        fusionCompanion->render(window);
    }
}





