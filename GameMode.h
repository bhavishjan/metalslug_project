#pragma once
#include <SFML/Graphics.hpp>
#include "Level.h"
#include "Biome.h"
using namespace sf;

// Forward declarations
class ScoreManager;
class SoundManager;
class Camera;
class CharacterManager;


class GameMode {
protected:
    const char* name;
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
    GameMode(const char* n);
    virtual ~GameMode();

    // pure virtual
    virtual void start() = 0;
    virtual void update(float dt) = 0;
    virtual void render(RenderWindow& window) = 0;
    virtual bool checkGameOver() = 0;

    // Common implementations
    virtual void pause();
    virtual void resume();
    virtual void end();
    virtual void reset();

    void setScoreManager(ScoreManager* s);
    void setSoundManager(SoundManager* s);
    void setCharManager(CharacterManager* c);
    void setCamera(Camera* c);

    const char* getName();
    bool        getIsActive();
    bool        getIsPaused();
    int         getScore();
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

    // Physics and game state
    float  cameraX;
    float  cameraY;
    int    screenX;
    int    screenY;

public:
    SurvivalMode(int sx, int sy);
    virtual ~SurvivalMode();

    // Child class
    virtual void loadAllLevels() = 0;

    void start() override;
    void update(float dt) override;
    void setCurrentLevel(Level* level);
    void update(float dt, CharacterManager* characters);
    void render(RenderWindow& window) override;
    bool checkGameOver() override;
    void switchToLevel(int index);
    void checkLevelComplete();
    void nextLevel();

    bool canUseFusion();
    void markFusionUsed();
    void onPlayerDamaged();
    void onEnemyKilled();

    Level* getCurrentLevel();
    int    getCurrentLevelIdx();
    bool   getIsBossLevel();
    bool   getIsFlawless();
    float  getScoreMultiplier();

    void setCamera(float x, float y);
    float getCameraX();
    float getCameraY();
};


// Concrete implementation for Survival Mode
class SurvivalGame : public SurvivalMode {
private:
    Texture backgroundTexture;
    Sprite backgroundSprite;

public:
    SurvivalGame(int sx, int sy);
    void loadAllLevels() override;
    void render(RenderWindow& window) override;
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

    // Physics and game state
    float  cameraX;
    float  cameraY;
    int    screenX;
    int    screenY;

    Texture backgroundTexture;
    Sprite backgroundSprite;

public:
    CampaignMode(int sx, int sy);
    ~CampaignMode();

    void start() override;
    void update(float dt) override;
    void update(float dt, CharacterManager* characters);
    void render(RenderWindow& window) override;
    bool checkGameOver() override;

    void checkKillQuota();
    bool checkVehicleQuota();
    bool getKillQuotaReached() const;

    void onEnemyKilled(int typeIndex);
    void onVehicleDestroyed(int typeIndex);

    bool canUseFusion();
    void fusionActivated();

    void setNoiseProfile(int choice);

    CampaignLevel* getCampaignLevel();
    bool           isKillQuotaDone();
    float          getFusionCooldown();

    void setCamera(float x, float y);
    float getCameraX();
    float getCameraY();
};


// Concrete implementation for Campaign Mode
class CampaignGame : public CampaignMode {
public:
    CampaignGame(int sx, int sy);
};