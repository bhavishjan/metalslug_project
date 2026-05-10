#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Weapon.h"
#include "Enemy.h"
#include "GameMode.h"
#include "Level.h"
#include "Biome.h"
#include "PlayerSoldier.h"
#include "Menu.h"
#include "GameState.h"
#include "Camera.h"
#include "ScoreSystem.h"

using namespace sf;
using namespace std;

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

    //BossLevel* bossLevel = nullptr;   // boss level ka alag pointer
    bool inBossLevel = false;          // track karne ke liye
    float lastCampaignSpawnX = 0.f;   // campaign spawning track karne ke liye

public:
    Game()
        : screenX(1600),
        screenY(900),
        gameMode(0),
        selectedCharacter(0),
        startMenu(false),
        pauseMenu(true),
        survivalGame(nullptr),
        campaignGame(nullptr),
        jumpHeld(false),
        showLevelTitle(false),
        currentLevelNumber(0),
        showPlayerDiedMessage(false),
        camera(screenX, screenY)
    {
        window.create(VideoMode(screenX, screenY), "Metal Slug", Style::Close);

        Delay.restart();
        pauseDelay.restart();
        playerDiedTimer.restart();

        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(60);

        enemies.setBulletManager(&bulletManager);
        scoreSystem.loadFont("arial.TTF");
    }

    ~Game() {
        cleanup();
    }

    // =========================================================
    //  MAIN LOOP
    // =========================================================
    void run() {
        while (window.isOpen()) {
            float dt = clock.restart().asSeconds();
            handleInput();
            update(dt);
            render();
        }
    }

    // =========================================================
    //  INPUT
    // =========================================================
    void handleInput() {
        Event ev;
        while (window.pollEvent(ev)) {

            if (ev.type == Event::Closed) {
                window.close();
                return;
            }

            // ---- Menu navigation (only in gameMode 0) ----
            if (gameMode == 0 && ev.type == Event::KeyPressed) {

                if (ev.key.code == Keyboard::Up) {
                    startMenu.moveSelectionUp();
                }
                else if (ev.key.code == Keyboard::Down) {
                    startMenu.moveSelectionDown();
                }
                else if (ev.key.code == Keyboard::Enter) {
                    int menuState = startMenu.getMenuState();

                    // Main screen -> character select
                    if (menuState == 0) {
                        startMenu.setMenuState(1);
                    }
                    // Character select -> mode select
                    else if (menuState == 1) {
                        selectedCharacter = startMenu.getSelectionIndex();
                        characters.switchCharacterToIndex(selectedCharacter);
                        startMenu.setMenuState(2);
                    }
                    // Mode select -> start game
                    else if (menuState == 2) {
                        int modeSelection = startMenu.getSelectionIndex();

                        // ---- SURVIVAL ----
                        if (modeSelection == 0) {
                            startSurvivalMode();
                        }
                        // ---- CAMPAIGN ----
                        else if (modeSelection == 1) {
                            startCampaignMode();
                        }
                    }
                }

                // Escape inside menu
                else if (ev.key.code == Keyboard::Escape) {
                    handleMenuEscape();
                }
            }

            // ---- Escape during gameplay ----
            if (gameMode != 0 && ev.type == Event::KeyPressed
                && ev.key.code == Keyboard::Escape
                && pauseDelay.getElapsedTime().asSeconds() > 0.3f)
            {
                if (pauseMenu.getPauseMenuVisible()) {
                    pauseMenu.handlePauseInput();

                    if (!pauseMenu.getPauseMenuVisible()) {
                        int sel = pauseMenu.getPauseSelection();
                        if (sel == 1) {          // Quit
                            cleanup();
                            window.close();
                            return;
                        }
                        // sel == 0  ->  Resume (menu already hidden)
                    }
                }
                else {
                    pauseMenu.showPauseMenu();
                }
                pauseDelay.restart();
            }
        } // end pollEvent loop
    }

    // =========================================================
    //  UPDATE
    // =========================================================
    void update(float dt) {

        if (gameMode == 0) {
            startMenu.updateAnimation(dt);
            return;
        }

        if (pauseMenu.getPauseMenuVisible()) {
            // Allow pause-menu navigation while paused
            pauseMenu.handlePauseInput();
            if (!pauseMenu.getPauseMenuVisible()) {
                int sel = pauseMenu.getPauseSelection();
                if (sel == 1) {
                    cleanup();
                    window.close();
                }
            }
            return;
        }

        // ---- Player died? ----
        if (!showPlayerDiedMessage
            && characters.getActivePlayer()
            && characters.getActivePlayer()->getHP() <= 0)
        {
            showPlayerDiedMessage = true;
            playerDiedTimer.restart();
        }

        if (showPlayerDiedMessage) {
            if (playerDiedTimer.getElapsedTime().asSeconds() > 3.0f) {
                showPlayerDiedMessage = false;
                cleanup();
                gameMode = 0;
                startMenu.setMenuState(2);
                if (characters.getActivePlayer())
                    characters.getActivePlayer()->forceRespawn();
            }
            return;
        }

        // ---- Common player update ----
        characters.getActivePlayer()->update(dt);

        // Character switch
        if (Keyboard::isKeyPressed(Keyboard::Z)
            && Delay.getElapsedTime().asSeconds() > 0.2f)
        {
            characters.switchCharacter();
            Delay.restart();
        }

        // Fire
        if (Keyboard::isKeyPressed(Keyboard::F)
            && playerFireTimer.getElapsedTime().asSeconds() > playerFireCooldown)
        {
            playerFire();
            playerFireTimer.restart();
        }

        // Horizontal movement
        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            if (characters.getActivePlayer()->isFacingRight()) {
                characters.getActivePlayer()->setVelocityX(0);
                characters.getActivePlayer()->flipToLeft();
            }
            characters.getActivePlayer()->moveLeft();
        }
        else if (Keyboard::isKeyPressed(Keyboard::Right)) {
            if (!characters.getActivePlayer()->isFacingRight()) {
                characters.getActivePlayer()->setVelocityX(0);
                characters.getActivePlayer()->flipToRight();
            }
            characters.getActivePlayer()->moveRight();
        }
        else {
            // Friction
            if (characters.getActivePlayer()->getIsGrounded()) {
                characters.getActivePlayer()->setVelocityX(
                    characters.getActivePlayer()->getVelocityX()
                    * characters.getActivePlayer()->getFriction());
            }
            else {
                characters.getActivePlayer()->setVelocityX(
                    characters.getActivePlayer()->getVelocityX()
                    * characters.getActivePlayer()->getAirFriction());
            }
        }

        // Velocity cap
        float velCap = (gameMode == 2) ? 14.f : 6.f;
        float vx = characters.getActivePlayer()->getVelocityX();
        if (vx > velCap) characters.getActivePlayer()->setVelocityX(velCap);
        if (vx < -velCap) characters.getActivePlayer()->setVelocityX(-velCap);

        // Jump
        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            if (characters.getActivePlayer()->getIsGrounded()) {
                characters.getActivePlayer()->setVelocityY(
                    characters.getActivePlayer()->getJumpPower());
                characters.getActivePlayer()->setGrounded(false);
                jumpHeld = true;
            }
        }
        else if (Keyboard::isKeyPressed(Keyboard::J)) {
            if (characters.getActivePlayer()->getIsGrounded()) {
                characters.getActivePlayer()->setVelocityY(-40.0f);
                characters.getActivePlayer()->setGrounded(false);
                jumpHeld = true;
            }
        }
        else {
            jumpHeld = false;
        }

        // Variable jump height
        if (!jumpHeld && characters.getActivePlayer()->getVelocityY() < 0) {
            characters.getActivePlayer()->setVelocityY(
                characters.getActivePlayer()->getVelocityY() * 0.5f);
        }

        // Gravity
        characters.getActivePlayer()->setVelocityY(
            characters.getActivePlayer()->getVelocityY()
            + characters.getActivePlayer()->getGravity());

        if (characters.getActivePlayer()->getVelocityY()
        > characters.getActivePlayer()->getMaxFallSpeed())
        {
            characters.getActivePlayer()->setVelocityY(
                characters.getActivePlayer()->getMaxFallSpeed());
        }

        // ---- Mode-specific update ----
        if (gameMode == 1 && survivalGame) {
            updateSurvival(dt);
        }
        else if (gameMode == 2 && campaignGame) {
            updateCampaign(dt);
        }
    }

    // =========================================================
    //  RENDER
    // =========================================================
    void render() {
        window.clear(Color(135, 206, 235));

        // ---- Menu ----
        if (gameMode == 0) {
            int menuState = startMenu.getMenuState();
            if (menuState == 0) startMenu.renderStartScreen(window);
            else if (menuState == 1) startMenu.renderCharacterSelection(window);
            else if (menuState == 2) startMenu.renderModeSelection(window);
            window.display();
            return;
        }

        // ---- Survival ----
        if (gameMode == 1 && survivalGame) {
            survivalGame->render(window);
            enemies.renderAll(window, camera.getX(), camera.getY());
            bulletManager.render(window, camera.getX(), camera.getY());
            characters.getActivePlayer()->render(window, camera.getX(), camera.getY());

            scoreSystem.render(window, screenX, screenY);
            renderPlayerDiedOverlay();
            renderLevelTitle();

            if (pauseMenu.getPauseMenuVisible()) {
                renderPauseOverlay();
                window.display();
                return;
            }
            window.display();
            return;
        }

        // ---- Campaign ----
        if (gameMode == 2 && campaignGame) {
            campaignGame->render(window);
            enemies.renderAll(window, camera.getX(), camera.getY());
            bulletManager.render(window, camera.getX(), camera.getY());
            characters.getActivePlayer()->render(window, camera.getX(), camera.getY());

            scoreSystem.render(window, screenX, screenY);
            renderPlayerDiedOverlay();

            if (pauseMenu.getPauseMenuVisible()) {
                renderPauseOverlay();
            }
            window.display();
            return;
        }

        window.display();
    }

    // =========================================================
    //  PLAYER FIRE
    // =========================================================
    void playerFire() {
        PlayerSoldier* player = characters.getActivePlayer();
        if (!player) return;

        float px = player->getPlayerX();
        float py = player->getPlayerY();
        bool  facingRight = player->isFacingRight();

        float bulletX = facingRight ? px + 30.f : px - 30.f;
        float bulletY = py + 10.f;
        float angle = facingRight ? 0.f : 3.14159f;

        bulletManager.spawnBullet(
            bulletX, bulletY, angle,
            10, PLAYER, 800.f, 600.f, sf::Color::Yellow);

        player->shoot();
    }

    // =========================================================
    //  CLEANUP
    // =========================================================
    void cleanup() {
        delete survivalGame;
        survivalGame = nullptr;
        delete campaignGame;
        campaignGame = nullptr;
        enemies.clearAll();
        bulletManager.clearAll();
    }

private:

    // =========================================================
    //  START HELPERS
    // =========================================================
    void startSurvivalMode() {
        gameMode = 1;

        if (survivalGame) { delete survivalGame; survivalGame = nullptr; }
        enemies.clearAll();
        bulletManager.clearAll();
        enemies.setBulletManager(&bulletManager);

        survivalGame = new SurvivalGame(screenX, screenY);
        survivalGame->setCharManager(&characters);
        survivalGame->start();   // internally calls loadAllLevels + switchToLevel(0)

        Level* level = survivalGame->getCurrentLevel();
        if (level) {
            characters.getActivePlayer()->setPlayerPosition(
                level->getPlayerSpawnX(),
                level->getPlayerSpawnY());
            level->spawnEnemies(enemies, characters.getActivePlayer());
        }

        characters.getActivePlayer()->setVelocity(0, 0);
        characters.getActivePlayer()->setGrounded(false);
        camera.reset();
        Delay.restart();

        currentLevelNumber = 1;
        showLevelTitle = true;
        levelTitleTimer.restart();

        // Sync levelManager with survivalGame's internal levels
        levelManager.loadAllLevels();
        levelManager.switchToLevel(0);
    }

    void startCampaignMode() {
        gameMode = 2;

        if (campaignGame) { delete campaignGame; campaignGame = nullptr; }
        enemies.clearAll();
        bulletManager.clearAll();
        enemies.setBulletManager(&bulletManager);   // FIX: was missing

        campaignGame = new CampaignGame(screenX, screenY);
        campaignGame->setCharManager(&characters);
        campaignGame->start();

        // Generate initial chunks so the player has terrain immediately
        CampaignLevel* cl = campaignGame->getCampaignLevel();
        if (cl) {
            // Pre-generate the first 8 chunks so there is terrain on screen
            for (int i = 0; i < 8; i++) {
                cl->update(i * 16 * 64.0f);
            }
        }

        characters.getActivePlayer()->setPlayerPosition(200, 50);
        characters.getActivePlayer()->setVelocity(0, 0);
        characters.getActivePlayer()->setGrounded(false);
        camera.reset();
        Delay.restart();
    }

    void handleMenuEscape() {
        int menuState = startMenu.getMenuState();
        if (menuState == 2) {
            startMenu.setMenuState(1);
            startMenu.resetSelection();
        }
        else if (menuState == 1) {
            startMenu.setMenuState(0);
            startMenu.resetSelection();
        }
        else {
            window.close();
        }
    }

    // =========================================================
    //  SURVIVAL UPDATE
    // =========================================================
    void updateSurvival(float dt) {
        Level* currentLevel = survivalGame->getCurrentLevel();
        if (!currentLevel) {
            gameMode = 0;
            return;
        }

        float pX = characters.getActivePlayer()->getPlayerX();
        float pY = characters.getActivePlayer()->getPlayerY();
        float pVelocityX = characters.getActivePlayer()->getVelocityX();
        float pVelocityY = characters.getActivePlayer()->getVelocityY();

        // Resolve player physics against level terrain
        updatePlayerPhysics(currentLevel, pX, pY, pVelocityX, pVelocityY);

        // Left world boundary
        if (pX < 10.f) {
            pX = 10.f;
            pVelocityX = 0.f;
        }

        // ---- Level end / advance ----
        float levelEndX = currentLevel->getLevelEnd();
        if (pX + characters.getActivePlayer()->getWidth() >= levelEndX - 10.f) {
            if (levelManager.getCurrentLevelIndex() < levelManager.getTotalLevels() - 1) {
                // Advance to next level
                levelManager.nextLevel();
                Level* newLevel = levelManager.getCurrentLevel();
                if (newLevel) {
                    bulletManager.clearAll();
                    enemies.clearAll();
                    enemies.setBulletManager(&bulletManager);

                    survivalGame->setCurrentLevel(newLevel);
                    currentLevel = newLevel;

                    newLevel->spawnEnemies(enemies, characters.getActivePlayer());

                    characters.getActivePlayer()->setPlayerPosition(
                        newLevel->getPlayerSpawnX(),
                        newLevel->getPlayerSpawnY());
                    characters.getActivePlayer()->setVelocity(0, 0);
                    characters.getActivePlayer()->setGrounded(false);

                    camera.reset();

                    currentLevelNumber = levelManager.getCurrentLevelIndex() + 1;
                    showLevelTitle = true;
                    levelTitleTimer.restart();

                    // Early return so new level values are used next frame
                    return;
                }
            }
            else {
                // Last level – check survival clear
                bool allDefeated = true;
                for (int i = 0; i < enemies.getEnemyCount(); i++) {
                    Enemy* e = enemies.getEnemyAt(i);
                    if (e && e->getIsAlive()) { allDefeated = false; break; }
                }
                if (allDefeated) {
                    scoreSystem.addFeatScore("Survival Clear");
                }

                // Clamp player at end
                pX = levelEndX - characters.getActivePlayer()->getWidth();
                pVelocityX = 0.f;
            }
        }

        // Apply final player position / velocity
        characters.getActivePlayer()->setPlayerPosition(pX, pY);
        characters.getActivePlayer()->setVelocity(pVelocityX, pVelocityY);

        // Camera
        camera.follow(pX, pY);
        camera.setBounds(0.f, currentLevel->getLevelEnd() - screenX, 0.f, 0.f);
        camera.update();
        survivalGame->setCamera(camera.getX(), camera.getY());

        // Enemies
        enemies.updateAll(dt, characters.getActivePlayer());

        // Enemy physics against terrain
        updateEnemyPhysics(currentLevel, dt);

        // Bullets
        bulletManager.update(dt);
        checkBulletEnemyCollisions();
        checkBulletPlayerCollisions();
        bulletManager.checkMultiKill(
            characters.getActivePlayer()->getPlayerX(),
            characters.getActivePlayer()->getPlayerY(),
            &scoreSystem);

        // Wire enemy death -> level kill counter
        for (int i = 0; i < enemies.getEnemyCount(); i++) {
            Enemy* e = enemies.getEnemyAt(i);
            if (e && !e->getIsAlive()) {
                SurvivalLevel* sl = dynamic_cast<SurvivalLevel*>(currentLevel);
                if (sl) sl->enemyKilled();
            }
        }
    }

    // =========================================================
    //  CAMPAIGN UPDATE
    // =========================================================
    // 
    void updateCampaign(float dt) {
        CampaignLevel* campaignLevel = campaignGame->getCampaignLevel();
        if (!campaignLevel) {
            gameMode = 0;
            return;
        }

        float pX = characters.getActivePlayer()->getPlayerX();
        float pY = characters.getActivePlayer()->getPlayerY();
        float pVelocityX = characters.getActivePlayer()->getVelocityX();
        float pVelocityY = characters.getActivePlayer()->getVelocityY();

        // FIX: generate Perlin noise chunks as player moves right
        campaignLevel->update(pX);

        // Player physics
        updatePlayerPhysicsCampaign(campaignLevel, pX, pY, pVelocityX, pVelocityY);

        // Floor clamp
        if (pY + characters.getActivePlayer()->getHeight() > screenY) {
            pY = screenY - characters.getActivePlayer()->getHeight();
            pVelocityY = 0.f;
            characters.getActivePlayer()->setGrounded(true);
        }

        // Left boundary
        if (pX < 0.f) {
            pX = 0.f;
            pVelocityX = 0.f;
        }

        characters.getActivePlayer()->setPlayerPosition(pX, pY);
        characters.getActivePlayer()->setVelocity(pVelocityX, pVelocityY);

        // Camera
        camera.follow(pX, pY);
        camera.setBounds(0.f, pX + (float)screenX * 2.f, 0.f, 0.f);
        camera.update();
        campaignGame->setCamera(camera.getX(), camera.getY());

        // ---------------------------------------------------------
        // Enemies ko player track karne do aur fire karne do
        // yeh line ZAROORI hai — iske baghair enemies sirf khadi rehti hain
        // ---------------------------------------------------------
        enemies.updateAll(dt, characters.getActivePlayer());

        // Enemy physics — gravity aur terrain collision
        for (int i = 0; i < enemies.getEnemyCount(); i++) {
            Enemy* e = enemies.getEnemyAt(i);
            if (!e || !e->getIsAlive()) continue;

            float ex = e->getX();
            float ey = e->getY();
            float evx = e->getVelocityX();
            float evy = e->getVelocityY();

            e->applyGravity(dt);
            evy = e->getVelocityY();
            ey += evy * dt;
            ex += evx * dt;

            bool onGround = false;
            campaignLevel->resolveCollisions(ex, ey,
                e->getWidth(), e->getHeight(), evx, evy, onGround);

            // Terrain mein ghus gaya toh bahar nikalo
            int tries = 0;
            while (campaignLevel->checkCollision(ex, ey,
                e->getWidth(), e->getHeight()) && tries < 100) {
                ey -= 1.f;
                tries++;
            }

            e->setGrounded(onGround);
            e->setVelocityY(evy);
            e->setVelocityX(evx);
            e->setPosition(ex, ey);
        }

        // Dead enemies remove karo taake slot free ho
        enemies.removeDeadEnemies();

        // Har 1000px baad naya wave spawn karo
        if (pX - lastCampaignSpawnX > 500.f) {
            campaignLevel->spawnWave(pX, enemies, characters.getActivePlayer());
            lastCampaignSpawnX = pX;
        }

        // Bullets
        bulletManager.update(dt);
        checkBulletEnemyCollisions();
        checkBulletPlayerCollisions();
        bulletManager.checkMultiKill(
            characters.getActivePlayer()->getPlayerX(),
            characters.getActivePlayer()->getPlayerY(),
            &scoreSystem);

        campaignGame->update(dt, &characters);

        if (campaignGame->getKillQuotaReached()) {
            scoreSystem.addFeatScore("Campaign Clear");
        }
    }
//    void updateCampaign(float dt) {
//        CampaignLevel* campaignLevel = campaignGame->getCampaignLevel();
//        if (!campaignLevel) {
//            gameMode = 0;
//            return;
//        }
//
//        float pX = characters.getActivePlayer()->getPlayerX();
//        float pY = characters.getActivePlayer()->getPlayerY();
//        float pVelocityX = characters.getActivePlayer()->getVelocityX();
//        float pVelocityY = characters.getActivePlayer()->getVelocityY();
//
//        // FIX: generate Perlin noise chunks as player moves right
//        campaignLevel->update(pX);
//
//        for (int i = 0; i < enemies.getEnemyCount(); i++) {
//    Enemy* e = enemies.getEnemyAt(i);
//    if (!e || !e->getIsAlive()) continue;
//
//    float ex  = e->getX();
//    float ey  = e->getY();
//    float evx = e->getVelocityX();
//    float evy = e->getVelocityY();
//
//    e->applyGravity(dt);
//    evy = e->getVelocityY();
//    ey += evy * dt;
//    ex += evx * dt;
//
//    bool onGround = false;
//    campaignLevel->resolveCollisions(ex, ey,
//        e->getWidth(), e->getHeight(), evx, evy, onGround);
//
//    int tries = 0;
//    while (campaignLevel->checkCollision(ex, ey,
//           e->getWidth(), e->getHeight()) && tries < 100) {
//        ey -= 1.f;
//        tries++;
//    }
//
//    e->setGrounded(onGround);
//    e->setVelocityY(evy);
//    e->setVelocityX(evx);
//    e->setPosition(ex, ey);
//}
//
//// Dead enemies hata do slot free karne ke liye
//enemies.removeDeadEnemies();
//
//
//        // Har 400px baad naya wave spawn karo
//        if (pX - lastCampaignSpawnX > 1000.f) {
//            campaignLevel->spawnWave(pX, enemies, characters.getActivePlayer());
//            lastCampaignSpawnX = pX;
//        }
//
//
//
//        // Player physics
//        updatePlayerPhysicsCampaign(campaignLevel, pX, pY, pVelocityX, pVelocityY);
//
//        // Floor clamp
//        if (pY + characters.getActivePlayer()->getHeight() > screenY) {
//            pY = screenY - characters.getActivePlayer()->getHeight();
//            pVelocityY = 0.f;
//            characters.getActivePlayer()->setGrounded(true);
//        }
//
//        // Left boundary
//        if (pX < 0.f) {
//            pX = 0.f;
//            pVelocityX = 0.f;
//        }
//
//        characters.getActivePlayer()->setPlayerPosition(pX, pY);
//        characters.getActivePlayer()->setVelocity(pVelocityX, pVelocityY);
//
//        // FIX: camera follows infinitely, not clamped to screenX
//        camera.follow(pX, pY);
//        camera.setBounds(0.f, pX + (float)screenX * 2.f, 0.f, 0.f);
//        camera.update();
//        campaignGame->setCamera(camera.getX(), camera.getY());
//
//        // Bullets
//        bulletManager.update(dt);
//        checkBulletEnemyCollisions();
//        checkBulletPlayerCollisions();
//        bulletManager.checkMultiKill(
//            characters.getActivePlayer()->getPlayerX(),
//            characters.getActivePlayer()->getPlayerY(),
//            &scoreSystem);
//
//        campaignGame->update(dt, &characters);
//
//        if (campaignGame->getKillQuotaReached()) {
//            scoreSystem.addFeatScore("Campaign Clear");
//        }
//    }

    // =========================================================
    //  PHYSICS HELPERS
    // =========================================================
    void updatePlayerPhysics(Level* level,
        float& pX, float& pY,
        float& pVelocityX, float& pVelocityY)
    {
        if (!level) return;

        // Horizontal
        pX += pVelocityX;

        const float stepHeight = 64.0f;
        float pw = characters.getActivePlayer()->getWidth();
        float ph = characters.getActivePlayer()->getHeight();

        if (pVelocityX != 0 && level->checkCollision(pX, pY, pw, ph)) {
            float tempY = pY - stepHeight;
            if (!level->checkCollision(pX, tempY, pw, ph)) {
                pY = tempY;
            }
            else {
                pX = characters.getActivePlayer()->getPlayerX();
                pVelocityX = 0;
            }
        }

        // Vertical
        pY += pVelocityY;

        if (level->checkCollision(pX, pY, pw, ph)) {
            if (pVelocityY > 0) {
                characters.getActivePlayer()->setGrounded(true);
                pVelocityY = 0;
                while (level->checkCollision(pX, pY, pw, ph)) pY -= 1.f;
            }
            else {
                pVelocityY = 0;
                while (level->checkCollision(pX, pY, pw, ph)) pY += 1.f;
            }
        }
        else {
            if (pVelocityY >= 0)
                characters.getActivePlayer()->setGrounded(false);
        }
    }

    void updatePlayerPhysicsCampaign(CampaignLevel* level,
        float& pX, float& pY,
        float& pVelocityX, float& pVelocityY)
    {
        if (!level) return;

        float pw = characters.getActivePlayer()->getWidth();
        float ph = characters.getActivePlayer()->getHeight();

        // Horizontal
        pX += pVelocityX;

        const float stepHeight = 20.0f;
        if (pVelocityX != 0 && level->checkCollision(pX, pY, pw, ph)) {
            float tempY = pY - stepHeight;
            if (!level->checkCollision(pX, tempY, pw, ph)) {
                pY = tempY;
            }
            else {
                pX = characters.getActivePlayer()->getPlayerX();
                pVelocityX = 0;
            }
        }

        // Vertical
        pY += pVelocityY;

        if (level->checkCollision(pX, pY, pw, ph)) {
            if (pVelocityY > 0) {
                characters.getActivePlayer()->setGrounded(true);
                pVelocityY = 0;
                while (level->checkCollision(pX, pY, pw, ph)) pY -= 1.f;
            }
            else {
                pVelocityY = 0;
                while (level->checkCollision(pX, pY, pw, ph)) pY += 1.f;
            }
        }
        else {
            if (pVelocityY >= 0)
                characters.getActivePlayer()->setGrounded(false);
        }
    }

    // =========================================================
    //  ENEMY PHYSICS (survival only)
    // =========================================================
    void updateEnemyPhysics(Level* currentLevel, float dt) {
        for (int i = 0; i < enemies.getEnemyCount(); i++) {
            Enemy* enemy = enemies.getEnemyAt(i);
            if (!enemy || !enemy->getIsAlive()) continue;

            float ex = enemy->getX();
            float ey = enemy->getY();
            float evx = enemy->getVelocityX();
            float evy = enemy->getVelocityY();
            const float ew = enemy->getWidth();
            const float eh = enemy->getHeight();

            enemy->applyGravity(dt);
            evy = enemy->getVelocityY();

            ey += evy * dt;
            ex += evx * dt;

            // Left boundary bounce
            if (ex < 0) {
                ex = 0;
                evx = -evx;
                enemy->setVelocityX(evx);
            }

            // Terrain collision
            bool onGround = false;
            currentLevel->resolveCollisions(ex, ey, ew, eh, evx, evy, onGround);

            // Push out of terrain if stuck
            int pushAttempts = 0;
            while (currentLevel->checkCollision(ex, ey, ew, eh) && pushAttempts < 100) {
                ey -= 1.0f;
                pushAttempts++;
            }

            // Jump over obstacles
            if (onGround && evx != 0.f) {
                float checkX = (evx > 0) ? (ex + ew + 5.f) : (ex - 5.f);
                bool blockAhead = currentLevel->checkCollision(checkX, ey, 5.f, eh);
                bool canJump = !currentLevel->checkCollision(checkX, ey - 40.f, 5.f, eh);

                if (blockAhead && canJump) {
                    evy = -250.0f;
                    enemy->setVelocityY(evy);
                    onGround = false;
                }
            }

            enemy->setGrounded(onGround);
            enemy->setVelocityY(evy);
            enemy->setVelocityX(evx);
            enemy->setPosition(ex, ey);
        }
    }

    // =========================================================
    //  BULLET COLLISION CHECKS
    // =========================================================
    void checkBulletEnemyCollisions() {
        for (int i = 0; i < bulletManager.getBulletCount(); i++) {
            Bullet* bullet = bulletManager.getBullet(i);
            if (!bullet || !bullet->isActive()) continue;
            if (bullet->getOwner() != PLAYER)   continue;

            float bx = bullet->getX();
            float by = bullet->getY();
            float br = bullet->getRadius();

            for (int j = 0; j < enemies.getEnemyCount(); j++) {
                Enemy* enemy = enemies.getEnemyAt(j);
                if (!enemy || !enemy->getIsAlive()) continue;

                float ex = enemy->getX();
                float ey = enemy->getY();
                float ew = enemy->getWidth();
                float eh = enemy->getHeight();

                if (bx + br > ex && bx - br < ex + ew &&
                    by + br > ey && by - br < ey + eh)
                {
                    enemy->takeDamage(bullet->getDamage(), bx, by, false);

                    if (!enemy->getIsAlive())
                        scoreSystem.addEnemyKillScore(enemy->getName());
                    // Campaign mode mein kill quota track karo
                    if (gameMode == 2 && campaignGame) {
                        CampaignLevel* cl = campaignGame->getCampaignLevel();
                        if (cl) cl->recordEnemyKill(enemy->getName());
                    }

                    bullet->deactivate();
                    break;
                }
            }
        }
    }

    void checkBulletPlayerCollisions() {
        PlayerSoldier* player = characters.getActivePlayer();
        if (!player || !player->getIsAlive()) return;

        float px = player->getPlayerX();
        float py = player->getPlayerY();
        float pw = player->getWidth();
        float ph = player->getHeight();

        for (int i = 0; i < bulletManager.getBulletCount(); i++) {
            Bullet* bullet = bulletManager.getBullet(i);
            if (!bullet || !bullet->isActive()) continue;
            if (bullet->getOwner() != ENEMY)   continue;

            float bx = bullet->getX();
            float by = bullet->getY();
            float br = bullet->getRadius();

            if (bx + br > px && bx - br < px + pw &&
                by + br > py && by - br < py + ph)
            {
                player->takeDamage(bullet->getDamage());
                bullet->deactivate();
            }
        }
    }

    // =========================================================
    //  RENDER HELPERS
    // =========================================================
    void renderPlayerDiedOverlay() {
        if (!showPlayerDiedMessage) return;

        Font font;
        if (!font.loadFromFile("arial.TTF")) return;

        RectangleShape overlay(Vector2f((float)screenX, 150.f));
        overlay.setFillColor(Color(0, 0, 0, 150));
        overlay.setPosition(0, 0);
        window.draw(overlay);

        Text diedText;
        diedText.setFont(font);
        diedText.setString("PLAYER DIED");
        diedText.setCharacterSize(60);
        diedText.setFillColor(Color::Red);
        diedText.setStyle(Text::Bold);
        FloatRect tb = diedText.getLocalBounds();
        diedText.setPosition(screenX / 2.f - tb.width / 2.f, 30.f);
        window.draw(diedText);

        Text returnText;
        returnText.setFont(font);
        returnText.setString("Returning to menu...");
        returnText.setCharacterSize(25);
        returnText.setFillColor(Color::White);
        FloatRect rb = returnText.getLocalBounds();
        returnText.setPosition(screenX / 2.f - rb.width / 2.f, 100.f);
        window.draw(returnText);
    }

    void renderLevelTitle() {
        if (!showLevelTitle) return;

        if (levelTitleTimer.getElapsedTime().asSeconds() < 3.0f) {
            Font font;
            if (!font.loadFromFile("arial.TTF")) return;

            Text levelTitle;
            levelTitle.setFont(font);
            levelTitle.setString("LEVEL " + to_string(currentLevelNumber));
            levelTitle.setCharacterSize(100);
            levelTitle.setFillColor(Color::White);
            levelTitle.setStyle(Text::Bold);
            FloatRect tb = levelTitle.getLocalBounds();
            levelTitle.setPosition(
                screenX / 2.f - tb.width / 2.f,
                screenY / 2.f - 50.f);
            window.draw(levelTitle);
        }
        else {
            showLevelTitle = false;
        }
    }

    void renderPauseOverlay() {
        RectangleShape overlay;
        overlay.setSize(Vector2f((float)screenX, (float)screenY));
        overlay.setFillColor(Color(0, 0, 0, 150));
        overlay.setPosition(0, 0);
        window.draw(overlay);
        pauseMenu.renderPauseMenu(window);
    }

public:
    // (no extra public members needed)
};