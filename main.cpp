#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Header.h"
#include "Player.h"
#include "Weapon.h"

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
    // GAME MODE MENU SHAPES
    // -------------------------
    RectangleShape menuBg(Vector2f(screen_x, screen_y));
    menuBg.setFillColor(Color::Blue);

    RectangleShape titleBar(Vector2f(600, 90));
    titleBar.setFillColor(Color::Red);
    titleBar.setPosition(500, 150);

    RectangleShape survivalBtn(Vector2f(500, 70));
    survivalBtn.setFillColor(Color::Green);
    survivalBtn.setPosition(550, 350);
    survivalBtn.setOutlineThickness(3);
    survivalBtn.setOutlineColor(Color::White);

    RectangleShape campaignBtn(Vector2f(500, 70));
    campaignBtn.setFillColor(Color::Green);
    campaignBtn.setPosition(550, 460);
    campaignBtn.setOutlineThickness(3);
    campaignBtn.setOutlineColor(Color::White);

    // Small indicator boxes inside buttons
    RectangleShape key1(Vector2f(40, 40));
    key1.setFillColor(Color::Red);
    key1.setPosition(570, 365);

    RectangleShape key2(Vector2f(40, 40));
    key2.setFillColor(Color::Red);
    key2.setPosition(570, 475);

    // text in game mode menu boxes
    // Loading font
    Font menuFont;
    menuFont.loadFromFile("arial.ttf");

    // Title text
    Text titleText;
    titleText.setFont(menuFont);
    titleText.setString("SELECT MODE:");
    titleText.setCharacterSize(50);
    titleText.setFillColor(Color::White);
    titleText.setPosition(600, 165);

    // Survival button text
    Text survivalText;
    survivalText.setFont(menuFont);
    survivalText.setString("1 - SURVIVAL MODE");
    survivalText.setCharacterSize(30);
    survivalText.setFillColor(Color::White);
    survivalText.setPosition(630, 370);

    // Campaign button text
    Text campaignText;
    campaignText.setFont(menuFont);
    campaignText.setString("2 - CAMPAIGN MODE");
    campaignText.setCharacterSize(30);
    campaignText.setFillColor(Color::White);
    campaignText.setPosition(630, 480);
    // -------------------------
    // PLAYER SELECTION MENU SHAPES
    // -------------------------
    RectangleShape playerSelectMenuBg(Vector2f(screen_x, screen_y));
    playerSelectMenuBg.setFillColor(Color::Blue);

    RectangleShape playerSelectTitleBar(Vector2f(700, 90));
    playerSelectTitleBar.setFillColor(Color::Red);
    playerSelectTitleBar.setPosition(450, 150);

    RectangleShape macroRossiBtn(Vector2f(500, 70));
    macroRossiBtn.setFillColor(Color::Green);
    macroRossiBtn.setPosition(550, 350);
    macroRossiBtn.setOutlineThickness(3);
    macroRossiBtn.setOutlineColor(Color::White);

    RectangleShape tarmaRovingBtn(Vector2f(500, 70));
    tarmaRovingBtn.setFillColor(Color::Green);
    tarmaRovingBtn.setPosition(550, 460);
    tarmaRovingBtn.setOutlineThickness(3);
    tarmaRovingBtn.setOutlineColor(Color::White);

    RectangleShape eriKasamotoBtn(Vector2f(500, 70));
    eriKasamotoBtn.setFillColor(Color::Green);
    eriKasamotoBtn.setPosition(550, 570);
    eriKasamotoBtn.setOutlineThickness(3);
    eriKasamotoBtn.setOutlineColor(Color::White);

    RectangleShape fiolinaGermiBtn(Vector2f(500, 70));
    fiolinaGermiBtn.setFillColor(Color::Green);
    fiolinaGermiBtn.setPosition(550, 680);
    fiolinaGermiBtn.setOutlineThickness(3);
    fiolinaGermiBtn.setOutlineColor(Color::White);

    // Small indicator boxes inside buttons
    RectangleShape playerSelectKey1(Vector2f(40, 40));
    playerSelectKey1.setFillColor(Color::Red);
    playerSelectKey1.setPosition(570, 365);

    RectangleShape playerSelectKey2(Vector2f(40, 40));
    playerSelectKey2.setFillColor(Color::Red);
    playerSelectKey2.setPosition(570, 475);

    RectangleShape playerSelectKey3(Vector2f(40, 40));
    playerSelectKey3.setFillColor(Color::Red);
    playerSelectKey3.setPosition(570, 585);

    RectangleShape playerSelectKey4(Vector2f(40, 40));
    playerSelectKey4.setFillColor(Color::Red);
    playerSelectKey4.setPosition(570, 695);

    // text in player selection menu boxes
    // Title text
    Text playerSelectTitleText;
    playerSelectTitleText.setFont(menuFont);
    playerSelectTitleText.setString("CHOOSE CHARACTER");
    playerSelectTitleText.setCharacterSize(50);
    playerSelectTitleText.setFillColor(Color::White);
    playerSelectTitleText.setPosition(540, 165);

    // Macro Rossi text
    Text macroRossiText;
    macroRossiText.setFont(menuFont);
    macroRossiText.setString("1 - Macro Rossi");
    macroRossiText.setCharacterSize(30);
    macroRossiText.setFillColor(Color::White);
    macroRossiText.setPosition(630, 365);

    // Tarma Roving text
    Text tarmaRovingText;
    tarmaRovingText.setFont(menuFont);
    tarmaRovingText.setString("2 - Tarma Roving");
    tarmaRovingText.setCharacterSize(30);
    tarmaRovingText.setFillColor(Color::White);
    tarmaRovingText.setPosition(630, 480);

    // Eri Kasamoto text
    Text eriKasamotoText;
    eriKasamotoText.setFont(menuFont);
    eriKasamotoText.setString("3 - Eri Kasamoto");
    eriKasamotoText.setCharacterSize(30);
    eriKasamotoText.setFillColor(Color::White);
    eriKasamotoText.setPosition(630, 590);

    // Fiolina Germi text
    Text fiolinaGermiText;
    fiolinaGermiText.setFont(menuFont);
    fiolinaGermiText.setString("4 - Fiolina Germi");
    fiolinaGermiText.setCharacterSize(30);
    fiolinaGermiText.setFillColor(Color::White);
    fiolinaGermiText.setPosition(630, 700);

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
            window.clear(Color(20, 20, 20));
            window.draw(menuBg);
            window.draw(titleBar);
            window.draw(survivalBtn);
            window.draw(campaignBtn);
            window.draw(key1);
            window.draw(key2);

            // Draw text
            window.draw(titleText);
            window.draw(survivalText);
            window.draw(campaignText);

            window.display();
            continue;
        }

        // =========================================
		// Player Selection Menu Render
        // =========================================
        if (gameMode != 0 && playerSelection == false) {
            window.clear(Color(20, 20, 20));
            window.draw(playerSelectMenuBg);
            window.draw(playerSelectTitleBar);
            window.draw(macroRossiBtn);
			window.draw(tarmaRovingBtn);
			window.draw(eriKasamotoBtn);
			window.draw(fiolinaGermiBtn);
            window.draw(playerSelectKey1);
			window.draw(playerSelectKey2);
			window.draw(playerSelectKey3);
			window.draw(playerSelectKey4);

            // Draw text
            window.draw(playerSelectTitleText);
            window.draw(macroRossiText);
            window.draw(tarmaRovingText);
            window.draw(eriKasamotoText);
            window.draw(fiolinaGermiText);

            window.display();
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

            // Step-up mechanism
            float stepHeight = 20.0f;
            float tempY = pY - stepHeight;
            if (currentLevel->checkCollision(pX, tempY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                if (currentLevel->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                    pX = characters.getActivePlayer()->getPlayerX();
                    pVelocityX = 0;
                }
            }
            else {
                pY = tempY;
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

            // Camera
            if (currentLevel) {
                float targetCamX = characters.getActivePlayer()->getPlayerX() - screen_x / 0.25f;
                cameraX += (targetCamX - cameraX) * 0.25f;
                if (cameraX < 0)
                    cameraX = 0;
                if (cameraX > currentLevel->getLevelEnd() - screen_x)
                    cameraX = currentLevel->getLevelEnd() - screen_x;
            }
            cameraY = 0;

            if (Keyboard::isKeyPressed(Keyboard::Space) && Delay.getElapsedTime().asSeconds() > 0.2f)
            {
                characters.switchCharacter();
                Delay.restart();
            }

            // Update
            currentLevel->update(dt);

            // Render
            window.clear(Color(135, 206, 235));
            currentLevel->render(window, cameraX, cameraY);
            characters.getActivePlayer()->render(window);
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

            // Camera
            float targetCamX = characters.getActivePlayer()->getPlayerX() - screen_x / 2.0f;
            cameraX += (targetCamX - cameraX) * 0.12f;
            if (cameraX < 0) cameraX = 0;
            cameraY = 0;

            if (Keyboard::isKeyPressed(Keyboard::Space) && Delay.getElapsedTime().asSeconds() > 0.2f)
            {
                characters.switchCharacter();
				Delay.restart();
            }

            // Render
            window.clear(Color(135, 206, 235));
            campaignLevel->render(window, cameraX, cameraY);
            characters.getActivePlayer()->render(window);
            window.display();
        }
    }

    delete currentLevel;
    delete campaignLevel;
    return 0;
}