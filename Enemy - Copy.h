#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include "Entity.h"
#include "Animation.h"

// Forward declarations to avoid circular dependency
class Level;
class PlayerSoldier;

#include "Weapon.h"        // BulletManager + PLAYER/ENEMY constants yahan hain
#include "PlayerSoldier.h" // PlayerSoldier yahan hai


using namespace sf;


// Helper function for rectangle overlap
static bool rectsOverlap(float ax, float ay, float aw, float ah,
    float bx, float by, float bw, float bh)
{
    return ax < bx + bw && ax + aw > bx &&
        ay < by + bh && ay + ah > by;
}


// Abstract base class for all enemies
class Enemy : public Soldier {
protected:
    const char* name;
    float speed;
    bool  isGrounded;
    bool  isCrouched;
    float spawnX, spawnY;
    int   scoreValue;
    bool  isShielded;
    float aggressionLevel;

    // Coward system
    bool  hasGrudge;
    float grudgeMultiplier;
    bool  isEnhanced;

    // detection variables
    float detectionRange;
    float attackRange;
    int   currentBiome;
    bool  isPatrolling;
    bool  isTargetingPlayer;
    PlayerSoldier* largestPlayer;
    BulletManager* bulletMgr;

    float gravityConstant = 20.0f;
    float jumpPower = -200.0f; // Higher jump power than player to cross walls
    float patrolRange;

    float fireTimer;
    float fireCooldown;
public:
    Enemy();
    virtual ~Enemy();

    virtual void attack() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) = 0;

    virtual void move(float dt);
    virtual void takeDamage(int dmg, float bulletX = 0, float bulletY = 0, bool isExplosive = false);
    virtual void die();
    void detectPlayer(PlayerSoldier* player);
    void chasePlayer(PlayerSoldier* player);
    void setPatrolRange(float range);
    float getPatrolRange() const;
    void patrol();
    void applyGravity(float deltaTime);
    void jump();
    void checkPlayerCollision(PlayerSoldier* player) const;
    void dropLoot();
    void checkEnemyCollision(Enemy* other) const;
    void spawnLoot();

    int getScoreValue() const;
    void applyGrudge();
    void enhancePower();
    void returnToPlayer(PlayerSoldier* player);

    float getWidth() const;
    float getHeight() const;
    const char* getName() const;
    bool getIsTargetingPlayer() const;
    bool getIsGrounded() const;

    void setPosition(float nx, float ny);
    void setPlayer(PlayerSoldier* p);
    void setBulletManager(BulletManager* bm);
    void setX(float nx);

    void fireAtPlayer(float speed = 300.f, int damage = 5, float range = 400.f, Color color = Color(255, 70, 70));
    void fireInDirection(float angle, float speed = 300.f, int damage = 5, float range = 400.f, Color color = Color(255, 70, 70));

    bool canFire() const;
    void updateFireTimer(float dt);
    void resetFireTimer();

    float getVelocityY() const;
    void setVelocityY(float vy);
    float getVelocityX() const;
    void setVelocityX(float vx);
    void setGrounded(bool g);
};

// Enemy Manager class - moved here to fix declaration order issues
class EnemyManager {
private:
    Enemy* enemies[500];
    int enemyCount;
    int maxEnemies;
    BulletManager* bulletMgr;

public:
    EnemyManager();
    ~EnemyManager();

    void setBulletManager(BulletManager* bm);
    void removeDeadEnemies();
    void clearAll();
    void addEnemy(Enemy* enemy);
    Enemy* getEnemyAt(int index);
    void updateAll(float dt, PlayerSoldier* player);
    void renderAll(RenderWindow& window, float camX, float camY);
    int getEnemyCount() const;
};


// Base class for infantry enemies
class InfantryEnemy : public Enemy {
protected:
    static const int WALK = 0;
    static const int STAND = 1;
    static const int SHOOT = 2;
    static const int DIE = 3;

    float   fireRate;
    float   fireTimer;
    float   fireCooldown;
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
    InfantryEnemy();
    virtual ~InfantryEnemy();

    virtual void move(float deltaTime);
    void attack() override;
    void update(float dt) override;
    virtual void updateAnimState(float dt);
    virtual void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};


// Basic enemy with pistol and patrols, spawns in batches
class RebelSoldier : public InfantryEnemy {
private:
    float patrolRange;
    bool isShooting;
    int bulletsFired;
    float shootingTimer;
    float bulletDelay;

public:
    RebelSoldier();
    void attack() override;
    void move(float dt) override;
    void update(float dt) override;
    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};


// Throws grenades in ballistic arc, spawns in batches
class GrenadeSoldier : public InfantryEnemy {
private:
    int          grenadeCount;
    float        throwCooldown;
    int          activeGrenades;
    bool         isActive;

public:
    GrenadeSoldier();
    void attack() override;
    void throwGrenade();
    void update(float dt) override;
    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};


// Rocket class Weapon.h mein defined hai, yahan duplicate nhi rakhni


// Slow moving, fires rockets in steep arc, spawns in batches
class BazookaSoldier : public InfantryEnemy {
private:
    float  rocketReloadTimer;
    float  rocketReloadDuration;
    int    activeRockets;

public:
    BazookaSoldier();
    void attack() override;
    void fireRocket();
    void update(float dt) override;
    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};


// has riot shield blocks frontal bullets
// must be hit from above behind or with explosives
// spawns in batches of 1 to 2
class ShieldedSoldier : public InfantryEnemy {
private:
    bool shieldActive;
    int  shieldDurability;

public:
    ShieldedSoldier();
    void takeDamage(int dmg, float bulletX, float bulletY, bool isExplosive) override;
    void checkShieldBreak();
    void attack() override;
    void update(float dt) override;
    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};


// abstract base for all aerial enemies
// gravity is ignored for these
class AerialEnemy : public Enemy {
protected:
    static const int FLY = 0;
    static const int DESCEND = 1;
    static const int ATTACK = 2;
    static const int DIE = 3;

    float   flyHeight;
    bool    isDescending;
    float   descentSpeed;
    int     batchSize;
    Sprite  sprite;
    Texture texture;
    Animation anims[4];
    int currentAnim;

public:
    AerialEnemy();
    virtual ~AerialEnemy();

    virtual void fly();
    virtual void descend();
    void move(float dt) override;
    virtual void attack() = 0;
    void update(float dt) override;
    virtual void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};


// descends from top as one of the 4 infantry types
// doesnt attack while descending becomes infantry on landing
class Paratrooper : public AerialEnemy {
private:
    bool hasLanded;
    float fireCooldown;

public:
    Paratrooper();
    ~Paratrooper();

    void descend() override;
    void spawnInfantry();
    void attack() override;
    void update(float dt) override;
    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};


// abstract base for undead enemies
// immune to bullets only fire and explosions kill them
class UndeadEnemy : public Enemy {
protected:
    static const int WALK = 0;
    static const int ATTACK = 1;
    static const int SHOOT = 2;
    static const int HURT = 3;
    static const int DIE = 4;

    bool transformOnContact;
    bool onlyDeadFromFire;
    Sprite  sprite;
    Texture texture;
    Animation anims[5];
    int currentAnim;
    bool isCrumbled;

public:
    UndeadEnemy();
    virtual ~UndeadEnemy();

    virtual void transformPlayer(PlayerSoldier* p);
    void takeDamage(int dmg, float bulletX, float bulletY, bool isExplosive) override;
    void update(float dt) override;
    virtual void updateAnimState(float dt);
    virtual void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};


// slowly walks toward player and transforms on contact
// crumbles when hit but resurrects after 3 seconds
// only fire kills it instantly per project spec
class MummyWarrior : public UndeadEnemy {
private:

    float resumeTimer;
    float resumeDuration;

public:
    MummyWarrior();
    void move(float dt) override;
    void attack() override;
    void takeDamage(int dmg, float bx, float by, bool isExplosive) override;
    void crumble();
    void resurrect();
    void checkResumeTimer(float dt);
    void update(float dt) override;
    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};


// walks toward player transforms on contact
// also has a pistol for range attacks
// spawns in batches of 3 to 5
class Zombie : public UndeadEnemy {
private:
    float fireRate;

public:
    Zombie();
    void move(float dt) override;
    void attack() override;
    void update(float dt) override;
    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};


// abstract base for alien enemies
// has 2 phases like martian
class AlienEnemy : public Enemy {
protected:
    static const int PHASE1 = 0;
    static const int PHASE2 = 1;
    static const int ATTACK1 = 2;
    static const int ATTACK2 = 3;
    static const int HURT = 4;
    static const int DIE = 5;

    int   phase;
    float massPhase;
    int   batchSize;
    float switchPhase;
    Sprite  sprite;
    Texture texture;
    Animation anims[6];
    int currentAnim;

public:
    AlienEnemy();
    virtual ~AlienEnemy();

    virtual void updatePhase();
    virtual void attack() override = 0;
    void update(float dt) override;
    virtual void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};

// Hovers at medium height, drops grenade when above player
class FlyingTara : public AerialEnemy {
private:
    float grenadeCooldown;
    float grenadeCooldownDuration;
    float dropHeight;
    float GroundY;

public:
    FlyingTara();
    float setGroundY() const;
    void attack() override;
    void update(float dt) override;
    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
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
    float fireCooldown;

public:
    Martian();
    void move(float dt) override;
    void attack() override;
    void updatePhase() override;
    void explodePod();
    void checkFlatGround();
    void update(float dt) override;
    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
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
    bool  hasRetreated;
    float fireCooldown;

public:
    IronNokana();
    void retreat();
    bool getHasRetreated() const;
    void takeDamage(int dmg);
    void move(float dt) override;
    void attack() override;
    void reload();
    void activateLauncher();
    void activateFlamethrower();
    void fireBomb();
    void missileAttackAngle();
    void checkFlatGround();
    void update(float dt) override;
    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};

// Aerial boss phase 2
// Flies and drops energy bombs from above
// 30 HP spawns in aerial biome only
class HairbusterRiberts : public AerialEnemy {
private:
    bool  energyBombActive;
    float bombCooldown;
    float bombCooldownDuration;
    float flightHeight;
    bool  isDiving;
    bool  hasRetreated;

public:
    HairbusterRiberts();
    void attack() override;
    void dropEnergyBomb();
    void retreat();
    bool getHasRetreated() const;
    void takeDamage(int dmg);
    void update(float dt) override;
    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};

// Aquatic boss phase 3
// Swims and fires torpedoes underwater
// 30 HP spawns in aquatic biome only
class SeaSatan : public Enemy {
private:
    bool  underwater;
    bool  torpedoActive;
    float torpedoCooldown;
    float torpedoCooldownDuration;
    float swimSpeed;
    float surfaceLevel;
    bool  hasRetreated;

public:
    SeaSatan();
    void attack() override;
    void fireTorpedo();
    void update(float dt) override;
    void retreat();
    bool getHasRetreated() const;
    void takeDamage(int dmg);
    void render(RenderWindow& window, float camX = 0.f, float camY = 0.f) override;
};

