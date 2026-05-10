#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
// Game.h ke top mein yeh order rakho
#include "Weapon.h"
#include "Enemy.h"
#include "GameMode.h"    // SurvivalGame aur CampaignGame yahan hain
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
    EnemyManager enemies;
    LevelManager levelManager;
    bool jumpHeld;
    bool showLevelTitle;
    Clock levelTitleTimer;
    int currentLevelNumber;

    BulletManager bulletManager;
    Clock clock;
    Clock Delay;
    Clock pauseDelay;
    Clock playerFireTimer;
    Clock playerDiedTimer;
    bool showPlayerDiedMessage;
    float playerFireCooldown = 0.2f;

    // Score system
    ScoreSystem scoreSystem;

    Camera camera;
    Menu startMenu;
    Menu pauseMenu;

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
        camera(screenX, screenY) {

        window.create(VideoMode(screenX, screenY), "Metal Slug", Style::Close);

        Delay.restart();
        pauseDelay.restart();
        playerDiedTimer.restart();

        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(60);
        enemies.setBulletManager(&bulletManager);  // ADD KARO

        scoreSystem.loadFont("arial.TTF");
    }

    ~Game() {
        cleanup();
    }

    void run() {
        while (window.isOpen()) {
            float dt = clock.restart().asSeconds();

            handleInput();
            update(dt);
            render();
        }
    }

    void handleInput() {
        Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == Event::Closed) {
                window.close();
            }

            if (gameMode == 0 && ev.type == Event::KeyPressed) {
                if (ev.key.code == Keyboard::Up) {
                    startMenu.moveSelectionUp();
                }
                else if (ev.key.code == Keyboard::Down) {
                    startMenu.moveSelectionDown();
                }
                else if (ev.key.code == Keyboard::Enter) {
                    int menuState = startMenu.getMenuState();
                    if (menuState == 0) {
                        startMenu.setMenuState(1);
                    }
                    else if (menuState == 1) {
                        selectedCharacter = startMenu.getSelectionIndex();
                        characters.switchCharacterToIndex(selectedCharacter);
                        startMenu.setMenuState(2);
                    }
                    else if (menuState == 2) {
                        int modeSelection = startMenu.getSelectionIndex();
                        if (modeSelection == 0) {
                            gameMode = 1;

                            // Clean up previous survival game if exists
                            if (survivalGame) {
                                delete survivalGame;
                                survivalGame = nullptr;
                            }

                            // Reset enemies
                            enemies.clearAll();
                            bulletManager.clearAll();

                            survivalGame = new SurvivalGame(screenX, screenY);

                            survivalGame->setCharManager(&characters);
                            survivalGame->start();
                            levelManager.loadAllLevels();
                            // Reset to first level (set current level index to 0)
                            survivalGame->setCurrentLevel(levelManager.getCurrentLevel());

                            // Reset player
                            characters.getActivePlayer()->forceRespawn();
                            characters.getActivePlayer()->setPlayerPosition(survivalGame->getCurrentLevel()->getPlayerSpawnX(), survivalGame->getCurrentLevel()->getPlayerSpawnY());
                            characters.getActivePlayer()->setVelocity(0, 0);
                            characters.getActivePlayer()->setGrounded(false);
                            camera.reset();
                            Delay.restart();

                            // Get the level from survivalGame and spawn enemies directly
                            Level* level = survivalGame->getCurrentLevel();
                            if (level) {
                                level->spawnEnemies(enemies, characters.getActivePlayer());
                            }

                            currentLevelNumber = 1;
                            showLevelTitle = true;
                            levelTitleTimer.restart();
                        }
                        else if (modeSelection == 1) {
                            gameMode = 2;
                            campaignGame = new CampaignGame(screenX, screenY);
                            campaignGame->setCharManager(&characters);
                            campaignGame->start();
                            characters.getActivePlayer()->setPlayerPosition(200, 50);
                            characters.getActivePlayer()->setVelocity(0, 0);
                            characters.getActivePlayer()->setGrounded(false);
                            camera.reset();
                            Delay.restart();
                        }
                    }
                }
            }
        }


        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            if (gameMode != 0) {
                if (pauseMenu.getPauseMenuVisible()) {
                    pauseMenu.handlePauseInput();
                    if (!pauseMenu.getPauseMenuVisible()) {
                        int selectedOption = pauseMenu.getPauseSelection();
                        if (selectedOption == 1) {
                            cleanup();
                            window.close();
                        }
                    }
                    pauseDelay.restart();
                }
                else {
                    pauseMenu.showPauseMenu();
                    pauseDelay.restart();
                }
            }
            else {
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
                pauseDelay.restart();
            }
        }
    }

    void update(float dt) {
        if (gameMode == 0) {
            startMenu.updateAnimation(dt);
            return;
        }

        if (pauseMenu.getPauseMenuVisible()) {
            pauseMenu.handlePauseInput();
            return;
        }

        // Check if player died (HP <= 0)
        if (!showPlayerDiedMessage && characters.getActivePlayer() && characters.getActivePlayer()->getHP() <= 0) {
            showPlayerDiedMessage = true;
            playerDiedTimer.restart();
        }

        // Handle player died message display
        if (showPlayerDiedMessage) {
            float elapsed = playerDiedTimer.getElapsedTime().asSeconds();
            if (elapsed > 3.0f) {
                // Return to menu after 3 seconds
                showPlayerDiedMessage = false;
                cleanup();
                gameMode = 0;
                startMenu.setMenuState(2); // Go to mode selection
                // Reset player for next game
                if (characters.getActivePlayer()) {
                    characters.getActivePlayer()->forceRespawn();
                }
                return;
            }
            return; // Don't update game while showing death message
        }

        characters.getActivePlayer()->update(dt);

        if (Keyboard::isKeyPressed(Keyboard::Z) && Delay.getElapsedTime().asSeconds() > 0.2f) {
            characters.switchCharacter();
            Delay.restart();
        }

        // F key for firing
        if (Keyboard::isKeyPressed(Keyboard::F) && playerFireTimer.getElapsedTime().asSeconds() > playerFireCooldown) {
            playerFire();
            playerFireTimer.restart();
        }

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
            if (characters.getActivePlayer()->getIsGrounded()) {
                characters.getActivePlayer()->setVelocityX(characters.getActivePlayer()->getVelocityX() * characters.getActivePlayer()->getFriction());
            }
            else {
                characters.getActivePlayer()->setVelocityX(characters.getActivePlayer()->getVelocityX() * characters.getActivePlayer()->getAirFriction());
            }
        }

        float velCap;
        if (gameMode == 2) {
            velCap = 14.f;
        }
        else {
            velCap = 6.f;
        }
        if (characters.getActivePlayer()->getVelocityX() > velCap) {
            characters.getActivePlayer()->setVelocityX(velCap);
        }
        if (characters.getActivePlayer()->getVelocityX() < -velCap) {
            characters.getActivePlayer()->setVelocityX(-velCap);
        }


        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            if (characters.getActivePlayer()->getIsGrounded()) {
                characters.getActivePlayer()->setVelocityY(characters.getActivePlayer()->getJumpPower());
                characters.getActivePlayer()->setGrounded(false);
                jumpHeld = true;
            }
        }
        else if (Keyboard::isKeyPressed(Keyboard::J)) {
            if (characters.getActivePlayer()->getIsGrounded()) {

                // direct velocity set — no jumpPower multiplier nonsense
                characters.getActivePlayer()->setVelocityY(-40.0f);
                characters.getActivePlayer()->setGrounded(false);
                jumpHeld = true;  // true rakho taake halving na ho
            }
        }
        else {
            jumpHeld = false;
        }

        if (!jumpHeld && characters.getActivePlayer()->getVelocityY() < 0) {
            characters.getActivePlayer()->setVelocityY(characters.getActivePlayer()->getVelocityY() * 0.5f);
        }


        characters.getActivePlayer()->setVelocityY(characters.getActivePlayer()->getVelocityY() + characters.getActivePlayer()->getGravity());
        if (characters.getActivePlayer()->getVelocityY() > characters.getActivePlayer()->getMaxFallSpeed()) {
            characters.getActivePlayer()->setVelocityY(characters.getActivePlayer()->getMaxFallSpeed());
        }

        if (gameMode == 1 && survivalGame) {
            Level* currentLevel = survivalGame->getCurrentLevel();
            if (currentLevel) {
                float pX = characters.getActivePlayer()->getPlayerX();
                float pY = characters.getActivePlayer()->getPlayerY();
                float pVelocityX = characters.getActivePlayer()->getVelocityX();
                float pVelocityY = characters.getActivePlayer()->getVelocityY();

                checkEnemyCollisions(pX, pY, pVelocityX);
                updatePlayerPhysics(currentLevel, pX, pY, pVelocityX, pVelocityY);

                if (pX < 0) {
                    pX = 0;
                    pVelocityX = 0;
                }

                if (pX < 10) {
                    pX = 10;
                    pVelocityX = 0;
                }
                // Check if player reached level end - switch to next level
                if (pX + characters.getActivePlayer()->getWidth() >= currentLevel->getLevelEnd() - 10.f) {
                    if (levelManager.getCurrentLevelIndex() < levelManager.getTotalLevels() - 1) {
                        levelManager.nextLevel();
                        Level* newLevel = levelManager.getCurrentLevel();
                        if (newLevel) {
                            // Clear bullets and enemies
                            bulletManager.clearAll();
                            enemies.clearAll();
                            enemies.setBulletManager(&bulletManager);

                            // Set new level in survival game
                            survivalGame->setCurrentLevel(newLevel);
                            currentLevel = newLevel;

                            // Spawn enemies for new level
                            newLevel->spawnEnemies(enemies, characters.getActivePlayer());

                            // Reset player position for new level
                            characters.getActivePlayer()->setPlayerPosition(newLevel->getPlayerSpawnX(), newLevel->getPlayerSpawnY());
                            characters.getActivePlayer()->setVelocity(0, 0);
                            characters.getActivePlayer()->setGrounded(false);

                            // Reset camera for new level
                            camera.reset();

                            // Update level number and show title
                            currentLevelNumber = levelManager.getCurrentLevelIndex() + 1;
                            showLevelTitle = true;
                            levelTitleTimer.restart();
                        }
                    }
                    else {
                        // Last level completed - check if all enemies are defeated for survival clear
                        bool allEnemiesDefeated = true;
                        for (int i = 0; i < enemies.getEnemyCount(); i++) {
                            Enemy* enemy = enemies.getEnemyAt(i);
                            if (enemy && enemy->getIsAlive()) {
                                allEnemiesDefeated = false;
                                break;
                            }
                        }

                        if (allEnemiesDefeated) {
                            // Survival mode clear bonus
                            scoreSystem.addFeatScore("Survival Clear");
                        }
                    }
                }
                else if (pX + characters.getActivePlayer()->getWidth() > currentLevel->getLevelEnd()) {
                    pX = currentLevel->getLevelEnd() - characters.getActivePlayer()->getWidth();
                    pVelocityX = 0;
                }



                characters.getActivePlayer()->setPlayerPosition(pX, pY);
                characters.getActivePlayer()->setVelocity(pVelocityX, pVelocityY);

                camera.follow(pX, pY);
                camera.setBounds(0.0f, currentLevel->getLevelEnd() - screenX, 0.0f, 0.0f);
                camera.update();

                survivalGame->setCamera(camera.getX(), camera.getY());

                enemies.updateAll(dt, characters.getActivePlayer());

                // Update bullets
                bulletManager.update(dt);

                // Check bullet collisions
                checkBulletEnemyCollisions();    // Player bullets hit enemies
                checkBulletPlayerCollisions();   // Enemy bullets hit player

                // Check for multi-kills (flawless victory)
                bulletManager.checkMultiKill(characters.getActivePlayer()->getPlayerX(), characters.getActivePlayer()->getPlayerY(), &scoreSystem);

                // Handle enemy collision with level blocks (must be in Game.h to avoid circular dependency)
                for (int i = 0; i < enemies.getEnemyCount(); i++) {
                    Enemy* enemy = enemies.getEnemyAt(i);
                    if (!enemy || !enemy->getIsAlive()) {
                        continue;
                    }

                    float enemy_x = enemy->getX();
                    float enemy_y = enemy->getY();
                    float enemy_vx = enemy->getVelocityX();
                    float enemy_vy = enemy->getVelocityY();
                    const float enemy_w = enemy->getWidth();
                    const float enemy_h = enemy->getHeight();

                    enemy->applyGravity(dt);
                    enemy_vy = enemy->getVelocityY();

                    enemy_y += enemy_vy * dt;
                    enemy_x += enemy_vx * dt;

                    if (enemy_x < 0) {
                        enemy_x = 0;
                        enemy_vx = -enemy_vx;
                        enemy->setVelocityX(enemy_vx);
                    }

                    // resolve collisions
                    bool onGround = false;
                    currentLevel->resolveCollisions(enemy_x, enemy_y, enemy_w, enemy_h, enemy_vx, enemy_vy, onGround);

                    // push enemy out of terrain if stuck inside
                    int pushAttempts = 0;
                    while (currentLevel->checkCollision(enemy_x, enemy_y, enemy_w, enemy_h) && pushAttempts < 100) {
                        enemy_y -= 1.0f;
                        pushAttempts++;
                    }

                    // jump if block ahead
                    if (onGround && enemy_vx != 0.0f) {
                        float checkX = (enemy_vx > 0)
                            ? (enemy_x + enemy_w + 5.f)
                            : (enemy_x - 5.f);

                        bool blockAhead = currentLevel->checkCollision(checkX, enemy_y, 5.f, enemy_h);
                        bool canJumpOver = !currentLevel->checkCollision(checkX, enemy_y - 40.f, 5.f, enemy_h);

                        if (blockAhead && canJumpOver) {
                            enemy_vy = -250.0f;
                            enemy->setVelocityY(enemy_vy);
                            onGround = false;
                        }
                    }

                    enemy->setGrounded(onGround);
                    enemy->setVelocityY(enemy_vy);
                    enemy->setVelocityX(enemy_vx);
                    enemy->setPosition(enemy_x, enemy_y);
                }

                // Wire enemy death to level kill counter
                for (int i = 0; i < enemies.getEnemyCount(); i++) {
                    Enemy* e = enemies.getEnemyAt(i);
                    if (e && !e->getIsAlive()) {
                        SurvivalLevel* sl = dynamic_cast<SurvivalLevel*>(currentLevel);
                        if (sl) sl->enemyKilled();
                    }
                }
                // Baad mein - level switch hone par enemies respawn


            }
            else {
                gameMode = 0;
            }
        }

        else if (gameMode == 2 && campaignGame) {
            CampaignLevel* campaignLevel = campaignGame->getCampaignLevel();
            if (campaignLevel) {
                float pX = characters.getActivePlayer()->getPlayerX();
                float pY = characters.getActivePlayer()->getPlayerY();
                float pVelocityX = characters.getActivePlayer()->getVelocityX();
                float pVelocityY = characters.getActivePlayer()->getVelocityY();

                updatePlayerPhysicsCampaign(campaignLevel, pX, pY, pVelocityX, pVelocityY);

                if (pY + characters.getActivePlayer()->getHeight() > screenY) {
                    pY = screenY - characters.getActivePlayer()->getHeight();
                    pVelocityY = 0;
                    characters.getActivePlayer()->setGrounded(true);
                }

                if (pX < 0) {
                    pX = 0;
                }

                characters.getActivePlayer()->setPlayerPosition(pX, pY);
                characters.getActivePlayer()->setVelocity(pVelocityX, pVelocityY);

                camera.follow(pX, pY);
                camera.setBounds(0.0f, screenX, 0.0f, 0.0f);
                camera.update();

                campaignGame->setCamera(camera.getX(), camera.getY());

                // Update bullets for campaign mode
                bulletManager.update(dt);

                // Check bullet collisions
                checkBulletEnemyCollisions();
                checkBulletPlayerCollisions();

                // Check for multi-kills (flawless victory)
                bulletManager.checkMultiKill(characters.getActivePlayer()->getPlayerX(), characters.getActivePlayer()->getPlayerY(), &scoreSystem);

                campaignGame->update(dt, &characters);

                // Check if campaign kill quota reached for boss level
                if (campaignGame->getKillQuotaReached()) {
                    // Campaign mode clear bonus
                    scoreSystem.addFeatScore("Campaign Clear");
                }
            }
            else {
                gameMode = 0;
            }
        }
    }

    void render() {
        window.clear(Color(135, 206, 235));

        if (gameMode == 0) {
            int menuState = startMenu.getMenuState();
            if (menuState == 0) {
                startMenu.renderStartScreen(window);
            }
            else if (menuState == 1) {
                startMenu.renderCharacterSelection(window);
            }
            else if (menuState == 2) {
                startMenu.renderModeSelection(window);
            }
            window.display();
            return;
        }

        if (gameMode == 1 && survivalGame) {
            survivalGame->render(window);
            enemies.renderAll(window, camera.getX(), camera.getY());
            bulletManager.render(window, camera.getX(), camera.getY());
            characters.getActivePlayer()->render(window, camera.getX(), camera.getY());

            // Render score
            scoreSystem.render(window, screenX, screenY);

            // Render player died message overlay on game screen
            if (showPlayerDiedMessage) {
                Font font;
                if (!font.loadFromFile("arial.TTF")) {
                    // Fallback if font fails to load
                    return;
                }

                // Semi-transparent overlay (smaller, at top)
                RectangleShape overlay(Vector2f(screenX, 150));
                overlay.setFillColor(Color(0, 0, 0, 150));
                overlay.setPosition(0, 0);
                window.draw(overlay);

                // Player Died text
                Text diedText;
                diedText.setFont(font);
                diedText.setString("PLAYER DIED");
                diedText.setCharacterSize(60);
                diedText.setFillColor(Color::Red);
                diedText.setStyle(Text::Bold);
                FloatRect textBounds = diedText.getLocalBounds();
                diedText.setPosition(screenX / 2.0f - textBounds.width / 2.0f, 30.0f);
                window.draw(diedText);

                // Returning to menu text
                Text returnText;
                returnText.setFont(font);
                returnText.setString("Returning to menu...");
                returnText.setCharacterSize(25);
                returnText.setFillColor(Color::White);
                FloatRect returnBounds = returnText.getLocalBounds();
                returnText.setPosition(screenX / 2.0f - returnBounds.width / 2.0f, 100.0f);
                window.draw(returnText);
            }

            if (showLevelTitle) {
                if (levelTitleTimer.getElapsedTime().asSeconds() < 3.0f) {
                    Font font;
                    font.loadFromFile("arial.TTF");
                    Text levelTitle;
                    levelTitle.setFont(font);
                    levelTitle.setString("LEVEL " + to_string(currentLevelNumber));
                    levelTitle.setCharacterSize(100);
                    levelTitle.setFillColor(Color::White);
                    levelTitle.setPosition(screenX / 2.0f - 200.0f, screenY / 2.0f - 50.0f);
                    window.draw(levelTitle);
                }
                else {
                    showLevelTitle = false;
                }
            }

            if (pauseMenu.getPauseMenuVisible()) {
                RectangleShape overlay;
                overlay.setSize(Vector2f((float)screenX, (float)screenY));
                overlay.setFillColor(Color(0, 0, 0, 100));
                overlay.setPosition(0, 0);
                window.draw(overlay);
                pauseMenu.renderPauseMenu(window);
                window.display();
                return;
            }

            window.display();
        }

        else if (gameMode == 2 && campaignGame) {
            campaignGame->render(window);
            enemies.renderAll(window, camera.getX(), camera.getY());
            bulletManager.render(window, camera.getX(), camera.getY());
            characters.getActivePlayer()->render(window, camera.getX(), camera.getY());

            // Render score
            scoreSystem.render(window, screenX, screenY);

            // Render player died message overlay on game screen
            if (showPlayerDiedMessage) {
                Font font;
                if (!font.loadFromFile("arial.TTF")) {
                    // Fallback if font fails to load
                    return;
                }

                // Semi-transparent overlay (smaller, at top)
                RectangleShape overlay(Vector2f(screenX, 150));
                overlay.setFillColor(Color(0, 0, 0, 150));
                overlay.setPosition(0, 0);
                window.draw(overlay);

                // Player Died text
                Text diedText;
                diedText.setFont(font);
                diedText.setString("PLAYER DIED");
                diedText.setCharacterSize(60);
                diedText.setFillColor(Color::Red);
                diedText.setStyle(Text::Bold);
                FloatRect textBounds = diedText.getLocalBounds();
                diedText.setPosition(screenX / 2.0f - textBounds.width / 2.0f, 30.0f);
                window.draw(diedText);

                // Returning to menu text
                Text returnText;
                returnText.setFont(font);
                returnText.setString("Returning to menu...");
                returnText.setCharacterSize(25);
                returnText.setFillColor(Color::White);
                FloatRect returnBounds = returnText.getLocalBounds();
                returnText.setPosition(screenX / 2.0f - returnBounds.width / 2.0f, 100.0f);
                window.draw(returnText);
            }

            if (pauseMenu.getPauseMenuVisible()) {
                RectangleShape overlay;
                overlay.setSize(Vector2f(screenX, screenY));
                overlay.setFillColor(Color(0, 0, 0, 180));
                window.draw(overlay);
                pauseMenu.renderPauseMenu(window);
            }
        }
    }

    // Score system handled by ScoreSystem class

    void playerFire() {
        PlayerSoldier* player = characters.getActivePlayer();
        if (!player) return;

        // Get player position and facing direction
        float px = player->getPlayerX();
        float py = player->getPlayerY();
        bool facingRight = player->isFacingRight();

        // Calculate bullet spawn position (slightly in front of player)
        float bulletX = facingRight ? px + 30 : px - 30;
        float bulletY = py + 10; // Center height

        // Calculate angle (0 for right, PI for left)
        float angle = facingRight ? 0.f : 3.14159f;

        // Spawn bullet with PLAYER owner type
        bulletManager.spawnBullet(bulletX, bulletY, angle, 10, PLAYER, 800.f, 600.f, sf::Color::Yellow);

        // Call player's shoot animation
        player->shoot();
    }

    void cleanup() {
        delete survivalGame;
        survivalGame = nullptr;
        delete campaignGame;
        campaignGame = nullptr;
    }

private:
    void updatePlayerPhysics(Level* level, float& pX, float& pY, float& pVelocityX, float& pVelocityY) {
        if (!level) {
            return;
        }

        pX += pVelocityX;

        float stepHeight = 64.0f;
        if (pVelocityX != 0 &&
            level->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
            float tempY = pY - stepHeight;
            if (!level->checkCollision(pX, tempY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                pY = tempY;
            }
            else {
                pX = characters.getActivePlayer()->getPlayerX();
                pVelocityX = 0;
            }
        }

        pY += pVelocityY;

        if (level->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
            if (pVelocityY > 0) {
                characters.getActivePlayer()->setGrounded(true);
                pVelocityY = 0;
                while (level->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                    pY -= 1;
                }
            }
            else {
                pVelocityY = 0;
                while (level->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                    pY += 1;
                }
            }
        }
        else {
            if (pVelocityY >= 0) {
                characters.getActivePlayer()->setGrounded(false);
            }
        }
    }

    void updatePlayerPhysicsCampaign(CampaignLevel* level, float& pX, float& pY, float& pVelocityX, float& pVelocityY) {
        if (!level) {
            return;
        }

        pX += pVelocityX;

        float stepHeight = 20.0f;
        if (pVelocityX != 0 &&
            level->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
            float tempY = pY - stepHeight;
            if (!level->checkCollision(pX, tempY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                pY = tempY;
            }
            else {
                pX = characters.getActivePlayer()->getPlayerX();
                pVelocityX = 0;
            }
        }

        pY += pVelocityY;

        if (level->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
            if (pVelocityY > 0) {
                characters.getActivePlayer()->setGrounded(true);
                pVelocityY = 0;
                while (level->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                    pY -= 1;
                }
            }
            else {
                pVelocityY = 0;
                while (level->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                    pY += 1;
                }
            }
        }
        else {
            if (pVelocityY >= 0) {
                characters.getActivePlayer()->setGrounded(false);
            }
        }
    }

    void checkEnemyCollisions(float& pX, float& pY, float& pVelocityX) {
        // Commented out - let player pass through enemies
        /*
        if (pVelocityX == 0) {
            return;
        }

        if (!characters.getIsGrounded()) {
            return;
        }

        float pW = (float)characters.getWidth();
        float pH = (float)characters.getHeight();

        for (int i = 0; i < enemies.getEnemyCount(); i++) {
            Enemy* enemy = enemies.getEnemy(i);
            if (!enemy) {
                continue;
            }
            if (!enemy->getIsAlive()) {
                continue;
            }
            float ex2 = enemy->getX();
            float ey2 = enemy->getY();
            float ew2 = enemy->getWidth();
            float eh2 = enemy->getHeight();
            if (pX < ex2 + ew2 && pX + pW > ex2 &&
                pY < ey2 + eh2 && pY + pH > ey2) {
                pX = characters.getX();
                pVelocityX = 0;
                break;
            }
        }
        */
    }

    // Check player bullets hitting enemies
    void checkBulletEnemyCollisions() {
        PlayerSoldier* player = characters.getActivePlayer();
        if (!player) return;

        // Get all bullets from bullet manager
        for (int i = 0; i < bulletManager.getBulletCount(); i++) {
            Bullet* bullet = bulletManager.getBullet(i);
            if (!bullet || !bullet->isActive()) continue;
            if (bullet->getOwner() != PLAYER) continue; // Only player bullets

            float bx = bullet->getX();
            float by = bullet->getY();
            float br = bullet->getRadius();

            // Check collision with each enemy
            for (int j = 0; j < enemies.getEnemyCount(); j++) {
                Enemy* enemy = enemies.getEnemyAt(j);
                if (!enemy || !enemy->getIsAlive()) continue;

                float ex = enemy->getX();
                float ey = enemy->getY();
                float ew = enemy->getWidth();
                float eh = enemy->getHeight();

                // Simple AABB collision
                if (bx + br > ex && bx - br < ex + ew &&
                    by + br > ey && by - br < ey + eh) {
                    // Hit enemy
                    enemy->takeDamage(bullet->getDamage(), bx, by, false);

                    // Check if enemy died and add score
                    if (!enemy->getIsAlive()) {
                        scoreSystem.addEnemyKillScore(enemy->getName());
                    }

                    bullet->deactivate();
                    break; // Bullet hit one enemy, move to next bullet
                }
            }
        }
    }

    // Check enemy bullets hitting player
    void checkBulletPlayerCollisions() {
        PlayerSoldier* player = characters.getActivePlayer();
        if (!player || !player->getIsAlive()) return;

        float px = player->getPlayerX();
        float py = player->getPlayerY();
        float pw = player->getWidth();
        float ph = player->getHeight();

        // Get all bullets from bullet manager
        for (int i = 0; i < bulletManager.getBulletCount(); i++) {
            Bullet* bullet = bulletManager.getBullet(i);
            if (!bullet || !bullet->isActive()) continue;
            if (bullet->getOwner() != ENEMY) continue; // Only enemy bullets

            float bx = bullet->getX();
            float by = bullet->getY();
            float br = bullet->getRadius();

            // Check collision with player
            if (bx + br > px && bx - br < px + pw &&
                by + br > py && by - br < py + ph) {
                // Hit player
                player->takeDamage(bullet->getDamage());
                bullet->deactivate();
            }
        }
    }

public:
};