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
    float playerFireCooldown = 0.2f;

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
          camera(screenX, screenY) {

        window.create(VideoMode(screenX, screenY), "Metal Slug", Style::Close);

        Delay.restart();
        pauseDelay.restart();

        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(60);
        enemies.setBulletManager(&bulletManager);  // ADD KARO
    }

    ~Game() {
        cleanup();
    }

    void run() {
        Event ev;

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
                            survivalGame = new SurvivalGame(screenX, screenY);
                          
                            survivalGame->setCharManager(&characters);
                            survivalGame->start();
                            levelManager.loadAllLevels();
                            survivalGame->setCurrentLevel(levelManager.getCurrentLevel());
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
                            enemies.clearAll();
                            enemies.setBulletManager(&bulletManager);
                            newLevel->spawnEnemies(enemies, characters.getActivePlayer());
                            characters.getActivePlayer()->setPlayerPosition(newLevel->getPlayerSpawnX(), newLevel->getPlayerSpawnY());
                            characters.getActivePlayer()->setVelocity(0, 0);
                            characters.getActivePlayer()->setGrounded(false);
                            survivalGame->setCurrentLevel(newLevel);
                            camera.reset();
                            currentLevelNumber = levelManager.getCurrentLevelIndex() + 1;
                            showLevelTitle = true;
                            levelTitleTimer.restart();
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

                campaignGame->update(dt, &characters);
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
    }

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
        delete campaignGame;
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

public:
};
