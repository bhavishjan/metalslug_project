#define _CRT_SECURE_NO_WARNINGS
#include "Enemy.h"

// Enemy base class implementations
Enemy::Enemy() : Soldier(), name("Enemy"), speed(80.f),
    isGrounded(false), isCrouched(false),
    spawnX(0), spawnY(0),
    scoreValue(0), isShielded(false), aggressionLevel(1.f),
    hasGrudge(false), grudgeMultiplier(1.f), isEnhanced(false),
    detectionRange(300.f), attackRange(250.f), currentBiome(0),
    isPatrolling(true), isTargetingPlayer(false), largestPlayer(nullptr), bulletMgr(nullptr), patrolRange(50.f) {
    x = 0;
    y = 0;
    width = 32;
    height = 48;
}

Enemy::~Enemy() {}

void Enemy::move(float dt) {
    if (isTargetingPlayer) {
        chasePlayer(largestPlayer);
    }
    else {
        patrol();
    }
}

void Enemy::takeDamage(int dmg, float bulletX, float bulletY,
    bool isExplosive)
{
    hp -= dmg / grudgeMultiplier;
    if (hp <= 0) {
        die();
    }
}

void Enemy::die() {
    isAlive = false;
    dropLoot();
}

void Enemy::detectPlayer(PlayerSoldier* player) {
    float xx = player->getPlayerX();
    float yy = player->getPlayerY();

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

void Enemy::chasePlayer(PlayerSoldier* player) {
    float xp = player->getPlayerX();
    if (xp > x) {
        velocityX = speed;
        facingRight = true;
    }
    else {
        velocityX = -speed;
        facingRight = false;
    }

    if (x < 0) {
        x = 0;
        velocityX = speed;
        facingRight = true;
    }
}

void Enemy::setPatrolRange(float range) {
    patrolRange = range;
}

float Enemy::getPatrolRange() {
    return patrolRange;
}

void Enemy::patrol() {
    if (x > spawnX + patrolRange) {
        velocityX = -speed;
        facingRight = false;
    }
    else if (x < spawnX - patrolRange) {
        velocityX = speed;
        facingRight = true;
    }
    else {
        if (facingRight) {
            velocityX = speed;
        }
        else {
            velocityX = -speed;
        }
    }
}

void Enemy::applyGravity(float deltaTime) {
    if (isGrounded == false) {
        velocityY += gravityConstant * deltaTime;
    }
}

void Enemy::jump() {
    if (isGrounded) {
        velocityY = jumpPower;
        isGrounded = false;
    }
}

void Enemy::checkPlayerCollision(PlayerSoldier* player) {
    (void)player;
}

void Enemy::dropLoot() {
    int roll = rand() % 100;
    if (roll < 20) {
        spawnLoot();
    }
}

void Enemy::checkEnemyCollision(Enemy* other) {
    (void)other;
}

void Enemy::spawnLoot() {
}

int Enemy::getScoreValue() const { return scoreValue; }

void Enemy::applyGrudge() {
    hasGrudge = true;
    isEnhanced = true;
    grudgeMultiplier = 1.5f;
    speed *= 1.25f;
    damage = (int)(damage * 1.3f);
    hp = (float)maxHp;
}

void Enemy::enhancePower() {
    speed *= 1.15f;
    damage = (int)(damage * 1.2f);
    hp += (float)(maxHp) * 0.25f;
}

void Enemy::returnToPlayer(PlayerSoldier* player) {
    if (!player) {
        return;
    }
    largestPlayer = player;
    isTargetingPlayer = true;
    chasePlayer(player);
}

float Enemy::getWidth() const { return width; }
float Enemy::getHeight() const { return height; }
const char* Enemy::getName() const { return name; }
bool Enemy::getIsTargetingPlayer() const { return isTargetingPlayer; }
bool Enemy::getIsGrounded() const { return isGrounded; }

void Enemy::setPosition(float nx, float ny) { x = nx; y = ny; }
void Enemy::setPlayer(PlayerSoldier* p) { largestPlayer = p; }
void Enemy::setBulletManager(BulletManager* bm) { bulletMgr = bm; }
void Enemy::setX(float nx) { x = nx; }

void Enemy::fireAtPlayer(float speed, int damage, float range, Color color) {
    if (!bulletMgr || !largestPlayer) return;
    
    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    float angle = atan2(dy, dx);
    
    bulletMgr->spawnBullet(x + width * 0.5f, y + height * 0.5f, angle,
        damage, ENEMY, speed, range, color);
}

void Enemy::fireInDirection(float angle, float speed, int damage, float range, Color color) {
    if (!bulletMgr) return;

    bulletMgr->spawnBullet(x + width * 0.5f, y + height * 0.5f, angle,
        damage, ENEMY, speed, range, color);
}

bool Enemy::canFire() {
    return fireTimer <= 0.0f;
}

void Enemy::updateFireTimer(float dt) {
    if (fireTimer > 0.0f) fireTimer -= dt;
}

void Enemy::resetFireTimer() {
    fireTimer = fireCooldown;
}

float Enemy::getVelocityY() const { return velocityY; }
void Enemy::setVelocityY(float vy) { velocityY = vy; }
float Enemy::getVelocityX() const { return velocityX; }
void Enemy::setVelocityX(float vx) { velocityX = vx; }
void Enemy::setGrounded(bool g) {
    isGrounded = g;
    if (g) {
        velocityY = 0.f;
    }
}

// EnemyManager implementations
EnemyManager::EnemyManager() {
    enemyCount = 0;
    maxEnemies = 500;
    bulletMgr = nullptr;
    for (int i = 0; i < maxEnemies; i++) {
        enemies[i] = nullptr;
    }
}

void EnemyManager::setBulletManager(BulletManager* bm) {
    bulletMgr = bm;
}

void EnemyManager::removeDeadEnemies() {
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i] && !enemies[i]->getIsAlive()) {
            delete enemies[i];
            enemies[i] = nullptr;
            enemies[i] = enemies[enemyCount - 1];
            enemies[enemyCount - 1] = nullptr;
            enemyCount--;
            i--;
        }
    }
}

EnemyManager::~EnemyManager() {
    clearAll();
}

void EnemyManager::clearAll() {
    for (int i = 0; i < maxEnemies; i++) {
        delete enemies[i];
        enemies[i] = nullptr;
    }
    enemyCount = 0;
}

void EnemyManager::addEnemy(Enemy* enemy) {
    if (enemyCount < maxEnemies) {
        enemies[enemyCount] = enemy;
        enemyCount++;
    }
}

Enemy* EnemyManager::getEnemyAt(int index) {
    if (index >= 0 && index < enemyCount) {
        return enemies[index];
    }
    return nullptr;
}

void EnemyManager::updateAll(float dt, PlayerSoldier* player) {
    for (int i = 0; i < enemyCount; i++) {
        if (!enemies[i]) {
            continue;
        }
        if (!enemies[i]->getIsAlive()) {
            continue;
        }

        enemies[i]->setPlayer(player);
        enemies[i]->setBulletManager(bulletMgr);
        enemies[i]->update(dt);
    }
}

void EnemyManager::renderAll(RenderWindow& window, float camX, float camY) {
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i]) {
            enemies[i]->render(window, camX, camY);
        }
    }
}

int EnemyManager::getEnemyCount() const {
    return enemyCount;
}

// InfantryEnemy implementations
InfantryEnemy::InfantryEnemy() : Enemy(),
    fireRate(3.f), fireTimer(0.f),
    bulletDamage(3), batchSize(2),
    reloadTimer(0.f), reloadDuration(2.f),
    currentAnim(STAND) {
}

InfantryEnemy::~InfantryEnemy() {}

void InfantryEnemy::move(float deltaTime) {
    if (isTargetingPlayer)
        chasePlayer(largestPlayer);
    else
        patrol();
}

void InfantryEnemy::attack() {
    if (largestPlayer == nullptr) {
        return;
    }

    if (!pistol.canFire()) {
        return;
    }

    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance > attackRange) {
        return;
    }

    float angle = atan2(dy, dx);
    pistol.fire(angle);
    fireTimer = 0.4f;

    facingRight = (dx > 0);
}

void InfantryEnemy::update(float dt) {
    if (!isAlive) {
        return;
    }
    if (largestPlayer == nullptr) {
        pistol.update(dt);
        return;
    }

    pistol.setActive(true);
    pistol.setPosition(x + width * 0.5f, y + height * 0.45f);

    detectPlayer(largestPlayer);
    move(dt);

    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    float distanceToplayer = sqrt(dx * dx + dy * dy);

    if (isTargetingPlayer && distanceToplayer < attackRange) {
        attack();
    }

    pistol.update(dt);
    if (fireTimer > 0.f) {
        fireTimer -= dt;
    }

    updateAnimState(dt);
}

void InfantryEnemy::updateAnimState(float dt) {
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

void InfantryEnemy::render(RenderWindow& window, float camX, float camY) {
    if (!isAlive && currentAnim != DIE) {
        return;
    }

    Animation& a = anims[currentAnim];
    IntRect r = a.currentRect();

    if (r.width == 0 || r.height == 0) {
        RectangleShape fallbackRect;
        fallbackRect.setSize(Vector2f(width, height));
        fallbackRect.setPosition(x - camX, y - camY);
        fallbackRect.setFillColor(Color(255, 0, 0));
        window.draw(fallbackRect);
        return;
    }

    sprite.setTexture(a.getTexture(), true);
    sprite.setTextureRect(r);

    float sc = 2.0f;
    float scX;
    if (facingRight) {
        scX = -sc;
    }
    else {
        scX = sc;
    }
    sprite.setOrigin(r.width / 2.0f, (float)r.height);
    sprite.setPosition((x + width / 2.0f) - camX, (y + height) - camY);
    sprite.setScale(scX, sc);

    window.draw(sprite);
    pistol.render(window);
}

// RebelSoldier implementations
RebelSoldier::RebelSoldier() {
    name = "Rebel Soldier";
    hp = 2;
    maxHp = 2;
    scoreValue = 50;
    batchSize = 3;
    patrolRange = 150.0f;

    speed = 80.0f;
    damage = 3;

    fireRate = 4.0f;
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

    isShooting = false;
    bulletsFired = 0;
    shootingTimer = 0.0f;
    bulletDelay = 0.3f;
    
    fireCooldown = 5.0f;

    grudgeMultiplier = 1.0f;

    width = 32.0f;
    height = 48.0f;

    static const int walkXs[12] = { 3,  40,  80, 125, 170, 210, 248, 285, 326, 369, 410, 450 };
    static const int walkYs[12] = { 44,  44,  45,  46,  45,  44,  44,  44,  45,  46,  45,  44 };
    static const int walkWs[12] = { 34,  37,  42,  42,  37,  35,  34,  38,  40,  38,  37,  35 };
    static const int walkHs[12] = { 37,  37,  31,  33,  35,  37,  37,  36,  30,  32,  35,  37 };

    static const int standXs[1] = { 3 };
    static const int standYs[1] = { 3 };
    static const int standWs[1] = { 40 };
    static const int standHs[1] = { 38 };

    static const int shootXs[10] = { 3,  50, 101, 151, 196, 284, 257, 330, 380, 429 };
    static const int shootYs[10] = { 298, 298, 298, 298, 289, 292, 298, 298, 298, 298 };
    static const int shootWs[10] = { 44,  48,  47,  42,  41,  43,  38,  47,  46,  40 };
    static const int shootHs[10] = { 38,  38,  38,  40,  47,  44,  38,  38,  38,  38 };

    static const int dieXs[4] = { 3,  44,  86, 129 };
    static const int dieYs[4] = { 784, 785, 786, 786 };
    static const int dieWs[4] = { 38,  39,  40,  40 };
    static const int dieHs[4] = { 33,  32,  31,  31 };

    anims[WALK].load("Sprites/Enemies/Rebel Soldier.png", walkXs, walkYs, walkWs, walkHs, 12, 0.07f);
    anims[STAND].load("Sprites/Enemies/Rebel Soldier.png", standXs, standYs, standWs, standHs, 1, 0.18f);
    anims[SHOOT].load("Sprites/Enemies/Rebel Soldier.png", shootXs, shootYs, shootWs, shootHs, 10, 0.08f);
    anims[DIE].load("Sprites/Enemies/Rebel Soldier.png", dieXs, dieYs, dieWs, dieHs, 4, 0.15f);
}

void RebelSoldier::attack() {
    if (isShooting) {
        return;
    }

    isShooting = true;
    bulletsFired = 0;
    shootingTimer = 0.0f;
    currentAnim = SHOOT;
}

void RebelSoldier::move(float dt) {
    if (isShooting) {
        return;
    }
    InfantryEnemy::move(dt);
}

void RebelSoldier::update(float dt) {
    if (largestPlayer == nullptr) {
        isShooting = false;
        InfantryEnemy::update(dt);
        return;
    }

    if (isShooting) {
        shootingTimer += dt;

        if (bulletsFired < 1 && shootingTimer >= bulletDelay) {
            pistol.fire(0);
            fireAtPlayer(100.f, bulletDamage, 200.f, Color(255, 165, 0));
            bulletsFired++;
            shootingTimer = 0.0f;
        }

        if (bulletsFired >= 1) {
            isShooting = false;
            bulletsFired = 0;
            fireTimer = 3.0f;
        }
        pistol.update(dt);
        updateAnimState(dt);
        return;
    }

    InfantryEnemy::update(dt);
}

void RebelSoldier::render(RenderWindow& window, float camX, float camY) {
    InfantryEnemy::render(window, camX, camY);
}

// GrenadeSoldier implementations
GrenadeSoldier::GrenadeSoldier() : grenadeCount(5), throwCooldown(0.f), activeGrenades(0), isActive(false)
{
    name = "Grenade Soldier";
    hp = 2.f;
    maxHp = 2;
    speed = 60.f;
    damage = 8;
    scoreValue = 100;
    batchSize = 1;

    detectionRange = 400.f;
    attackRange = 350.f;

    width = 25.f;
    height = 35.f;

    static const int walkXs[7] = { 2,  25,  51,  81, 110, 153, 203 };
    static const int walkYs[7] = { 35,  25,  19,  17,  20,  24,  25 };
    static const int walkWs[7] = { 18,  21,  26,  25,  36,  46,  46 };
    static const int walkHs[7] = { 17,  27,  33,  35,  32,  28,  27 };

    static const int standXs[1] = { 203 };
    static const int standYs[1] = { 25 };
    static const int standWs[1] = { 46 };
    static const int standHs[1] = { 27 };

    static const int throwXs[5] = { 2,  45,  90, 125, 157 };
    static const int throwYs[5] = { 54,  74,  67,  66,  70 };
    static const int throwWs[5] = { 42,  39,  30,  26,  37 };
    static const int throwHs[5] = { 47,  27,  34,  35,  31 };

    static const int dieXs[6] = { 2,  68, 103, 130, 155, 180 };
    static const int dieYs[6] = { 162, 165, 167, 167, 167, 167 };
    static const int dieWs[6] = { 65,  30,  23,  20,  20,  21 };
    static const int dieHs[6] = { 49,  46,  28,  30,  32,  30 };

    anims[WALK].load("Sprites/Enemies/Rebel Soldier (RPG-2).png", walkXs, walkYs, walkWs, walkHs, 7, 0.08f);
    anims[STAND].load("Sprites/Enemies/Rebel Soldier (RPG-2).png", standXs, standYs, standWs, standHs, 1, 0.18f);
    anims[SHOOT].load("Sprites/Enemies/Rebel Soldier (RPG-2).png", throwXs, throwYs, throwWs, throwHs, 5, 0.12f);
    anims[DIE].load("Sprites/Enemies/Rebel Soldier (RPG-2).png", dieXs, dieYs, dieWs, dieHs, 6, 0.15f);
}

void GrenadeSoldier::attack() {
    if (largestPlayer == nullptr) {
        return;
    }

    if (throwCooldown > 0) {
        return;
    }

    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance > attackRange) {
        return;
    }

    throwGrenade();
    throwCooldown = 2.0f;
}

void GrenadeSoldier::throwGrenade() {
    if (grenadeCount <= 0) {
        return;
    }

    float tx = largestPlayer->getPlayerX();
    float ty = largestPlayer->getPlayerY();

    float flightTime = 1.2f;
    float gravity = 980.0f;

    float dx = tx - x;
    float dy = ty - y;

    float velX = dx / flightTime;
    float velY = (dy - 0.5 * gravity * flightTime * flightTime) / flightTime;

    if (bulletMgr) {
        bulletMgr->spawnGrenade(x, y, velX, velY);
    }
    grenadeCount--;
}

void GrenadeSoldier::update(float dt) {
    if (!isAlive) {
        return;
    }
    if (largestPlayer == nullptr) {
        return;
    }

    if (throwCooldown > 0) {
        throwCooldown -= dt;
    }

    detectPlayer(largestPlayer);
    move(dt);

    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    float distanceToplayer = sqrt(dx * dx + dy * dy);

    if (isTargetingPlayer && distanceToplayer < attackRange) {
        attack();
    }

    pistol.update(dt);
    updateAnimState(dt);
}

void GrenadeSoldier::render(RenderWindow& window, float camX, float camY) {
    InfantryEnemy::render(window, camX, camY);
}

// BazookaSoldier implementations
BazookaSoldier::BazookaSoldier() {
    name = "Bazooka Soldier";
    hp = 2.f;
    maxHp = 2;
    speed = 50.f;
    damage = 15;
    scoreValue = 100;
    batchSize = 1;

    detectionRange = 450.f;
    attackRange = 400.f;

    width = 40.f;
    height = 48.f;

    rocketReloadDuration = 2.5f;
    rocketReloadTimer = 0;
    activeRockets = 0;

    static const int walkXs[11] = { 3,  47,  91, 134, 178, 221, 265,  309,  350,  390, 429 };
    static const int walkYs[11] = { 55,  54,  53,  54,  56,  55,  54,   53,   52,   51,  53 };
    static const int walkWs[11] = { 41,  41,  40,  41,  40,  41,  41,   38,   37,   36,  37 };
    static const int walkHs[11] = { 39,  40,  40,  39,  38,  39,  40,   41,   41,   42,  41 };

    static const int standXs[1] = { 3 };
    static const int standYs[1] = { 3 };
    static const int standWs[1] = { 36 };
    static const int standHs[1] = { 45 };

    static const int fireXs[3] = { 3,  45,  89 };
    static const int fireYs[3] = { 241, 243, 245 };
    static const int fireWs[3] = { 39,  41,  41 };
    static const int fireHs[3] = { 44,  42,  40 };

    static const int dieXs[9] = { 3,  52,  99, 147, 191, 235, 279, 318, 361 };
    static const int dieYs[9] = { 404, 407, 401, 403, 398, 398, 398, 395, 399 };
    static const int dieWs[9] = { 46,  44,  45,  41,  41,  41,  36,  38,  31 };
    static const int dieHs[9] = { 42,  39,  45,  43,  48,  48,  48,  51,  47 };

    anims[WALK].load("Sprites/Enemies/Rebel Soldier (Bazooka).png", walkXs, walkYs, walkWs, walkHs, 11, 0.08f);
    anims[STAND].load("Sprites/Enemies/Rebel Soldier (Bazooka).png", standXs, standYs, standWs, standHs, 1, 0.18f);
    anims[SHOOT].load("Sprites/Enemies/Rebel Soldier (Bazooka).png", fireXs, fireYs, fireWs, fireHs, 3, 0.14f);
    anims[DIE].load("Sprites/Enemies/Rebel Soldier (Bazooka).png", dieXs, dieYs, dieWs, dieHs, 9, 0.12f);
}

void BazookaSoldier::attack() {
    if (largestPlayer == nullptr) {
        return;
    }

    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance > attackRange) {
        return;
    }

    fireRocket();
    facingRight = (dx > 0);
}

void BazookaSoldier::fireRocket() {
    if (rocketReloadTimer > 0) {
        return;
    }
    if (largestPlayer == nullptr) {
        return;
    }

    float tx = largestPlayer->getPlayerX();
    float ty = largestPlayer->getPlayerY();

    float flightTime = 1.5f;
    float gravity = 980.0f;

    float dx = tx - x;
    float dy = ty - y;

    float velX = dx / flightTime;
    float velY = (dy - 0.5 * gravity * flightTime * flightTime) / flightTime;

    if (bulletMgr) {
        bulletMgr->spawnRocket(x, y, velX, velY);
    }

    rocketReloadTimer = rocketReloadDuration;
}

void BazookaSoldier::update(float dt) {
    if (!isAlive) return;
    if (largestPlayer == nullptr) return;

    if (rocketReloadTimer > 0.f) rocketReloadTimer -= dt;

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

void BazookaSoldier::render(RenderWindow& window, float camX, float camY) {
    InfantryEnemy::render(window, camX, camY);
}

// ShieldedSoldier implementations
ShieldedSoldier::ShieldedSoldier() : shieldActive(true), shieldDurability(3)
{
    name = "Shielded Soldier";
    hp = 5.f;
    maxHp = 5;
    speed = 70.f;
    damage = 3;
    scoreValue = 120;
    batchSize = 1;
    
    fireCooldown = 6.0f;

    detectionRange = 300.f;
    attackRange = 250.f;

    width = 55.f;
    height = 48.f;

    static const int walkXs[12] = { 3,   35,  66,  99, 133, 167, 202, 235, 267, 298, 329, 359 };
    static const int walkYs[12] = { 47,  46,  46,  47,  49,  48,  47,  46,  46,  47,  49,  48 };
    static const int walkWs[12] = { 29,  28,  30,  31,  31,  32,  30,  29,  28,  28,  27,  27 };
    static const int walkHs[12] = { 40,  41,  40,  39,  38,  39,  40,  41,  40,  39,  38,  39 };

    static const int standXs[6] = { 3, 38, 73, 107, 143, 177 };
    static const int standYs[6] = { 5,  5,  5,   5,   5,   5 };
    static const int standWs[6] = { 32, 32, 31,  32,  31,  32 };
    static const int standHs[6] = { 38, 38, 38,  39,  38,  38 };

    static const int shootXs[9] = { 3,  39,  82, 126, 167, 218, 274, 326, 369 };
    static const int shootYs[9] = { 389, 388, 379, 379, 385, 389, 389, 387, 393 };
    static const int shootWs[9] = { 33,  40,  41,  38,  48,  53,  49,  40,  55 };
    static const int shootHs[9] = { 38,  39,  48,  48,  42,  38,  38,  40,  34 };

    static const int dieXs[9] = { 3,  37,  72, 115, 162, 209, 257, 305, 353 };
    static const int dieYs[9] = { 481, 482, 482, 482, 483, 484, 485, 484, 483 };
    static const int dieWs[9] = { 31,  32,  38,  44,  44,  45,  45,  45,  44 };
    static const int dieHs[9] = { 39,  40,  40,  41,  41,  40,  40,  40,  41 };

    anims[WALK].load("Sprites/Enemies/Rebel Soldier (Shield).png", walkXs, walkYs, walkWs, walkHs, 12, 0.08f);
    anims[STAND].load("Sprites/Enemies/Rebel Soldier (Shield).png", standXs, standYs, standWs, standHs, 6, 0.18f);
    anims[SHOOT].load("Sprites/Enemies/Rebel Soldier (Shield).png", shootXs, shootYs, shootWs, shootHs, 9, 0.10f);
    anims[DIE].load("Sprites/Enemies/Rebel Soldier (Shield).png", dieXs, dieYs, dieWs, dieHs, 9, 0.12f);
}

void ShieldedSoldier::takeDamage(int dmg, float bulletX, float bulletY,
    bool isExplosive)
{
    if (isExplosive) {
        hp -= dmg;
        if (hp <= 0) die();
        return;
    }

    bool isFront = false;
    bool isAbove = false;

    if (bulletY < y) isAbove = true;

    if (facingRight) {
        if (bulletX > x) isFront = true;
    }
    else {
        if (bulletX < x) isFront = true;
    }

    if (shieldActive && isFront && !isAbove) {
        shieldDurability--;
        if (shieldDurability <= 0)
            shieldActive = false;
        return;
    }

    hp -= dmg;
    if (hp <= 0) die();
}

void ShieldedSoldier::checkShieldBreak() {
    if (shieldDurability <= 0)
        shieldActive = false;
}

void ShieldedSoldier::attack() {
    if (largestPlayer == nullptr || !canFire()) return;
    
    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    float distance = sqrt(dx * dx + dy * dy);
    
    if (distance > attackRange) return;
    
    float angle = atan2(dy, dx);
    pistol.fire(angle);
    fireInDirection(angle, 200.f, 5, 300.f, Color(255, 0, 0));
    resetFireTimer();
    facingRight = (dx > 0);
}

void ShieldedSoldier::update(float dt) {
    if (!isAlive) return;
    if (largestPlayer == nullptr) return;

    detectPlayer(largestPlayer);
    move(dt);

    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    float distanceToplayer = sqrt(dx * dx + dy * dy);

    if (isTargetingPlayer && distanceToplayer < attackRange)
        attack();

    pistol.update(dt);
    updateFireTimer(dt);

    updateAnimState(dt);
}

void ShieldedSoldier::render(RenderWindow& window, float camX, float camY) {
    InfantryEnemy::render(window, camX, camY);
}

// AerialEnemy implementations
AerialEnemy::AerialEnemy() : currentAnim(FLY) {
    flyHeight = 200.f;
    isDescending = false;
    descentSpeed = 60.f;
    batchSize = 2;
    speed = 90.f;
    detectionRange = 400.f;
    attackRange = 300.f;
    width = 32.f;
    height = 32.f;
    isGrounded = true;
}

AerialEnemy::~AerialEnemy() {}

void AerialEnemy::fly() {
    if (y > flyHeight)
        velocityY = -descentSpeed * 0.5f;
    else if (y < flyHeight - 10.f)
        velocityY = descentSpeed * 0.5f;
    else
        velocityY = 0.f;
}

void AerialEnemy::descend() {
    isDescending = true;
    velocityY = descentSpeed;
}

void AerialEnemy::move(float dt) {
    if (isTargetingPlayer && largestPlayer)
        chasePlayer(largestPlayer);
    else
        patrol();

    if (!isDescending) fly();

    x += velocityX * dt;
    y += velocityY * dt;
}

void AerialEnemy::update(float dt) {
    if (!isAlive || !largestPlayer) return;
    detectPlayer(largestPlayer);
    move(dt);
    checkPlayerCollision(largestPlayer);
}

void AerialEnemy::render(RenderWindow& window, float camX, float camY) {
    if (!isAlive && currentAnim != DIE) return;
    Animation& a = anims[currentAnim];
    IntRect r = a.currentRect();
    if (r.width == 0 || r.height == 0) {
        RectangleShape fallbackRect;
        fallbackRect.setSize(Vector2f(width, height));
        fallbackRect.setPosition(x - camX, y - camY);
        fallbackRect.setFillColor(Color(150, 180, 255));
        window.draw(fallbackRect);
        return;
    }
    sprite.setTexture(a.getTexture(), true);
    sprite.setTextureRect(r);
    float sc = 2.0f;
    float scX = facingRight ? sc : -sc;
    sprite.setOrigin(r.width / 2.0f, (float)r.height);
    sprite.setPosition((x + width / 2.0f) - camX, (y + height) - camY);
    sprite.setScale(scX, sc);
    window.draw(sprite);
}

// Paratrooper implementations
Paratrooper::Paratrooper() {
    hasLanded = false;
    name = "Paratrooper";
    hp = maxHp = 2;
    scoreValue = 75;
    flyHeight = -50.f;
    isDescending = true;

    static const int flyXs[2] = { 0, 0 };
    static const int flyYs[2] = { 0, 0 };
    static const int flyWs[2] = { 0, 0 };
    static const int flyHs[2] = { 0, 0 };

    static const int descendXs[2] = { 0, 0 };
    static const int descendYs[2] = { 0, 0 };
    static const int descendWs[2] = { 0, 0 };
    static const int descendHs[2] = { 0, 0 };

    static const int attackXs[4] = { 0, 0, 0, 0 };
    static const int attackYs[4] = { 0, 0, 0, 0 };
    static const int attackWs[4] = { 0, 0, 0, 0 };
    static const int attackHs[4] = { 0, 0, 0, 0 };

    static const int dieXs[4] = { 0, 0, 0, 0 };
    static const int dieYs[4] = { 0, 0, 0, 0 };
    static const int dieWs[4] = { 0, 0, 0, 0 };
    static const int dieHs[4] = { 0, 0, 0, 0 };

    anims[FLY].load("Sprites/Paratrooper.png", flyXs, flyYs, flyWs, flyHs, 2, 0.15f);
    anims[DESCEND].load("Sprites/Paratrooper.png", descendXs, descendYs, descendWs, descendHs, 2, 0.20f);
    anims[ATTACK].load("Sprites/Paratrooper.png", attackXs, attackYs, attackWs, attackHs, 4, 0.10f);
    anims[DIE].load("Sprites/Paratrooper.png", dieXs, dieYs, dieWs, dieHs, 4, 0.12f);
}

Paratrooper::~Paratrooper() {}

void Paratrooper::descend() {
    AerialEnemy::descend();
}

void Paratrooper::spawnInfantry() {
    hasLanded = true;
    name = "Rebel Soldier";
    speed = 80.f;
    isDescending = false;
    fireCooldown = 5.5f;
}

void Paratrooper::attack() {
    if (!hasLanded || !largestPlayer || !canFire()) return;
    
    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    float distance = sqrt(dx * dx + dy * dy);
    
    if (distance > 300.f) return;
    
    fireAtPlayer(200.f, 3, 250.f, Color(0, 100, 255));
    resetFireTimer();
}

void Paratrooper::update(float dt) {
    if (!isAlive) return;

    move(dt);

    float GROUND_Y = 520.f;
    if (y + height >= GROUND_Y) {
        y = GROUND_Y - height;
        if (!hasLanded)
            spawnInfantry();
    }
    
    if (hasLanded && largestPlayer) {
        detectPlayer(largestPlayer);
        if (isTargetingPlayer)
            attack();
        updateFireTimer(dt);
    }

    if (largestPlayer)
        checkPlayerCollision(largestPlayer);
}

void Paratrooper::render(RenderWindow& window, float camX, float camY) {
    AerialEnemy::render(window, camX, camY);
}

// UndeadEnemy implementations
UndeadEnemy::UndeadEnemy() : transformOnContact(true), onlyDeadFromFire(true), currentAnim(WALK), isCrumbled(false)
{
    speed = 40.f;
    detectionRange = 500.f;
    attackRange = 30.f;
    hp = maxHp = 5;
}

UndeadEnemy::~UndeadEnemy() {}

void UndeadEnemy::transformPlayer(PlayerSoldier* p) {
    if (!p) return;
}

void UndeadEnemy::takeDamage(int dmg, float bulletX, float bulletY,
    bool isExplosive)
{
    if (!isExplosive) return;
    hp -= dmg;
    if (hp <= 0.f) die();
}

void UndeadEnemy::update(float dt) {
    if (!isAlive || !largestPlayer) return;
    detectPlayer(largestPlayer);
    move(dt);
    updateAnimState(dt);
}

void UndeadEnemy::updateAnimState(float dt) {
    int desired;
    if (!isAlive) {
        desired = DIE;
    }
    else if (isCrumbled) {
        desired = HURT;
    }
    else if (velocityX > 0.5f || velocityX < -0.5f) {
        desired = WALK;
    }
    else {
        desired = WALK;
    }

    if (desired != currentAnim) {
        currentAnim = desired;
        anims[currentAnim].reset();
    }

    anims[currentAnim].update(dt);
}

void UndeadEnemy::render(RenderWindow& window, float camX, float camY) {
    if (!isAlive && currentAnim != DIE) return;
    Animation& a = anims[currentAnim];
    IntRect r = a.currentRect();
    if (r.width == 0 || r.height == 0) {
        RectangleShape fallbackRect;
        fallbackRect.setSize(Vector2f(width, height));
        fallbackRect.setPosition(x - camX, y - camY);
        fallbackRect.setFillColor(Color(180, 180, 120));
        window.draw(fallbackRect);
        return;
    }
    sprite.setTexture(a.getTexture(), true);
    sprite.setTextureRect(r);
    float sc = 2.0f;
    float scX = facingRight ? -sc : sc;
    sprite.setOrigin(r.width / 2.0f, (float)r.height);
    sprite.setPosition((x + width / 2.0f) - camX, (y + height) - camY);
    sprite.setScale(scX, sc);
    window.draw(sprite);
}

// MummyWarrior implementations
MummyWarrior::MummyWarrior() : resumeTimer(0.f), resumeDuration(3.f)
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
    isCrumbled = (false);

    static const int walkXs[18] = { 196, 232, 270, 312, 353, 394, 432, 472, 508, 548, 592, 638, 680, 721, 768, 804, 843, 883 };
    static const int walkYs[18] = { 1010, 1010, 1010, 1010, 1011, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010 };
    static const int walkWs[18] = { 31, 35, 36, 37, 33, 32, 32, 35, 36, 34, 32, 34, 33, 38, 32, 36, 33, 31 };
    static const int walkHs[18] = { 45, 45, 45, 45, 44, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45 };

    static const int attackXs[4] = { 10, 59, 105, 146 };
    static const int attackYs[4] = { 140, 140, 140, 140 };
    static const int attackWs[4] = { 44, 41, 36, 28 };
    static const int attackHs[4] = { 44, 44, 44, 44 };

    static const int hurtXs[3] = { 10, 64, 118 };
    static const int hurtYs[3] = { 460, 460, 460 };
    static const int hurtWs[3] = { 36, 36, 36 };
    static const int hurtHs[3] = { 21, 21, 21 };

    static const int dieXs[4] = { 10, 10, 10, 10 };
    static const int dieYs[4] = { 831, 879, 926, 953 };
    static const int dieWs[4] = { 48, 51, 50, 52 };
    static const int dieHs[4] = { 43, 37, 22, 63 };

    anims[WALK].load("Sprites/Enemies/Mummy Warrior.png", walkXs, walkYs, walkWs, walkHs, 18, 0.12f);
    anims[ATTACK].load("Sprites/Enemies/Mummy Warrior.png", attackXs, attackYs, attackWs, attackHs, 4, 0.10f);
    anims[HURT].load("Sprites/Enemies/Mummy Warrior.png", hurtXs, hurtYs, hurtWs, hurtHs, 3, 0.12f);
    anims[DIE].load("Sprites/Enemies/Mummy Warrior.png", dieXs, dieYs, dieWs, dieHs, 4, 0.12f);
}

void MummyWarrior::move(float dt) {
    if (isCrumbled) return;

    if (largestPlayer)
        chasePlayer(largestPlayer);
}

void MummyWarrior::attack() {
    if (largestPlayer)
        transformPlayer(largestPlayer);
}

void MummyWarrior::takeDamage(int dmg, float bx, float by, bool isExplosive) {
    if (isExplosive) {
        hp = 0;
        die();
        return;
    }
    if (!isCrumbled)
        crumble();
}

void MummyWarrior::crumble() {
    isCrumbled = true;
    resumeTimer = resumeDuration;
    velocityX = 0.f;
    velocityY = 0.f;
}

void MummyWarrior::resurrect() {
    isCrumbled = false;
    hp = static_cast<float>(maxHp);
}

void MummyWarrior::checkResumeTimer(float dt) {
    if (!isCrumbled) return;
    resumeTimer -= dt;
    if (resumeTimer <= 0.f)
        resurrect();
}

void MummyWarrior::update(float dt) {
    if (!isAlive) return;

    checkResumeTimer(dt);
    if (!largestPlayer) return;

    detectPlayer(largestPlayer);
    move(dt);

    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    float dist = sqrt(dx * dx + dy * dy);
    if (dist < attackRange && !isCrumbled)
        attack();

    updateAnimState(dt);
}

void MummyWarrior::render(RenderWindow& window, float camX, float camY) {
    UndeadEnemy::render(window, camX, camY);
}

// Zombie implementations
Zombie::Zombie() {
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

    static const int walkXs[6] = { 10, 64, 118, 171, 10, 64 };
    static const int walkYs[6] = { 18, 18, 19, 19, 18, 18 };
    static const int walkWs[6] = { 49, 49, 48, 48, 49, 49 };
    static const int walkHs[6] = { 43, 43, 42, 42, 43, 43 };

    static const int attackXs[4] = { 10, 59, 105, 146 };
    static const int attackYs[4] = { 140, 140, 140, 140 };
    static const int attackWs[4] = { 44, 41, 36, 28 };
    static const int attackHs[4] = { 44, 44, 44, 44 };

    static const int shootXs[4] = { 10, 62, 111, 158 };
    static const int shootYs[4] = { 202, 202, 202, 202 };
    static const int shootWs[4] = { 47, 44, 42, 45 };
    static const int shootHs[4] = { 44, 44, 44, 44 };

    static const int hurtXs[2] = { 10, 64 };
    static const int hurtYs[2] = { 460, 460 };
    static const int hurtWs[2] = { 36, 36 };
    static const int hurtHs[2] = { 21, 21 };

    static const int dieXs[4] = { 10, 10, 10, 10 };
    static const int dieYs[4] = { 831, 879, 926, 953 };
    static const int dieWs[4] = { 48, 51, 50, 52 };
    static const int dieHs[4] = { 43, 37, 22, 63 };

    anims[WALK].load("Sprites/Enemies/Zombies 1.png", walkXs, walkYs, walkWs, walkHs, 6, 0.12f);
    anims[ATTACK].load("Sprites/Enemies/Zombies 1.png", attackXs, attackYs, attackWs, attackHs, 4, 0.10f);
    anims[SHOOT].load("Sprites/Enemies/Zombies 1.png", shootXs, shootYs, shootWs, shootHs, 4, 0.10f);
    anims[HURT].load("Sprites/Enemies/Zombies 1.png", hurtXs, hurtYs, hurtWs, hurtHs, 2, 0.12f);
    anims[DIE].load("Sprites/Enemies/Zombies 1.png", dieXs, dieYs, dieWs, dieHs, 4, 0.12f);
}

void Zombie::move(float dt) {
    if (largestPlayer)
        chasePlayer(largestPlayer);
}

void Zombie::attack() {
    if (!largestPlayer) return;

    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    float dist = sqrt(dx * dx + dy * dy);

    if (dist < attackRange) {
    }

    if (dist < 32.f)
        transformPlayer(largestPlayer);
}

void Zombie::update(float dt) {
    if (!isAlive || !largestPlayer) return;
    detectPlayer(largestPlayer);
    move(dt);
    attack();
    updateAnimState(dt);
}

void Zombie::render(RenderWindow& window, float camX, float camY) {
    UndeadEnemy::render(window, camX, camY);
}

// AlienEnemy implementations
AlienEnemy::AlienEnemy() : currentAnim(PHASE1) {
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

AlienEnemy::~AlienEnemy() {}

void AlienEnemy::updatePhase() {}

void AlienEnemy::update(float dt) {
    if (!isAlive || !largestPlayer) return;
    updatePhase();
    detectPlayer(largestPlayer);
    move(dt);
}

void AlienEnemy::render(RenderWindow& window, float camX, float camY) {
    if (!isAlive && currentAnim != DIE) return;
    Animation& a = anims[currentAnim];
    IntRect r = a.currentRect();
    if (r.width == 0 || r.height == 0) {
        RectangleShape fallbackRect;
        fallbackRect.setSize(Vector2f(width, height));
        fallbackRect.setPosition(x - camX, y - camY);
        fallbackRect.setFillColor(Color(200, 120, 220));
        window.draw(fallbackRect);
        return;
    }
    sprite.setTexture(a.getTexture(), true);
    sprite.setTextureRect(r);
    float sc = 2.0f;
    float scX = facingRight ? sc : -sc;
    sprite.setOrigin(r.width / 2.0f, (float)r.height);
    sprite.setPosition((x + width / 2.0f) - camX, (y + height) - camY);
    sprite.setScale(scX, sc);
    window.draw(sprite);
}

// FlyingTara implementations
FlyingTara::FlyingTara() : grenadeCooldownDuration(3.0f), dropHeight(250.f), GroundY(700.f) {
    name = "Flying Tara";
    hp = maxHp = 3;
    scoreValue = 100;
    speed = 70.f;
    flyHeight = 200.f;
    dropHeight = 250.f;
    grenadeCooldown = 0.f;
    grenadeCooldownDuration = 3.0f;
    width = 48.f;
    height = 32.f;
    detectionRange = 500.f;
    attackRange = 300.f;
    isGrounded = true;

    static const int flyXs[2] = { 0, 0 };
    static const int flyYs[2] = { 0, 0 };
    static const int flyWs[2] = { 0, 0 };
    static const int flyHs[2] = { 0, 0 };

    static const int dieXs[2] = { 0, 0 };
    static const int dieYs[2] = { 0, 0 };
    static const int dieWs[2] = { 0, 0 };
    static const int dieHs[2] = { 0, 0 };

    anims[FLY].load("Sprites/Enemies/Flying Tara.png",
        flyXs, flyYs, flyWs, flyHs, 2, 0.15f);
    anims[DIE].load("Sprites/Enemies/Flying Tara.png",
        dieXs, dieYs, dieWs, dieHs, 2, 0.15f);
}

float FlyingTara::setGroundY() {
    return GroundY;
}

void FlyingTara::attack() {
    if (!largestPlayer || !bulletMgr) return;
    if (grenadeCooldown > 0) return;

    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;

    if (abs(dx) < 60.f) {
        float velX = dx / 1.5f;
        float velY = 50.f;
        bulletMgr->spawnGrenade(x, y, velX, velY);
        grenadeCooldown = grenadeCooldownDuration;
    }
}

void FlyingTara::update(float dt) {
    if (!isAlive || !largestPlayer) return;

    if (grenadeCooldown > 0.f)
        grenadeCooldown -= dt;

    detectPlayer(largestPlayer);
    move(dt);
    attack();
    checkPlayerCollision(largestPlayer);

    anims[currentAnim].update(dt);
}

void FlyingTara::render(RenderWindow& window, float camX, float camY) {
    if (!isAlive) return;

    RectangleShape rect;
    rect.setSize(Vector2f(width, height));
    rect.setPosition(x - camX, y - camY);
    rect.setFillColor(Color(0, 200, 255, 180));
    window.draw(rect);
}

// Martian implementations
Martian::Martian() {
    name = "Martian";
    hp = maxHp = 3;
    scoreValue = 200;
    speed = 60.f;
    damage = 5;
    width = 40.f;
    height = 56.f;
    phase = 1;

    podX = x;
    podY = y - 80.f;
    isPodAlive = true;
    podTimer = 0.f;
    beamActive = false;
    podIsActive = true;
    fireSum = 0.f;
    isOnFlatGround = false;
    fireBombTimer = 0.f;
    
    fireCooldown = 7.0f;

    static const int phase1Xs[4] = { 10, 64, 118, 171 };
    static const int phase1Ys[4] = { 18, 18, 19, 19 };
    static const int phase1Ws[4] = { 49, 49, 48, 48 };
    static const int phase1Hs[4] = { 43, 43, 42, 42 };

    static const int phase2Xs[6] = { 10, 64, 119, 174, 10, 64 };
    static const int phase2Ys[6] = { 79, 79, 80, 80, 79, 79 };
    static const int phase2Ws[6] = { 49, 50, 50, 51, 49, 50 };
    static const int phase2Hs[6] = { 43, 43, 42, 42, 43, 43 };

    static const int attack1Xs[4] = { 10, 59, 105, 146 };
    static const int attack1Ys[4] = { 140, 140, 140, 140 };
    static const int attack1Ws[4] = { 44, 41, 36, 28 };
    static const int attack1Hs[4] = { 44, 44, 44, 44 };

    static const int attack2Xs[4] = { 10, 62, 111, 158 };
    static const int attack2Ys[4] = { 202, 202, 202, 202 };
    static const int attack2Ws[4] = { 47, 44, 42, 45 };
    static const int attack2Hs[4] = { 44, 44, 44, 44 };

    static const int hurtXs[2] = { 10, 64 };
    static const int hurtYs[2] = { 460, 460 };
    static const int hurtWs[2] = { 36, 36 };
    static const int hurtHs[2] = { 21, 21 };

    static const int dieXs[4] = { 10, 10, 10, 10 };
    static const int dieYs[4] = { 831, 879, 926, 953 };
    static const int dieWs[4] = { 48, 51, 50, 52 };
    static const int dieHs[4] = { 43, 37, 22, 63 };

    anims[PHASE1].load("Sprites/Martian.png", phase1Xs, phase1Ys, phase1Ws, phase1Hs, 4, 0.10f);
    anims[PHASE2].load("Sprites/Martian.png", phase2Xs, phase2Ys, phase2Ws, phase2Hs, 6, 0.10f);
    anims[ATTACK1].load("Sprites/Martian.png", attack1Xs, attack1Ys, attack1Ws, attack1Hs, 4, 0.10f);
    anims[ATTACK2].load("Sprites/Martian.png", attack2Xs, attack2Ys, attack2Ws, attack2Hs, 4, 0.10f);
    anims[HURT].load("Sprites/Martian.png", hurtXs, hurtYs, hurtWs, hurtHs, 2, 0.12f);
    anims[DIE].load("Sprites/Martian.png", dieXs, dieYs, dieWs, dieHs, 4, 0.12f);
}

void Martian::move(float dt) {
    if (phase == 1) {
        if (largestPlayer) {
            float targetX = largestPlayer->getPlayerX() - width * 0.5f;
            float dx = targetX - x;
            if (dx > 0) velocityX = 1.f * speed;
            else velocityX = -1.f * speed;
            facingRight = (dx > 0);
        }
        velocityY = 0.f;
        x += velocityX * dt;

        podX = x;
        podY = y - 80.f;
    }
    else {
        chasePlayer(largestPlayer);
        x += velocityX * dt;
    }
}

void Martian::attack() {
    if (!largestPlayer) return;

    if (phase == 1 && isPodAlive) {
        float dx = largestPlayer->getPlayerX() - x;
        if (abs(dx) < 20.f)
            beamActive = true;
        else
            beamActive = false;
    }
    else if (phase == 2 && canFire()) {
        float dx = largestPlayer->getPlayerX() - x;
        float dy = largestPlayer->getPlayerY() - y;
        float dist = sqrt(dx * dx + dy * dy);
        
        if (dist < 350.f) {
            float angle = atan2(dy, dx);
            fireInDirection(angle, 250.f, 5, 400.f, Color(0, 255, 100));
            resetFireTimer();
        }
    }
}

void Martian::updatePhase() {
    if (phase == 1 && !isPodAlive)
        phase = 2;
}

void Martian::explodePod() {
    isPodAlive = false;
    podIsActive = false;
    updatePhase();
}

void Martian::checkFlatGround() {
    isOnFlatGround = isGrounded;
}

void Martian::update(float dt) {
    if (!isAlive || !largestPlayer) return;

    updatePhase();
    detectPlayer(largestPlayer);
    move(dt);

    if (phase == 2) {
        checkFlatGround();
    }

    attack();
    checkPlayerCollision(largestPlayer);
    
    updateFireTimer(dt);
}

void Martian::render(RenderWindow& window, float camX, float camY) {
    AlienEnemy::render(window, camX, camY);
}

// IronNokana implementations
IronNokana::IronNokana() : missileLauncherActive(false),
    rocketLauncherTimer(0.f), flamethrowerActive(false),
    fireBombCooldown(false), flameStreamRange(200.f),
    missileLauncherAngle(45.f), isOnFlatGround(false),
    fireBombTimer(0.f)
{
    name = "Iron Nokana";
    hp = maxHp = 30;
    scoreValue = 500;
    speed = 30.f;
    damage = 10;
    width = 80.f;
    height = 60.f;
    detectionRange = 600.f;
    attackRange = 500.f;

    static const int idleXs[2] = { 0, 0 };
    static const int idleYs[2] = { 0, 0 };
    static const int idleWs[2] = { 0, 0 };
    static const int idleHs[2] = { 0, 0 };

    static const int moveXs[6] = { 0, 0, 0, 0, 0, 0 };
    static const int moveYs[6] = { 0, 0, 0, 0, 0, 0 };
    static const int moveWs[6] = { 0, 0, 0, 0, 0, 0 };
    static const int moveHs[6] = { 0, 0, 0, 0, 0, 0 };

    static const int missileXs[5] = { 0, 0, 0, 0, 0 };
    static const int missileYs[5] = { 0, 0, 0, 0, 0 };
    static const int missileWs[5] = { 0, 0, 0, 0, 0 };
    static const int missileHs[5] = { 0, 0, 0, 0, 0 };

    static const int flameXs[6] = { 0, 0, 0, 0, 0, 0 };
    static const int flameYs[6] = { 0, 0, 0, 0, 0, 0 };
    static const int flameWs[6] = { 0, 0, 0, 0, 0, 0 };
    static const int flameHs[6] = { 0, 0, 0, 0, 0, 0 };

    static const int hurtXs[2] = { 0, 0 };
    static const int hurtYs[2] = { 0, 0 };
    static const int hurtWs[2] = { 0, 0 };
    static const int hurtHs[2] = { 0, 0 };

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

void IronNokana::retreat() { hasRetreated = true; }
bool IronNokana::getHasRetreated() const { return hasRetreated; }

void IronNokana::takeDamage(int dmg) {
    hp -= dmg;
    if (hp <= 0) hp = 0;
    if (hp <= maxHp * 0.5f && !hasRetreated) {
        retreat();
    }
}

void IronNokana::move(float dt) {
    if (!isOnFlatGround) return;

    if (largestPlayer)
        chasePlayer(largestPlayer);

    x += velocityX * dt;
}

void IronNokana::attack() {
    if (rocketLauncherTimer <= 0.f)
        activateLauncher();

    if (!fireBombCooldown)
        fireBomb();

    if (flamethrowerActive)
        activateFlamethrower();
}

void IronNokana::reload() {
    rocketLauncherTimer = 3.f;
}

void IronNokana::activateLauncher() {
    if (!largestPlayer) return;
    missileAttackAngle();
    missileLauncherActive = true;
    reload();
}

void IronNokana::activateFlamethrower() {
    flamethrowerActive = false;
}

void IronNokana::fireBomb() {
    fireBombCooldown = true;
    fireBombTimer = 4.f;
}

void IronNokana::missileAttackAngle() {
    if (!largestPlayer) return;
    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    missileLauncherAngle = atan2(dy, dx) * 180.f / 3.14159f;
}

void IronNokana::checkFlatGround() {
    isOnFlatGround = isGrounded;
}

void IronNokana::update(float dt) {
    if (!isAlive || !largestPlayer) return;

    if (rocketLauncherTimer > 0.f) rocketLauncherTimer -= dt;
    if (fireBombTimer > 0.f) {
        fireBombTimer -= dt;
        if (fireBombTimer <= 0.f)
            fireBombCooldown = false;
    }

    detectPlayer(largestPlayer);
    checkFlatGround();
    move(dt);
    attack();
    checkPlayerCollision(largestPlayer);
}

void IronNokana::render(RenderWindow& window, float camX, float camY) {
    AlienEnemy::render(window, camX, camY);
}

// HairbusterRiberts implementations
HairbusterRiberts::HairbusterRiberts() : AerialEnemy(),
    energyBombActive(false), bombCooldown(0.f), 
    bombCooldownDuration(2.5f), flightHeight(200.f), isDiving(false)
{
    name = "Hairbuster Riberts";
    hp = 30.f;
    maxHp = 30;
    width = 80.f;
    height = 60.f;
    speed = 120.f;
    damage = 8;
    scoreValue = 800;
    detectionRange = 600.f;
    attackRange = 500.f;
    isAlive = true;
    facingRight = true;
}

void HairbusterRiberts::attack() {
    if (bombCooldown > 0.f) return;
    if (!largestPlayer) return;

    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance < attackRange) {
        dropEnergyBomb();
        bombCooldown = bombCooldownDuration;
    }
}

void HairbusterRiberts::dropEnergyBomb() {
    if (bulletMgr) {
        float angle = 3.14159f / 2.f;
        bulletMgr->spawnBullet(x + width * 0.5f, y + height, 
            angle, damage, ENEMY, 300.f, 400.f, Color(150, 150, 255));
    }
    energyBombActive = true;
}

void HairbusterRiberts::retreat() { hasRetreated = true; }
bool HairbusterRiberts::getHasRetreated() const { return hasRetreated; }

void HairbusterRiberts::takeDamage(int dmg) {
    hp -= dmg;
    if (hp <= 0) hp = 0;
    if (hp <= maxHp * 0.5f && !hasRetreated) {
        retreat();
    }
}

void HairbusterRiberts::update(float dt) {
    if (!isAlive || !largestPlayer) return;

    if (bombCooldown > 0.f) bombCooldown -= dt;

    detectPlayer(largestPlayer);
    move(dt);
    attack();
    checkPlayerCollision(largestPlayer);
}

void HairbusterRiberts::render(RenderWindow& window, float camX, float camY) {
    if (!isAlive) return;

    RectangleShape bossShape(Vector2f(width, height));
    bossShape.setFillColor(Color(100, 100, 200));
    bossShape.setPosition(x - camX, y - camY);
    window.draw(bossShape);
}

// SeaSatan implementations
SeaSatan::SeaSatan() : Enemy(),
    underwater(true), torpedoActive(false), torpedoCooldown(0.f),
    torpedoCooldownDuration(3.0f), swimSpeed(80.f), surfaceLevel(450.f)
{
    name = "Sea Satan";
    hp = 30.f;
    maxHp = 30;
    width = 90.f;
    height = 50.f;
    speed = 80.f;
    damage = 10;
    scoreValue = 1000;
    detectionRange = 400.f;
    attackRange = 350.f;
    isAlive = true;
    facingRight = true;
    isGrounded = false;
}

void SeaSatan::attack() {
    if (torpedoCooldown > 0.f) return;
    if (!largestPlayer) return;

    float dx = largestPlayer->getPlayerX() - x;
    float dy = largestPlayer->getPlayerY() - y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance < attackRange) {
        fireTorpedo();
        torpedoCooldown = torpedoCooldownDuration;
    }
}

void SeaSatan::fireTorpedo() {
    if (bulletMgr) {
        float angle = facingRight ? 0.f : 3.14159f;
        bulletMgr->spawnBullet(x + (facingRight ? width : 0), y + height * 0.5f,
            angle, damage, ENEMY, 200.f, 500.f, Color(0, 200, 200));
    }
    torpedoActive = true;
}

void SeaSatan::update(float dt) {
    if (!isAlive || !largestPlayer) return;

    if (torpedoCooldown > 0.f) torpedoCooldown -= dt;

    if (y > surfaceLevel) {
        y -= swimSpeed * dt;
        underwater = false;
    } else {
        underwater = true;
    }

    detectPlayer(largestPlayer);
    move(dt);
    attack();
    checkPlayerCollision(largestPlayer);
}

void SeaSatan::retreat() { hasRetreated = true; }
bool SeaSatan::getHasRetreated() const { return hasRetreated; }

void SeaSatan::takeDamage(int dmg) {
    hp -= dmg;
    if (hp <= 0) hp = 0;
    if (hp <= maxHp * 0.5f && !hasRetreated) {
        retreat();
    }
}

void SeaSatan::render(RenderWindow& window, float camX, float camY) {
    if (!isAlive) return;

    RectangleShape bossShape(Vector2f(width, height));
    bossShape.setFillColor(Color(0, 150, 150));
    bossShape.setPosition(x - camX, y - camY);
    window.draw(bossShape);
}