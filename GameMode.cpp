#define _CRT_SECURE_NO_WARNINGS
#include "GameMode.h"

// GameMode implementations
GameMode::GameMode(const char* n) {
    name = n;
    isActive = false;
    isPaused = false;
    score = 0;
    highScore = 0;
    gameTimer = 0.0f;
    sco = nullptr;
    sou = nullptr;
    chM = nullptr;
    camera = nullptr;
}

GameMode::~GameMode() {}

void GameMode::pause() { isPaused = true; }
void GameMode::resume() { isPaused = false; }
void GameMode::end() { isActive = false; }
void GameMode::reset() { score = 0; gameTimer = 0.0f; isActive = false; }

void GameMode::setScoreManager(ScoreManager* s) { sco = s; }
void GameMode::setSoundManager(SoundManager* s) { sou = s; }
void GameMode::setCharManager(CharacterManager* c) { chM = c; }
void GameMode::setCamera(Camera* c) { camera = c; }

const char* GameMode::getName() { return name; }
bool GameMode::getIsActive() { return isActive; }
bool GameMode::getIsPaused() { return isPaused; }
int GameMode::getScore() { return score; }

// SurvivalMode implementations
SurvivalMode::SurvivalMode(int sx, int sy) : GameMode("Survival Mode") {
    totalLevels = 4;
    currentLevelIndex = 0;
    currentLevel = nullptr;
    isBossLevel = false;
    fusionUsedThisLevel = false;
    isFlawlessVictory = true;
    totalEnemiesKilled = 0;
    scoreMultiplier = 1.0f;
    cameraX = 0;
    cameraY = 0;
    screenX = sx;
    screenY = sy;
    for (int i = 0; i < 4; i++) {
        levels[i] = nullptr;
    }
}

SurvivalMode::~SurvivalMode() {
    for (int i = 0; i < 4; i++) {
        delete levels[i];
        levels[i] = nullptr;
    }
    currentLevel = nullptr;
}

void SurvivalMode::start() {
    isActive = true;
    isPaused = false;
    currentLevelIndex = 0;
    isFlawlessVictory = true;
    fusionUsedThisLevel = false;
    cameraX = 0;
    cameraY = 0;
    loadAllLevels();
    switchToLevel(0);
}

void SurvivalMode::update(float dt) {
    if (!isActive || isPaused) {
        return;
    }
    gameTimer += dt;
    if (currentLevel) {
        currentLevel->update(dt);
    }
    checkLevelComplete();
}

void SurvivalMode::setCurrentLevel(Level* level) {
    currentLevel = level;
}

void SurvivalMode::update(float dt, CharacterManager* characters) {
    if (!isActive || isPaused) {
        return;
    }
    gameTimer += dt;
    if (currentLevel) {
        currentLevel->update(dt);
    }
    checkLevelComplete();
}

void SurvivalMode::render(RenderWindow& window) {
    if (currentLevel) {
        currentLevel->render(window, cameraX, cameraY);
    }
}

bool SurvivalMode::checkGameOver() {
    return false;
}

void SurvivalMode::switchToLevel(int index) {
    if (index < 0 || index >= totalLevels) {
        return;
    }
    currentLevelIndex = index;
    currentLevel = levels[index];
    isBossLevel = (index == 3);
    fusionUsedThisLevel = false;
    if (currentLevel) {
        //currentLevel->generateBiomes();
        // currentLevel->spawnEnemies(); - commented out, requires EnemyManager and PlayerSoldier*
    }
}

void SurvivalMode::checkLevelComplete() {
    if (!currentLevel) {
        return;
    }
    if (currentLevel->checkLevelComplete()) {
        nextLevel();
    }
}

void SurvivalMode::nextLevel() {
    int next = currentLevelIndex + 1;
    if (next >= totalLevels) {
        end();
        return;
    }
    switchToLevel(next);
}

bool SurvivalMode::canUseFusion() { return !fusionUsedThisLevel; }
void SurvivalMode::markFusionUsed() { fusionUsedThisLevel = true; }
void SurvivalMode::onPlayerDamaged() { isFlawlessVictory = false; }
void SurvivalMode::onEnemyKilled() { totalEnemiesKilled++; }

Level* SurvivalMode::getCurrentLevel() { return currentLevel; }
int SurvivalMode::getCurrentLevelIdx() { return currentLevelIndex; }
bool SurvivalMode::getIsBossLevel() { return isBossLevel; }
bool SurvivalMode::getIsFlawless() { return isFlawlessVictory; }
float SurvivalMode::getScoreMultiplier() { return scoreMultiplier; }

void SurvivalMode::setCamera(float x, float y) { cameraX = x; cameraY = y; }
float SurvivalMode::getCameraX() { return cameraX; }
float SurvivalMode::getCameraY() { return cameraY; }

// SurvivalGame implementations
SurvivalGame::SurvivalGame(int sx, int sy) : SurvivalMode(sx, sy) {
    if (backgroundTexture.loadFromFile("Sprites/background.png")) {
        backgroundSprite.setTexture(backgroundTexture);
        Vector2u sz = backgroundTexture.getSize();
        backgroundSprite.setScale((float)screenX / sz.x, (float)screenY / sz.y);
    }
}

void SurvivalGame::loadAllLevels() {
    levels[0] = new Level1();
    levels[1] = new Level2();
    levels[2] = new Level3();
    levels[3] = nullptr; // Boss level to be implemented
}

void SurvivalGame::render(RenderWindow& window) {
    backgroundSprite.setPosition(0, 0);
    window.draw(backgroundSprite);
    if (currentLevel) {
        currentLevel->render(window, cameraX, cameraY);
    }
}

// CampaignMode implementations
CampaignMode::CampaignMode(int sx, int sy) : GameMode("Campaign Mode") {
    campaignLevel = nullptr;
    killQuotaPerType = 5;
    vehicleDestroyQuota = 3;
    isKillQuotaReached = false;
    fusionCooldownTimer = 0.0f;
    fusionCooldownDuration = 180.0f;
    selectedNoiseProfile = 3;
    cameraX = 0;
    cameraY = 0;
    screenX = sx;
    screenY = sy;

    for (int i = 0; i < 8; i++) {
        enemiesKilledPerType[i] = 0;
    }
    for (int i = 0; i < 3; i++) {
        vehiclesDestroyedPerType[i] = 0;
    }

    if (backgroundTexture.loadFromFile("Sprites/background.png")) {
        backgroundSprite.setTexture(backgroundTexture);
        Vector2u sz = backgroundTexture.getSize();
        backgroundSprite.setScale((float)screenX / sz.x, (float)screenY / sz.y);
    }
}

CampaignMode::~CampaignMode() {
    delete campaignLevel;
    campaignLevel = nullptr;
}

void CampaignMode::start() {
    isActive = true;
    isPaused = false;
    campaignLevel = new CampaignLevel(selectedNoiseProfile);
    cameraX = 0;
    cameraY = 0;
}

void CampaignMode::update(float dt) {
    if (!isActive || isPaused) {
        return;
    }
    gameTimer += dt;

    if (fusionCooldownTimer > 0.0f) {
        fusionCooldownTimer -= dt;
        if (fusionCooldownTimer < 0.0f) {
            fusionCooldownTimer = 0.0f;
        }
    }

    if (campaignLevel) {
        campaignLevel->update(0);
    }
    checkKillQuota();
}

void CampaignMode::update(float dt, CharacterManager* characters) {
    if (!isActive || isPaused) {
        return;
    }
    gameTimer += dt;

    if (fusionCooldownTimer > 0.0f) {
        fusionCooldownTimer -= dt;
        if (fusionCooldownTimer < 0.0f) {
            fusionCooldownTimer = 0.0f;
        }
    }

    if (campaignLevel) {
        campaignLevel->update(characters->getActivePlayer()->getPlayerX());
    }
    checkKillQuota();
}

void CampaignMode::render(RenderWindow& window) {
    backgroundSprite.setPosition(0, 0);
    window.draw(backgroundSprite);
    if (campaignLevel) {
        campaignLevel->render(window, cameraX, cameraY);
    }
}

bool CampaignMode::checkGameOver() { return false; }

void CampaignMode::checkKillQuota() {
    for (int i = 0; i < 8; i++) {
        if (enemiesKilledPerType[i] < killQuotaPerType) {
            return;
        }
    }
    isKillQuotaReached = true;
}

bool CampaignMode::checkVehicleQuota() {
    for (int i = 0; i < 3; i++) {
        if (vehiclesDestroyedPerType[i] < vehicleDestroyQuota) {
            return false;
        }
    }
    return true;
}

bool CampaignMode::getKillQuotaReached() const { return isKillQuotaReached; }

void CampaignMode::onEnemyKilled(int typeIndex) {
    if (typeIndex >= 0 && typeIndex < 8) {
        enemiesKilledPerType[typeIndex]++;
    }
    checkKillQuota();
}

void CampaignMode::onVehicleDestroyed(int typeIndex) {
    if (typeIndex >= 0 && typeIndex < 3) {
        vehiclesDestroyedPerType[typeIndex]++;
    }
}

bool CampaignMode::canUseFusion() { return fusionCooldownTimer <= 0.0f; }
void CampaignMode::fusionActivated() { fusionCooldownTimer = fusionCooldownDuration; }

void CampaignMode::setNoiseProfile(int choice) { selectedNoiseProfile = choice; }

CampaignLevel* CampaignMode::getCampaignLevel() { return campaignLevel; }
bool CampaignMode::isKillQuotaDone() { return isKillQuotaReached; }
float CampaignMode::getFusionCooldown() { return fusionCooldownTimer; }

void CampaignMode::setCamera(float x, float y) { cameraX = x; cameraY = y; }
float CampaignMode::getCameraX() { return cameraX; }
float CampaignMode::getCameraY() { return cameraY; }

// CampaignGame implementations
CampaignGame::CampaignGame(int sx, int sy) : CampaignMode(sx, sy) {}
