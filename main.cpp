#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Header.h"
#include "Player.h"
#include "Weapon.h"
#include "Enemy.h"
#include "menu.h"

using namespace sf;
using namespace std;

int screen_x = 1600;
int screen_y = 900;

int main() {
    RenderWindow window(VideoMode(screen_x, screen_y), "Metal Slug", Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    float cameraX = 0, cameraY = 0;

    // -------------------------
    // GAME MODE
    // 0=Menu, 1=survival, 2=campaign
    // -------------------------
    int gameMode = 0;

    // -------------------------
    // PLAYER SELECTION
	// 0=pending 1=done
    // -------------------------
    bool playerSelection = false;


    // -------------------------
    // MENU SYSTEM
    // -------------------------
    Menu menu(screen_x, screen_y);

    // -------------------------
    // SURVIVAL MODE VARIABLES
    // -------------------------
    Level* currentLevel = nullptr;
    int currentLevelNum = 1;

    // -------------------------
    // CAMPAIGN MODE VARIABLES
    // -------------------------
    CampaignLevel* campaignLevel = nullptr;

    // -------------------------
    // PLAYER SETUP
    // -------------------------

    float moveAcceleration = 0.6f;
    float friction = 0.80f;
    float airFriction = 0.95f;
    float jumpPower = -22.0f;
    float gravity = 1.0f;
    float maxFallSpeed = 25.0f;

    bool onGround = false;
    bool jumpHeld = false;


    // -------------------------
    // CHARACTER MANAGER
    // -------------------------
    CharacterManager characters;

    Clock clock;
    Clock Delay;
    Event ev;

    // -------------------------
    // GAME LOOP
    // -------------------------
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (window.pollEvent(ev)) {
            if (ev.type == Event::Closed)
                window.close();

            // -------------------------
            // MENU INPUT
            // -------------------------
            if (gameMode == 0 && ev.type == Event::KeyPressed) {

                if (ev.key.code == Keyboard::Num1) {
                    gameMode = 1;
                    currentLevelNum = 1;
                    currentLevel = new Level1();
                    currentLevel->generateBiomes();
                    characters.getActivePlayer()->setPlayerPosition(currentLevel->getPlayerSpawnX(), currentLevel->getPlayerSpawnY());
					characters.getActivePlayer()->setVelocity(0, 0);
                    onGround = false;
                    cameraX = cameraY = 0;
                    Delay.restart();
					cout << "Entered Survival Mode" << endl;
                }
				else if (ev.key.code == Keyboard::Num2) {
					gameMode = 2;
					campaignLevel = new CampaignLevel(3); // Normal profile
					for (int i = 0; i < 8; i++)
						campaignLevel->generateChunk(i * 16);
					characters.getActivePlayer()->setPlayerPosition(200, 50);
					characters.getActivePlayer()->setVelocity(0, 0);
					onGround = false;
					cameraX = cameraY = 0;
					Delay.restart();
					cout << "Entered Campaign Mode" << endl;
				}
			}

			// -------------------------
			// PLAYER SELECTION MENU INPUT
			// -------------------------
			if (playerSelection == false && gameMode != 0 && ev.type == Event::KeyPressed && Delay.getElapsedTime().asSeconds() > 0.2f) {

				if (ev.key.code == Keyboard::Num1) {
					characters.switchCharacterToIndex(0);
                    playerSelection = true;
					cout << "Selected Macro Rossi" << endl;
				}
				else if (ev.key.code == Keyboard::Num2) {
					characters.switchCharacterToIndex(1);
                    playerSelection = true;
					cout << "Selected Tarma Roving" << endl;
				}
				else if (ev.key.code == Keyboard::Num3) {
					characters.switchCharacterToIndex(2);
                    playerSelection = true;
					cout << "Selected Eri Kasamoto" << endl;
				}
				else if (ev.key.code == Keyboard::Num4) {
					characters.switchCharacterToIndex(3);
					playerSelection = true;
					cout << "Selected Fiolina Germi" << endl;
				}
			}
        }

        // ESC - menu pe wapas
        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            if (gameMode != 0) {
                gameMode = 0;
                delete currentLevel;
                currentLevel = nullptr;
                delete campaignLevel;
                campaignLevel = nullptr;
                cameraX = cameraY = 0;
            }
            else {
                window.close();
            }
        }

        // =========================================
        // MENU RENDER
        // =========================================
        if (gameMode == 0) {
            menu.updateAnimation(dt);
            menu.renderGameModeMenu(window);
            continue;
        }

        // =========================================
		// Player Selection Menu Render
        // =========================================
        if (gameMode != 0 && playerSelection == false) {
            menu.updateAnimation(dt);
            menu.renderPlayerSelectionMenu(window);
            continue;
        }

		// =========================================
		// PLAYER INPUT (dono modes ke liye same)
		// =========================================
		if (Keyboard::isKeyPressed(Keyboard::Left)) {
			characters.getActivePlayer()->moveLeft();
            if (characters.getActivePlayer()->isFacingRight()) {
               characters.getActivePlayer()->flipToLeft();
            }
		}
		else if (Keyboard::isKeyPressed(Keyboard::Right)) {
			characters.getActivePlayer()->moveRight();
            if (!characters.getActivePlayer()->isFacingRight()) {
                characters.getActivePlayer()->flipToRight();
            }
        }
        else {
            if (onGround) characters.getActivePlayer()->setVelocityX(characters.getActivePlayer()->getVelocityX() * friction);
            else          characters.getActivePlayer()->setVelocityX(characters.getActivePlayer()->getVelocityX() * airFriction);
        }

        if (characters.getActivePlayer()->getVelocityX() > 6) characters.getActivePlayer()->setVelocityX(6);
        if (characters.getActivePlayer()->getVelocityX() < -6) characters.getActivePlayer()->setVelocityX(-6);

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

        // =========================================
        // SURVIVAL MODE
        // =========================================
        if (gameMode == 1 && currentLevel) {

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

            // Level switch
            if (currentLevel->checkLevelComplete()) {
                delete currentLevel;
                currentLevel = nullptr;

                if (currentLevelNum == 1) {
                    currentLevelNum = 2;
                    currentLevel = new Level2();
                }
                else if (currentLevelNum == 2) {
                    currentLevelNum = 3;
                    currentLevel = new Level3();
                }
                else {
                    gameMode = 0;
                }

                if (currentLevel) {
                    currentLevel->generateBiomes();
                    characters.getActivePlayer()->setPlayerPosition(currentLevel->getPlayerSpawnX(), currentLevel->getPlayerSpawnY());
                    pVelocityX = pVelocityY = 0;
                    onGround = false;
                    cameraX = cameraY = 0;
                }
            }

            // Camera follows player smoothly, centered horizontally
            if (currentLevel) {
                float targetCamX = characters.getActivePlayer()->getPlayerX() - screen_x / 2.0f;
                cameraX += (targetCamX - cameraX) * 0.2f;
                if (cameraX < 0)
                    cameraX = 0;
                if (cameraX > currentLevel->getLevelEnd() - screen_x)
                    cameraX = currentLevel->getLevelEnd() - screen_x;
            }
            cameraY = 0;

            // Z swaps character
            if (Keyboard::isKeyPressed(Keyboard::Z) && Delay.getElapsedTime().asSeconds() > 0.2f)
            {
                characters.switchCharacter();
                Delay.restart();
            }


            // Update
            currentLevel->update(dt);
            characters.getActivePlayer()->updateAnimation(dt);

            // Render
            window.clear(Color(135, 206, 235));
            currentLevel->render(window, cameraX, cameraY);
            characters.getActivePlayer()->render(window, cameraX, cameraY);
            window.display();
        }

        // =========================================
        // CAMPAIGN MODE
        // =========================================
        else if (gameMode == 2 && campaignLevel) {

            // X move
            characters.getActivePlayer()->setPlayerX(characters.getActivePlayer()->getPlayerX() + characters.getActivePlayer()->getVelocityX());

            // Y move
            characters.getActivePlayer()->setPlayerY(characters.getActivePlayer()->getPlayerY() + characters.getActivePlayer()->getVelocityY());

            // Screen bottom boundary
            if (characters.getActivePlayer()->getPlayerY() + characters.getActivePlayer()->getHeight() > screen_y) {
                characters.getActivePlayer()->setPlayerY(screen_y - characters.getActivePlayer()->getHeight());
                characters.getActivePlayer()->setVelocityY(0);
                onGround = true;
            }

            // Left bound
            if (characters.getActivePlayer()->getPlayerX() < 0) 
                characters.getActivePlayer()->setPlayerX(0);

            // Campaign update - naye chunks auto generate honge
            campaignLevel->update(characters.getActivePlayer()->getPlayerX());
            characters.getActivePlayer()->updateAnimation(dt);

            // Camera
            float targetCamX = characters.getActivePlayer()->getPlayerX() - screen_x / 2.0f;
            cameraX += (targetCamX - cameraX) * 0.2f;
            if (cameraX < 0) cameraX = 0;
            cameraY = 0;

            // Z swaps character
            if (Keyboard::isKeyPressed(Keyboard::Z) && Delay.getElapsedTime().asSeconds() > 0.2f)
            {
                characters.switchCharacter();
                Delay.restart();
            }

            // Render
            window.clear(Color(135, 206, 235));
            campaignLevel->render(window, cameraX, cameraY);
            characters.getActivePlayer()->render(window, cameraX, cameraY);
            window.display();
        }
    }

    delete currentLevel;
    delete campaignLevel;
    return 0;
}
