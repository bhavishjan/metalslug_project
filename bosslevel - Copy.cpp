#include "bosslevel.h"

// Boss stuff
Boss::Boss() {
    x = y = 0;
    width = height = 80;
    hp = maxHp = 30;
    isAlive = true;
    hasRetreated = false;
    scoreValue = 500;
    speed = 30.f;
    bulletMgr = nullptr;
    target = nullptr;
}

Boss::~Boss() {}

void Boss::takeDamage(int dmg) {
    hp -= dmg;
    if (hp <= 0) {
        hp = 0;
        isAlive = false;
    }
}

bool Boss::shouldRetreat() const {
    return (hp <= maxHp * 0.5f) && !hasRetreated;
}

void Boss::retreat() {
    hasRetreated = true;
    isAlive = false;
}

void Boss::setPosition(float nx, float ny) { x = nx; y = ny; }
void Boss::setPlayer(PlayerSoldier* p) { target = p; }
void Boss::setBulletManager(BulletManager* bm) { bulletMgr = bm; }

float Boss::getX()     const { return x; }
float Boss::getY()     const { return y; }
float Boss::getWidth() const { return width; }
float Boss::getHeight()const { return height; }
float Boss::getHp()    const { return hp; }
float Boss::getMaxHp() const { return maxHp; }
bool  Boss::getIsAlive()    const { return isAlive; }
bool  Boss::getHasRetreated()const { return hasRetreated; }
int   Boss::getScoreValue() const { return scoreValue; }

// UltimateBoss implementations
UltimateBoss::UltimateBoss(float startX, float startY) {
    x = startX;
    y = startY;
    width = 100.f;
    height = 80.f;
    maxHp = 30.f;
    groundHp = aerialHp = aquaticHp = maxHp;
    currentState = 0;
    stateTimer = 12.f;
    stateDuration = 12.f;
    isAlive = true;
    bulletMgr = nullptr;
    target = nullptr;

    groundBoss = new IronNokana();
    aerialBoss = new HairbusterRiberts();
    aquaticBoss = new SeaSatan();

    bodyShape.setSize(Vector2f(width, height));
}

UltimateBoss::~UltimateBoss() {
    delete groundBoss;
    delete aerialBoss;
    delete aquaticBoss;
}

void UltimateBoss::setPlayer(PlayerSoldier* p) {
    target = p;
    groundBoss->setPlayer(p);
    aerialBoss->setPlayer(p);
    aquaticBoss->setPlayer(p);
}

void UltimateBoss::setBulletManager(BulletManager* bm) {
    bulletMgr = bm;
    groundBoss->setBulletManager(bm);
    aerialBoss->setBulletManager(bm);
    aquaticBoss->setBulletManager(bm);
}

void UltimateBoss::syncPositions() {
    groundBoss->setPosition(x, y);
    aerialBoss->setPosition(x, y);
    aquaticBoss->setPosition(x, y);
}

void UltimateBoss::cycleState() {
    for (int attempt = 0; attempt < 3; attempt++) {
        currentState = (currentState + 1) % 3;
        if (currentState == 0 && groundHp > 0) break;
        if (currentState == 1 && aerialHp > 0) break;
        if (currentState == 2 && aquaticHp > 0) break;
    }
    stateTimer = stateDuration;
}

void UltimateBoss::takeDamage(int dmg) {
    if (currentState == 0) groundHp -= dmg;
    if (currentState == 1) aerialHp -= dmg;
    if (currentState == 2) aquaticHp -= dmg;

    if (groundHp < 0) groundHp = 0;
    if (aerialHp < 0) aerialHp = 0;
    if (aquaticHp < 0) aquaticHp = 0;

    if (groundHp <= 0 && aerialHp <= 0 && aquaticHp <= 0) {
        isAlive = false;
    }
}

void UltimateBoss::update(float dt) {
    if (!isAlive) return;

    syncPositions();

    stateTimer -= dt;
    if (stateTimer <= 0.f) {
        cycleState();
    }

    if (target) {
        float dx = target->getPlayerX() - x;
        float speed = 50.f;
        x += (dx > 0 ? speed : -speed) * dt;
    }

    if (currentState == 0 && groundHp > 0)
        groundBoss->attack();
    else if (currentState == 1 && aerialHp > 0)
        aerialBoss->attack();
    else if (currentState == 2 && aquaticHp > 0)
        aquaticBoss->attack();
}

void UltimateBoss::render(RenderWindow& window, float camX, float camY) {
    if (!isAlive) return;

    Color stateColor;
    if (currentState == 0) stateColor = Color(180, 80, 0);
    else if (currentState == 1) stateColor = Color(60, 60, 180);
    else                        stateColor = Color(30, 80, 60);

    bodyShape.setFillColor(stateColor);
    bodyShape.setPosition(x - camX, y - camY);
    window.draw(bodyShape);

    float barW = width;
    float barY = y - camY - 36.f;

    RectangleShape g(Vector2f(barW * (groundHp / maxHp), 7.f));
    g.setFillColor(Color(220, 80, 0));
    g.setPosition(x - camX, barY);
    window.draw(g);

    RectangleShape a(Vector2f(barW * (aerialHp / maxHp), 7.f));
    a.setFillColor(Color(80, 80, 220));
    a.setPosition(x - camX, barY + 9.f);
    window.draw(a);

    RectangleShape aq(Vector2f(barW * (aquaticHp / maxHp), 7.f));
    aq.setFillColor(Color(50, 200, 100));
    aq.setPosition(x - camX, barY + 18.f);
    window.draw(aq);
}

bool UltimateBoss::getIsAlive()    const { return isAlive; }
int  UltimateBoss::getCurrentState()const { return currentState; }
float UltimateBoss::getX()         const { return x; }
float UltimateBoss::getY()         const { return y; }
float UltimateBoss::getWidth()     const { return width; }
float UltimateBoss::getHeight()    const { return height; }

// BossLevel implementations
BossLevel::BossLevel() : SurvivalLevel("Boss Level", 4, 83, 15)
{
    currentPhase = 1;
    isPhase1Complete = false;
    isPhase2Complete = false;
    isPhase3Complete = false;
    isPhase4Complete = false;

    groundBoss = nullptr;
    aerialBoss = nullptr;
    aquaticBoss = nullptr;
    ultimateBoss = nullptr;

    bossMinions = new Enemy * [MAX_MINIONS];
    for (int i = 0; i < MAX_MINIONS; i++) bossMinions[i] = nullptr;
    minionCount = 0;
    minionBatchSize = 3;
    minionBatchKilled = 0;
    totalMinionBatches = 0;

    blendedBiomeActive = false;
    bulletMgr = nullptr;
    playerRef = nullptr;
    phaseMessageTimer = 0.f;

    scoreMultiplier = 3.0f;
    playerSpawnX = 400.f;
    playerSpawnY = 0.f;
    totalEnemies = 1;

    generateBiomes();
}

BossLevel::~BossLevel() {
    delete groundBoss;
    delete aerialBoss;
    delete aquaticBoss;
    delete ultimateBoss;
    if (bossMinions) {
        for (int i = 0; i < MAX_MINIONS; i++) {
            delete bossMinions[i];
        }
        delete[] bossMinions;
    }
}

void BossLevel::setBulletManager(BulletManager* bm) { bulletMgr = bm; }
void BossLevel::setPlayerRef(PlayerSoldier* p) { playerRef = p; }

void BossLevel::generateBiomes() {
    plains = new PlainsBiome(levelStart, plainsEnd);
    aerial = new AerialBiome(plainsEnd, aerialEnd);
    aquatic = new AquaticBiome(aerialEnd, aquaticEnd);

    loadTextures("Sprites/blocks/stone.png",
        "Sprites/blocks/water.png",
        "Sprites/blocks/grass.png",
        "Sprites/blocks/dirt.png");

    plains->generateTerrain(biomeWidth, biomeHeight);
    aerial->generateTerrain(biomeWidth, biomeHeight);
    aquatic->generateTerrain(biomeWidth, biomeHeight);

    isLoaded = true;
}

void BossLevel::spawnEnemies(EnemyManager& mgr, PlayerSoldier* player) {
    playerRef = player;
    startPhase1();
}

void BossLevel::startPhase1() {
    currentPhase = 1;
    delete groundBoss;
    groundBoss = new IronNokana();
    groundBoss->setPosition(levelStart + (plainsEnd - levelStart) * 0.5f, 400.f);
    if (playerRef)  groundBoss->setPlayer(playerRef);
    if (bulletMgr)  groundBoss->setBulletManager(bulletMgr);

    spawnMinionsForPhase(1);
    phaseMessageTimer = 3.f;
}

void BossLevel::startPhase2() {
    currentPhase = 2;
    delete aerialBoss;
    aerialBoss = new HairbusterRiberts();
    aerialBoss->setPosition(plainsEnd + (aerialEnd - plainsEnd) * 0.5f, 200.f);
    if (playerRef) aerialBoss->setPlayer(playerRef);
    if (bulletMgr) aerialBoss->setBulletManager(bulletMgr);

    spawnMinionsForPhase(2);
    phaseMessageTimer = 3.f;
}

void BossLevel::startPhase3() {
    currentPhase = 3;
    delete aquaticBoss;
    aquaticBoss = new SeaSatan();
    aquaticBoss->setPosition(aerialEnd + (aquaticEnd - aerialEnd) * 0.5f, 450.f);
    if (playerRef) aquaticBoss->setPlayer(playerRef);
    if (bulletMgr) aquaticBoss->setBulletManager(bulletMgr);

    spawnMinionsForPhase(3);
    phaseMessageTimer = 3.f;
}

void BossLevel::startPhase4() {
    currentPhase = 4;
    blendedBiomeActive = true;

    delete ultimateBoss;
    float midX = (levelStart + levelEnd) * 0.5f;
    ultimateBoss = new UltimateBoss(midX, 350.f);
    if (playerRef) ultimateBoss->setPlayer(playerRef);
    if (bulletMgr) ultimateBoss->setBulletManager(bulletMgr);

    spawnMinionsForPhase(4);
    phaseMessageTimer = 3.f;
}

void BossLevel::spawnMinionsForPhase(int phase) {
    for (int i = 0; i < MAX_MINIONS; i++) {
        delete bossMinions[i];
        bossMinions[i] = nullptr;
    }
    minionCount = 0;
    minionBatchKilled = 0;

    float spawnX = levelStart + 300.f;
    float spawnY = 300.f;
    int   batch = minionBatchSize;

    for (int i = 0; i < batch && minionCount < MAX_MINIONS; i++) {
        Enemy* m = nullptr;
        if (phase == 1) m = new RebelSoldier();
        if (phase == 2) m = new Paratrooper();
        if (phase == 3) m = new Zombie();
        if (phase == 4) m = new Martian();
        if (m) {
            m->setPosition(spawnX + i * 120.f, spawnY);
            if (playerRef) m->setPlayer(playerRef);
            if (bulletMgr) m->setBulletManager(bulletMgr);
            bossMinions[minionCount++] = m;
        }
    }
    totalMinionBatches++;
}

void BossLevel::checkBulletHitsOnBosses(BulletManager& bm) {
    if (currentPhase == 1 && groundBoss && groundBoss->getIsAlive()) {
        int dmg = 0; float hx, hy;
        if (bm.popPlayerBulletHit(groundBoss->getX(), groundBoss->getY(),
            groundBoss->getWidth(), groundBoss->getHeight(),
            dmg, hx, hy)) {
            groundBoss->takeDamage(dmg);
        }
    }
    if (currentPhase == 2 && aerialBoss && aerialBoss->getIsAlive()) {
        int dmg = 0; float hx, hy;
        if (bm.popPlayerBulletHit(aerialBoss->getX(), aerialBoss->getY(),
            aerialBoss->getWidth(), aerialBoss->getHeight(),
            dmg, hx, hy)) {
            aerialBoss->takeDamage(dmg);
        }
    }
    if (currentPhase == 3 && aquaticBoss && aquaticBoss->getIsAlive()) {
        int dmg = 0; float hx, hy;
        if (bm.popPlayerBulletHit(aquaticBoss->getX(), aquaticBoss->getY(),
            aquaticBoss->getWidth(), aquaticBoss->getHeight(),
            dmg, hx, hy)) {
            aquaticBoss->takeDamage(dmg);
        }
    }
    if (currentPhase == 4 && ultimateBoss && ultimateBoss->getIsAlive()) {
        int dmg = 0; float hx, hy;
        if (bm.popPlayerBulletHit(ultimateBoss->getX(), ultimateBoss->getY(),
            ultimateBoss->getWidth(), ultimateBoss->getHeight(),
            dmg, hx, hy)) {
            ultimateBoss->takeDamage(dmg);
        }
    }
}

void BossLevel::update(float dt) {
    Level::update(dt);

    if (phaseMessageTimer > 0.f) phaseMessageTimer -= dt;

    if (currentPhase == 1) {
        if (groundBoss) groundBoss->update(dt);
        updateMinions(dt);

        if (groundBoss && groundBoss->getHasRetreated() && !isPhase1Complete) {
            isPhase1Complete = true;
            startPhase2();
        }
    }
    else if (currentPhase == 2) {
        if (aerialBoss) aerialBoss->update(dt);
        updateMinions(dt);

        if (aerialBoss && aerialBoss->getHasRetreated() && !isPhase2Complete) {
            isPhase2Complete = true;
            startPhase3();
        }
    }
    else if (currentPhase == 3) {
        if (aquaticBoss) aquaticBoss->update(dt);
        updateMinions(dt);

        if (aquaticBoss && aquaticBoss->getHasRetreated() && !isPhase3Complete) {
            isPhase3Complete = true;
            startPhase4();
        }
    }
    else if (currentPhase == 4) {
        if (ultimateBoss) ultimateBoss->update(dt);
        updateMinions(dt);

        if (ultimateBoss && !ultimateBoss->getIsAlive() && !isPhase4Complete) {
            isPhase4Complete = true;
            isComplete = true;
        }
    }
}

void BossLevel::updateMinions(float dt) const {
    for (int i = 0; i < MAX_MINIONS; i++) {
        if (!bossMinions[i]) continue;
        if (!bossMinions[i]->getIsAlive()) continue;
        bossMinions[i]->update(dt);
    }
}

void BossLevel::render(RenderWindow& window, float camX, float camY) {
    Level::render(window, camX, camY);

    if (currentPhase == 1 && groundBoss)
        groundBoss->render(window, camX, camY);
    else if (currentPhase == 2 && aerialBoss)
        aerialBoss->render(window, camX, camY);
    else if (currentPhase == 3 && aquaticBoss)
        aquaticBoss->render(window, camX, camY);
    else if (currentPhase == 4 && ultimateBoss)
        ultimateBoss->render(window, camX, camY);

    for (int i = 0; i < MAX_MINIONS; i++) {
        if (bossMinions[i] && bossMinions[i]->getIsAlive())
            bossMinions[i]->render(window, camX, camY);
    }

    if (phaseMessageTimer > 0.f) {
        Font font;
        font.loadFromFile("arial.TTF");
        Text msg;
        msg.setFont(font);
        msg.setCharacterSize(60);
        msg.setFillColor(Color::Red);

        if (currentPhase == 1) msg.setString("PHASE 1 - IRON NOKANA!");
        else if (currentPhase == 2) msg.setString("PHASE 2 - HAIRBUSTER RIBERTS!");
        else if (currentPhase == 3) msg.setString("PHASE 3 - SEA SATAN!");
        else if (currentPhase == 4) msg.setString("PHASE 4 - ULTIMATE BOSS!");

        FloatRect bounds = msg.getLocalBounds();
        msg.setPosition(800.f - bounds.width * 0.5f, 400.f);
        window.draw(msg);
    }
}

bool BossLevel::checkLevelComplete() {
    return isPhase4Complete;
}

int  BossLevel::getCurrentPhase()    const { return currentPhase; }
bool BossLevel::getPhase1Complete()  const { return isPhase1Complete; }
bool BossLevel::getPhase4Complete()  const { return isPhase4Complete; }

Enemy* BossLevel::getMinionAt(int i) const {
    if (i >= 0 && i < MAX_MINIONS) return bossMinions[i];
    return nullptr;
}
int BossLevel::getMaxMinions() const { return MAX_MINIONS; }
