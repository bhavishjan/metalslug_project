#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdio>
#include <cstring>
#include "Weapon.h"
#include "Enemy.h"
#include "GameMode.h"
#include "Level.h"
#include "BossLevel.h"
#include "Biome.h"
#include "PlayerSoldier.h"
#include "Menu.h"
#include "GameState.h"
#include "Camera.h"
#include "ScoreSystem.h"

using namespace sf;

class Game {
private:
    RenderWindow window;
    int screenX;
    int screenY;

    int gameMode;
    int selectedCharacter;

    SurvivalGame* survivalGame;
    CampaignGame* campaignGame;

    CharacterManager characters;
    EnemyManager     enemies;
    LevelManager     levelManager;

    bool  jumpHeld;
    bool  showLevelTitle;
    Clock levelTitleTimer;
    int   currentLevelNumber;

    BulletManager bulletManager;
    Clock clock;
    Clock Delay;
    Clock pauseDelay;
    Clock playerFireTimer;
    Clock playerDiedTimer;
    bool  showPlayerDiedMessage;
    float playerFireCooldown = 0.2f;

    ScoreSystem scoreSystem;
    Camera      camera;
    Menu        startMenu;
    Menu        pauseMenu;

    bool  inBossLevel = false;
    float lastCampaignSpawnX = 0.f;

    // Level clear score sirf ek baar add ho
    bool survivalClearAdded = false;
    bool campaignClearAdded = false;

public:
    Game();
    ~Game();

    void run();
    void handleInput();
    void update(float dt);
    void render();
    void playerFire();
    void cleanup();

private:
    void startSurvivalMode();
    void startCampaignMode();
    void handleMenuEscape();
    void advanceToNextLevel();
    void updateSurvival(float dt);
    void updateBossLevel(float dt, Level* currentLevel);
    void updateCampaign(float dt);
    void updatePlayerPhysics(Level* level, float& pX, float& pY, float& pVelocityX, float& pVelocityY);
    void updatePlayerPhysicsCampaign(CampaignLevel* level, float& pX, float& pY, float& pVelocityX, float& pVelocityY);
    void updateEnemyPhysics(Level* currentLevel, float dt);
    void checkBulletEnemyCollisions();
    void checkBulletPlayerCollisions();
    void renderPlayerDiedOverlay();
    void renderLevelTitle();
    void renderPauseOverlay();
};