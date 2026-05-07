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
    static const int MAX_SURVIVAL_ENEMIES = 12;
    Enemy* survivalRebels[MAX_SURVIVAL_ENEMIES] = {};
    int survivalRebelCount = 0;

    // PLAYER SETUP

    float moveAcceleration = 0.6f;
    float friction = 0.80f;
    float airFriction = 0.95f;
    float jumpPower = -45.0f;
    float gravity = 50.0f;
    float maxFallSpeed = 25.0f;

    bool onGround = true;
    bool jumpHeld = false;


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
                            survivalGame = new SurvivalGame(screen_x, screen_y);
                            survivalGame->setCharManager(&characters);
                            survivalGame->start();
                            characters.getActivePlayer()->setPlayerPosition(survivalGame->getCurrentLevel()->getPlayerSpawnX(), 700.f);
                            characters.getActivePlayer()->setVelocity(0, 0);
                            onGround = true;
                            cameraX = cameraY = 0;
                            Delay.restart();
                            // Spawn all infantry enemy types in survival mode
                            for (int i = 0; i < MAX_SURVIVAL_ENEMIES; i++) {
                                delete survivalRebels[i];
                                survivalRebels[i] = nullptr;
                            }
                            int ei = 0;
                            // 4 Rebel Soldiers
                            for (int i = 0; i < 4; i++) {
                                survivalRebels[ei] = new RebelSoldier();
                                survivalRebels[ei]->setPosition(300.f + i * 350.f, 200.f);
                                survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                                survivalRebels[ei]->setGroundY((float)(screen_y - 48));
                                ei++;
                            }
                            // 3 Grenade Soldiers
                            for (int i = 0; i < 3; i++) {
                                survivalRebels[ei] = new GrenadeSoldier();
                                survivalRebels[ei]->setPosition(500.f + i * 400.f, 200.f);
                                survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                                survivalRebels[ei]->setGroundY((float)(screen_y - 48));
                                ei++;
                            }
                            // 3 Bazooka Soldiers
                            for (int i = 0; i < 3; i++) {
                                survivalRebels[ei] = new BazookaSoldier();
                                survivalRebels[ei]->setPosition(600.f + i * 400.f, 200.f);
                                survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                                survivalRebels[ei]->setGroundY((float)(screen_y - 48));
                                ei++;
                            }
                            // 2 Shielded Soldiers
                            for (int i = 0; i < 2; i++) {
                                survivalRebels[ei] = new ShieldedSoldier();
                                survivalRebels[ei]->setPosition(800.f + i * 500.f, 200.f);
                                survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                                survivalRebels[ei]->setGroundY((float)(screen_y - 48));
                                ei++;
                            }
                            survivalRebelCount = ei;
                            cout << "Entered Survival Mode" << endl;
                        }
                        else if (modeSelection == 1) {
                            gameMode = 2;
                            campaignGame = new CampaignGame(screen_x, screen_y);
                            campaignGame->setCharManager(&characters);
                            campaignGame->start();
                            characters.getActivePlayer()->setPlayerPosition(200, 50);
                            characters.getActivePlayer()->setVelocity(0, 0);
                            onGround = false;
                            cameraX = cameraY = 0;
                            Delay.restart();
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
                survivalRebelCount = 0;
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

		// PLAYER INPUT (dono modes ke liye same)
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
            if (onGround) characters.getActivePlayer()->setVelocityX(characters.getActivePlayer()->getVelocityX() * friction);
            else          characters.getActivePlayer()->setVelocityX(characters.getActivePlayer()->getVelocityX() * airFriction);
        }

        float velCap;
        if (gameMode == 2) velCap = 14.f;
        else velCap = 6.f;
        if (characters.getActivePlayer()->getVelocityX() >  velCap) characters.getActivePlayer()->setVelocityX( velCap);
        if (characters.getActivePlayer()->getVelocityX() < -velCap) characters.getActivePlayer()->setVelocityX(-velCap);

        // JUMP
        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            characters.getActivePlayer()->setVelocityY(jumpPower);
            onGround = false;
            jumpHeld = true;
        }
        else {
            jumpHeld = false;
        }

       if (!jumpHeld && characters.getActivePlayer()->getVelocityY() < 0)
       characters.getActivePlayer()->setVelocityY(characters.getActivePlayer()->getVelocityY() * 0.5f);

        // GRAVITY
        characters.getActivePlayer()->setVelocityY(characters.getActivePlayer()->getVelocityY() + gravity);
        if (characters.getActivePlayer()->getVelocityY() > maxFallSpeed)
            characters.getActivePlayer()->setVelocityY(maxFallSpeed);

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
                if (pVelocityX != 0 && onGround) {
                    float pW = (float)characters.getActivePlayer()->getWidth();
                    float pH = (float)characters.getActivePlayer()->getHeight();
                    for (int i = 0; i < survivalRebelCount; i++) {
                        if (!survivalRebels[i]) continue;
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
                        onGround = true;
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
                    if (pVelocityY > 0) {
                        onGround = false; // falling through air
                    }
                }

                // Screen bottom boundary (fallback)
                if (pY + characters.getActivePlayer()->getHeight() > screen_y) {
                    pY = screen_y - characters.getActivePlayer()->getHeight();
                    pVelocityY = 0;
                    onGround = true;
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
                    survivalRebels[i]->setPlayer(characters.getActivePlayer());
                    survivalRebels[i]->update(dt); // sets velocityX/Y only, no position change
                    survivalRebels[i]->applyGravity(dt); // accumulate velocityY externally

                    float enemy_x  = survivalRebels[i]->getX();
                    float enemy_y  = survivalRebels[i]->getY();
                    float enemy_vx = survivalRebels[i]->getVelocityX();
                    float enemy_vy = survivalRebels[i]->getVelocityY();
                    const float enemy_w = survivalRebels[i]->getWidth();
                    const float enemy_h = survivalRebels[i]->getHeight();

                    // horizontal
                    float newX = enemy_x + enemy_vx * dt;

                    // 1. block collision on x — use same method as player
                    if (currentLevel->checkCollision(newX, enemy_y, enemy_w, enemy_h)) {
                        // hit a wall
                        if (survivalRebels[i]->getIsTargetingPlayer()) {
                            // chasing player: jump to pass the wall
                            survivalRebels[i]->setVelocityY(-700.0f); // jump
                            survivalRebels[i]->setGrounded(false);
                            // continue moving
                        }
                        else {
                            // patrolling: flip direction
                            enemy_vx = -enemy_vx;
                            survivalRebels[i]->setVelocityX(enemy_vx);
                            newX = enemy_x; // stay put this frame
                        }
                    }

                    // 2. player boundary on x — revert if would overlap player
                    float px = characters.getActivePlayer()->getPlayerX();
                    float py = characters.getActivePlayer()->getPlayerY();
                    float pw = (float)characters.getActivePlayer()->getWidth();
                    float ph = (float)characters.getActivePlayer()->getHeight();
                    if (newX < px + pw && newX + enemy_w > px &&
                        enemy_y        < py + ph && enemy_y        + enemy_h > py) {
                        newX = enemy_x; // revert — enemy stops at player edge
                    }

                    // 3. enemy-enemy boundary on x — revert against all j < i
                    for (int j = 0; j < i; j++) {
                        if (!survivalRebels[j]) continue;
                        float bx2 = survivalRebels[j]->getX();
                        float by2 = survivalRebels[j]->getY();
                        float bw2 = survivalRebels[j]->getWidth();
                        float bh2 = survivalRebels[j]->getHeight();
                        if (newX < bx2 + bw2 && newX + enemy_w > bx2 &&
                            enemy_y        < by2 + bh2 && enemy_y        + enemy_h > by2) {
                            newX = enemy_x; // revert — enemy stops at neighbour edge
                        }
                    }

                    enemy_x = newX;
                    // --- VERTICAL ---
                    enemy_y += enemy_vy * dt;

                    // Screen bottom boundary (enforce before collision check)
                    if (enemy_y + enemy_h > (float)screen_y) {
                        enemy_y = (float)(screen_y - enemy_h);
                        enemy_vy = 0;
                        onGround = true;
                    }

                    // 4. player boundary on y — push enemy away if overlapping player vertically
                    if (enemy_x < px + pw && enemy_x + enemy_w > px &&
                        enemy_y < py + ph && enemy_y + enemy_h > py) {
                        // Calculate overlap on both axes
                        float overlapLeft = (px + pw) - enemy_x;
                        float overlapRight = (enemy_x + enemy_w) - px;
                        float overlapTop = (py + ph) - enemy_y;
                        float overlapBottom = (enemy_y + enemy_h) - py;

                        // Find smallest overlap and push enemy that direction
                        float minOverlap = min(min(overlapLeft, overlapRight), min(overlapTop, overlapBottom));
                        if (minOverlap == overlapLeft) enemy_x -= overlapLeft;
                        else if (minOverlap == overlapRight) enemy_x += overlapRight;
                        else if (minOverlap == overlapTop) enemy_y -= overlapTop;
                        else enemy_y += overlapBottom;
                    }

                    // Use same collision method as player
                    if (currentLevel->checkCollision(enemy_x, enemy_y, enemy_w, enemy_h)) {
                        if (enemy_vy > 0) {
                            // Falling down - landed on ground
                            enemy_vy = 0;
                            onGround = true;
                            // Move up until no collision
                            while (currentLevel->checkCollision(enemy_x, enemy_y, enemy_w, enemy_h)) {
                                enemy_y -= 1;
                            }
                        }
                        else {
                            // Moving up - hit ceiling
                            enemy_vy = 0;
                            // Move down until no collision
                            while (currentLevel->checkCollision(enemy_x, enemy_y, enemy_w, enemy_h)) {
                                enemy_y += 1;
                            }
                        }
                    }
                    else {
                        if (enemy_vy >= 0) {
                            onGround = false;
                        }
                    }

                    // Debug output for first enemy only
                    // if (i == 0) {
                    //     cout << "Enemy 0: pos(" << enemy_x << "," << enemy_y << ") vel(" << enemy_vx << "," << enemy_vy << ") grounded=" << onGround << " size(" << enemy_w << "," << enemy_h << ")" << endl;
                    // }

                    survivalRebels[i]->setVelocityY(enemy_vy);
                    survivalRebels[i]->setGrounded(onGround);
                    survivalRebels[i]->setPosition(enemy_x, enemy_y);
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
                        onGround = true;
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
                    if (pVelocityY >= 0) onGround = false;
                }

                // Screen bottom boundary (fallback)
                if (pY + characters.getActivePlayer()->getHeight() > screen_y) {
                    pY = screen_y - characters.getActivePlayer()->getHeight();
                    pVelocityY = 0;
                    onGround = true;
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

                // Render
                window.clear(Color(135, 206, 235));
                campaignGame->render(window);
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
    return 0;
}
