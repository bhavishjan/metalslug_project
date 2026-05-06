#pragma once
#include "Player.h"
#include <cmath>
#include <cstring>
#include "Weapon.h"

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
    char name[100];
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

    float gravityConstant = 900.0f;

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
    virtual void render(RenderWindow& window) = 0;

    // overide in case of special behaviour
    virtual void move(float dt) {
        if (isTargetingPlayer) chasePlayer(largestPlayer);
        else                   patrol();
        x += velocityX * dt;
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
            y += velocityY * deltaTime;
        }
    }

    void checkGrounded() {
        // treat any surface below enemy as ground
        float GROUND_Y = 520.f;  // placeholder floor level
        if (y + height >= GROUND_Y) {
            y = GROUND_Y - height;
            velocityY = 0.f;
            isGrounded = true;
        }
        else {
            isGrounded = false;
        }
    }

    // when collide push enemy back slightly
    void checkPlayerCollision(Player* player) {
        if (!player || !isAlive) return;

        float px = player->getPlayerX();
        float py = player->getPlayerY();
        float PLAYER_W = 32.f;
        float PLAYER_H = 48.f;

        if (rectsOverlap(x, y, width, height, px, py, PLAYER_W, PLAYER_H)) {
            // push enemy away from player
            if (x < px) velocityX = -80.f;
            else         velocityX = 80.f;
        }
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

    void setPosition(float nx, float ny) { x = nx; y = ny; }
    void setPlayer(Player* p) { largestPlayer = p; }
    void setX(float nx) { x = nx; }
};


// base class for all infantry type enemies
// has pistol and basic movement
class InfantryEnemy : public Enemy {
protected:
    float   fireRate;
    float   fireTimer;
    int     bulletDamage;
    int     batchSize;
    float   reloadTimer;
    float   reloadDuration;
    Pistol  pistol;
    Sprite  sprite;
    Texture texture;

public:
    InfantryEnemy() : Enemy(),
        fireRate(3.f), fireTimer(0.f),
        bulletDamage(3), batchSize(2),
        reloadTimer(0.f), reloadDuration(2.f) {
    }

    virtual ~InfantryEnemy() {}

    virtual void move(float deltaTime) {
        if (isTargetingPlayer)
            chasePlayer(largestPlayer);
        else
            patrol();
        x += velocityX * deltaTime;  // pos update
    }

    void attack() override {
        // check player exist
        if (largestPlayer == nullptr) return;

        // can pistol fire
        if (!pistol.canFire()) return;

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
        applyGravity(dt);
        checkGrounded();
        checkPlayerCollision(largestPlayer);
    }

    virtual void render(RenderWindow& window) {
        if (!isAlive) return;

        // sprite pos
        sprite.setPosition(x, y);

        // flip for left face
        if (!facingRight) {
            sprite.setScale(-1.f, 1.f);
            sprite.setOrigin(width, 0);
        }
        else {
            sprite.setScale(1.f, 1.f);
            sprite.setOrigin(0, 0);
        }

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

        detectionRange = 350.0f;
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
    }

    // uses parent InfantryEnemy attack no need to override
    void attack() override {
        InfantryEnemy::attack();
    }

    void update(float dt) override {
        InfantryEnemy::update(dt);
    }

    void render(RenderWindow& window) override {
        InfantryEnemy::render(window);
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

        width = 32.f;
        height = 48.f;
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
        applyGravity(dt);
        checkGrounded();
        checkPlayerCollision(largestPlayer);
    }

    void render(RenderWindow& window) override;
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

    void render(RenderWindow& window);
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

        applyGravity(dt);
        checkGrounded();
        checkPlayerCollision(largestPlayer);
    }

    void render(RenderWindow& window) override;
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

        width = 40.f;   // bit wider because of shield
        height = 48.f;
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
        applyGravity(dt);
        checkGrounded();
        checkPlayerCollision(largestPlayer);
    }

    void render(RenderWindow& window) override;
};


// abstract base for all aerial enemies
// gravity is ignored for these
class AerialEnemy : public Enemy {
protected:
    float   flyHeight;
    bool    isDescending;
    float   descentSpeed;
    int     batchSize;
    Sprite  sprite;
    Texture texture;

public:
    AerialEnemy() {
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

    void render(RenderWindow& window) override;
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

    void render(RenderWindow& window) override;
};


// abstract base for undead enemies
// immune to bullets only fire and explosions kill them
class UndeadEnemy : public Enemy {
protected:
    bool transformOnContact;
    bool onlyDeadFromFire;

public:
    UndeadEnemy() : transformOnContact(true), onlyDeadFromFire(true)
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
        applyGravity(dt);
        checkGrounded();
        checkPlayerCollision(largestPlayer);
    }

    void render(RenderWindow& window) override;
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
        applyGravity(dt);
        checkGrounded();

        // on contact transform the player
        float dx = largestPlayer->getPlayerX() - x;
        float dy = largestPlayer->getPlayerY() - y;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist < attackRange && !isCrumbled)
            attack();
    }

    void render(RenderWindow& window) override;
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
        applyGravity(dt);
        checkGrounded();
        attack();
        checkPlayerCollision(largestPlayer);
    }

    void render(RenderWindow& window) override;
};


// abstract base for alien enemies
// has 2 phases like martian
class AlienEnemy : public Enemy {
protected:
    int   phase;
    float massPhase;
    int   batchSize;
    float switchPhase;

public:
    AlienEnemy() {
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
        applyGravity(dt);
        checkGrounded();
        checkPlayerCollision(largestPlayer);
    }

    void render(RenderWindow& window) override;
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
    }

    void move(float dt) override {
        if (phase == 1) {
            // phase 1 pod flies above player horizontally
            if (largestPlayer) {
                float targetX = largestPlayer->getPlayerX() - width * 0.5f;
                float dx = targetX - x;
                velocityX = (dx > 0 ? 1.f : -1.f) * speed;
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

    void render(RenderWindow& window) override;
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

    void render(RenderWindow& window) override;
};