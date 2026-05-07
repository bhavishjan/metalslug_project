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

int screen_x = 1600;
int screen_y = 900;

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

    // Survival mode enemies
    static const int MAX_REBELS = 5;
    RebelSoldier* survivalRebels[MAX_REBELS] = {};
    int survivalRebelCount = 0;

    // PLAYER SETUP

    float moveAcceleration = 0.6f;
    float friction = 0.80f;
    float airFriction = 0.95f;
    float jumpPower = -22.0f;
    float gravity = 1.0f;
    float maxFallSpeed = 25.0f;

    bool onGround = false;
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
                            characters.getActivePlayer()->setPlayerPosition(survivalGame->getCurrentLevel()->getPlayerSpawnX(), survivalGame->getCurrentLevel()->getPlayerSpawnY());
                            characters.getActivePlayer()->setVelocity(0, 0);
                            onGround = false;
                            cameraX = cameraY = 0;
                            Delay.restart();
                            // Spawn rebel soldiers in survival mode
                            for (int i = 0; i < MAX_REBELS; i++) {
                                delete survivalRebels[i];
                                survivalRebels[i] = new RebelSoldier();
                                survivalRebels[i]->setPosition(400.f + i * 300.f, 100.f);
                                survivalRebels[i]->setPlayer(characters.getActivePlayer());
                                survivalRebels[i]->setGroundY((float)(screen_y - 48));
                            }
                            survivalRebelCount = MAX_REBELS;
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
                for (int i = 0; i < MAX_REBELS; i++) {
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
            if (onGround) {
                characters.getActivePlayer()->setVelocityY(jumpPower);
                onGround = false;
                jumpHeld = true;
            }
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

        onGround = false;

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
                    if (pVelocityY >= 0) {
                        onGround = false;
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

                // Update survival enemies
                for (int i = 0; i < survivalRebelCount; i++) {
                    if (!survivalRebels[i]) continue;
                    survivalRebels[i]->setPlayer(characters.getActivePlayer());
                    survivalRebels[i]->update(dt);

                    // apply vertical movement externally (gravity was only accumulated)
                    float ex  = survivalRebels[i]->getX();
                    float ey  = survivalRebels[i]->getY();
                    float evy = survivalRebels[i]->getVelocityY();
                    float ew  = 32.f, eh = 48.f;

                    // horizontal block resolve
                    float evx = 0.f;
                    bool  eGnd = false;
                    currentLevel->resolveCollisions(ex, ey, ew, eh, evx, evy, eGnd);

                    // move y then vertical resolve
                    ey += evy * dt;
                    float evx2 = 0.f, evy2 = evy;
                    bool  eGnd2 = false;
                    currentLevel->resolveCollisions(ex, ey, ew, eh, evx2, evy2, eGnd2);

                    // screen bottom fallback
                    if (ey + eh > (float)screen_y) {
                        ey = (float)(screen_y - eh);
                        evy2 = 0.f;
                        eGnd2 = true;
                    }

                    survivalRebels[i]->setPosition(ex, ey);
                    survivalRebels[i]->setVelocityY(evy2);
                    survivalRebels[i]->setGrounded(eGnd2);

                    // stop enemy at player boundary — neither body moves the other
                    survivalRebels[i]->checkPlayerCollision(characters.getActivePlayer());
                }

                // enemy-enemy AABB separation — 3 iterations for stability
                for (int iter = 0; iter < 3; iter++) {
                    for (int i = 0; i < survivalRebelCount; i++) {
                        if (!survivalRebels[i]) continue;
                        for (int j = i + 1; j < survivalRebelCount; j++) {
                            if (!survivalRebels[j]) continue;
                            float ax = survivalRebels[i]->getX(), ay = survivalRebels[i]->getY();
                            float bx = survivalRebels[j]->getX(), by = survivalRebels[j]->getY();
                            float ew = 32.f, eh = 48.f;
                            if (!(ax < bx + ew && ax + ew > bx && ay < by + eh && ay + eh > by)) continue;
                            float olL = (ax + ew) - bx;
                            float olR = (bx + ew) - ax;
                            float half;
                            if (olL < olR) {
                                half = olL / 2.f;
                                survivalRebels[i]->setPosition(ax - half, ay);
                                survivalRebels[j]->setPosition(bx + half, by);
                            }
                            else {
                                half = olR / 2.f;
                                survivalRebels[i]->setPosition(ax + half, ay);
                                survivalRebels[j]->setPosition(bx - half, by);
                            }
                        }
                    }
                    // re-snap both enemies to terrain after each separation iteration
                    for (int i = 0; i < survivalRebelCount; i++) {
                        if (!survivalRebels[i]) continue;
                        float ex = survivalRebels[i]->getX(), ey = survivalRebels[i]->getY();
                        float evx = 0.f, evy = 0.f; bool eg = false;
                        currentLevel->resolveCollisions(ex, ey, 32.f, 48.f, evx, evy, eg);
                        survivalRebels[i]->setPosition(ex, ey);
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
    for (int i = 0; i < MAX_REBELS; i++) delete survivalRebels[i];
    return 0;
}
