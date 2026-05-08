#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Level.h"
#include "Biome.h"
#include "PlayerSoldier.h"
#include "Weapon.h"
#include "Enemy.h"
#include "Menu.h"
#include "GameMode.h"
#include "GameState.h"

using namespace sf;
using namespace std;

class Game {
private:

    RenderWindow window;
    int screenX;
    int screenY;


    int gameMode;


    int selectedCharacter;


    Menu menu;


    SurvivalGame* survivalGame;
    static const int MAX_SURVIVAL_ENEMIES = 50;
    Enemy* survivalRebels[MAX_SURVIVAL_ENEMIES];
    int survivalRebelCount;


    CampaignGame* campaignGame;


    CharacterManager characters;
    bool jumpHeld;


    Clock clock;
    Clock Delay;


    float cameraX;
    float cameraY;

public:
    Game() 
        : screenX(1600), screenY(900),
          gameMode(0),
          selectedCharacter(0),
          menu(),
          survivalGame(nullptr),
          campaignGame(nullptr),
          jumpHeld(false),
          cameraX(0), cameraY(0) {


        for (int i = 0; i < MAX_SURVIVAL_ENEMIES; i++) {
            survivalRebels[i] = nullptr;
        }
        survivalRebelCount = 0;


        window.create(VideoMode(screenX, screenY), "Metal Slug", Style::Close);
        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(60);
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
                    menu.moveSelectionUp();
                }
                else if (ev.key.code == Keyboard::Down) {
                    menu.moveSelectionDown();
                }
                else if (ev.key.code == Keyboard::Enter) {
                    int menuState = menu.getMenuState();
                    if (menuState == 0) {
                        menu.setMenuState(1);
                    }
                    else if (menuState == 1) {
                        selectedCharacter = menu.getSelectionIndex();
                        characters.switchCharacterToIndex(selectedCharacter);
                        menu.setMenuState(2);
                    }
                    else if (menuState == 2) {
                        int modeSelection = menu.getSelectionIndex();
                        if (modeSelection == 0) {
                            gameMode = 1;
                            survivalGame = new SurvivalGame(screenX, screenY);
                            survivalGame->setCharManager(&characters);
                            survivalGame->start();
                            characters.getActivePlayer()->setPlayerPosition(survivalGame->getCurrentLevel()->getPlayerSpawnX(), survivalGame->getCurrentLevel()->getPlayerSpawnY());
                            characters.getActivePlayer()->setVelocity(0, 0);
                            characters.getActivePlayer()->setGrounded(false);
                            cameraX = cameraY = 0;
                            Delay.restart();
                            

                            for (int i = 0; i < MAX_SURVIVAL_ENEMIES; i++) {
                                delete survivalRebels[i];
                                survivalRebels[i] = nullptr;
                            }
                            int ei = 0;
                            for (int i = 0; i < 2; i++) {
                                survivalRebels[ei] = new RebelSoldier();
                                survivalRebels[ei]->setPosition(300.f + i * 350.f, 200.f);
                                survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                                survivalRebels[ei]->setGroundY((float)(screenY - 48));
                                ei++;
                            }
                            for (int i = 0; i < 2; i++) {
                                survivalRebels[ei] = new GrenadeSoldier();
                                survivalRebels[ei]->setPosition(500.f + i * 400.f, 200.f);
                                survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                                survivalRebels[ei]->setGroundY((float)(screenY - 48));
                                ei++;
                            }
                            survivalRebels[ei] = new BazookaSoldier();
                            survivalRebels[ei]->setPosition(600.f, 200.f);
                            survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                            survivalRebels[ei]->setGroundY((float)(screenY - 48));
                            ei++;
                            survivalRebels[ei] = new ShieldedSoldier();
                            survivalRebels[ei]->setPosition(800.f, 200.f);
                            survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                            survivalRebels[ei]->setGroundY((float)(screenY - 48));
                            ei++;
                            for (int i = 0; i < 2; i++) {
                                survivalRebels[ei] = new MummyWarrior();
                                survivalRebels[ei]->setPosition(1000.f + i * 400.f, 200.f);
                                survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                                survivalRebels[ei]->setGroundY((float)(screenY - 48));
                                ei++;
                            }
                            for (int i = 0; i < 2; i++) {
                                survivalRebels[ei] = new Zombie();
                                survivalRebels[ei]->setPosition(1200.f + i * 400.f, 200.f);
                                survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                                survivalRebels[ei]->setGroundY((float)(screenY - 48));
                                ei++;
                            }
                            survivalRebels[ei] = new Martian();
                            survivalRebels[ei]->setPosition(1400.f, 200.f);
                            survivalRebels[ei]->setPlayer(characters.getActivePlayer());
                            survivalRebels[ei]->setGroundY((float)(screenY - 48));
                            ei++;
                            survivalRebelCount = ei;
                        }
                        else if (modeSelection == 1) {
                            gameMode = 2;
                            campaignGame = new CampaignGame(screenX, screenY);
                            campaignGame->setCharManager(&characters);
                            campaignGame->start();
                            characters.getActivePlayer()->setPlayerPosition(200, 50);
                            characters.getActivePlayer()->setVelocity(0, 0);
                            characters.getActivePlayer()->setGrounded(false);
                            cameraX = cameraY = 0;
                            Delay.restart();
                        }
                    }
                }
            }
        }


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
    }

    void update(float dt) {
        if (gameMode == 0) {
            menu.updateAnimation(dt);
            return;
        }

        characters.getActivePlayer()->update(dt);

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


                pX += pVelocityX;


                float stepHeight = 20.0f;
                if (pVelocityX != 0 &&
                    currentLevel->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                    float tempY = pY - stepHeight;
                    if (!currentLevel->checkCollision(pX, tempY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                        pY = tempY;
                    }
                    else {
                        pX = characters.getActivePlayer()->getPlayerX();
                        pVelocityX = 0;
                    }
                }


                if (pVelocityX != 0 && characters.getActivePlayer()->getIsGrounded()) {
                    float pW = (float)characters.getActivePlayer()->getWidth();
                    float pH = (float)characters.getActivePlayer()->getHeight();
                    for (int i = 0; i < survivalRebelCount; i++) {
                        if (!survivalRebels[i]) {
                            continue;
                        }
                        if (!survivalRebels[i]->getIsAlive()) {
                            continue;
                        }
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


                pY += pVelocityY;


                if (currentLevel->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                    if (pVelocityY > 0) {
                        characters.getActivePlayer()->setGrounded(true);
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
                        characters.getActivePlayer()->setGrounded(false);
                    }
                }

                characters.getActivePlayer()->setPlayerPosition(pX, pY);
                characters.getActivePlayer()->setVelocity(pVelocityX, pVelocityY);


                if (pX < 0) {
                    characters.getActivePlayer()->setPlayerX(0);
                }
                if (characters.getActivePlayer()->getPlayerX() + characters.getActivePlayer()->getWidth() > currentLevel->getLevelEnd())
                    characters.getActivePlayer()->setPlayerX(currentLevel->getLevelEnd() - characters.getActivePlayer()->getWidth());


                if (characters.getActivePlayer()->getPlayerX() + characters.getActivePlayer()->getWidth() >= currentLevel->getLevelEnd() - 100)
                    currentLevel->playerReachedEnd();

            
                float targetCamX = characters.getActivePlayer()->getPlayerX() - screenX / 2.0f;
                cameraX += (targetCamX - cameraX) * 0.2f;
                if (cameraX < 0) {
                    cameraX = 0;
                }
                if (cameraX > currentLevel->getLevelEnd() - screenX) {
                    cameraX = currentLevel->getLevelEnd() - screenX;
                }
                cameraY = 0;

                survivalGame->setCamera(cameraX, cameraY);


                if (Keyboard::isKeyPressed(Keyboard::Z) && Delay.getElapsedTime().asSeconds() > 0.2f) {
                    characters.switchCharacter();
                    Delay.restart();
                }


                for (int i = 0; i < survivalRebelCount; i++) {
                    if (!survivalRebels[i]) {
                        continue;
                    }
                    if (!survivalRebels[i]->getIsAlive()) {
                        continue;
                    }

                    survivalRebels[i]->setPlayer(characters.getActivePlayer());
                    survivalRebels[i]->update(dt);

                    float enemy_x = survivalRebels[i]->getX();
                    float enemy_y = survivalRebels[i]->getY();
                    float enemy_vx = survivalRebels[i]->getVelocityX();
                    float enemy_vy = survivalRebels[i]->getVelocityY();
                    const float enemy_w = survivalRebels[i]->getWidth();
                    const float enemy_h = survivalRebels[i]->getHeight();

                    survivalRebels[i]->applyGravity(dt);
                    enemy_vy = survivalRebels[i]->getVelocityY();


                    float newX = enemy_x + enemy_vx * dt;

                    float lookAhead = 30.0f;
                    if (enemy_vx != 0 && survivalRebels[i]->getIsGrounded()) {
                        float checkX = enemy_x + (enemy_vx > 0 ? lookAhead : -lookAhead);
                        if (currentLevel->checkCollision(checkX, enemy_y, enemy_w, enemy_h)) {
                            enemy_vy = -200.0f;
                            survivalRebels[i]->setVelocityY(enemy_vy);
                            survivalRebels[i]->setGrounded(false);
                        }
                    }

    
                    float stepHeightE = 20.0f;
                    if (enemy_vx != 0 &&
                        currentLevel->checkCollision(newX, enemy_y, enemy_w, enemy_h)) {
                        float tempY = enemy_y - stepHeightE;
                        if (!currentLevel->checkCollision(newX, tempY, enemy_w, enemy_h)) {
                            enemy_y = tempY;
                        }
                        else if (survivalRebels[i]->getIsGrounded()) {
                            enemy_vy = -400.0f;
                            survivalRebels[i]->setVelocityY(enemy_vy);
                            survivalRebels[i]->setGrounded(false);
                        }
                        else {
                            enemy_vx = -enemy_vx;
                            survivalRebels[i]->setVelocityX(enemy_vx);
                            newX = enemy_x;
                        }
                    }

                    float px = characters.getActivePlayer()->getPlayerX();
                    float py = characters.getActivePlayer()->getPlayerY();
                    float pw = (float)characters.getActivePlayer()->getWidth();
                    float ph = (float)characters.getActivePlayer()->getHeight();
                    if (newX < px + pw && newX + enemy_w > px &&
                        enemy_y < py + ph && enemy_y + enemy_h > py) {
                        newX = enemy_x;
                    }


                    for (int j = 0; j < survivalRebelCount; j++) {
                        if (j == i) {
                            continue;
                        }
                        if (!survivalRebels[j]) {
                            continue;
                        }
                        if (!survivalRebels[j]->getIsAlive()) {
                            continue;
                        }

                        float bx = survivalRebels[j]->getX();
                        float by = survivalRebels[j]->getY();
                        float bw = survivalRebels[j]->getWidth();
                        float bh = survivalRebels[j]->getHeight();

                        bool vertOverlap = (enemy_y < by + bh && enemy_y + enemy_h > by);
                        if (!vertOverlap) {
                            continue;
                        }

                        bool horzOverlap = (newX < bx + bw && newX + enemy_w > bx);
                        if (!horzOverlap) {
                            continue;
                        }

                        float iCenter = newX + enemy_w / 2.f;
                        float jCenter = bx + bw / 2.f;

                        if (iCenter < jCenter) {
                            newX = bx - enemy_w;
                        }
                        else {
                            newX = bx + bw;
                        }
                        enemy_vx = 0.f;
                        survivalRebels[i]->setVelocityX(0.f);
                    }

                    enemy_x = newX;


                    enemy_y += enemy_vy * dt;

                    if (currentLevel->checkCollision(enemy_x, enemy_y, enemy_w, enemy_h)) {
                        if (enemy_vy > 0) {
                            enemy_vy = 0;
                            survivalRebels[i]->setGrounded(true);
                            while (currentLevel->checkCollision(enemy_x, enemy_y, enemy_w, enemy_h)) {
                                enemy_y -= 1;
                            }
                        }
                        else {
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

                survivalGame->update(dt, &characters);
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

                pX += pVelocityX;

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

                pY += pVelocityY;

                if (campaignLevel->checkCollision(pX, pY, characters.getActivePlayer()->getWidth(), characters.getActivePlayer()->getHeight())) {
                    if (pVelocityY > 0) {
                        characters.getActivePlayer()->setGrounded(true);
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
                    if (pVelocityY >= 0) {
                        characters.getActivePlayer()->setGrounded(false);
                    }
                }

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

                float targetCamX = characters.getActivePlayer()->getPlayerX() - screenX / 2.0f;
                cameraX += (targetCamX - cameraX) * 0.2f;
                if (cameraX < 0) {
                    cameraX = 0;
                }
                cameraY = 0;

                campaignGame->setCamera(cameraX, cameraY);

                if (Keyboard::isKeyPressed(Keyboard::Z) && Delay.getElapsedTime().asSeconds() > 0.2f) {
                    characters.switchCharacter();
                    Delay.restart();
                }

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
            window.display();
            return;
        }

        if (gameMode == 1 && survivalGame) {
            survivalGame->render(window);
            for (int i = 0; i < survivalRebelCount; i++) {
                if (survivalRebels[i])
                    survivalRebels[i]->render(window, cameraX, cameraY);
            }
            characters.getActivePlayer()->render(window, cameraX, cameraY);
            window.display();
        }

        else if (gameMode == 2 && campaignGame) {
            campaignGame->render(window);
            characters.getActivePlayer()->render(window, cameraX, cameraY);
            window.display();
        }
    }

    void cleanup() {
        delete survivalGame;
        delete campaignGame;
        for (int i = 0; i < MAX_SURVIVAL_ENEMIES; i++) {
            delete survivalRebels[i];
            survivalRebels[i] = nullptr;
        }
    }
};
