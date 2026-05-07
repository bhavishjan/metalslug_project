#pragma once
#include "Player.h"
#include <cmath>
#include "Weapon.h"

using namespace std;
using namespace sf;


// helper function to check if two rectangles overlap
// used for collision detection between enemies and player
static bool rectsOverlap(float ax, float ay, float aw, float ah,
    float bx, float by, float bw, float bh)
{
    return ax < bx + bw && ax + aw > bx &&
        ay < by + bh && ay + ah > by;
}


// abstract base class for all enemies
// every enemy type inherits from this
class Enemy {
protected:
    const char* name;
    float x, y;
    float width, height;
    float velocityX, velocityY;
    float hp;
    int   maxHp;
    float speed;
    int   damage;
    bool  isAlive;
    bool  isGrounded;
    bool  isCrouched;
    bool  facingRight;
    float spawnX, spawnY;
    int   scoreValue;
    bool  isShielded;
    float aggressionLevel;

    // memory of coward system
    bool  hasGrudge;
    float grudgeMultiplier;
    bool  isEnhanced;        // power boost when player scrolls past

    // detection variables
    float detectionRange;
    float attackRange;
    int   currentBiome;
    bool  isPatrolling;
    bool  isTargetingPlayer;
    Player* largestPlayer;

    float gravityConstant = 80.0f;
    float groundY = 1600 - height;  // default floor (screen_y - height)
public:
    Enemy() : x(0), y(0), width(32), height(48),
        velocityX(0), velocityY(0),
        hp(1), maxHp(1), speed(80.f), damage(1),
        isAlive(true), isGrounded(false), isCrouched(false),
        facingRight(true), spawnX(0), spawnY(0),
        scoreValue(0), isShielded(false), aggressionLevel(1.f),
        hasGrudge(false), grudgeMultiplier(1.f), isEnhanced(false),
        detectionRange(300.f), attackRange(250.f), currentBiome(0),
        isPatrolling(true), isTargetingPlayer(false), largestPlayer(nullptr) {
    }

    // virtual destructor so child destructors get called properly
    virtual ~Enemy() {}

    // pure virtuals every enemy must implement these
    virtual void attack() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) = 0;

    // overide in case of special behaviour
    virtual void move(float dt) {
        if (isTargetingPlayer) chasePlayer(largestPlayer);
        else                   patrol();
        // x position updated externally in main.cpp after collision checks
    }

    virtual void takeDamage(int dmg, float bulletX = 0, float bulletY = 0,
        bool isExplosive = false)
    {
        // grudge enemies need more damage to kill
        hp -= dmg / grudgeMultiplier;
        if (hp <= 0) die();
    }

    virtual void die() {
        isAlive = false;
        dropLoot();
    }

    void detectPlayer(Player* player) {
        float xx = player->getPlayerX();
        float yy = player->getPlayerY();

        // distance of player from enemy
        float dx = xx - x;
        float dy = yy - y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < detectionRange) {
            isTargetingPlayer = true;
            largestPlayer = player;
        }
        else {
            isTargetingPlayer = false;
        }
    }

    void chasePlayer(Player* player) {
        float xp = player->getPlayerX();
        if (xp > x) {
            velocityX = speed;
            facingRight = true;
        }
        else {
            velocityX = -speed;
            facingRight = false;
        }
    }

    void patrol() {
        float patrollingRange = 50;

        if (x > spawnX + patrollingRange) {
            velocityX = -speed;  // move left
            facingRight = false;
        }
        else if (x < spawnX - patrollingRange) {
            velocityX = speed;   // move right
            facingRight = true;
        }
        else {
            // inside range so keep moving in current direction
            velocityX = speed;
        }
    }

    void applyGravity(float deltaTime) {
        if (isGrounded == false) {
            velocityY += gravityConstant * deltaTime;
        }
    }

    void checkGrounded() {
        if (y + height >= groundY) {
            y = groundY - height;
            velocityY = 0.f;
            isGrounded = true;
        }
        else {
            isGrounded = false;
        }
    }

    // block enemy movement into player — revert enemy x so they stop at player edge
    void checkPlayerCollision(Player* player) {
        if (!player || !isAlive) return;

        float px = player->getPlayerX();
        float py = player->getPlayerY();
        float pw = (float)player->getWidth();
        float ph = (float)player->getHeight();

        if (!rectsOverlap(x, y, width, height, px, py, pw, ph)) return;

        float overlapLeft  = (x + width) - px;
        float overlapRight = (px + pw)   - x;

        // stop enemy at player boundary — neither pushed
        if (overlapLeft < overlapRight) {
            x -= overlapLeft;   // enemy was moving right, push enemy back left
        }
        else {
            x += overlapRight;  // enemy was moving left, push enemy back right
        }
        velocityX = 0.f;
    }

    // call when enemy dies chance of food drop
    void dropLoot() {
        int roll = rand() % 100;
        if (roll < 20)  // 20 percent chance of food drop
            spawnLoot();
    }

    void spawnLoot();

    int getScoreValue() const { return scoreValue; }

    // memory of cowardice power boost
    void applyGrudge() {
        // enhance enemy when player scrolls past him
        hasGrudge = true;
        isEnhanced = true;
        grudgeMultiplier = 1.5f;
        speed *= 1.25f;
        damage = (int)(damage * 1.3f);
        hp = (float)maxHp;  // full HP restored
    }

    // extra power boost on top of grudge
    void enhancePower() {
        speed *= 1.15f;
        damage = (int)(damage * 1.2f);
        hp += (float)(maxHp) * 0.25f;  // plus 25 percent hp
    }

    void returnToPlayer(Player* player) {
        if (!player) return;
        largestPlayer = player;
        isTargetingPlayer = true;
        chasePlayer(player);
    }

    // getters and setters
    bool  getIsAlive()  const { return isAlive; }
    float getX()        const { return x; }
    float getY()        const { return y; }
    float getHP()       const { return hp; }
    float getMaxHP()    const { return (float)maxHp; }
    bool  isDead()      const { return !isAlive; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
	bool getIsTargetingPlayer() const { return isTargetingPlayer; }
	bool getIsGrounded() const { return isGrounded; }


    void setPosition(float nx, float ny) { x = nx; y = ny; }
    void setPlayer(Player* p) { largestPlayer = p; }
    void setX(float nx) { x = nx; }
    void setGroundY(float gy) { groundY = gy; }
    float getVelocityY() const { return velocityY; }
    void  setVelocityY(float vy) { velocityY = vy; }
    float getVelocityX() const { return velocityX; }
    void  setVelocityX(float vx) { velocityX = vx; }
    void  setGrounded(bool g) { isGrounded = g; if (g) velocityY = 0.f; }
};


// base class for all infantry type enemies
// has pistol and basic movement
class InfantryEnemy : public Enemy {
protected:
    static const int WALK  = 0;
    static const int STAND = 1;
    static const int SHOOT = 2;
    static const int DIE   = 3;

    float   fireRate;
    float   fireTimer;
    int     bulletDamage;
    int     batchSize;
    float   reloadTimer;
    float   reloadDuration;
    Pistol  pistol;
    Sprite  sprite;
    Texture texture;
    Animation anims[4];
    int currentAnim;

public:
    InfantryEnemy() : Enemy(),
        fireRate(3.f), fireTimer(0.f),
        bulletDamage(3), batchSize(2),
        reloadTimer(0.f), reloadDuration(2.f),
        currentAnim(STAND) {
    }

    virtual ~InfantryEnemy() {}

    virtual void move(float deltaTime) {
        if (isTargetingPlayer)
            chasePlayer(largestPlayer);
        else
            patrol();
        // x position updated externally in main.cpp after collision checks
    }

    void attack() override {
        // check player exist
        if (largestPlayer == nullptr) 
            return;

        // can pistol fire
        if (!pistol.canFire()) 
            return;

        // distance check
        float dx = largestPlayer->getPlayerX() - x;
        float dy = largestPlayer->getPlayerY() - y;
        float distance = sqrt(dx * dx + dy * dy);

        // range check
        if (distance > attackRange) return;

        // angle towards player
        float angle = atan2(dy, dx);

        // firing
        pistol.fire(angle);
        fireTimer = 0.4f;   // drives SHOOT animation state

        // facing direction update
        facingRight = (dx > 0);
    }

    void update(float dt) override {
        if (!isAlive)             return;
        if (largestPlayer == nullptr) return;

        detectPlayer(largestPlayer);
        move(dt);

        float dx = largestPlayer->getPlayerX() - x;
        float dy = largestPlayer->getPlayerY() - y;
        float distanceToplayer = sqrt(dx * dx + dy * dy);

        if (isTargetingPlayer && distanceToplayer < attackRange)
            attack();

        pistol.update(dt);
        if (fireTimer > 0.f)  
            fireTimer -= dt;

        updateAnimState(dt);
    }

    // pick animation based on current enemy behavior and advance its frame
    virtual void updateAnimState(float dt) {
        int desired;
        if (!isAlive) {
            desired = DIE;
        }
        else if (fireTimer > 0.f) {
            desired = SHOOT;
        }
        else if (velocityX > 0.5f || velocityX < -0.5f) {
            desired = WALK;
        }
        else {
            desired = STAND;
        }

        if (desired != currentAnim) {
            currentAnim = desired;
            anims[currentAnim].reset();
        }

        anims[currentAnim].update(dt);
    }

    virtual void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override {
        if (!isAlive && currentAnim != DIE) return;

        Animation& a = anims[currentAnim];
        IntRect r = a.currentRect();
        if (r.width == 0 || r.height == 0) return;

        sprite.setTexture(a.getTexture(), true);
        sprite.setTextureRect(r);

        // sprite sheet faces LEFT by default, so flip when facing right
        float sc = 2.0f;
        float scX;
        if (facingRight) scX = -sc;
        else scX = sc;
        sprite.setOrigin(r.width / 2.0f, (float)r.height);
        sprite.setPosition((x + width / 2.0f) - camX, (y + height) - camY);
        sprite.setScale(scX, sc);

        window.draw(sprite);
    }
};


// most basic enemy has pistol and patrols
// spawns in batches of 2 to 4
class RebelSoldier : public InfantryEnemy {
private:
    float patrolRange;

public:
    RebelSoldier() {
        name = "Rebel Soldier";
        hp = 2;
        maxHp = 2;
        scoreValue = 50;       // 50 points
        batchSize = 3;        // 2 to 4 middle value
        patrolRange = 150.0f;

        speed = 80.0f;  // pixels per second
        damage = 3;      // pistol is 3 HP

        fireRate = 4.0f;   // 4 shots per sec
        fireTimer = 0.0f;
        bulletDamage = 3;

        detectionRange = 800.0f;
        attackRange = 300.0f;

        isAlive = true;
        isGrounded = false;
        facingRight = true;
        isPatrolling = true;
        isTargetingPlayer = false;
        largestPlayer = nullptr;

        grudgeMultiplier = 1.0f;

        width = 32.0f;
        height = 48.0f;

        // walk animation (12 frame running cycle - row 1)
        static const int walkXs[12] = {   3,  40,  80, 125, 170, 210, 248, 285, 326, 369, 410, 450 };
        static const int walkYs[12] = {  44,  44,  45,  46,  45,  44,  44,  44,  45,  46,  45,  44 };
        static const int walkWs[12] = {  34,  37,  42,  42,  37,  35,  34,  38,  40,  38,  37,  35 };
        static const int walkHs[12] = {  37,  37,  31,  33,  35,  37,  37,  36,  30,  32,  35,  37 };

        // stand animation (1 frame idle - row 0)
        static const int standXs[1] = {  3 };
        static const int standYs[1] = {  3 };
        static const int standWs[1] = { 40 };
        static const int standHs[1] = { 38 };

        // shoot animation (10 frame rifle fire - row 8, y≈330)
        static const int shootXs[10] = {   3,  50, 101, 151, 196, 284, 257, 330, 380, 429 };
        static const int shootYs[10] = { 298, 298, 298, 298, 289, 292, 298, 298, 298, 298 };
        static const int shootWs[10] = {  44,  48,  47,  42,  41,  43,  38,  47,  46,  40 };
        static const int shootHs[10] = {  38,  38,  38,  40,  47,  44,  38,  38,  38,  38 };

        // die animation (4 frame collapse - row 20)
        static const int dieXs[4] = {   3,  44,  86, 129 };
        static const int dieYs[4] = { 784, 785, 786, 786 };
        static const int dieWs[4] = {  38,  39,  40,  40 };
        static const int dieHs[4] = {  33,  32,  31,  31 };

        anims[WALK].load("Sprites/Enemies/Rebel Soldier.png", walkXs, walkYs, walkWs, walkHs, 12, 0.07f);
        anims[STAND].load("Sprites/Enemies/Rebel Soldier.png", standXs, standYs, standWs, standHs, 1, 0.18f);
        anims[SHOOT].load("Sprites/Enemies/Rebel Soldier.png", shootXs, shootYs, shootWs, shootHs, 10, 0.08f);
        anims[DIE].load("Sprites/Enemies/Rebel Soldier.png", dieXs, dieYs, dieWs, dieHs, 4, 0.15f);
    }

    // uses parent InfantryEnemy attack no need to override
    void attack() override {
        InfantryEnemy::attack();
    }

    void update(float dt) override {
        InfantryEnemy::update(dt);
    }

    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override {
        InfantryEnemy::render(window, camX, camY);
    }
};


// throws grenades in ballistic arc toward player
// spawns in batches of 1 to 2
class GrenadeSoldier : public InfantryEnemy {
private:
    int          grenadeCount;
    float        throwCooldown;
    HandGrenade  grenades[5];
    int          activeGrenades;
    bool         isActive;

public:
    GrenadeSoldier() : grenadeCount(5), throwCooldown(0.f), activeGrenades(0), isActive(false)
    {
        name = "Grenade Soldier";
        hp = 2.f;
        maxHp = 2;
        speed = 60.f;
        damage = 8;       // grenade does more damage
        scoreValue = 100;
        batchSize = 1;       // spawns in 1 to 2

        detectionRange = 400.f;
        attackRange = 350.f;  // grenade has longer range

        width = 25.f;
        height = 35.f;

        // walk animation - row Y=17 (7 frames)
        static const int walkXs[7] = {  2,  25,  51,  81, 110, 153, 203 };
        static const int walkYs[7] = { 35,  25,  19,  17,  20,  24,  25 };
        static const int walkWs[7] = { 18,  21,  26,  25,  36,  46,  46 };
        static const int walkHs[7] = { 17,  27,  33,  35,  32,  28,  27 };

        // stand animation - first frame of walk row
        static const int standXs[1] = {  81 };
        static const int standYs[1] = {  17 };
        static const int standWs[1] = {  25 };
        static const int standHs[1] = {  35 };

        // throw grenade animation - row Y=54 (5 frames)
        static const int throwXs[5] = {  2,  45,  90, 125, 157 };
        static const int throwYs[5] = { 54,  74,  67,  66,  70 };
        static const int throwWs[5] = { 42,  39,  30,  26,  37 };
        static const int throwHs[5] = { 47,  27,  34,  35,  31 };

        // die animation - row Y=162 (6 frames)
        static const int dieXs[6] = {   2,  68, 103, 130, 155, 180 };
        static const int dieYs[6] = { 162, 165, 167, 167, 167, 167 };
        static const int dieWs[6] = {  65,  30,  23,  20,  20,  21 };
        static const int dieHs[6] = {  49,  46,  28,  30,  32,  30 };

        anims[WALK].load("Sprites/Enemies/Rebel Soldier (RPG-2).png", walkXs, walkYs, walkWs, walkHs, 7, 0.08f);
        anims[STAND].load("Sprites/Enemies/Rebel Soldier (RPG-2).png", standXs, standYs, standWs, standHs, 1, 0.18f);
        anims[SHOOT].load("Sprites/Enemies/Rebel Soldier (RPG-2).png", throwXs, throwYs, throwWs, throwHs, 5, 0.12f);
        anims[DIE].load("Sprites/Enemies/Rebel Soldier (RPG-2).png", dieXs, dieYs, dieWs, dieHs, 6, 0.15f);
    }

    void attack() override {
        // check player exist
        if (largestPlayer == nullptr) return;

        // cooldown check
        if (throwCooldown > 0) return;

        // distance check
        float dx = largestPlayer->getPlayerX() - x;
        float dy = largestPlayer->getPlayerY() - y;
        float distance = sqrt(dx * dx + dy * dy);

        // range check
        if (distance > attackRange) return;

        // throw grenade toward player
        throwGrenade();

        // reset cooldown
        throwCooldown = 2.0f;
    }

    void throwGrenade() {
        if (activeGrenades >= 5) return;
        if (grenadeCount <= 0)   return;

        float tx = largestPlayer->getPlayerX();
        float ty = largestPlayer->getPlayerY();

        // time to reach player in 1.2 sec
        float flightTime = 1.2f;
        float gravity = 980.0f;

        float dx = tx - x;
        float dy = ty - y;

        // using ballistic projectile formula
        // velocity is distance over time
        float velX = dx / flightTime;
        float velY = (dy - 0.5 * gravity * flightTime * flightTime) / flightTime;

        HandGrenade& g = grenades[activeGrenades];
        g.launch(atan2(velY, velX), sqrt(velX * velX + velY * velY));

        activeGrenades++;
        grenadeCount--;
    }

    void update(float dt) override {
        if (!isAlive)             return;
        if (largestPlayer == nullptr) return;

        if (throwCooldown > 0) throwCooldown -= dt;

        detectPlayer(largestPlayer);
        move(dt);

        float dx = largestPlayer->getPlayerX() - x;
        float dy = largestPlayer->getPlayerY() - y;
        float distanceToplayer = sqrt(dx * dx + dy * dy);

        if (isTargetingPlayer && distanceToplayer < attackRange)
            attack();

        pistol.update(dt);
    }

    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override {
        InfantryEnemy::render(window, camX, camY);
    }
};


// rocket class used by bazooka soldier and bosses
// gravity affects it after firing
class Rocket {
private:
    float x, y;
    float velX, velY;
    bool  active;

public:
    Rocket() {
        x = y = 0;
        velX = velY = 0;
        active = false;
    }

    void setPosition(float px, float py) { x = px; y = py; }
    void setVelocity(float vx, float vy) { velX = vx; velY = vy; }
    void setActive(bool state) { active = state; }

    bool  isActive() const { return active; }
    float getX()     const { return x; }
    float getY()     const { return y; }

    void update(float dt) {
        if (!active) return;
        velY += 980 * dt;  // gravity pulls rocket down
        x += velX * dt;
        y += velY * dt;
    }

    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f);
};


// slow moving fires rockets in steep arc
// spawns in batches of 1 to 2
class BazookaSoldier : public InfantryEnemy {
private:
    float  rocketReloadTimer;
    float  rocketReloadDuration;
    Rocket rockets[5];  // rocket cant exist without bazooka
    int    activeRockets;

public:
    BazookaSoldier() {
        name = "Bazooka Soldier";
        hp = 2.f;
        maxHp = 2;
        speed = 50.f;   // slow moving
        damage = 15;     // rocket does more damage
        scoreValue = 100;
        batchSize = 1;      // spawns in 1 to 2

        detectionRange = 450.f;
        attackRange = 400.f;

        width = 40.f;
        height = 48.f;

        rocketReloadDuration = 2.5f;
        rocketReloadTimer = 0;
        activeRockets = 0;

        // walk animation - row Y=3 H=45 (6 frames)
        static const int walkXs[6] = {   3,  42,  83, 124, 163, 204 };
        static const int walkYs[6] = {   3,   3,   5,   6,   5,   3 };
        static const int walkWs[6] = {  36,  38,  38,  36,  38,  38 };
        static const int walkHs[6] = {  45,  45,  43,  42,  43,  45 };

        // stand animation - row Y=51 H=43 first frame
        static const int standXs[1] = {   3 };
        static const int standYs[1] = {  55 };
        static const int standWs[1] = {  41 };
        static const int standHs[1] = {  39 };

        // fire rocket animation - row Y=241 H=44 (3 frames)
        static const int fireXs[3] = {   3,  45,  89 };
        static const int fireYs[3] = { 241, 243, 245 };
        static const int fireWs[3] = {  39,  41,  41 };
        static const int fireHs[3] = {  44,  42,  40 };

        // die animation - row Y=395 H=51 (9 frames)
        static const int dieXs[9] = {   3,  52,  99, 147, 191, 235, 279, 318, 361 };
        static const int dieYs[9] = { 404, 407, 401, 403, 398, 398, 398, 395, 399 };
        static const int dieWs[9] = {  46,  44,  45,  41,  41,  41,  36,  38,  31 };
        static const int dieHs[9] = {  42,  39,  45,  43,  48,  48,  48,  51,  47 };

        anims[WALK].load("Sprites/Enemies/Rebel Soldier (Bazooka).png", walkXs, walkYs, walkWs, walkHs, 6, 0.08f);
        anims[STAND].load("Sprites/Enemies/Rebel Soldier (Bazooka).png", standXs, standYs, standWs, standHs, 1, 0.18f);
        anims[SHOOT].load("Sprites/Enemies/Rebel Soldier (Bazooka).png", fireXs, fireYs, fireWs, fireHs, 3, 0.14f);
        anims[DIE].load("Sprites/Enemies/Rebel Soldier (Bazooka).png", dieXs, dieYs, dieWs, dieHs, 9, 0.12f);
    }

    void attack() override {
        // check player exist
        if (largestPlayer == nullptr) return;

        // distance check
        float dx = largestPlayer->getPlayerX() - x;
        float dy = largestPlayer->getPlayerY() - y;
        float distance = sqrt(dx * dx + dy * dy);

        // range check
        if (distance > attackRange) return;

        // fire rocket toward player
        fireRocket();

        // facing direction update
        facingRight = (dx > 0);
    }

    void fireRocket() {
        // cooldown check
        if (rocketReloadTimer > 0)    return;
        if (activeRockets >= 5)       return;
        if (largestPlayer == nullptr) return;

        float tx = largestPlayer->getPlayerX();
        float ty = largestPlayer->getPlayerY();

        // steeper arc flight time
        float flightTime = 1.5f;
        float gravity = 980.0f;

        float dx = tx - x;
        float dy = ty - y;

        // using ballistic projectile formula
        float velX = dx / flightTime;
        float velY = (dy - 0.5 * gravity * flightTime * flightTime) / flightTime;

        // rocket spawn at soldier position
        rockets[activeRockets].setPosition(x, y);
        rockets[activeRockets].setVelocity(velX, velY);
        rockets[activeRockets].setActive(true);

        rocketReloadTimer = rocketReloadDuration;
        activeRockets++;
    }

    void update(float dt) override {
        if (!isAlive)             return;
        if (largestPlayer == nullptr) return;

        if (rocketReloadTimer > 0.f) rocketReloadTimer -= dt;

        detectPlayer(largestPlayer);
        move(dt);

        float dx = largestPlayer->getPlayerX() - x;
        float dy = largestPlayer->getPlayerY() - y;
        float distanceToplayer = sqrt(dx * dx + dy * dy);

        if (isTargetingPlayer && distanceToplayer < attackRange)
            attack();

        // update all active rockets
        for (int i = 0; i < activeRockets; ++i)
            rockets[i].update(dt);
    }

    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override {
        InfantryEnemy::render(window, camX, camY);
    }
};


// has riot shield blocks frontal bullets
// must be hit from above behind or with explosives
// spawns in batches of 1 to 2
class ShieldedSoldier : public InfantryEnemy {
private:
    bool shieldActive;
    int  shieldDurability;

public:
    ShieldedSoldier() : shieldActive(true), shieldDurability(3)
    {
        name = "Shielded Soldier";
        hp = 5.f;    // 5 HP per project spec
        maxHp = 5;
        speed = 70.f;
        damage = 3;
        scoreValue = 120;
        batchSize = 1;      // spawns in 1 to 2

        detectionRange = 300.f;
        attackRange = 250.f;

        width = 55.f; 
        height = 48.f;

        // walk animation - row Y=4 H=39 (6 frames)
        static const int walkXs[6] = {   3,  38,  73, 107, 143, 177 };
        static const int walkYs[6] = {   5,   5,   5,   4,   5,   5 };
        static const int walkWs[6] = {  32,  32,  31,  32,  31,  32 };
        static const int walkHs[6] = {  38,  38,  38,  39,  38,  38 };

        // stand animation - row Y=131 H=38 first frame
        static const int standXs[1] = {   3 };
        static const int standYs[1] = { 131 };
        static const int standWs[1] = {  32 };
        static const int standHs[1] = {  38 };

        // shoot animation - row Y=379 H=48 (9 frames)
        static const int shootXs[9] = {   3,  39,  82, 126, 167, 218, 274, 326, 369 };
        static const int shootYs[9] = { 389, 388, 379, 379, 385, 389, 389, 387, 393 };
        static const int shootWs[9] = {  33,  40,  41,  38,  48,  53,  49,  40,  55 };
        static const int shootHs[9] = {  38,  39,  48,  48,  42,  38,  38,  40,  34 };

        // die animation - row Y=481 H=44 (9 frames)
        static const int dieXs[9] = {   3,  37,  72, 115, 162, 209, 257, 305, 353 };
        static const int dieYs[9] = { 481, 482, 482, 482, 483, 484, 485, 484, 483 };
        static const int dieWs[9] = {  31,  32,  38,  44,  44,  45,  45,  45,  44 };
        static const int dieHs[9] = {  39,  40,  40,  41,  41,  40,  40,  40,  41 };

        anims[WALK].load("Sprites/Enemies/Rebel Soldier (Shield).png", walkXs, walkYs, walkWs, walkHs, 6, 0.08f);
        anims[STAND].load("Sprites/Enemies/Rebel Soldier (Shield).png", standXs, standYs, standWs, standHs, 1, 0.18f);
        anims[SHOOT].load("Sprites/Enemies/Rebel Soldier (Shield).png", shootXs, shootYs, shootWs, shootHs, 9, 0.10f);
        anims[DIE].load("Sprites/Enemies/Rebel Soldier (Shield).png", dieXs, dieYs, dieWs, dieHs, 9, 0.12f);
    }

    void takeDamage(int dmg, float bulletX, float bulletY,
        bool isExplosive) override
    {
        // explosives bypass shield completly
        if (isExplosive) {
            hp -= dmg;
            if (hp <= 0) die();
            return;
        }

        bool isFront = false;
        bool isAbove = false;

        // bullet from above check
        if (bulletY < y) isAbove = true;

        // bullet from front check
        if (facingRight) {
            if (bulletX > x) isFront = true;
        }
        else {
            if (bulletX < x) isFront = true;
        }

        // shield blocks frontal non explosive bullets
        if (shieldActive && isFront && !isAbove) {
            shieldDurability--;
            if (shieldDurability <= 0)
                shieldActive = false;
            return;  // no damage taken
        }

        // hit from behind or above so take damage
        hp -= dmg;
        if (hp <= 0) die();
    }

    void checkShieldBreak() {
        if (shieldDurability <= 0)
            shieldActive = false;
    }

    void attack() override {
        InfantryEnemy::attack();
    }

    // shield soldier update same as infantry
    void update(float dt) override {
        if (!isAlive)             return;
        if (largestPlayer == nullptr) return;

        detectPlayer(largestPlayer);
        move(dt);

        float dx = largestPlayer->getPlayerX() - x;
        float dy = largestPlayer->getPlayerY() - y;
        float distanceToplayer = sqrt(dx * dx + dy * dy);

        if (isTargetingPlayer && distanceToplayer < attackRange)
            attack();

        pistol.update(dt);
    }

    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override {
        InfantryEnemy::render(window, camX, camY);
    }
};


// abstract base for all aerial enemies
// gravity is ignored for these
class AerialEnemy : public Enemy {
protected:
    static const int FLY     = 0;
    static const int DESCEND = 1;
    static const int ATTACK  = 2;
    static const int DIE     = 3;

    float   flyHeight;
    bool    isDescending;
    float   descentSpeed;
    int     batchSize;
    Sprite  sprite;
    Texture texture;
    Animation anims[4];
    int currentAnim;

public:
    AerialEnemy() : currentAnim(FLY) {
        flyHeight = 200.f;
        isDescending = false;
        descentSpeed = 60.f;
        batchSize = 2;
        speed = 90.f;
        detectionRange = 400.f;
        attackRange = 300.f;
        width = 32.f;
        height = 32.f;
        isGrounded = true;  // aerial enemies ignore gravity

    }

    virtual ~AerialEnemy() {}

    virtual void fly() {
        // if too low go up if too high go down
        if (y > flyHeight)
            velocityY = -descentSpeed * 0.5f;
        else if (y < flyHeight - 10.f)
            velocityY = descentSpeed * 0.5f;
        else
            velocityY = 0.f;
    }

    virtual void descend() {
        isDescending = true;
        velocityY = descentSpeed;
    }

    void move(float dt) override {
        if (isTargetingPlayer && largestPlayer)
            chasePlayer(largestPlayer);
        else
            patrol();

        if (!isDescending) fly();

        x += velocityX * dt;
        y += velocityY * dt;
    }

    void attack()  override;

    void update(float dt) override {
        if (!isAlive || !largestPlayer) return;
        detectPlayer(largestPlayer);
        move(dt);
        checkPlayerCollision(largestPlayer);
    }

    virtual void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override {
        if (!isAlive && currentAnim != DIE) return;
        Animation& a = anims[currentAnim];
        IntRect r = a.currentRect();
        if (r.width == 0 || r.height == 0) return;
        sprite.setTexture(a.getTexture(), true);
        sprite.setTextureRect(r);
        float sc = 2.0f;
        float scX = facingRight ? sc : -sc;
        sprite.setOrigin(r.width / 2.0f, (float)r.height);
        sprite.setPosition((x + width / 2.0f) - camX, (y + height) - camY);
        sprite.setScale(scX, sc);
        window.draw(sprite);
    }
};


// descends from top as one of the 4 infantry types
// doesnt attack while descending becomes infantry on landing
class Paratrooper : public AerialEnemy {
private:
    bool hasLanded;

public:
    Paratrooper() {
        hasLanded = false;
        name = "Paratrooper";
        hp = maxHp = 2;
        scoreValue = 75;       // base infantry plus 25
        flyHeight = -50.f;    // starts above screen
        isDescending = true;

        // fly animation
        static const int flyXs[2] = { 0, 0 };
        static const int flyYs[2] = { 0, 0 };
        static const int flyWs[2] = { 0, 0 };
        static const int flyHs[2] = { 0, 0 };

        // descend animation (parachute open)
        static const int descendXs[2] = { 0, 0 };
        static const int descendYs[2] = { 0, 0 };
        static const int descendWs[2] = { 0, 0 };
        static const int descendHs[2] = { 0, 0 };

        // attack animation (after landing)
        static const int attackXs[4] = { 0, 0, 0, 0 };
        static const int attackYs[4] = { 0, 0, 0, 0 };
        static const int attackWs[4] = { 0, 0, 0, 0 };
        static const int attackHs[4] = { 0, 0, 0, 0 };

        // die animation
        static const int dieXs[4] = { 0, 0, 0, 0 };
        static const int dieYs[4] = { 0, 0, 0, 0 };
        static const int dieWs[4] = { 0, 0, 0, 0 };
        static const int dieHs[4] = { 0, 0, 0, 0 };

        anims[FLY].load("Sprites/Paratrooper.png", flyXs, flyYs, flyWs, flyHs, 2, 0.15f);
        anims[DESCEND].load("Sprites/Paratrooper.png", descendXs, descendYs, descendWs, descendHs, 2, 0.20f);
        anims[ATTACK].load("Sprites/Paratrooper.png", attackXs, attackYs, attackWs, attackHs, 4, 0.10f);
        anims[DIE].load("Sprites/Paratrooper.png", dieXs, dieYs, dieWs, dieHs, 4, 0.12f);
    }

    ~Paratrooper() {}

    void descend() override {
        AerialEnemy::descend();
    }

    // paratrooper doesnt attack while in air
    // becomes infantry after landing
    void attack() override {}

    void spawnInfantry() {
        // paratrooper landed so act like rebel soldier now
        hasLanded = true;
        name = "Rebel Soldier";
        speed = 80.f;
        isDescending = false;
        // now normal infantry behaviour starts
    }

    void update(float dt) override {
        if (!isAlive) return;

        move(dt);  // descend toward ground

        // check if landed floor check
        float GROUND_Y = 520.f;
        if (y + height >= GROUND_Y) {
            y = GROUND_Y - height;
            if (!hasLanded)
                spawnInfantry();
        }

        if (largestPlayer)
            checkPlayerCollision(largestPlayer);
    }

    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override {
        AerialEnemy::render(window, camX, camY);
    }
};


// abstract base for undead enemies
// immune to bullets only fire and explosions kill them
class UndeadEnemy : public Enemy {
protected:
    static const int WALK   = 0;
    static const int ATTACK = 1;
    static const int SHOOT  = 2;
    static const int HURT   = 3;
    static const int DIE    = 4;

    bool transformOnContact;
    bool onlyDeadFromFire;
    Sprite  sprite;
    Texture texture;
    Animation anims[5];
    int currentAnim;

public:
    UndeadEnemy() : transformOnContact(true), onlyDeadFromFire(true), currentAnim(WALK)
    {
        speed = 40.f;   // undead are slow
        detectionRange = 500.f;  // but sense player from far away
        attackRange = 30.f;   // melee only
        hp = maxHp = 5;
    }

    virtual ~UndeadEnemy() {}

    virtual void transformPlayer(Player* p) {
        if (!p) return;
        // child classes implement the actual transform effect
    }

    // undead immune to bullets only explosions and fire kill them
    void takeDamage(int dmg, float bulletX, float bulletY,
        bool isExplosive) override
    {
        if (!isExplosive) return;  // normal bullets do nothing
        hp -= dmg;
        if (hp <= 0.f) die();
    }

    void update(float dt) override {
        if (!isAlive || !largestPlayer) return;
        detectPlayer(largestPlayer);
        move(dt);
    }

    virtual void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override {
        if (!isAlive && currentAnim != DIE) return;
        Animation& a = anims[currentAnim];
        IntRect r = a.currentRect();
        if (r.width == 0 || r.height == 0) return;
        sprite.setTexture(a.getTexture(), true);
        sprite.setTextureRect(r);
        float sc = 2.0f;
        float scX = facingRight ? sc : -sc;
        sprite.setOrigin(r.width / 2.0f, (float)r.height);
        sprite.setPosition((x + width / 2.0f) - camX, (y + height) - camY);
        sprite.setScale(scX, sc);
        window.draw(sprite);
    }
};


// slowly walks toward player and transforms on contact
// crumbles when hit but resurrects after 3 seconds
// only fire kills it instantly per project spec
class MummyWarrior : public UndeadEnemy {
private:
    bool  isCrumbled;
    float resumeTimer;
    float resumeDuration;

public:
    MummyWarrior() : isCrumbled(false), resumeTimer(0.f), resumeDuration(3.f)
    {
        name = "Mummy Warrior";
        hp = maxHp = 5;
        scoreValue = 150;
        speed = 35.f;
        damage = 5;
        width = 32.f;
        height = 52.f;
        transformOnContact = true;
        onlyDeadFromFire = true;


        // walk animation (slow shamble)
        static const int walkXs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int walkYs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int walkWs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int walkHs[6] = { 0, 0, 0, 0, 0, 0 };

        // attack animation (melee transform)
        static const int attackXs[4] = { 0, 0, 0, 0 };
        static const int attackYs[4] = { 0, 0, 0, 0 };
        static const int attackWs[4] = { 0, 0, 0, 0 };
        static const int attackHs[4] = { 0, 0, 0, 0 };

        // shoot animation (unused for mummy but kept for consistency)
        static const int shootXs[1] = { 0 };
        static const int shootYs[1] = { 0 };
        static const int shootWs[1] = { 0 };
        static const int shootHs[1] = { 0 };

        // hurt animation (crumble)
        static const int hurtXs[3] = { 0, 0, 0 };
        static const int hurtYs[3] = { 0, 0, 0 };
        static const int hurtWs[3] = { 0, 0, 0 };
        static const int hurtHs[3] = { 0, 0, 0 };

        // die animation
        static const int dieXs[4] = { 0, 0, 0, 0 };
        static const int dieYs[4] = { 0, 0, 0, 0 };
        static const int dieWs[4] = { 0, 0, 0, 0 };
        static const int dieHs[4] = { 0, 0, 0, 0 };

        anims[WALK].load("Sprites/Mummy Warrior.png", walkXs, walkYs, walkWs, walkHs, 6, 0.12f);
        anims[ATTACK].load("Sprites/Mummy Warrior.png", attackXs, attackYs, attackWs, attackHs, 4, 0.10f);
        anims[SHOOT].load("Sprites/Mummy Warrior.png", shootXs, shootYs, shootWs, shootHs, 1, 0.18f);
        anims[HURT].load("Sprites/Mummy Warrior.png", hurtXs, hurtYs, hurtWs, hurtHs, 3, 0.12f);
        anims[DIE].load("Sprites/Mummy Warrior.png", dieXs, dieYs, dieWs, dieHs, 4, 0.12f);
    }

    void move(float dt) override {
        if (isCrumbled) return;  // cant move while crumbled

        // always chases player with slow shamble
        if (largestPlayer)
            chasePlayer(largestPlayer);

        x += velocityX * dt;
    }

    void attack() override {
        // mummy attacks on contact transform player
        if (largestPlayer)
            transformPlayer(largestPlayer);
    }

    // fire kills instantly bullets just crumble it
    void takeDamage(int dmg, float bx, float by, bool isExplosive) override {
        if (isExplosive) {
            // explosions and fire kill instantly per spec
            hp = 0;
            die();
            return;
        }
        // normal bullets make it crumble but not die
        if (!isCrumbled)
            crumble();
    }

    void crumble() {
        isCrumbled = true;
        resumeTimer = resumeDuration;
        velocityX = 0.f;
        velocityY = 0.f;
    }

    void resurrect() {
        isCrumbled = false;
        hp = static_cast<float>(maxHp);
    }

    void checkResumeTimer(float dt) {
        if (!isCrumbled) return;
        resumeTimer -= dt;
        if (resumeTimer <= 0.f)
            resurrect();
    }

    void update(float dt) override {
        if (!isAlive) return;

        checkResumeTimer(dt);
        if (!largestPlayer) return;

        detectPlayer(largestPlayer);
        move(dt);

        // on contact transform the player
        float dx = largestPlayer->getPlayerX() - x;
        float dy = largestPlayer->getPlayerY() - y;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist < attackRange && !isCrumbled)
            attack();
    }

    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override {
        UndeadEnemy::render(window, camX, camY);
    }
};


// walks toward player transforms on contact
// also has a pistol for range attacks
// spawns in batches of 3 to 5
class Zombie : public UndeadEnemy {
private:
    float fireRate;

public:
    Zombie() {
        name = "Zombie";
        hp = maxHp = 5;
        scoreValue = 100;
        speed = 45.f;
        damage = 3;
        fireRate = 1.5f;
        width = 32.f;
        height = 48.f;
        transformOnContact = true;
        onlyDeadFromFire = true;

        // walk animation
        static const int walkXs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int walkYs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int walkWs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int walkHs[6] = { 0, 0, 0, 0, 0, 0 };

        // attack animation (melee)
        static const int attackXs[4] = { 0, 0, 0, 0 };
        static const int attackYs[4] = { 0, 0, 0, 0 };
        static const int attackWs[4] = { 0, 0, 0, 0 };
        static const int attackHs[4] = { 0, 0, 0, 0 };

        // shoot animation (pistol)
        static const int shootXs[4] = { 0, 0, 0, 0 };
        static const int shootYs[4] = { 0, 0, 0, 0 };
        static const int shootWs[4] = { 0, 0, 0, 0 };
        static const int shootHs[4] = { 0, 0, 0, 0 };

        // hurt animation
        static const int hurtXs[2] = { 0, 0 };
        static const int hurtYs[2] = { 0, 0 };
        static const int hurtWs[2] = { 0, 0 };
        static const int hurtHs[2] = { 0, 0 };

        // die animation
        static const int dieXs[4] = { 0, 0, 0, 0 };
        static const int dieYs[4] = { 0, 0, 0, 0 };
        static const int dieWs[4] = { 0, 0, 0, 0 };
        static const int dieHs[4] = { 0, 0, 0, 0 };

        anims[WALK].load("Sprites/Zombie.png", walkXs, walkYs, walkWs, walkHs, 6, 0.12f);
        anims[ATTACK].load("Sprites/Zombie.png", attackXs, attackYs, attackWs, attackHs, 4, 0.10f);
        anims[SHOOT].load("Sprites/Zombie.png", shootXs, shootYs, shootWs, shootHs, 4, 0.10f);
        anims[HURT].load("Sprites/Zombie.png", hurtXs, hurtYs, hurtWs, hurtHs, 2, 0.12f);
        anims[DIE].load("Sprites/Zombie.png", dieXs, dieYs, dieWs, dieHs, 4, 0.12f);
    }

    void move(float dt) override {
        if (largestPlayer)
            chasePlayer(largestPlayer);
        x += velocityX * dt;
    }

    void attack() override {
        // zombies have pistol and also transform on contact
        if (!largestPlayer) return;

        float dx = largestPlayer->getPlayerX() - x;
        float dy = largestPlayer->getPlayerY() - y;
        float dist = sqrt(dx * dx + dy * dy);

        // pistol shot if in range
        if (dist < attackRange) {
            // pistol.fire(atan2(dy, dx));  // enable when weapon linked
        }

        // melee transform on contact
        if (dist < 32.f)
            transformPlayer(largestPlayer);
    }

    void update(float dt) override {
        if (!isAlive || !largestPlayer) return;
        detectPlayer(largestPlayer);
        move(dt);
        attack();
    }

    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override {
        UndeadEnemy::render(window, camX, camY);
    }
};


// abstract base for alien enemies
// has 2 phases like martian
class AlienEnemy : public Enemy {
protected:
    static const int PHASE1  = 0;
    static const int PHASE2  = 1;
    static const int ATTACK1 = 2;
    static const int ATTACK2 = 3;
    static const int HURT    = 4;
    static const int DIE     = 5;

    int   phase;
    float massPhase;
    int   batchSize;
    float switchPhase;
    Sprite  sprite;
    Texture texture;
    Animation anims[6];
    int currentAnim;

public:
    AlienEnemy() : currentAnim(PHASE1) {
        speed = 70.f;
        detectionRange = 500.f;
        attackRange = 400.f;
        hp = maxHp = 3;
        scoreValue = 200;
        phase = 1;
        massPhase = 0.f;
        batchSize = 1;
        switchPhase = 0.f;
    }

    virtual ~AlienEnemy() {}

    // child classes override this for phase switching logic
    virtual void updatePhase() {}

    void attack() override;

    void update(float dt) override {
        if (!isAlive || !largestPlayer) return;
        updatePhase();
        detectPlayer(largestPlayer);
        move(dt);
    }

    virtual void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override {
        if (!isAlive && currentAnim != DIE) return;
        Animation& a = anims[currentAnim];
        IntRect r = a.currentRect();
        if (r.width == 0 || r.height == 0) return;
        sprite.setTexture(a.getTexture(), true);
        sprite.setTextureRect(r);
        float sc = 2.0f;
        float scX = facingRight ? sc : -sc;
        sprite.setOrigin(r.width / 2.0f, (float)r.height);
        sprite.setPosition((x + width / 2.0f) - camX, (y + height) - camY);
        sprite.setScale(scX, sc);
        window.draw(sprite);
    }
};


// 2 phase enemy
// phase 1 flies in pod above player fires energy beam
// phase 2 pod explodes martian on foot with pistol
class Martian : public AlienEnemy {
private:
    float podX, podY;
    bool  isPodAlive;
    float podTimer;
    bool  beamActive;
    bool  podIsActive;
    float fireSum;
    bool  isOnFlatGround;
    float fireBombTimer;

public:
    Martian() {
        name = "Martian";
        hp = maxHp = 3;
        scoreValue = 200;
        speed = 60.f;
        damage = 5;
        width = 40.f;
        height = 56.f;
        phase = 1;

        podX = x;
        podY = y - 80.f;   // pod hovers above martian
        isPodAlive = true;
        podTimer = 0.f;
        beamActive = false;
        podIsActive = true;
        fireSum = 0.f;
        isOnFlatGround = false;
        fireBombTimer = 0.f;

        // phase 1 (pod flying)
        static const int phase1Xs[4] = { 0, 0, 0, 0 };
        static const int phase1Ys[4] = { 0, 0, 0, 0 };
        static const int phase1Ws[4] = { 0, 0, 0, 0 };
        static const int phase1Hs[4] = { 0, 0, 0, 0 };

        // phase 2 (martian on foot)
        static const int phase2Xs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int phase2Ys[6] = { 0, 0, 0, 0, 0, 0 };
        static const int phase2Ws[6] = { 0, 0, 0, 0, 0, 0 };
        static const int phase2Hs[6] = { 0, 0, 0, 0, 0, 0 };

        // attack1 (energy beam from pod)
        static const int attack1Xs[4] = { 0, 0, 0, 0 };
        static const int attack1Ys[4] = { 0, 0, 0, 0 };
        static const int attack1Ws[4] = { 0, 0, 0, 0 };
        static const int attack1Hs[4] = { 0, 0, 0, 0 };

        // attack2 (pistol on foot)
        static const int attack2Xs[4] = { 0, 0, 0, 0 };
        static const int attack2Ys[4] = { 0, 0, 0, 0 };
        static const int attack2Ws[4] = { 0, 0, 0, 0 };
        static const int attack2Hs[4] = { 0, 0, 0, 0 };

        // hurt animation
        static const int hurtXs[2] = { 0, 0 };
        static const int hurtYs[2] = { 0, 0 };
        static const int hurtWs[2] = { 0, 0 };
        static const int hurtHs[2] = { 0, 0 };

        // die animation
        static const int dieXs[4] = { 0, 0, 0, 0 };
        static const int dieYs[4] = { 0, 0, 0, 0 };
        static const int dieWs[4] = { 0, 0, 0, 0 };
        static const int dieHs[4] = { 0, 0, 0, 0 };

        anims[PHASE1].load("Sprites/Martian.png", phase1Xs, phase1Ys, phase1Ws, phase1Hs, 4, 0.10f);
        anims[PHASE2].load("Sprites/Martian.png", phase2Xs, phase2Ys, phase2Ws, phase2Hs, 6, 0.10f);
        anims[ATTACK1].load("Sprites/Martian.png", attack1Xs, attack1Ys, attack1Ws, attack1Hs, 4, 0.10f);
        anims[ATTACK2].load("Sprites/Martian.png", attack2Xs, attack2Ys, attack2Ws, attack2Hs, 4, 0.10f);
        anims[HURT].load("Sprites/Martian.png", hurtXs, hurtYs, hurtWs, hurtHs, 2, 0.12f);
        anims[DIE].load("Sprites/Martian.png", dieXs, dieYs, dieWs, dieHs, 4, 0.12f);
    }

    void move(float dt) override {
        if (phase == 1) {
            // phase 1 pod flies above player horizontally
            if (largestPlayer) {
                float targetX = largestPlayer->getPlayerX() - width * 0.5f;
                float dx = targetX - x;
                if (dx > 0) velocityX = 1.f * speed;
                else velocityX = -1.f * speed;
                facingRight = (dx > 0);
            }
            // hover at fixed height no vertical movement
            velocityY = 0.f;
            x += velocityX * dt;

            // pod moves with body in phase 1
            podX = x;
            podY = y - 80.f;
        }
        else {
            // phase 2 martian on foot normal chase
            chasePlayer(largestPlayer);
            x += velocityX * dt;
        }
    }

    void attack() override {
        if (!largestPlayer) return;

        if (phase == 1 && isPodAlive) {
            // fire beam downward when directly above player
            float dx = largestPlayer->getPlayerX() - x;
            if (abs(dx) < 20.f)
                beamActive = true;
            else
                beamActive = false;
        }
        else if (phase == 2) {
            // pistol fire in phase 2
            float dx = largestPlayer->getPlayerX() - x;
            float dy = largestPlayer->getPlayerY() - y;
            float dist = sqrt(dx * dx + dy * dy);
            // pistol.fire(atan2(dy, dx));  // enable when weapon linked
        }
    }

    // phase switch logic pod destroyed means phase 2
    void updatePhase() override {
        if (phase == 1 && !isPodAlive)
            phase = 2;
    }

    void explodePod() {
        isPodAlive = false;
        podIsActive = false;
        updatePhase();
    }

    void checkFlatGround() {
        isOnFlatGround = isGrounded;
    }

    void update(float dt) override {
        if (!isAlive || !largestPlayer) return;

        updatePhase();
        detectPlayer(largestPlayer);
        move(dt);

        // gravity only in phase 2 when on foot
        if (phase == 2) {
            applyGravity(dt);
            checkGrounded();
            checkFlatGround();
        }

        attack();
        checkPlayerCollision(largestPlayer);
    }

    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override {
        AlienEnemy::render(window, camX, camY);
    }
};


// ground boss phase 1
// missile launcher on top flamethrower on bottom
// 30 HP spawns on flat ground only
class IronNokana : public AlienEnemy {
private:
    bool  missileLauncherActive;   // fixed from float to bool
    bool  flamethrowerActive;
    bool  fireBombCooldown;
    float flameStreamRange;
    float missileLauncherAngle;
    bool  isOnFlatGround;
    float fireBombTimer;
    float rocketLauncherTimer;

public:
    IronNokana() : missileLauncherActive(false),
        rocketLauncherTimer(0.f), flamethrowerActive(false),
        fireBombCooldown(false), flameStreamRange(200.f),
        missileLauncherAngle(45.f), isOnFlatGround(false),
        fireBombTimer(0.f)
    {
        name = "Iron Nokana";
        hp = maxHp = 30;
        scoreValue = 500;
        speed = 30.f;   // very slow tank
        damage = 10;
        width = 80.f;
        height = 60.f;
        detectionRange = 600.f;
        attackRange = 500.f;



        // idle animation (uses PHASE1 slot)
        static const int idleXs[2] = { 0, 0 };
        static const int idleYs[2] = { 0, 0 };
        static const int idleWs[2] = { 0, 0 };
        static const int idleHs[2] = { 0, 0 };

        // moving animation (uses PHASE2 slot)
        static const int moveXs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int moveYs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int moveWs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int moveHs[6] = { 0, 0, 0, 0, 0, 0 };

        // missile attack animation (uses ATTACK1 slot)
        static const int missileXs[5] = { 0, 0, 0, 0, 0 };
        static const int missileYs[5] = { 0, 0, 0, 0, 0 };
        static const int missileWs[5] = { 0, 0, 0, 0, 0 };
        static const int missileHs[5] = { 0, 0, 0, 0, 0 };

        // flamethrower animation (uses ATTACK2 slot)
        static const int flameXs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int flameYs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int flameWs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int flameHs[6] = { 0, 0, 0, 0, 0, 0 };

        // hurt animation
        static const int hurtXs[2] = { 0, 0 };
        static const int hurtYs[2] = { 0, 0 };
        static const int hurtWs[2] = { 0, 0 };
        static const int hurtHs[2] = { 0, 0 };

        // die animation (explosion)
        static const int dieXs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int dieYs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int dieWs[6] = { 0, 0, 0, 0, 0, 0 };
        static const int dieHs[6] = { 0, 0, 0, 0, 0, 0 };

        anims[PHASE1].load("Sprites/Iron Nokana.png", idleXs, idleYs, idleWs, idleHs, 2, 0.20f);
        anims[PHASE2].load("Sprites/Iron Nokana.png", moveXs, moveYs, moveWs, moveHs, 6, 0.10f);
        anims[ATTACK1].load("Sprites/Iron Nokana.png", missileXs, missileYs, missileWs, missileHs, 5, 0.12f);
        anims[ATTACK2].load("Sprites/Iron Nokana.png", flameXs, flameYs, flameWs, flameHs, 6, 0.10f);
        anims[HURT].load("Sprites/Iron Nokana.png", hurtXs, hurtYs, hurtWs, hurtHs, 2, 0.12f);
        anims[DIE].load("Sprites/Iron Nokana.png", dieXs, dieYs, dieWs, dieHs, 6, 0.15f);
    }

    // only moves on flat terrain
    void move(float dt) override {
        if (!isOnFlatGround) return;

        if (largestPlayer)
            chasePlayer(largestPlayer);

        x += velocityX * dt;
    }

    void attack() override {
        // missile launcher fires when cooldown done
        if (rocketLauncherTimer <= 0.f)
            activateLauncher();

        // fire bomb thrown ocasionally
        if (!fireBombCooldown)
            fireBomb();

        // flamethrower activated separately
        if (flamethrowerActive)
            activateFlamethrower();
    }

    void reload() {
        rocketLauncherTimer = 3.f;  // reload cooldown
    }

    void activateLauncher() {
        if (!largestPlayer) return;
        missileAttackAngle();
        // MissileManager::getInstance().spawn(x, y, missileLauncherAngle);
        missileLauncherActive = true;
        reload();
    }

    void activateFlamethrower() {
        // flame stream damages in facing direction
        // hitbox handled in full game implementation
        flamethrowerActive = false;  // one shot activation timer managed in update
    }

    void fireBomb() {
        // lob a fire bomb grenade toward player
        // GrenadeLauncher::getInstance().spawnFireBomb(x, y, largestPlayer);
        fireBombCooldown = true;
        fireBombTimer = 4.f;
    }

    void missileAttackAngle() {
        if (!largestPlayer) return;
        float dx = largestPlayer->getPlayerX() - x;
        float dy = largestPlayer->getPlayerY() - y;
        missileLauncherAngle = atan2(dy, dx) * 180.f / 3.14159f;
    }

    void checkFlatGround() {
        isOnFlatGround = isGrounded;
    }

    void update(float dt) override {
        if (!isAlive || !largestPlayer) return;

        // countdown timers
        if (rocketLauncherTimer > 0.f) rocketLauncherTimer -= dt;
        if (fireBombTimer > 0.f) {
            fireBombTimer -= dt;
            if (fireBombTimer <= 0.f)
                fireBombCooldown = false;  // ready to fire bomb again
        }

        detectPlayer(largestPlayer);
        applyGravity(dt);
        checkGrounded();
        checkFlatGround();
        move(dt);
        attack();
        checkPlayerCollision(largestPlayer);
    }

    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override {
        AlienEnemy::render(window, camX, camY);
    }
};
void Enemy::spawnLoot() {}