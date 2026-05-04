#pragma once
#include <SFML/Graphics.hpp>
#include <cstring>
#include "Header.h"
using namespace sf;

// Forward declarations
class ScoreManager;
class SoundManager;
class CharacterManager;
class Camera;


class GameMode {
protected:
    char   name[32];
    bool   isActive;
    bool   isPaused;
    int    score;
    int    highScore;
    float  gameTimer;

    ScoreManager* sco;
    SoundManager* sou;
    CharacterManager* chM;
    Camera* camera;

public:
    GameMode(const char* n) {
        strncpy(name, n, 31);
        name[31] = '\0';
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

    virtual ~GameMode() {}

    // pure virtual
    virtual void start() = 0;
    virtual void update(float dt) = 0;
    virtual void render(RenderWindow& window) = 0;
    virtual bool checkGameOver() = 0;

    // Common implementations
    virtual void pause() { isPaused = true; }
    virtual void resume() { isPaused = false; }
    virtual void end() { isActive = false; }
    virtual void reset() { score = 0; gameTimer = 0.0f; isActive = false; }

    void setScoreManager(ScoreManager* s) { sco = s; }
    void setSoundManager(SoundManager* s) { sou = s; }
    void setCharManager(CharacterManager* c) { chM = c; }
    void setCamera(Camera* c) { camera = c; }

    const char* getName() { return name; }
    bool        getIsActive() { return isActive; }
    bool        getIsPaused() { return isPaused; }
    int         getScore() { return score; }
};


//survival
class SurvivalMode : public GameMode {
protected:
    Level* levels[4];
    int    totalLevels;//4
    int    currentLevelIndex;
    Level* currentLevel;

    bool   isBossLevel;
    bool   fusionUsedThisLevel;
    bool   isFlawlessVictory;
    int    totalEnemiesKilled;
    float  scoreMultiplier;

public:
    SurvivalMode() : GameMode("Survival Mode") {
        totalLevels = 4;
        currentLevelIndex = 0;
        currentLevel = nullptr;
        isBossLevel = false;
        fusionUsedThisLevel = false;
        isFlawlessVictory = true;
        totalEnemiesKilled = 0;
        scoreMultiplier = 1.0f;
        for (int i = 0; i < 4; i++) levels[i] = nullptr;
    }

    virtual ~SurvivalMode() {
        for (int i = 0; i < 4; i++) {
            delete levels[i];
            levels[i] = nullptr;
        }
        currentLevel = nullptr;
    }

    // Child class
    virtual void loadAllLevels() = 0;

    void start() override {
        isActive = true;
        isPaused = false;
        currentLevelIndex = 0;
        isFlawlessVictory = true;
        fusionUsedThisLevel = false;
        loadAllLevels();
        switchToLevel(0);
    }

    void update(float dt) override {
        if (!isActive || isPaused) return;
        gameTimer += dt;
        if (currentLevel) currentLevel->update(dt);
        checkLevelComplete();
    }

    void render(RenderWindow& window) override {
        if (currentLevel) currentLevel->render(window, 0, 0);
    }

    bool checkGameOver() override { return false; }

    void switchToLevel(int index) {
        if (index < 0 || index >= totalLevels) return;
        currentLevelIndex = index;
        currentLevel = levels[index];
        isBossLevel = (index == 3);
        fusionUsedThisLevel = false;
        if (currentLevel) {
            currentLevel->generateBiomes();
            currentLevel->spawnEnemies();
        }
    }

    void checkLevelComplete() {
        if (!currentLevel) return;
        if (currentLevel->checkLevelComplete()) nextLevel();
    }

    void nextLevel() {
        int next = currentLevelIndex + 1;
        if (next >= totalLevels) { end(); return; }
        switchToLevel(next);
    }

    bool canUseFusion() { return !fusionUsedThisLevel; }
    void markFusionUsed() { fusionUsedThisLevel = true; }
    void onPlayerDamaged() { isFlawlessVictory = false; }
    void onEnemyKilled() { totalEnemiesKilled++; }

    Level* getCurrentLevel() { return currentLevel; }
    int    getCurrentLevelIdx() { return currentLevelIndex; }
    bool   getIsBossLevel() { return isBossLevel; }
    bool   getIsFlawless() { return isFlawlessVictory; }
    float  getScoreMultiplier() { return scoreMultiplier; }
};


// ================================================================
//  CONCRETE — CampaignMode  (child of GameMode)
//  Uses CampaignLevel from Level.h.
//  Infinite world, dynamic spawning, kill quota, 3-min fusion CD.
// ================================================================
class CampaignMode : public GameMode {
private:
    CampaignLevel* campaignLevel;

    int   killQuotaPerType;
    int   vehicleDestroyQuota;
    int   enemiesKilledPerType[8];
    int   vehiclesDestroyedPerType[3];
    bool  isKillQuotaReached;

    float fusionCooldownTimer;
    float fusionCooldownDuration;  // 180 seconds

    int   selectedNoiseProfile;    // 1=Amplified 2=Flat 3=Normal

public:
    CampaignMode() : GameMode("Campaign Mode") {
        campaignLevel = nullptr;
        killQuotaPerType = 5;
        vehicleDestroyQuota = 3;
        isKillQuotaReached = false;
        fusionCooldownTimer = 0.0f;
        fusionCooldownDuration = 180.0f;
        selectedNoiseProfile = 3;

        for (int i = 0; i < 8; i++) enemiesKilledPerType[i] = 0;
        for (int i = 0; i < 3; i++) vehiclesDestroyedPerType[i] = 0;
    }

    ~CampaignMode() {
        delete campaignLevel;
        campaignLevel = nullptr;
    }

    void start() override {
        isActive = true;
        isPaused = false;
        campaignLevel = new CampaignLevel(selectedNoiseProfile);
    }

    void update(float dt) override {
        if (!isActive || isPaused) return;
        gameTimer += dt;

        if (fusionCooldownTimer > 0.0f) {
            fusionCooldownTimer -= dt;
            if (fusionCooldownTimer < 0.0f) fusionCooldownTimer = 0.0f;
        }

        if (campaignLevel) campaignLevel->update(0);  // player X passed at runtime
        checkKillQuota();
    }

    void render(RenderWindow& window) override {
        if (campaignLevel) campaignLevel->render(window, 0, 0);
    }

    bool checkGameOver() override { return false; }

    void checkKillQuota() {
        for (int i = 0; i < 8; i++)
            if (enemiesKilledPerType[i] < killQuotaPerType) return;
        isKillQuotaReached = true;
    }

    bool checkVehicleQuota() {
        for (int i = 0; i < 3; i++)
            if (vehiclesDestroyedPerType[i] < vehicleDestroyQuota) return false;
        return true;
    }

    void onEnemyKilled(int typeIndex) {
        if (typeIndex >= 0 && typeIndex < 8)
            enemiesKilledPerType[typeIndex]++;
        checkKillQuota();
    }

    void onVehicleDestroyed(int typeIndex) {
        if (typeIndex >= 0 && typeIndex < 3)
            vehiclesDestroyedPerType[typeIndex]++;
    }

    bool canUseFusion() { return fusionCooldownTimer <= 0.0f; }
    void fusionActivated() { fusionCooldownTimer = fusionCooldownDuration; }

    void setNoiseProfile(int choice) { selectedNoiseProfile = choice; }

    CampaignLevel* getCampaignLevel() { return campaignLevel; }
    bool           isKillQuotaDone() { return isKillQuotaReached; }
    float          getFusionCooldown() { return fusionCooldownTimer; }
};