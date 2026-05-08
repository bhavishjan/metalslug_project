#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Header.h"
#include "Player.h"
#include "Weapon.h"
#include "Enemy.h"
#include "Menu.h"
#include "GameMode.h"

using namespace sf;
using namespace std;

int main() {
    RenderWindow window(VideoMode(screen_x, screen_y), "Metal Slug", Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    float cameraX = 0, cameraY = 0;

    // GAME MODE: 0=Menu, 1=survival, 2=campaign
    int gameMode = 0;

    // Character selection result
    int selectedCharacter = 0;


    // MENU SYSTEM
    Menu menu(screen_x, screen_y);

    // SURVIVAL MODE VARIABLES
    SurvivalGame* survivalGame = nullptr;

    // CAMPAIGN MODE VARIABLES
    CampaignGame* campaignGame = nullptr;

    // Survival mode enemies (all types pooled together)
    static const int MAX_SURVIVAL_ENEMIES = 50;
    Enemy* survivalRebels[MAX_SURVIVAL_ENEMIES] = {};
    int survivalRebelCount = 0;
    static const int MAX_CAMPAIGN_ENEMIES = 30;
    Enemy* campaignRebels[MAX_CAMPAIGN_ENEMIES] = {};
    int campaignRebelCount = 0;

    // PLAYER SETUP

    bool jumpHeld = false;
    bool fireHeld = false;
    float playerFireCooldown = 0.f;
    const float playerFireInterval = 0.18f;
    const float playerBulletSpeed = 700.f;
    const float playerBulletRange = 900.f;
    const int playerBulletDamage = 3;
    BulletManager bulletManager;


    // CHARACTER MANAGER
    CharacterManager characters;

    Clock clock;
    Clock Delay;
    Event ev;

    // GAME LOOP
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (window.pollEvent(ev)) {
            if (ev.type == Event::Closed)
                window.close();

            // MENU INPUT
            if (gameMode == 0 && ev.type == Event::KeyPressed) {
                if (ev.key.code == Keyboard::Up) {
                    menu.moveSelectionUp();
                }
                else if (ev.key.code == Keyboard::Down) {
                    menu.moveSelectionDown();
                }
                else if (ev.key.code == Keyboard::Enter) {
                    int menuState = menu.getMenuState();
                    if (menuState == 0) {
                        // Start screen - go to character selection
                        menu.setMenuState(1);
                        cout << "Entered Character Selection" << endl;
                    }
                    else if (menuState == 1) {
                        // Character selection - go to mode selection
                        selectedCharacter = menu.getSelectionIndex();
                        characters.switchCharacterToIndex(selectedCharacter);
                        menu.setMenuState(2);
                        cout << "Selected character at index: " << selectedCharacter << endl;
                    }
                    else if (menuState == 2) {
                        // Mode selection - start game
                        int modeSelection = menu.getSelectionIndex();
                        if (modeSelection == 0) {
                            gameMode = 1;
                            bulletManager = BulletManager(); // reset all projectiles on mode start
                            survivalGame = new SurvivalGame(screen_x, screen_y);
                            survivalGame->setCharManager(&characters);
                            survivalGame->start();
                            characters.getActivePlayer()->setPlayerPosition(survivalGame->getCurrentLevel()->getPlayerSpawnX(), survivalGame->getCurrentLevel()->getPlayerSpawnY());
                            characters.getActivePlayer()->setVelocity(0, 0);
                            characters.getActivePlayer()->setIsGrounded(false);
                            cameraX = cameraY = 0;
                            Delay.restart();
                            // Spawn all infantry enemy types in survival mode
                            for (int i = 0; i < MAX_SURVIVAL_ENEMIES; i++) {
                                delete survivalRebels[i];
                                survivalRebels[i] = nullptr;
                            }
                            int ei = 0;
                            //// 2 Rebel Soldiers
                            for (int i = 0; i < 2; i++) {
                                survivalRebels[ei] = new RebelSoldier();
                                survivalRebels[ei]->setPosition(300.f + i * 350.f, 200.f);
                                survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                                survivalRebels[ei]->setBulletManager(&bulletManager);
                                survivalRebels[ei]->setGroundY((float)(screen_y - 48));
                                ei++;
                            }
                            // 2 Grenade Soldiers
                            for (int i = 0; i < 2; i++) {
                                survivalRebels[ei] = new GrenadeSoldier();
                                survivalRebels[ei]->setPosition(500.f + i * 400.f, 200.f);
                                survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                                survivalRebels[ei]->setBulletManager(&bulletManager);
                                survivalRebels[ei]->setGroundY((float)(screen_y - 48));
                                ei++;
                            }
                            //// 1 Bazooka Soldier
                            survivalRebels[ei] = new BazookaSoldier();
                            survivalRebels[ei]->setPosition(600.f, 200.f);
                            survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                            survivalRebels[ei]->setBulletManager(&bulletManager);
                            survivalRebels[ei]->setGroundY((float)(screen_y - 48));
                            ei++;
                            // 1 Shielded Soldier
                            survivalRebels[ei] = new ShieldedSoldier();
                            survivalRebels[ei]->setPosition(800.f, 200.f);
                            survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                            survivalRebels[ei]->setBulletManager(&bulletManager);
                            survivalRebels[ei]->setGroundY((float)(screen_y - 48));
                            ei++;
                            ////// 2 Mummy Warriors
                            for (int i = 0; i < 2; i++) {
                                survivalRebels[ei] = new MummyWarrior();
                                survivalRebels[ei]->setPosition(1000.f + i * 400.f, 200.f);
                                survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                                survivalRebels[ei]->setBulletManager(&bulletManager);
                                survivalRebels[ei]->setGroundY((float)(screen_y - 48));
                                ei++;
                            }
                            ////// 2 Zombies
                            for (int i = 0; i < 2; i++) {
                                survivalRebels[ei] = new Zombie();
                                survivalRebels[ei]->setPosition(1200.f + i * 400.f, 200.f);
                                survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                                survivalRebels[ei]->setBulletManager(&bulletManager);
                                survivalRebels[ei]->setGroundY((float)(screen_y - 48));
                                ei++;
                            }

                            ////// 1 Martian
                            survivalRebels[ei] = new Martian();
                            survivalRebels[ei]->setPosition(1400.f, 200.f);
                            survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                            survivalRebels[ei]->setBulletManager(&bulletManager);
                            survivalRebels[ei]->setGroundY((float)(screen_y - 48));
                            ei++;
                            survivalRebelCount = ei;
                            cout << "Entered Survival Mode" << endl;
                        }
                        else if (modeSelection == 1) {
                            gameMode = 2;
                            bulletManager = BulletManager(); // reset all projectiles on mode start
                            campaignGame = new CampaignGame(screen_x, screen_y);
                            campaignGame->setCharManager(&characters);
                            campaignGame->start();
                            characters.getActivePlayer()->setPlayerPosition(200, 50);
                            characters.getActivePlayer()->setVelocity(0, 0);
                            characters.getActivePlayer()->setIsGrounded(false);
                            cameraX = cameraY = 0;
                            Delay.restart();

                            for (int i = 0; i < MAX_CAMPAIGN_ENEMIES; i++) {
                                delete campaignRebels[i];
                                campaignRebels[i] = nullptr;
                            }
                            int ci = 0;
                            for (int i = 0; i < 4; i++) {
                                campaignRebels[ci] = new RebelSoldier();
                                campaignRebels[ci]->setPosition(700.f + i * 280.f, 260.f);
                                campaignRebels[ci]->setPlayer(characters.getActivePlayer());
                                campaignRebels[ci]->setBulletManager(&bulletManager);
                                campaignRebels[ci]->setGroundY((float)(screen_y - 48));
                                ci++;
                            }
                            for (int i = 0; i < 2; i++) {
                                campaignRebels[ci] = new GrenadeSoldier();
                                campaignRebels[ci]->setPosition(1700.f + i * 360.f, 260.f);
                                campaignRebels[ci]->setPlayer(characters.getActivePlayer());
                                campaignRebels[ci]->setBulletManager(&bulletManager);
                                campaignRebels[ci]->setGroundY((float)(screen_y - 48));
                                ci++;
                            }
                            campaignRebels[ci] = new BazookaSoldier();
                            campaignRebels[ci]->setPosition(2500.f, 240.f);
                            campaignRebels[ci]->setPlayer(characters.getActivePlayer());
                            campaignRebels[ci]->setBulletManager(&bulletManager);
                            campaignRebels[ci]->setGroundY((float)(screen_y - 48));
                            ci++;
                            campaignRebels[ci] = new ShieldedSoldier();
                            campaignRebels[ci]->setPosition(3000.f, 240.f);
                            campaignRebels[ci]->setPlayer(characters.getActivePlayer());
                            campaignRebels[ci]->setBulletManager(&bulletManager);
                            campaignRebels[ci]->setGroundY((float)(screen_y - 48));
                            ci++;
                            campaignRebelCount = ci;
                            cout << "Entered Campaign Mode" << endl;
                        }
                    }
                }
            }
        }

        // ESC - menu pe wapas
        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            if (gameMode != 0) {
                gameMode = 0;
                delete survivalGame;
                survivalGame = nullptr;
                delete campaignGame;
                campaignGame = nullptr;
                for (int i = 0; i < MAX_SURVIVAL_ENEMIES; i++) {
                    delete survivalRebels[i];
                    survivalRebels[i] = nullptr;
                }
                for (int i = 0; i < MAX_CAMPAIGN_ENEMIES; i++) {
                    delete campaignRebels[i];
                    campaignRebels[i] = nullptr;
                }
                bulletManager = BulletManager(); // clear leftover bullets when returning to menu
                survivalRebelCount = 0;
                campaignRebelCount = 0;
                cameraX = cameraY = 0;
                menu.setMenuState(0);
                menu.resetSelection();
            }
            else {
                // If in character selection, go back to start
                int menuState = menu.getMenuState();
                if (menuState == 2) {
                    menu.setMenuState(1);
                    menu.resetSelection();
                }
                else if (menuState == 1) {
                    menu.setMenuState(0);
                    menu.resetSelection();
                }
                else {
                    window.close();
                }
            }
        }

        // MENU RENDER
        if (gameMode == 0) {
            menu.updateAnimation(dt);
            int menuState = menu.getMenuState();
            if (menuState == 0) {
                menu.renderStartScreen(window);
            }
            else if (menuState == 1) {
                menu.renderCharacterSelection(window);
            }
            else if (menuState == 2) {
                menu.renderModeSelection(window);
            }
            continue;
        }

        // PLAYER MOVEMENT
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
            if (characters.getActivePlayer()->getIsGrounded()) characters.getActivePlayer()->setVelocityX(characters.getActivePlayer()->getVelocityX() * characters.getActivePlayer()->getFriction());
            else          characters.getActivePlayer()->setVelocityX(characters.getActivePlayer()->getVelocityX() * characters.getActivePlayer()->getAirFriction());
        }

        float velCap;
        if (gameMode == 2) velCap = 14.f;
        else velCap = 6.f;
        if (characters.getActivePlayer()->getVelocityX() > velCap) characters.getActivePlayer()->setVelocityX(velCap);
        if (characters.getActivePlayer()->getVelocityX() < -velCap) characters.getActivePlayer()->setVelocityX(-velCap);

        // JUMP
        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            if (characters.getActivePlayer()->getIsGrounded()) {
                characters.getActivePlayer()->setVelocityY(characters.getActivePlayer()->getJumpPower());
                characters.getActivePlayer()->setIsGrounded(false);
                jumpHeld = true;
            }
        }
        else {
            jumpHeld = false;
        }

        if (!jumpHeld && characters.getActivePlayer()->getVelocityY() < 0)
            characters.getActivePlayer()->setVelocityY(characters.getActivePlayer()->getVelocityY() * 0.5f);

        // GRAVITY
        characters.getActivePlayer()->setVelocityY(characters.getActivePlayer()->getVelocityY() + characters.getActivePlayer()->getGravity());
        if (characters.getActivePlayer()->getVelocityY() > characters.getActivePlayer()->getMaxFallSpeed())
            characters.getActivePlayer()->setVelocityY(characters.getActivePlayer()->getMaxFallSpeed());

        // Player shooting (X key)
        if (playerFireCooldown > 0.f) playerFireCooldown -= dt;
        fireHeld = Keyboard::isKeyPressed(Keyboard::X) || Keyboard::isKeyPressed(Keyboard::F);
        if (fireHeld && playerFireCooldown <= 0.f) {
            float px = characters.getActivePlayer()->getPlayerX();
            float py = characters.getActivePlayer()->getPlayerY();
            float pw = (float)characters.getActivePlayer()->getWidth();
            float ph = (float)characters.getActivePlayer()->getHeight();
            float angle = characters.getActivePlayer()->isFacingRight() ? 0.f : 3.14159f;

            bulletManager.spawnBullet(px + pw * 0.5f, py + ph * 0.45f, angle,
                playerBulletDamage, PLAYER, playerBulletSpeed, playerBulletRange);
            playerFireCooldown = playerFireInterval;
        }

        // SURVIVAL MODE
        if (gameMode == 1 && survivalGame) {
            Level* currentLevel = survivalGame->getCurrentLevel();
            if (currentLevel) {
                /// Get current state
                float pX = characters.getActivePlayer()->getPlayerX();
                float pY = characters.getActivePlayer()->getPlayerY();
                float pVelocityX = characters.getActivePlayer()->getVelocityX();
                float pVelocityY = characters.getActivePlayer()->getVelocityY();

                // Apply horizontal movement
                pX += pVelocityX;

                // Step-up mechanism: only when moving horizontally AND blocked at current height
                float stepHeight = 20.0f;
                if (pVelocityX != 0 &&
                    currentLevel->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                    float tempY = pY - stepHeight;
                    if (!currentLevel->checkCollision(pX, tempY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                        // Small obstacle — step up over it
                        pY = tempY;
                    }
                    else {
                        // Wall — revert horizontal movement
                        pX = characters.getActivePlayer()->getPlayerX();
                        pVelocityX = 0;
                    }
                }

                // Enemy wall — stop player if walking into any enemy (only when moving horizontally)
                if (pVelocityX != 0 && characters.getActivePlayer()->getIsGrounded()) {
                    float pW = (float)characters.getActivePlayer()->getWidth();
                    float pH = (float)characters.getActivePlayer()->getHeight();
                    for (int i = 0; i < survivalRebelCount; i++) {
                        if (!survivalRebels[i]) continue;
                        if (!survivalRebels[i]->getIsAlive()) continue;
                        float ex2 = survivalRebels[i]->getX(), ey2 = survivalRebels[i]->getY();
                        float ew2 = survivalRebels[i]->getWidth(), eh2 = survivalRebels[i]->getHeight();
                        if (pX < ex2 + ew2 && pX + pW > ex2 &&
                            pY < ey2 + eh2 && pY + pH > ey2) {
                            pX = characters.getActivePlayer()->getPlayerX();
                            pVelocityX = 0;
                            break;
                        }
                    }
                }

                // Apply vertical movement
                pY += pVelocityY;

                // Check vertical collision
                if (currentLevel->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                    if (pVelocityY > 0) {
                        characters.getActivePlayer()->setIsGrounded(true);
                        pVelocityY = 0;
                        while (currentLevel->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                            pY -= 1;
                        }
                    }
                    else {
                        pVelocityY = 0;
                        while (currentLevel->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                            pY += 1;
                        }
                    }
                }
                else {
                    if (pVelocityY >= 0) {
                        characters.getActivePlayer()->setIsGrounded(false);
                    }
                }

                // Update player position and velocity
                characters.getActivePlayer()->setPlayerPosition(pX, pY);
                characters.getActivePlayer()->setVelocity(pVelocityX, pVelocityY);

                // Bounds
                if (pX < 0) {
                    characters.getActivePlayer()->setPlayerX(0);
                }
                if (characters.getActivePlayer()->getPlayerX() + characters.getActivePlayer()->getWidth() > currentLevel->getLevelEnd())
                    characters.getActivePlayer()->setPlayerX(currentLevel->getLevelEnd() - characters.getActivePlayer()->getWidth());

                // Level end check
                if (characters.getActivePlayer()->getPlayerX() + characters.getActivePlayer()->getWidth() >= currentLevel->getLevelEnd() - 100)
                    currentLevel->playerReachedEnd();

                // Camera follows player smoothly, centered horizontally
                float targetCamX = characters.getActivePlayer()->getPlayerX() - screen_x / 2.0f;
                cameraX += (targetCamX - cameraX) * 0.2f;
                if (cameraX < 0)
                    cameraX = 0;
                if (cameraX > currentLevel->getLevelEnd() - screen_x)
                    cameraX = currentLevel->getLevelEnd() - screen_x;
                cameraY = 0;

                survivalGame->setCamera(cameraX, cameraY);

                // Z swaps character
                if (Keyboard::isKeyPressed(Keyboard::Z) && Delay.getElapsedTime().asSeconds() > 0.2f)
                {
                    characters.switchCharacter();
                    Delay.restart();
                }

                // Update survival enemies — full external movement pipeline
                for (int i = 0; i < survivalRebelCount; i++) {
                    if (!survivalRebels[i]) continue;
                    if (!survivalRebels[i]->getIsAlive()) continue;

                    survivalRebels[i]->setPlayer(characters.getActivePlayer());
                    survivalRebels[i]->setBulletManager(&bulletManager);
                    survivalRebels[i]->update(dt); // sets velocityX/Y only, no position change

                    float enemy_x = survivalRebels[i]->getX();
                    float enemy_y = survivalRebels[i]->getY();
                    float enemy_vx = survivalRebels[i]->getVelocityX();
                    float enemy_vy = survivalRebels[i]->getVelocityY();
                    const float enemy_w = survivalRebels[i]->getWidth();
                    const float enemy_h = survivalRebels[i]->getHeight();

                    // Apply gravity only if not grounded
                    survivalRebels[i]->applyGravity(dt);
                    enemy_vy = survivalRebels[i]->getVelocityY();

                    // --- HORIZONTAL ---
                    float newX = enemy_x + enemy_vx * dt;

                    // Check for upcoming wall and jump proactively if grounded
                    float lookAhead = 30.0f;
                    if (enemy_vx != 0 && survivalRebels[i]->getIsGrounded()) {
                        float checkX = enemy_x + (enemy_vx > 0 ? lookAhead : -lookAhead);
                        if (currentLevel->checkCollision(checkX, enemy_y, enemy_w, enemy_h)) {
                            enemy_vy = -200.0f; // jump over upcoming wall
                            survivalRebels[i]->setVelocityY(enemy_vy);
                            survivalRebels[i]->setGrounded(false);
                        }
                    }

                    // Step-up mechanism
                    float stepHeightE = 20.0f;
                    if (enemy_vx != 0 &&
                        currentLevel->checkCollision(newX, enemy_y, enemy_w, enemy_h)) {
                        float tempY = enemy_y - stepHeightE;
                        if (!currentLevel->checkCollision(newX, tempY, enemy_w, enemy_h)) {
                            // Small obstacle — step up
                            enemy_y = tempY;
                        }
                        else if (survivalRebels[i]->getIsGrounded()) {
                            // Wall and grounded — jump
                            enemy_vy = -400.0f;
                            survivalRebels[i]->setVelocityY(enemy_vy);
                            survivalRebels[i]->setGrounded(false);
                        }
                        else {
                            // Wall in air — flip direction
                            enemy_vx = -enemy_vx;
                            survivalRebels[i]->setVelocityX(enemy_vx);
                            newX = enemy_x;
                        }
                    }

                    // 1. Player boundary on x
                    float px = characters.getActivePlayer()->getPlayerX();
                    float py = characters.getActivePlayer()->getPlayerY();
                    float pw = (float)characters.getActivePlayer()->getWidth();
                    float ph = (float)characters.getActivePlayer()->getHeight();
                    if (newX < px + pw && newX + enemy_w > px &&
                        enemy_y < py + ph && enemy_y + enemy_h > py) {
                        newX = enemy_x; // stop at player edge
                    }

                    // 2. Enemy-enemy collision — push apart properly
                    for (int j = 0; j < survivalRebelCount; j++) {
                        if (j == i) continue;
                        if (!survivalRebels[j]) continue;
                        if (!survivalRebels[j]->getIsAlive()) continue;

                        float bx = survivalRebels[j]->getX();
                        float by = survivalRebels[j]->getY();
                        float bw = survivalRebels[j]->getWidth();
                        float bh = survivalRebels[j]->getHeight();

                        // vertical overlap check — sirf tab push karo jab same height pe hon
                        bool vertOverlap = (enemy_y < by + bh && enemy_y + enemy_h > by);
                        if (!vertOverlap) continue;

                        // horizontal overlap check
                        bool horzOverlap = (newX < bx + bw && newX + enemy_w > bx);
                        if (!horzOverlap) continue;

                        // push enemy i away from enemy j
                        float iCenter = newX + enemy_w / 2.f;
                        float jCenter = bx + bw / 2.f;

                        if (iCenter < jCenter) {
                            // enemy i is left — push left
                            newX = bx - enemy_w;
                        }
                        else {
                            // enemy i is right — push right
                            newX = bx + bw;
                        }
                        enemy_vx = 0.f;
                        survivalRebels[i]->setVelocityX(0.f);
                    }

                    enemy_x = newX;

                    // --- VERTICAL ---
                    enemy_y += enemy_vy * dt;

                    if (currentLevel->checkCollision(enemy_x, enemy_y, enemy_w, enemy_h)) {
                        if (enemy_vy > 0) {
                            // Landed on ground
                            enemy_vy = 0;
                            survivalRebels[i]->setGrounded(true);
                            while (currentLevel->checkCollision(enemy_x, enemy_y, enemy_w, enemy_h)) {
                                enemy_y -= 1;
                            }
                        }
                        else {
                            // Hit ceiling
                            enemy_vy = 0;
                            while (currentLevel->checkCollision(enemy_x, enemy_y, enemy_w, enemy_h)) {
                                enemy_y += 1;
                            }
                        }
                    }
                    else {
                        if (enemy_vy >= 0) {
                            survivalRebels[i]->setGrounded(false);
                        }
                    }

                    survivalRebels[i]->setVelocityY(enemy_vy);
                    survivalRebels[i]->setGrounded(survivalRebels[i]->getIsGrounded());
                    survivalRebels[i]->setPosition(enemy_x, enemy_y);
                }

                // Projectile simulation and collisions
                bulletManager.update(dt);

                int playerDamageFromProjectiles = 0;
                bulletManager.checkPlayerCollisions(
                    characters.getActivePlayer()->getPlayerX(),
                    characters.getActivePlayer()->getPlayerY(),
                    (float)characters.getActivePlayer()->getWidth(),
                    (float)characters.getActivePlayer()->getHeight(),
                    playerDamageFromProjectiles
                );
                if (playerDamageFromProjectiles > 0) {
                    characters.getActivePlayer()->takeDamage(playerDamageFromProjectiles);
                }

                for (int i = 0; i < survivalRebelCount; i++) {
                    if (!survivalRebels[i] || !survivalRebels[i]->getIsAlive()) continue;
                    int dmg = 0;
                    float hitX = 0.f, hitY = 0.f;
                    while (bulletManager.popPlayerBulletHit(
                        survivalRebels[i]->getX(),
                        survivalRebels[i]->getY(),
                        survivalRebels[i]->getWidth(),
                        survivalRebels[i]->getHeight(),
                        dmg, hitX, hitY))
                    {
                        survivalRebels[i]->takeDamage(dmg, hitX, hitY, false);
                        if (!survivalRebels[i]->getIsAlive()) break;
                    }
                }

                // Update
                survivalGame->update(dt, &characters);

                // Render
                window.clear(Color(135, 206, 235));
                survivalGame->render(window);
                for (int i = 0; i < survivalRebelCount; i++) {
                    if (survivalRebels[i])
                        survivalRebels[i]->render(window, cameraX, cameraY);
                }
                bulletManager.render(window, cameraX, cameraY);
                characters.getActivePlayer()->render(window, cameraX, cameraY);
                window.display();
            }
            else {
                gameMode = 0;
            }
        }

        // CAMPAIGN MODE
        else if (gameMode == 2 && campaignGame) {
            CampaignLevel* campaignLevel = campaignGame->getCampaignLevel();
            if (campaignLevel) {
                float pX = characters.getActivePlayer()->getPlayerX();
                float pY = characters.getActivePlayer()->getPlayerY();
                float pVelocityX = characters.getActivePlayer()->getVelocityX();
                float pVelocityY = characters.getActivePlayer()->getVelocityY();

                // Apply horizontal movement
                pX += pVelocityX;

                // Step-up mechanism
                float stepHeight = 20.0f;
                if (pVelocityX != 0 &&
                    campaignLevel->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                    float tempY = pY - stepHeight;
                    if (!campaignLevel->checkCollision(pX, tempY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                        pY = tempY;
                    }
                    else {
                        pX = characters.getActivePlayer()->getPlayerX();
                        pVelocityX = 0;
                    }
                }

                // Apply vertical movement
                pY += pVelocityY;

                // Check vertical collision with blocks
                if (campaignLevel->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                    if (pVelocityY > 0) {
                        characters.getActivePlayer()->setIsGrounded(true);
                        pVelocityY = 0;
                        while (campaignLevel->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                            pY -= 1;
                        }
                    }
                    else {
                        pVelocityY = 0;
                        while (campaignLevel->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                            pY += 1;
                        }
                    }
                }
                else {
                    if (pVelocityY >= 0) characters.getActivePlayer()->setIsGrounded(false);
                }

                // Screen bottom boundary (fallback)
                if (pY + characters.getActivePlayer()->getHeight() > screen_y) {
                    pY = screen_y - characters.getActivePlayer()->getHeight();
                    pVelocityY = 0;
                    characters.getActivePlayer()->setIsGrounded(true);
                }

                // Left bound
                if (pX < 0) pX = 0;

                characters.getActivePlayer()->setPlayerPosition(pX, pY);
                characters.getActivePlayer()->setVelocity(pVelocityX, pVelocityY);

                // Camera
                float targetCamX = characters.getActivePlayer()->getPlayerX() - screen_x / 2.0f;
                cameraX += (targetCamX - cameraX) * 0.2f;
                if (cameraX < 0) cameraX = 0;
                cameraY = 0;

                campaignGame->setCamera(cameraX, cameraY);

                // Z swaps character
                if (Keyboard::isKeyPressed(Keyboard::Z) && Delay.getElapsedTime().asSeconds() > 0.2f)
                {
                    characters.switchCharacter();
                    Delay.restart();
                }

                // Update
                campaignGame->update(dt, &characters);

                // Update campaign enemies and let them auto fire
                for (int i = 0; i < campaignRebelCount; i++) {
                    if (!campaignRebels[i]) continue;
                    if (!campaignRebels[i]->getIsAlive()) continue;

                    campaignRebels[i]->setPlayer(characters.getActivePlayer());
                    campaignRebels[i]->setBulletManager(&bulletManager);
                    campaignRebels[i]->update(dt);

                    float enemyX = campaignRebels[i]->getX();
                    float enemyY = campaignRebels[i]->getY();
                    float enemyVx = campaignRebels[i]->getVelocityX();
                    float enemyVy = campaignRebels[i]->getVelocityY();
                    const float enemyW = campaignRebels[i]->getWidth();
                    const float enemyH = campaignRebels[i]->getHeight();

                    campaignRebels[i]->applyGravity(dt);
                    enemyVy = campaignRebels[i]->getVelocityY();

                    // Horizontal collision with terrain
                    float newX = enemyX + enemyVx * dt;
                    if (campaignLevel->checkCollision(newX, enemyY, enemyW, enemyH)) {
                        enemyVx = -enemyVx;
                        campaignRebels[i]->setVelocityX(enemyVx);
                        newX = enemyX;
                    }
                    enemyX = newX;

                    // Vertical collision with terrain
                    enemyY += enemyVy * dt;
                    if (campaignLevel->checkCollision(enemyX, enemyY, enemyW, enemyH)) {
                        if (enemyVy > 0) {
                            enemyVy = 0.f;
                            campaignRebels[i]->setGrounded(true);
                            while (campaignLevel->checkCollision(enemyX, enemyY, enemyW, enemyH)) {
                                enemyY -= 1.f;
                            }
                        }
                        else {
                            enemyVy = 0.f;
                            while (campaignLevel->checkCollision(enemyX, enemyY, enemyW, enemyH)) {
                                enemyY += 1.f;
                            }
                        }
                    }
                    else if (enemyVy >= 0.f) {
                        campaignRebels[i]->setGrounded(false);
                    }

                    campaignRebels[i]->setVelocityY(enemyVy);
                    campaignRebels[i]->setPosition(enemyX, enemyY);
                }

                bulletManager.update(dt);

                int playerDamageFromProjectiles = 0;
                bulletManager.checkPlayerCollisions(
                    characters.getActivePlayer()->getPlayerX(),
                    characters.getActivePlayer()->getPlayerY(),
                    (float)characters.getActivePlayer()->getWidth(),
                    (float)characters.getActivePlayer()->getHeight(),
                    playerDamageFromProjectiles
                );
                if (playerDamageFromProjectiles > 0) {
                    characters.getActivePlayer()->takeDamage(playerDamageFromProjectiles);
                }

                // Player bullets hit campaign enemies
                for (int i = 0; i < campaignRebelCount; i++) {
                    if (!campaignRebels[i] || !campaignRebels[i]->getIsAlive()) continue;
                    int dmg = 0;
                    float hitX = 0.f, hitY = 0.f;
                    while (bulletManager.popPlayerBulletHit(
                        campaignRebels[i]->getX(),
                        campaignRebels[i]->getY(),
                        campaignRebels[i]->getWidth(),
                        campaignRebels[i]->getHeight(),
                        dmg, hitX, hitY))
                    {
                        campaignRebels[i]->takeDamage(dmg, hitX, hitY, false);
                        if (!campaignRebels[i]->getIsAlive()) break;
                    }
                }

                // Render
                window.clear(Color(135, 206, 235));
                campaignGame->render(window);
                for (int i = 0; i < campaignRebelCount; i++) {
                    if (campaignRebels[i] && campaignRebels[i]->getIsAlive())
                        campaignRebels[i]->render(window, cameraX, cameraY);
                }
                bulletManager.render(window, cameraX, cameraY);
                characters.getActivePlayer()->render(window, cameraX, cameraY);
                window.display();
            }
            else {
                gameMode = 0;
            }
        }
    }

    delete survivalGame;
    delete campaignGame;
    for (int i = 0; i < MAX_SURVIVAL_ENEMIES; i++) delete survivalRebels[i];
    for (int i = 0; i < MAX_CAMPAIGN_ENEMIES; i++) delete campaignRebels[i];
    return 0;
}