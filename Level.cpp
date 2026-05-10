#include "Level.h"

// Level implementations
Level::Level(const char* name, int levelNumber, int biomeWidth, int biomeHeight) {
    this->name = name;
    this->levelNumber = levelNumber;
    this->biomeWidth = biomeWidth;
    this->biomeHeight = biomeHeight;
    this->blockSize = 64.0f;
    this->isComplete = false;
    this->isLoaded = false;

    playerSpawnX = 0;
    playerSpawnY = 0;
    plains = nullptr;
    aerial = nullptr;
    aquatic = nullptr;

    levelStart = 0;
    plainsEnd = biomeWidth * blockSize;
    aerialEnd = plainsEnd + biomeWidth * blockSize;
    aquaticEnd = aerialEnd + biomeWidth * blockSize;
    levelEnd = aquaticEnd;
}

Level::~Level() {
    delete plains;
    delete aerial;
    delete aquatic;
    plains = nullptr;
    aerial = nullptr;
    aquatic = nullptr;
}

void Level::loadTextures(const char* solidPath, const char* waterPath, const char* grassPath, const char* dirtPath) {
    if (plains) {
        plains->loadTextures(solidPath, waterPath, grassPath, dirtPath);
    }
    if (aerial) {
        aerial->loadTextures(solidPath, waterPath, grassPath, dirtPath);
    }
    if (aquatic) {
        aquatic->loadTextures(solidPath, waterPath, grassPath, dirtPath);
    }
}

void Level::update(float dt) {
    if (plains) {
        plains->update(dt);
    }
    if (aerial) {
        aerial->update(dt);
    }
    if (aquatic) {
        aquatic->update(dt);
    }
}

void Level::render(RenderWindow& window, float camX, float camY) {
    if (plains) {
        plains->render(window, camX, camY);
    }
    if (aerial) {
        aerial->render(window, camX, camY);
    }
    if (aquatic) {
        aquatic->render(window, camX, camY);
    }
}

void Level::resolveCollisions(float& px, float& py, float pw, float ph, float& velX, float& velY, bool& onGround) {
    if (plains) {
        plains->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
    }
    if (aerial) {
        aerial->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
    }
    if (aquatic) {
        aquatic->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
    }
}

bool Level::checkCollision(float px, float py, float pw, float ph) {
    if (plains && plains->checkCollision(px, py, pw, ph))
        return true;

    if (aerial && aerial->checkCollision(px, py, pw, ph))
        return true;

    if (aquatic && aquatic->checkCollision(px, py, pw, ph))
        return true;

    return false;
}

float Level::getPlayerSpawnX() { return playerSpawnX; }
float Level::getPlayerSpawnY() { return playerSpawnY; }
void Level::playerReachedEnd() { isComplete = true; }
void Level::spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) {
}

const char* Level::getName() { return name; }
int Level::getLevelNum() { return levelNumber; }
float Level::getLevelEnd() { return levelEnd; }
float Level::getLevelStart() { return levelStart; }
float Level::getPlainsEnd() { return plainsEnd; }
float Level::getAerialEnd() { return aerialEnd; }
float Level::getAquaticEnd() { return aquaticEnd; }
bool Level::getIsComplete() { return isComplete; }
bool Level::getIsLoaded() { return isLoaded; }

bool Level::checkWaterAt(float px, float py, float pw, float ph) {
    if (plains && plains->checkWaterCollision(px, py, pw, ph)) {
        return true;
    }
    if (aerial && aerial->checkWaterCollision(px, py, pw, ph)) {
        return true;
    }
    if (aquatic && aquatic->checkWaterCollision(px, py, pw, ph)) {
        return true;
    }
    return false;
}

// SurvivalLevel implementations
SurvivalLevel::SurvivalLevel(const char* name, int levelNumber, int biomeWidth, int biomeHeight)
    : Level(name, levelNumber, biomeWidth, biomeHeight)
{
    totalEnemies = 0;
    enemiesKilled = 0;

    infantryBatchCount = 0;
    aerialBatchCount = 0;
    undeadBatchCount = 0;
    zombieBatchCount = 0;
    flyingTaraBatchCount = 0;
    powPrisonerCount = 0;

    hasHorizontalScroll = true;
    hasVerticalScroll = true;

    scoreMultiplier = 1.0f;
}

SurvivalLevel::~SurvivalLevel() {}

bool SurvivalLevel::checkLevelComplete() {
    if (enemiesKilled >= totalEnemies && totalEnemies > 0) {
        isComplete = true;
    }
    return isComplete;
}

void SurvivalLevel::playerReachedEnd() {
    isComplete = true;
}

void SurvivalLevel::enemyKilled() {
    enemiesKilled++;
    checkLevelComplete();
}

float SurvivalLevel::getScoreMultiplier() { return scoreMultiplier; }
int SurvivalLevel::getEnemiesKilled() { return enemiesKilled; }
int SurvivalLevel::getTotalEnemies() { return totalEnemies; }
bool SurvivalLevel::isHorizontalScroll() { return hasHorizontalScroll; }
bool SurvivalLevel::isVerticalScroll() { return hasVerticalScroll; }

// Level1 implementations
Level1::Level1() : SurvivalLevel("Level 1", 1, 83, 15)
{
    rebelBatchCount = 2;
    shieldedBatchCount = 2;
    bazookaBatchCount = 2;
    grenadierBatchCount = 2;

    paratrooperBatchCount = 1;
    martianBatchCount = 1;

    mummyBatchCount = 2;

    zombieBatchCountAquatic = 2;
    enemySubCount = 1;

    flyingTaraCount = 2;
    powCount = 3;

    totalEnemies =
        rebelBatchCount * 3 +
        shieldedBatchCount * 1 +
        bazookaBatchCount * 1 +
        grenadierBatchCount * 1 +
        paratrooperBatchCount * 2 +
        martianBatchCount * 1 +
        mummyBatchCount * 1 +
        zombieBatchCountAquatic * 4 + flyingTaraCount * 2 +
        enemySubCount * 1;

    scoreMultiplier = 1.0f;

    playerSpawnX = 400;
    playerSpawnY = 0;
    generateBiomes();
}

Level1::~Level1() {}

void Level1::generateBiomes() {
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

void Level1::spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) {
    float spawnY = 200.0f;

    float plainsStart = levelStart;
    float plainsEnd_ = plainsEnd;
    float aerialStart = plainsEnd;
    float aerialEnd_ = aerialEnd;
    float aquaticStart = aerialEnd;
    float aquaticEnd_ = aquaticEnd;

    float plainsW = plainsEnd_ - plainsStart;
    float aerialW = aerialEnd_ - aerialStart;
    float aquaticW = aquaticEnd_ - aquaticStart;

    for (int b = 0; b < 2; b++) {
        float base = plainsStart + (plainsW * 0.25f) + b * (plainsW * 0.45f);

        for (int j = 0; j < 3; j++) {
            RebelSoldier* r = new RebelSoldier();
            r->setPosition(base + j * 150, spawnY);
            r->setPlayer(player); r->setPatrolRange(100.f);
            enemyManager.addEnemy(r);
        }
        ShieldedSoldier* sh = new ShieldedSoldier();
        sh->setPosition(base + 500, spawnY);
        sh->setPlayer(player); sh->setPatrolRange(80.f);
        enemyManager.addEnemy(sh);

        BazookaSoldier* bz = new BazookaSoldier();
        bz->setPosition(base + 700, spawnY);
        bz->setPlayer(player); bz->setPatrolRange(80.f);
        enemyManager.addEnemy(bz);

        GrenadeSoldier* gr = new GrenadeSoldier();
        gr->setPosition(base + 900, spawnY);
        gr->setPlayer(player); gr->setPatrolRange(80.f);
        enemyManager.addEnemy(gr);
    }

    for (int b = 0; b < 2; b++) {
        float base = plainsStart + (plainsW * 0.30f) + b * (plainsW * 0.40f);
        MummyWarrior* m = new MummyWarrior();
        m->setPosition(base, spawnY);
        m->setPlayer(player); m->setPatrolRange(60.f);
        enemyManager.addEnemy(m);
    }

    for (int b = 0; b < 2; b++) {
        float base = aerialStart + (aerialW * 0.25f) + b * (aerialW * 0.45f);

        for (int j = 0; j < 3; j++) {
            RebelSoldier* r = new RebelSoldier();
            r->setPosition(base + j * 150, spawnY);
            r->setPlayer(player); r->setPatrolRange(100.f);
            enemyManager.addEnemy(r);
        }
        ShieldedSoldier* sh = new ShieldedSoldier();
        sh->setPosition(base + 500, spawnY);
        sh->setPlayer(player); sh->setPatrolRange(80.f);
        enemyManager.addEnemy(sh);

        BazookaSoldier* bz = new BazookaSoldier();
        bz->setPosition(base + 700, spawnY);
        bz->setPlayer(player); bz->setPatrolRange(80.f);
        enemyManager.addEnemy(bz);

        GrenadeSoldier* gr = new GrenadeSoldier();
        gr->setPosition(base + 900, spawnY);
        gr->setPlayer(player); gr->setPatrolRange(80.f);
        enemyManager.addEnemy(gr);
    }

    for (int j = 0; j < 2; j++) {
        Paratrooper* p = new Paratrooper();
        p->setPosition(aerialStart + (aerialW * 0.35f) + j * 250, -150.f);
        p->setPlayer(player);
        enemyManager.addEnemy(p);
    }

    Martian* mar = new Martian();
    mar->setPosition(aerialStart + (aerialW * 0.65f), -50.f);
    mar->setPlayer(player);
    enemyManager.addEnemy(mar);

    for (int j = 0; j < 2; j++) {
        FlyingTara* ft = new FlyingTara();
        ft->setPosition(aerialStart + (aerialW * 0.40f) + j * 300, 150.f);
        ft->setPlayer(player);
        enemyManager.addEnemy(ft);
    }

    for (int b = 0; b < 2; b++) {
        float base = aquaticStart + (aquaticW * 0.25f) + b * (aquaticW * 0.45f);

        for (int j = 0; j < 3; j++) {
            RebelSoldier* r = new RebelSoldier();
            r->setPosition(base + j * 150, spawnY);
            r->setPlayer(player); r->setPatrolRange(100.f);
            enemyManager.addEnemy(r);
        }
        ShieldedSoldier* sh = new ShieldedSoldier();
        sh->setPosition(base + 500, spawnY);
        sh->setPlayer(player); sh->setPatrolRange(80.f);
        enemyManager.addEnemy(sh);

        BazookaSoldier* bz = new BazookaSoldier();
        bz->setPosition(base + 700, spawnY);
        bz->setPlayer(player); bz->setPatrolRange(80.f);
        enemyManager.addEnemy(bz);

        GrenadeSoldier* gr = new GrenadeSoldier();
        gr->setPosition(base + 900, spawnY);
        gr->setPlayer(player); gr->setPatrolRange(80.f);
        enemyManager.addEnemy(gr);
    }

    for (int b = 0; b < 2; b++) {
        float base = aquaticStart + (aquaticW * 0.25f) + b * (aquaticW * 0.45f);
        for (int j = 0; j < 3; j++) {
            Zombie* z = new Zombie();
            z->setPosition(base + j * 150, spawnY);
            z->setPlayer(player); z->setPatrolRange(80.f);
            enemyManager.addEnemy(z);
        }
    }

    for (int j = 0; j < 2; j++) {
        FlyingTara* ft = new FlyingTara();
        ft->setPosition(aquaticStart + (aquaticW * 0.40f) + j * 300, 150.f);
        ft->setPlayer(player);
        enemyManager.addEnemy(ft);
    }
}

void Level1::update(float dt) {
    Level::update(dt);
    checkLevelComplete();
}

void Level1::render(RenderWindow& window, float camX, float camY) {
    Level::render(window, camX, camY);
}

int Level1::getRebelBatchCount() { return rebelBatchCount; }
int Level1::getMummyBatchCount() { return mummyBatchCount; }
int Level1::getZombieCount() { return zombieBatchCountAquatic; }
int Level1::getEnemySubCount() { return enemySubCount; }
int Level1::getFlyingTaraCount() { return flyingTaraCount; }
int Level1::getPowCount() { return powCount; }

// Level2 implementations
Level2::Level2() : SurvivalLevel("Level 2", 2, 83, 14)
{
    rebelBatchCount = 3;
    shieldedBatchCount = 3;
    bazookaBatchCount = 3;
    grenadierBatchCount = 3;

    paratrooperBatchCount = 2;
    martianBatchCount = 2;

    mummyBatchCount = 3;

    zombieBatchCountAquatic = 3;
    enemySubCount = 1;

    flyingTaraCount = 3;
    bradleyCount = 2;

    powPrisonerPlains = 2;
    powPrisonerOther = 1;

    totalEnemies =
        rebelBatchCount * 3 +
        shieldedBatchCount * 1 +
        bazookaBatchCount * 1 +
        grenadierBatchCount * 1 +
        paratrooperBatchCount * 2 +
        martianBatchCount * 1 +
        mummyBatchCount * 1 +
        zombieBatchCountAquatic * 4 + flyingTaraCount * 2 +
        enemySubCount * 1;

    scoreMultiplier = 1.5f;

    playerSpawnX = 400;
    playerSpawnY = 0;
    generateBiomes();
}

Level2::~Level2() {}

void Level2::generateBiomes() {
    aerial = new AerialBiome(levelStart, plainsEnd);
    aquatic = new AquaticBiome(plainsEnd, aerialEnd);
    plains = new PlainsBiome(aerialEnd, aquaticEnd);
    loadTextures("Sprites/blocks/stone.png",
        "Sprites/blocks/water.png",
        "Sprites/blocks/grass.png",
        "Sprites/blocks/dirt.png");

    aerial->generateTerrain(biomeWidth, biomeHeight);
    aquatic->generateTerrain(biomeWidth, biomeHeight);
    plains->generateTerrain(biomeWidth, biomeHeight);

    isLoaded = true;
}

void Level2::spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) {
    float spawnY = -200.0f;

    float aerialStart = levelStart;
    float aerialEnd_ = plainsEnd;
    float aquaticStart = plainsEnd;
    float aquaticEnd_ = aerialEnd;
    float plainsStart = aerialEnd;
    float plainsEnd_ = aquaticEnd;

    float aerialW = aerialEnd_ - aerialStart;
    float aquaticW = aquaticEnd_ - aquaticStart;
    float plainsW = plainsEnd_ - plainsStart;

    for (int b = 0; b < 3; b++) {
        float base = aerialStart + 300 + b * (aerialW / 3.5f);
        for (int j = 0; j < 3; j++) {
            RebelSoldier* r = new RebelSoldier();
            r->setPosition(base + j * 120, spawnY);
            r->setPlayer(player); r->setPatrolRange(100.f);
            enemyManager.addEnemy(r);
        }
        ShieldedSoldier* sh = new ShieldedSoldier();
        sh->setPosition(base + 400, spawnY);
        sh->setPlayer(player); sh->setPatrolRange(80.f);
        enemyManager.addEnemy(sh);

        BazookaSoldier* bz = new BazookaSoldier();
        bz->setPosition(base + 600, spawnY);
        bz->setPlayer(player); bz->setPatrolRange(80.f);
        enemyManager.addEnemy(bz);

        GrenadeSoldier* gr = new GrenadeSoldier();
        gr->setPosition(base + 800, spawnY);
        gr->setPlayer(player); gr->setPatrolRange(80.f);
        enemyManager.addEnemy(gr);
    }

    for (int b = 0; b < 2; b++) {
        for (int j = 0; j < 2; j++) {
            Paratrooper* p = new Paratrooper();
            p->setPosition(aerialStart + 400 + b * (aerialW / 2.5f) + j * 200, -150.f);
            p->setPlayer(player);
            enemyManager.addEnemy(p);
        }
    }

    for (int b = 0; b < 2; b++) {
        Martian* mar = new Martian();
        mar->setPosition(aerialStart + 500 + b * (aerialW / 2.5f), 150.f);
        mar->setPlayer(player);
        enemyManager.addEnemy(mar);
    }

    for (int b = 0; b < 3; b++) {
        float base = aquaticStart + 300 + b * (aquaticW / 3.5f);
        for (int j = 0; j < 3; j++) {
            RebelSoldier* r = new RebelSoldier();
            r->setPosition(base + j * 120, spawnY);
            r->setPlayer(player); r->setPatrolRange(100.f);
            enemyManager.addEnemy(r);
        }
        ShieldedSoldier* sh = new ShieldedSoldier();
        sh->setPosition(base + 400, spawnY);
        sh->setPlayer(player); sh->setPatrolRange(80.f);
        enemyManager.addEnemy(sh);

        BazookaSoldier* bz = new BazookaSoldier();
        bz->setPosition(base + 600, spawnY);
        bz->setPlayer(player); bz->setPatrolRange(80.f);
        enemyManager.addEnemy(bz);

        GrenadeSoldier* gr = new GrenadeSoldier();
        gr->setPosition(base + 800, spawnY);
        gr->setPlayer(player); gr->setPatrolRange(80.f);
        enemyManager.addEnemy(gr);
    }

    for (int b = 0; b < 3; b++) {
        for (int j = 0; j < 3; j++) {
            Zombie* z = new Zombie();
            z->setPosition(aquaticStart + 200 + b * (aquaticW / 3.5f) + j * 150, spawnY);
            z->setPlayer(player); z->setPatrolRange(80.f);
            enemyManager.addEnemy(z);
        }
    }

    for (int b = 0; b < 3; b++) {
        float base = plainsStart + 300 + b * (plainsW / 3.5f);
        for (int j = 0; j < 3; j++) {
            RebelSoldier* r = new RebelSoldier();
            r->setPosition(base + j * 120, spawnY);
            r->setPlayer(player); r->setPatrolRange(100.f);
            enemyManager.addEnemy(r);
        }
        ShieldedSoldier* sh = new ShieldedSoldier();
        sh->setPosition(base + 400, spawnY);
        sh->setPlayer(player); sh->setPatrolRange(80.f);
        enemyManager.addEnemy(sh);

        BazookaSoldier* bz = new BazookaSoldier();
        bz->setPosition(base + 600, spawnY);
        bz->setPlayer(player); bz->setPatrolRange(80.f);
        enemyManager.addEnemy(bz);

        GrenadeSoldier* gr = new GrenadeSoldier();
        gr->setPosition(base + 800, spawnY);
        gr->setPlayer(player); gr->setPatrolRange(80.f);
        enemyManager.addEnemy(gr);
    }

    for (int b = 0; b < 3; b++) {
        MummyWarrior* m = new MummyWarrior();
        m->setPosition(plainsStart + 500 + b * (plainsW / 3.5f), spawnY);
        m->setPlayer(player); m->setPatrolRange(60.f);
        enemyManager.addEnemy(m);
    }

    for (int b = 0; b < 3; b++) {
        for (int j = 0; j < 2; j++) {
            FlyingTara* ft = new FlyingTara();
            float ftX = (b == 0) ? aerialStart + 500 + j * 300
                : (b == 1) ? aquaticStart + 300 + j * 300
                : plainsStart + 400 + j * 300;
            ft->setPosition(ftX, 150.f);
            ft->setPlayer(player);
            enemyManager.addEnemy(ft);
        }
    }
}

void Level2::update(float dt) {
    Level::update(dt);
    checkLevelComplete();
}

void Level2::render(RenderWindow& window, float camX, float camY) {
    if (aquatic) {
        aquatic->render(window, camX, camY);
    }
    if (aerial) {
        aerial->render(window, camX, camY);
    }
    if (plains) {
        plains->render(window, camX, camY);
    }
}

int Level2::getRebelBatchCount() { return rebelBatchCount; }
int Level2::getMummyBatchCount() { return mummyBatchCount; }
int Level2::getZombieCount() { return zombieBatchCountAquatic; }
int Level2::getEnemySubCount() { return enemySubCount; }
int Level2::getFlyingTaraCount() { return flyingTaraCount; }
int Level2::getBradleyCount() { return bradleyCount; }
int Level2::getPowPrisonerPlains() { return powPrisonerPlains; }
int Level2::getPowPrisonerOther() { return powPrisonerOther; }

// Level3 implementations
Level3::Level3() : SurvivalLevel("Level 3", 3, 83, 15)
{
    rebelBatchCount = 4;
    shieldedBatchCount = 4;
    bazookaBatchCount = 4;
    grenadierBatchCount = 4;

    paratrooperBatchCount = 3;
    martianBatchCount = 3;

    mummyBatchCount = 4;

    zombieBatchCountAquatic = 4;
    enemySubCount = 2;

    flyingTaraCount = 4;
    bradleyCount = 3;

    powPrisonerPlains = 3;
    powPrisonerOther = 2;

    totalEnemies =
        rebelBatchCount * 3 +
        shieldedBatchCount * 1 +
        bazookaBatchCount * 1 +
        grenadierBatchCount * 1 +
        paratrooperBatchCount * 2 +
        martianBatchCount * 1 +
        mummyBatchCount * 1 + flyingTaraCount * 2 +
        zombieBatchCountAquatic * 4 + enemySubCount * 1 +
        bradleyCount * 1;

    scoreMultiplier = 2.0f;

    playerSpawnX = 400;
    playerSpawnY = 0;
    generateBiomes();
}

Level3::~Level3() {}

void Level3::generateBiomes() {
    aquatic = new AquaticBiome(levelStart, plainsEnd);
    aerial = new AerialBiome(plainsEnd, aerialEnd);
    plains = new PlainsBiome(aerialEnd, aquaticEnd);
    loadTextures("Sprites/blocks/stone.png",
        "Sprites/blocks/water.png",
        "Sprites/blocks/grass.png",
        "Sprites/blocks/dirt.png");

    aquatic->generateTerrain(biomeWidth, biomeHeight);
    aerial->generateTerrain(biomeWidth, biomeHeight);
    plains->generateTerrain(biomeWidth, biomeHeight);

    isLoaded = true;
}

void Level3::spawnEnemies(EnemyManager& enemyManager, PlayerSoldier* player) {
    float spawnY = -200.0f;

    float aquaticStart = levelStart;
    float aquaticEnd_ = plainsEnd;
    float aerialStart = plainsEnd;
    float aerialEnd_ = aerialEnd;
    float plainsStart = aerialEnd;
    float plainsEnd_ = aquaticEnd;

    float aquaticW = aquaticEnd_ - aquaticStart;
    float aerialW = aerialEnd_ - aerialStart;
    float plainsW = plainsEnd_ - plainsStart;

    for (int b = 0; b < 3; b++) {
        float base = aquaticStart + 300 + b * (aquaticW / 3.5f);
        for (int j = 0; j < 3; j++) {
            RebelSoldier* r = new RebelSoldier();
            r->setPosition(base + j * 120, spawnY);
            r->setPlayer(player); r->setPatrolRange(100.f);
            enemyManager.addEnemy(r);
        }
        ShieldedSoldier* sh = new ShieldedSoldier();
        sh->setPosition(base + 400, spawnY);
        sh->setPlayer(player); sh->setPatrolRange(80.f);
        enemyManager.addEnemy(sh);

        BazookaSoldier* bz = new BazookaSoldier();
        bz->setPosition(base + 600, spawnY);
        bz->setPlayer(player); bz->setPatrolRange(80.f);
        enemyManager.addEnemy(bz);

        GrenadeSoldier* gr = new GrenadeSoldier();
        gr->setPosition(base + 800, spawnY);
        gr->setPlayer(player); gr->setPatrolRange(80.f);
        enemyManager.addEnemy(gr);
    }

    for (int b = 0; b < 4; b++) {
        for (int j = 0; j < 3; j++) {
            Zombie* z = new Zombie();
            z->setPosition(aquaticStart + 200 + b * (aquaticW / 4.5f) + j * 150, spawnY);
            z->setPlayer(player); z->setPatrolRange(80.f);
            enemyManager.addEnemy(z);
        }
    }

    for (int b = 0; b < 3; b++) {
        float base = aerialStart + 300 + b * (aerialW / 3.5f);
        for (int j = 0; j < 3; j++) {
            RebelSoldier* r = new RebelSoldier();
            r->setPosition(base + j * 120, spawnY);
            r->setPlayer(player); r->setPatrolRange(100.f);
            enemyManager.addEnemy(r);
        }
        ShieldedSoldier* sh = new ShieldedSoldier();
        sh->setPosition(base + 400, spawnY);
        sh->setPlayer(player); sh->setPatrolRange(80.f);
        enemyManager.addEnemy(sh);

        BazookaSoldier* bz = new BazookaSoldier();
        bz->setPosition(base + 600, spawnY);
        bz->setPlayer(player); bz->setPatrolRange(80.f);
        enemyManager.addEnemy(bz);

        GrenadeSoldier* gr = new GrenadeSoldier();
        gr->setPosition(base + 800, spawnY);
        gr->setPlayer(player); gr->setPatrolRange(80.f);
        enemyManager.addEnemy(gr);
    }

    for (int b = 0; b < 3; b++) {
        for (int j = 0; j < 2; j++) {
            Paratrooper* p = new Paratrooper();
            p->setPosition(aerialStart + 400 + b * (aerialW / 3.5f) + j * 200, -150.f);
            p->setPlayer(player);
            enemyManager.addEnemy(p);
        }
    }

    for (int b = 0; b < 3; b++) {
        Martian* mar = new Martian();
        mar->setPosition(aerialStart + 500 + b * (aerialW / 3.5f), 150.f);
        mar->setPlayer(player);
        enemyManager.addEnemy(mar);
    }

    for (int b = 0; b < 3; b++) {
        float base = plainsStart + 300 + b * (plainsW / 3.5f);
        for (int j = 0; j < 3; j++) {
            RebelSoldier* r = new RebelSoldier();
            r->setPosition(base + j * 120, spawnY);
            r->setPlayer(player); r->setPatrolRange(100.f);
            enemyManager.addEnemy(r);
        }
        ShieldedSoldier* sh = new ShieldedSoldier();
        sh->setPosition(base + 400, spawnY);
        sh->setPlayer(player); sh->setPatrolRange(80.f);
        enemyManager.addEnemy(sh);

        BazookaSoldier* bz = new BazookaSoldier();
        bz->setPosition(base + 600, spawnY);
        bz->setPlayer(player); bz->setPatrolRange(80.f);
        enemyManager.addEnemy(bz);

        GrenadeSoldier* gr = new GrenadeSoldier();
        gr->setPosition(base + 800, spawnY);
        gr->setPlayer(player); gr->setPatrolRange(80.f);
        enemyManager.addEnemy(gr);
    }

    for (int b = 0; b < 4; b++) {
        MummyWarrior* m = new MummyWarrior();
        m->setPosition(plainsStart + 500 + b * (plainsW / 4.5f), spawnY);
        m->setPlayer(player); m->setPatrolRange(60.f);
        enemyManager.addEnemy(m);
    }

    for (int b = 0; b < 4; b++) {
        for (int j = 0; j < 2; j++) {
            FlyingTara* ft = new FlyingTara();
            float ftX = (b == 0) ? aquaticStart + 400 + j * 300
                : (b == 1) ? aerialStart + 300 + j * 300
                : (b == 2) ? plainsStart + 300 + j * 300
                : aerialStart + 700 + j * 300;
            ft->setPosition(ftX, 150.f);
            ft->setPlayer(player);
            enemyManager.addEnemy(ft);
        }
    }
}

void Level3::update(float dt) {
    Level::update(dt);
    checkLevelComplete();
}

void Level3::render(RenderWindow& window, float camX, float camY) {
    if (aquatic) {
        aquatic->render(window, camX, camY);
    }
    if (aerial) {
        aerial->render(window, camX, camY);
    }
    if (plains) {
        plains->render(window, camX, camY);
    }
}

int Level3::getRebelBatchCount() { return rebelBatchCount; }
int Level3::getMummyBatchCount() { return mummyBatchCount; }
int Level3::getZombieCount() { return zombieBatchCountAquatic; }
int Level3::getEnemySubCount() { return enemySubCount; }
int Level3::getFlyingTaraCount() { return flyingTaraCount; }
int Level3::getBradleyCount() { return bradleyCount; }
int Level3::getPowPrisonerPlains() { return powPrisonerPlains; }
int Level3::getPowPrisonerOther() { return powPrisonerOther; }

// CampaignLevel implementations
CampaignLevel::CampaignLevel(int profileChoice) {
    isInfinite = true;
    generationChunkSize = 16;
    killQuotaPerType = 5;
    vehicleDestroyQuota = 3;
    fusionCooldownDuration = 180.0f;
    fusionCooldownTimer = 0.0f;
    spawnRadius = 500.0f;

    currentChunkX = 0;
    generatedLeft = false;
    generatedRight = false;
    currentGeneratedWidth = 0;
    maxGeneratedWidth = 1000000;

    isKillQuotaReached = false;
    isDynamicSpawning = true;

    enemiesKilledPerType = new int[8];
    vehiclesDestroyedPerType = new int[3];

    for (int i = 0; i < 8; i++)
        enemiesKilledPerType[i] = 0;

    for (int i = 0; i < 3; i++)
        vehiclesDestroyedPerType[i] = 0;

    NoiseProfileFactory factory;
    profile = factory.createProfile(profileChoice);
    perlin = new PerlinNoise(42);

    maxBiomes = 5000000;
    biomeCount = 0;
    activeBiomes = new Biome * [maxBiomes];

    spawnRecordCount = 0;
    lastSpawnCheckX = 0.f;
    for (int i = 0; i < MAX_SPAWN_RECORDS; i++) {
        spawnRecords[i].x = 0.f;
        spawnRecords[i].used = false;
    }
}

CampaignLevel::~CampaignLevel() {
    delete[] enemiesKilledPerType;
    delete[] vehiclesDestroyedPerType;

    for (int i = 0; i < biomeCount; i++) {
        delete activeBiomes[i];
        activeBiomes[i] = nullptr;
    }
    delete[] activeBiomes;

    delete perlin;
    delete profile;
}

bool CampaignLevel::checkCollision(float px, float py, float pw, float ph) {
    for (int i = 0; i < biomeCount; i++)
        if (activeBiomes[i]->checkCollision(px, py, pw, ph))
            return true;
    return false;
}

void CampaignLevel::resolveCollisions(float& px, float& py, float pw, float ph,
    float& velX, float& velY, bool& onGround) {
    for (int i = 0; i < biomeCount; i++)
        activeBiomes[i]->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
}

void CampaignLevel::generateChunk(int startCol) {
    float scale = profile->getScale();
    int octaves = profile->getOctaves();

    float noiseX = startCol * 0.03f;

    int biomeType = perlin->getBiome(noiseX, 0);

    float startX = startCol * 64.0f;
    float endX = (startCol + generationChunkSize) * 64.0f;

    Biome* b = nullptr;
    if (biomeType == BIOME_AERIAL) {
        b = new AerialBiome(startX, endX, true);
    }
    else if (biomeType == BIOME_AQUATIC) {
        b = new AquaticBiome(startX, endX, true);
    }
    else {
        b = new PlainsBiome(startX, endX, true);
    }

    b->loadTextures("Sprites/blocks/stone.png", "Sprites/blocks/water.png", "Sprites/blocks/grass.png", "Sprites/blocks/dirt.png");
    b->generateTerrain(generationChunkSize, 20);

    if (biomeCount < maxBiomes) {
        activeBiomes[biomeCount++] = b;
    }
}

void CampaignLevel::render(RenderWindow& window, float camX, float camY) {
    for (int i = 0; i < biomeCount; i++)
        activeBiomes[i]->render(window, camX, camY);
}

void CampaignLevel::update(float playerX) {
    int playerChunk = (int)(playerX / (generationChunkSize * 64.0f));
    if (playerChunk + 5 > currentChunkX) {
        generateChunk(currentChunkX * generationChunkSize);
        currentChunkX++;
        generatedRight = true;
    }
    if (playerChunk - 2 < 0) {
        generateChunk((currentChunkX - 1) * generationChunkSize);
        generatedLeft = true;
    }
}

bool CampaignLevel::checkWaterAt(float px, float py, float pw, float ph) {
    for (int i = 0; i < biomeCount; i++) {
        if (activeBiomes[i]->checkWaterCollision(px, py, pw, ph)) {
            return true;
        }
    }
    return false;
}

bool CampaignLevel::hasSpawnedNear(float x) {
    for (int i = 0; i < spawnRecordCount; i++) {
        if (!spawnRecords[i].used) continue;
        float diff = spawnRecords[i].x - x;
        if (diff < 0.f) diff = -diff;
        if (diff < 300.f) return true;
    }
    return false;
}

void CampaignLevel::recordSpawn(float x) {
    if (spawnRecordCount < MAX_SPAWN_RECORDS) {
        spawnRecords[spawnRecordCount].x = x;
        spawnRecords[spawnRecordCount].used = true;
        spawnRecordCount++;
    }
    else {
        static int overwriteIdx = 0;
        spawnRecords[overwriteIdx].x = x;
        spawnRecords[overwriteIdx].used = true;
        overwriteIdx = (overwriteIdx + 1) % MAX_SPAWN_RECORDS;
    }
}

void CampaignLevel::spawnWave(float playerX, EnemyManager& enemyManager, PlayerSoldier* player) {
    float baseX = playerX + 700.f;

    if (hasSpawnedNear(baseX)) return;

    recordSpawn(baseX);

    float noiseX = (baseX / 64.f) * 0.03f;
    int   biome = perlin->getBiome(noiseX, 0);

    float spawnY = -10.f;

    int   seed = (int)(baseX / 100.f);

    if (biome == BIOME_PLAINS) {
        for (int j = 0; j < 2; j++) {
            RebelSoldier* r = new RebelSoldier();
            r->setPosition(baseX + j * 120.f, spawnY);
            r->setPlayer(player);
            r->setPatrolRange(120.f);
            enemyManager.addEnemy(r);
        }

        GrenadeSoldier* g = new GrenadeSoldier();
        g->setPosition(baseX + 420.f, spawnY);
        g->setPlayer(player);
        g->setPatrolRange(80.f);
        enemyManager.addEnemy(g);

        if (seed % 3 == 0) {
            ShieldedSoldier* s = new ShieldedSoldier();
            s->setPosition(baseX + 600.f, spawnY);
            s->setPlayer(player);
            s->setPatrolRange(80.f);
            enemyManager.addEnemy(s);
        }

        if (seed % 3 == 0) {
            MummyWarrior* m = new MummyWarrior();
            m->setPosition(baseX + 780.f, spawnY);
            m->setPlayer(player);
            m->setPatrolRange(60.f);
            enemyManager.addEnemy(m);
        }
    }
    else if (biome == BIOME_AERIAL) {
        for (int j = 0; j < 2; j++) {
            Paratrooper* p = new Paratrooper();
            p->setPosition(baseX + j * 250.f, -150.f);
            p->setPlayer(player);
            enemyManager.addEnemy(p);
        }

        if (seed % 3 == 0) {
            Martian* mar = new Martian();
            mar->setPosition(baseX + 150.f, -50.f);
            mar->setPlayer(player);
            enemyManager.addEnemy(mar);
        }

        for (int j = 0; j < 2; j++) {
            RebelSoldier* r = new RebelSoldier();
            r->setPosition(baseX + 100.f + j * 150.f, spawnY);
            r->setPlayer(player);
            r->setPatrolRange(100.f);
            enemyManager.addEnemy(r);
        }

        FlyingTara* ft = new FlyingTara();
        ft->setPosition(baseX + 350.f, 150.f);
        ft->setPlayer(player);
        enemyManager.addEnemy(ft);
    }
    else {
        for (int j = 0; j < 2; j++) {
            Zombie* z = new Zombie();
            z->setPosition(baseX + j * 150.f, spawnY);
            z->setPlayer(player);
            z->setPatrolRange(80.f);
            enemyManager.addEnemy(z);
        }

        BazookaSoldier* bz = new BazookaSoldier();
        bz->setPosition(baseX + 520.f, spawnY);
        bz->setPlayer(player);
        bz->setPatrolRange(80.f);
        enemyManager.addEnemy(bz);

        if (seed % 3 == 0) {
            ShieldedSoldier* sh = new ShieldedSoldier();
            sh->setPosition(baseX + 700.f, spawnY);
            sh->setPlayer(player);
            sh->setPatrolRange(80.f);
            enemyManager.addEnemy(sh);
        }
    }
}

void CampaignLevel::recordEnemyKill(const char* name) {
    int idx = -1;
    if (name == "Rebel Soldier")  idx = 0;
    else if (name == "Shielded")       idx = 1;
    else if (name == "Bazooka")        idx = 2;
    else if (name == "Grenade")        idx = 3;
    else if (name == "Paratrooper")    idx = 4;
    else if (name == "Mummy")          idx = 5;
    else if (name == "Zombie")         idx = 6;
    else if (name == "Martian")        idx = 7;
    else if (name == "Grenade")        idx = 3;
    else if (name == "Paratrooper")    idx = 4;
    else if (name == "Mummy")          idx = 5;
    else if (name == "Zombie")         idx = 6;
    else if (name == "Martian")        idx = 7;

    if (idx >= 0) {
        enemiesKilledPerType[idx]++;
        checkKillQuota();
    }
}

void CampaignLevel::checkKillQuota() {
    for (int i = 0; i < 8; i++) {
        if (enemiesKilledPerType[i] < killQuotaPerType)
            return;
    }
    isKillQuotaReached = true;
}

bool CampaignLevel::getKillQuotaReached() { return isKillQuotaReached; }

int CampaignLevel::getKillCount(int typeIdx) {
    if (typeIdx >= 0 && typeIdx < 8)
        return enemiesKilledPerType[typeIdx];
    return 0;
}

// LevelManager implementations
LevelManager::LevelManager() {
    currentLevelIndex = 0;
    totalLevels = 0;
    for (int i = 0; i < 10; i++) {
        levels[i] = nullptr;
    }
}

LevelManager::~LevelManager() {
    for (int i = 0; i < totalLevels; i++) {
        levels[i] = nullptr;
    }
}

void LevelManager::addLevel(Level* level) {
    if (totalLevels < 10) {
        levels[totalLevels] = level;
        totalLevels++;
    }
}

Level* LevelManager::getCurrentLevel() {
    if (currentLevelIndex >= 0 && currentLevelIndex < totalLevels) {
        return levels[currentLevelIndex];
    }
    return nullptr;
}

Level* LevelManager::getLevel(int index) {
    if (index >= 0 && index < totalLevels) {
        return levels[index];
    }
    return nullptr;
}

void LevelManager::setCurrentLevel(int index) {
    if (index >= 0 && index < totalLevels) {
        currentLevelIndex = index;
    }
}

int LevelManager::getCurrentLevelIndex() {
    return currentLevelIndex;
}

int LevelManager::getTotalLevels() {
    return totalLevels;
}

void LevelManager::nextLevel() {
    if (currentLevelIndex < totalLevels - 1) {
        currentLevelIndex++;
    }
}

void LevelManager::previousLevel() {
    if (currentLevelIndex > 0) {
        currentLevelIndex--;
    }
}

void LevelManager::setCurrentLevelByPointer(Level* level) {
    for (int i = 0; i < totalLevels; i++) {
        if (levels[i] == level) {
            currentLevelIndex = i;
            return;
        }
    }
}

void LevelManager::spawnCurrentLevelEnemies(EnemyManager& enemyManager, PlayerSoldier* player) {
    Level* level = getCurrentLevel();
    if (level) {
        level->spawnEnemies(enemyManager, player);
    }
}

void LevelManager::loadAllLevels() {
    levels[0] = new Level1();
    levels[1] = new Level2();
    levels[2] = new Level3();
    totalLevels = 3;
    currentLevelIndex = 0;
}

void LevelManager::switchToLevel(int index) {
    if (index >= 0 && index < totalLevels) {
        currentLevelIndex = index;
    }
}

void LevelManager::update(float dt) {
    Level* level = getCurrentLevel();
    if (level) {
        level->update(dt);
    }
}
