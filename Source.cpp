#define _CRT_SECURE_NO_WARNINGS
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Header.h"
#include "Player.h"
#include "Weapon.h"
#include "gamestate.h"

using namespace sf;

int screen_x = 1600;
int screen_y = 900;


/*
    =========================================================
    UML RELATIONSHIPS IN THIS FILE
    =========================================================

    Game (main) :
        COMPOSITION  with GameStateManager
            — created on stack in main, owns all pushed states
        COMPOSITION  with CharacterManager
            — created on stack in main, owns all 4 soldiers
              and FusionCompanion (see Player.h)

    SurvivalPlayState : public GameState
        INHERITANCE  from GameState  (abstract)
        AGGREGATION  with CharacterManager*
            — pointer passed in from main, NOT deleted here
        COMPOSITION  with LevelManager
            — created with new in ctor, deleted in dtor

    CampaignPlayState : public GameState
        INHERITANCE  from GameState  (abstract)
        AGGREGATION  with CharacterManager*
            — pointer passed in from main, NOT deleted here
        COMPOSITION  with CampaignLevel
            — created with new in ctor, deleted in dtor

    GameStateManager :
        COMPOSITION  with GameState* stack
            — pushState takes ownership, popState deletes

    CharacterManager :
        COMPOSITION  with PlayerSoldier* [4]  (Marco/Tarma/Eri/Fiolina)
        COMPOSITION  with FusionCompanion*
            — all created in CharacterManager ctor, deleted in dtor

    PlayerSoldier :
        COMPOSITION  with TransformationState*  (State Pattern)
            — always non-null, created in ctor, deleted in dtor
            — swapped via changeTransformState()
        AGGREGATION  with Weapon*, Vehicle*
            — pointers received from outside, NOT deleted here

    =========================================================
*/


// =========================================================
//  SURVIVAL PLAY STATE
//  INHERITANCE  : public GameState  (abstract base)
//  AGGREGATION  : CharacterManager* — given from outside
//  COMPOSITION  : LevelManager*     — created & deleted here
// =========================================================
class SurvivalPlayState : public GameState {

private:

    // AGGREGATION — CharacterManager lives in main, we just use it
    CharacterManager* characters;

    // COMPOSITION — we create this, we delete this
    LevelManager* levelManager;

    // physics
    float friction;
    float airFriction;
    float jumpPower;
    float gravity;
    float maxFallSpeed;
    bool  onGround;
    bool  jumpHeld;

    // camera
    float cameraX;
    float cameraY;

    int screenX;
    int screenY;

    // debounce clock for character swap key
    Clock swapClock;

public:

    // constructor — CharacterManager pointer is AGGREGATION (not owned)
    SurvivalPlayState(CharacterManager* chars, int screenW, int screenH)
        : characters(chars),
        screenX(screenW),
        screenY(screenH),
        friction(0.80f),
        airFriction(0.95f),
        jumpPower(-22.0f),
        gravity(1.0f),
        maxFallSpeed(25.0f),
        onGround(false),
        jumpHeld(false),
        cameraX(0.0f),
        cameraY(0.0f)
    {
        // COMPOSITION — we create LevelManager here, we own it
        levelManager = new LevelManager();
    }

    ~SurvivalPlayState() {
        // COMPOSITION — we created it, we delete it
        delete levelManager;
        levelManager = nullptr;
        // AGGREGATION — characters NOT deleted, main owns it
    }

    // -------------------------------------------------
    // onEnter — called by GameStateManager::pushState
    // sets state active, loads levels, spawns player
    // -------------------------------------------------
    void onEnter() override {
        active = true;
        paused = false;
        onGround = false;
        cameraX = 0.0f;
        cameraY = 0.0f;

        levelManager->loadAllLevels();
        levelManager->switchToLevel(0);

        Level* current = levelManager->getCurrentLevel();
        if (current != nullptr && characters != nullptr) {
            characters->getActivePlayer()->setPlayerPosition(
                current->getPlayerSpawnX(),
                current->getPlayerSpawnY()
            );
            characters->getActivePlayer()->setVelocity(0.0f, 0.0f);
        }

        swapClock.restart();
    }

    // -------------------------------------------------
    // onExit — called by GameStateManager::popState
    // -------------------------------------------------
    void onExit() override {
        active = false;
    }

    // -------------------------------------------------
    // update — physics, input, collision, camera
    // -------------------------------------------------
    void update(float dt) override {
        if (!active || paused) return;

        Level* current = levelManager->getCurrentLevel();
        if (current == nullptr || characters == nullptr) return;

        PlayerSoldier* player = characters->getActivePlayer();

        // ---- HORIZONTAL INPUT ----
        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            player->moveLeft();
            if (player->isFacingRight()) player->flipToLeft();
        }
        else if (Keyboard::isKeyPressed(Keyboard::Right)) {
            player->moveRight();
            if (!player->isFacingRight()) player->flipToRight();
        }
        else {
            float fx = onGround ? friction : airFriction;
            player->setVelocityX(player->getVelocityX() * fx);
        }

        // clamp horizontal speed
        if (player->getVelocityX() > 6.0f) player->setVelocityX(6.0f);
        if (player->getVelocityX() < -6.0f) player->setVelocityX(-6.0f);

        // ---- JUMP INPUT ----
        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            if (onGround) {
                player->setVelocityY(jumpPower);
                onGround = false;
                jumpHeld = true;
            }
        }
        else {
            jumpHeld = false;
        }

        // variable jump height — cut upward velocity when key released
        if (!jumpHeld && player->getVelocityY() < 0.0f) {
            player->setVelocityY(player->getVelocityY() * 0.5f);
        }

        // ---- GRAVITY ----
        player->setVelocityY(player->getVelocityY() + gravity);
        if (player->getVelocityY() > maxFallSpeed)
            player->setVelocityY(maxFallSpeed);

        onGround = false;

        // ---- PHYSICS + COLLISION ----
        float pX = player->getPlayerX();
        float pY = player->getPlayerY();
        float vX = player->getVelocityX();
        float vY = player->getVelocityY();
        int   pw = player->getWidth();
        int   ph = player->getHeight();

        // horizontal move
        pX += vX;

        // step up small ledges
        float stepHeight = 20.0f;
        if (vX != 0.0f && current->checkCollision(pX, pY, pw, ph)) {
            float stepY = pY - stepHeight;
            if (!current->checkCollision(pX, stepY, pw, ph)) {
                pY = stepY;
            }
            else {
                pX = player->getPlayerX(); // revert
                vX = 0.0f;
            }
        }

        // vertical move
        pY += vY;

        if (current->checkCollision(pX, pY, pw, ph)) {
            if (vY > 0.0f) {
                // landing
                onGround = true;
                vY = 0.0f;
                while (current->checkCollision(pX, pY, pw, ph)) pY -= 1.0f;
            }
            else {
                // ceiling
                vY = 0.0f;
                while (current->checkCollision(pX, pY, pw, ph)) pY += 1.0f;
            }
        }

        // write back
        player->setPlayerPosition(pX, pY);
        player->setVelocity(vX, vY);

        // ---- BOUNDS ----
        if (pX < 0.0f) player->setPlayerX(0.0f);
        float levelEnd = (float)current->getLevelEnd();
        if (player->getPlayerX() + pw > levelEnd)
            player->setPlayerX(levelEnd - pw);

        // ---- LEVEL END ----
        if (player->getPlayerX() + pw >= levelEnd - 100.0f)
            current->playerReachedEnd();

        // ---- LEVEL SWITCH ----
        if (current->checkLevelComplete()) {
            levelManager->nextLevel();
            Level* next = levelManager->getCurrentLevel();
            if (next == nullptr) {
                // all levels done — signal wantsToExit
                active = false;
                return;
            }
            player->setPlayerPosition(next->getPlayerSpawnX(), next->getPlayerSpawnY());
            player->setVelocity(0.0f, 0.0f);
            onGround = false;
            cameraX = 0.0f;
            cameraY = 0.0f;
        }

        // ---- CAMERA ----
        current = levelManager->getCurrentLevel();
        if (current != nullptr) {
            float targetCamX = player->getPlayerX() - screenX / 2.0f;
            cameraX += (targetCamX - cameraX) * 0.2f;
            if (cameraX < 0.0f) cameraX = 0.0f;
            float maxCam = (float)current->getLevelEnd() - screenX;
            if (cameraX > maxCam) cameraX = maxCam;
        }
        cameraY = 0.0f;

        // ---- CHARACTER SWAP (Z key, debounced) ----
        if (Keyboard::isKeyPressed(Keyboard::Z) &&
            swapClock.getElapsedTime().asSeconds() > 0.2f)
        {
            characters->switchCharacter();
            swapClock.restart();
        }

        // ---- UPDATE LEVEL + ANIMATION ----
        current = levelManager->getCurrentLevel();
        if (current != nullptr) current->update(dt);
        characters->getActivePlayer()->updateAnimation(dt);
    }

    // -------------------------------------------------
    // render — clear, draw level, draw player, display
    // -------------------------------------------------
    void render(RenderWindow& window) override {
        if (!active) return;

        Level* current = levelManager->getCurrentLevel();

        window.clear(Color(135, 206, 235));
        if (current != nullptr) current->render(window, cameraX, cameraY);
        if (characters != nullptr)
            characters->getActivePlayer()->render(window, cameraX, cameraY);
        window.display();
    }

    // main checks this to know when to pop this state
    bool wantsToExit() const { return !active; }
};


// =========================================================
//  CAMPAIGN PLAY STATE
//  INHERITANCE  : public GameState  (abstract base)
//  AGGREGATION  : CharacterManager* — given from outside
//  COMPOSITION  : CampaignLevel*    — created & deleted here
// =========================================================
class CampaignPlayState : public GameState {

private:

    // AGGREGATION — lives in main
    CharacterManager* characters;

    // COMPOSITION — we create, we delete
    CampaignLevel* campaignLevel;

    float gravity;
    float maxFallSpeed;
    float friction;
    float airFriction;
    float jumpPower;
    bool  onGround;
    bool  jumpHeld;

    float cameraX;
    float cameraY;

    int screenX;
    int screenY;

    Clock swapClock;

public:

    CampaignPlayState(CharacterManager* chars, int screenW, int screenH)
        : characters(chars),
        screenX(screenW),
        screenY(screenH),
        gravity(1.0f),
        maxFallSpeed(25.0f),
        friction(0.80f),
        airFriction(0.95f),
        jumpPower(-22.0f),
        onGround(false),
        jumpHeld(false),
        cameraX(0.0f),
        cameraY(0.0f)
    {
        // COMPOSITION — created here, owned here
        campaignLevel = new CampaignLevel(3);
    }

    ~CampaignPlayState() {
        // COMPOSITION — we delete what we created
        delete campaignLevel;
        campaignLevel = nullptr;
        // AGGREGATION — characters NOT deleted
    }

    void onEnter() override {
        active = true;
        paused = false;
        onGround = false;
        cameraX = 0.0f;
        cameraY = 0.0f;

        // generate initial chunks
        for (int i = 0; i < 8; i++)
            campaignLevel->generateChunk(i * 16);

        if (characters != nullptr) {
            characters->getActivePlayer()->setPlayerPosition(200.0f, 50.0f);
            characters->getActivePlayer()->setVelocity(0.0f, 0.0f);
        }

        swapClock.restart();
    }

    void onExit() override {
        active = false;
    }

    void update(float dt) override {
        if (!active || paused) return;
        if (campaignLevel == nullptr || characters == nullptr) return;

        PlayerSoldier* player = characters->getActivePlayer();

        // ---- HORIZONTAL INPUT ----
        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            player->moveLeft();
            if (player->isFacingRight()) player->flipToLeft();
        }
        else if (Keyboard::isKeyPressed(Keyboard::Right)) {
            player->moveRight();
            if (!player->isFacingRight()) player->flipToRight();
        }
        else {
            float fx = onGround ? friction : airFriction;
            player->setVelocityX(player->getVelocityX() * fx);
        }

        if (player->getVelocityX() > 6.0f) player->setVelocityX(6.0f);
        if (player->getVelocityX() < -6.0f) player->setVelocityX(-6.0f);

        // ---- JUMP ----
        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            if (onGround) {
                player->setVelocityY(jumpPower);
                onGround = false;
                jumpHeld = true;
            }
        }
        else {
            jumpHeld = false;
        }

        if (!jumpHeld && player->getVelocityY() < 0.0f)
            player->setVelocityY(player->getVelocityY() * 0.5f);

        // ---- GRAVITY ----
        player->setVelocityY(player->getVelocityY() + gravity);
        if (player->getVelocityY() > maxFallSpeed)
            player->setVelocityY(maxFallSpeed);

        // ---- MOVE ----
        player->setPlayerX(player->getPlayerX() + player->getVelocityX());
        player->setPlayerY(player->getPlayerY() + player->getVelocityY());

        // screen bottom boundary
        float bottomBound = (float)(screenY - player->getHeight());
        if (player->getPlayerY() > bottomBound) {
            player->setPlayerY(bottomBound);
            player->setVelocityY(0.0f);
            onGround = true;
        }

        // left bound
        if (player->getPlayerX() < 0.0f)
            player->setPlayerX(0.0f);

        // ---- CAMPAIGN CHUNK UPDATE ----
        campaignLevel->update(player->getPlayerX());

        // ---- CAMERA ----
        float targetCamX = player->getPlayerX() - screenX / 2.0f;
        cameraX += (targetCamX - cameraX) * 0.2f;
        if (cameraX < 0.0f) cameraX = 0.0f;
        cameraY = 0.0f;

        // ---- CHARACTER SWAP ----
        if (Keyboard::isKeyPressed(Keyboard::Z) &&
            swapClock.getElapsedTime().asSeconds() > 0.2f)
        {
            characters->switchCharacter();
            swapClock.restart();
        }

        player->updateAnimation(dt);
    }

    void render(RenderWindow& window) override {
        if (!active) return;

        window.clear(Color(135, 206, 235));
        campaignLevel->render(window, cameraX, cameraY);
        characters->getActivePlayer()->render(window, cameraX, cameraY);
        window.display();
    }

    bool wantsToExit() const { return !active; }
};


// =========================================================
//  MAIN
//
//  UML: Game class owns everything at the top level
//
//  COMPOSITION  : GameStateManager  — stack-allocated, owns pushed states
//  COMPOSITION  : CharacterManager  — stack-allocated, owns 4 soldiers
//                                     + FusionCompanion (see Player.h)
//  AGGREGATION  : CharacterManager* passed into each PlayState
//                 (PlayState uses it but does NOT delete it)
//
//  Menu logic stays in main — it is simple UI, not a full GameState
//  When a mode + character is chosen, the correct PlayState is
//  pushed onto GameStateManager which then takes ownership.
// =========================================================
int main() {

    RenderWindow window(VideoMode(screen_x, screen_y), "Metal Slug", Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    // --------------------------------------------------
    // COMPOSITION — GameStateManager owns pushed states
    // created on stack so destroyed automatically at end
    // --------------------------------------------------
    GameStateManager stateManager;

    // --------------------------------------------------
    // COMPOSITION — CharacterManager owns all 4 soldiers
    // and FusionCompanion (created inside CharacterManager)
    // passed as AGGREGATION pointer into play states
    // --------------------------------------------------
    CharacterManager characters;

    // --------------------------------------------------
    // MENU STATE MACHINE
    //   menuPhase 0 = mode select screen
    //   menuPhase 1 = character select after survival picked
    //   menuPhase 2 = character select after campaign picked
    // --------------------------------------------------
    int  menuPhase = 0;
    bool playerSelected = false;
    bool charClockStarted = false;
    Clock charSelectClock;

    // --------------------------------------------------
    // FONT — load once, used by all text objects
    // --------------------------------------------------
    Font font;
    font.loadFromFile("arial.ttf");

    // --------------------------------------------------
    // MODE SELECT UI
    // --------------------------------------------------
    RectangleShape modeBg(Vector2f((float)screen_x, (float)screen_y));
    modeBg.setFillColor(Color(20, 20, 20));

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

    RectangleShape modeKey1(Vector2f(40, 40));
    modeKey1.setFillColor(Color::Red);
    modeKey1.setPosition(570, 365);

    RectangleShape modeKey2(Vector2f(40, 40));
    modeKey2.setFillColor(Color::Red);
    modeKey2.setPosition(570, 475);

    Text titleText;
    titleText.setFont(font);
    titleText.setString("SELECT MODE:");
    titleText.setCharacterSize(50);
    titleText.setFillColor(Color::White);
    titleText.setPosition(600, 165);

    Text survivalText;
    survivalText.setFont(font);
    survivalText.setString("1 - SURVIVAL MODE");
    survivalText.setCharacterSize(30);
    survivalText.setFillColor(Color::White);
    survivalText.setPosition(630, 370);

    Text campaignText;
    campaignText.setFont(font);
    campaignText.setString("2 - CAMPAIGN MODE");
    campaignText.setCharacterSize(30);
    campaignText.setFillColor(Color::White);
    campaignText.setPosition(630, 480);

    // --------------------------------------------------
    // CHARACTER SELECT UI
    // --------------------------------------------------
    RectangleShape charBg(Vector2f((float)screen_x, (float)screen_y));
    charBg.setFillColor(Color(20, 20, 20));

    RectangleShape charTitleBar(Vector2f(700, 90));
    charTitleBar.setFillColor(Color::Red);
    charTitleBar.setPosition(450, 150);

    // four character buttons
    const char* charNames[4] = {
        "1 - Marco Rossi",
        "2 - Tarma Roving",
        "3 - Eri Kasamoto",
        "4 - Fiolina Germi"
    };
    float charBtnY[4] = { 350, 460, 570, 680 };

    RectangleShape charBtns[4];
    RectangleShape charKeys[4];
    Text           charTexts[4];

    for (int i = 0; i < 4; i++) {
        charBtns[i].setSize(Vector2f(500, 70));
        charBtns[i].setFillColor(Color::Green);
        charBtns[i].setPosition(550, charBtnY[i]);
        charBtns[i].setOutlineThickness(3);
        charBtns[i].setOutlineColor(Color::White);

        charKeys[i].setSize(Vector2f(40, 40));
        charKeys[i].setFillColor(Color::Red);
        charKeys[i].setPosition(570, charBtnY[i] + 15);

        charTexts[i].setFont(font);
        charTexts[i].setString(charNames[i]);
        charTexts[i].setCharacterSize(30);
        charTexts[i].setFillColor(Color::White);
        charTexts[i].setPosition(630, charBtnY[i] + 15);
    }

    Text charTitleText;
    charTitleText.setFont(font);
    charTitleText.setString("CHOOSE CHARACTER");
    charTitleText.setCharacterSize(50);
    charTitleText.setFillColor(Color::White);
    charTitleText.setPosition(540, 165);

    // --------------------------------------------------
    // MAIN CLOCK
    // --------------------------------------------------
    Clock clock;
    Event ev;

    // --------------------------------------------------
    // GAME LOOP
    // --------------------------------------------------
    while (window.isOpen()) {

        float dt = clock.restart().asSeconds();

        // ---- EVENT POLLING ----
        while (window.pollEvent(ev)) {

            if (ev.type == Event::Closed) {
                window.close();
            }

            // ESC — back to menu or close
            if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Escape) {
                if (menuPhase != 0) {
                    // pop any active play state
                    if (!stateManager.isEmpty())
                        stateManager.popState();
                    menuPhase = 0;
                    playerSelected = false;
                    charClockStarted = false;
                }
                else {
                    window.close();
                }
            }

            // ---- MODE SELECT (menuPhase == 0) ----
            if (menuPhase == 0 && ev.type == Event::KeyPressed) {
                if (ev.key.code == Keyboard::Num1) {
                    menuPhase = 1; // survival chosen
                    playerSelected = false;
                    charClockStarted = false;
                }
                else if (ev.key.code == Keyboard::Num2) {
                    menuPhase = 2; // campaign chosen
                    playerSelected = false;
                    charClockStarted = false;
                }
            }

            // ---- CHARACTER SELECT (menuPhase != 0, not yet selected) ----
            // charSelectClock guard prevents mode-select keypress bleeding in
            if (menuPhase != 0 &&
                !playerSelected &&
                charClockStarted &&
                ev.type == Event::KeyPressed &&
                charSelectClock.getElapsedTime().asSeconds() > 0.3f)
            {
                int picked = -1;
                if (ev.key.code == Keyboard::Num1) picked = 0; // Marco
                if (ev.key.code == Keyboard::Num2) picked = 1; // Tarma
                if (ev.key.code == Keyboard::Num3) picked = 2; // Eri
                if (ev.key.code == Keyboard::Num4) picked = 3; // Fiolina

                if (picked != -1) {
                    // CharacterManager switches active soldier (composition inside it)
                    characters.switchCharacterToIndex(picked);
                    playerSelected = true;

                    if (menuPhase == 1) {
                        // AGGREGATION — passing &characters pointer into SurvivalPlayState
                        // GameStateManager takes COMPOSITION ownership via pushState
                        // pushState calls onEnter() internally — do NOT call it again
                        SurvivalPlayState* s = new SurvivalPlayState(
                            &characters, screen_x, screen_y
                        );
                        stateManager.pushState(s);
                        // s->onEnter() is already called inside pushState
                    }
                    else if (menuPhase == 2) {
                        // same pattern for campaign
                        CampaignPlayState* c = new CampaignPlayState(
                            &characters, screen_x, screen_y
                        );
                        stateManager.pushState(c);
                        // c->onEnter() is already called inside pushState
                    }
                }
            }
        } // end event loop

        // ---- START charSelectClock the frame AFTER menuPhase changes ----
        // ensures the mode-select keypress is fully consumed before we
        // start reading character-select keypresses
        if (menuPhase != 0 && !playerSelected && !charClockStarted) {
            charSelectClock.restart();
            charClockStarted = true;
        }

        // ---- CHECK IF ACTIVE STATE FINISHED ON ITS OWN ----
        // (e.g. all levels complete in survival)
        if (!stateManager.isEmpty()) {
            GameState* cur = stateManager.getCurrentState();

            // polymorphic downcast — only way to call wantsToExit()
            // which is not on the GameState interface
            SurvivalPlayState* surv = dynamic_cast<SurvivalPlayState*>(cur);
            if (surv != nullptr && surv->wantsToExit()) {
                stateManager.popState(); // GameStateManager deletes the state (composition)
                menuPhase = 0;
                playerSelected = false;
                charClockStarted = false;
            }

            CampaignPlayState* camp = dynamic_cast<CampaignPlayState*>(cur);
            if (camp != nullptr && camp->wantsToExit()) {
                stateManager.popState();
                menuPhase = 0;
                playerSelected = false;
                charClockStarted = false;
            }
        }

        // =========================================
        // RENDER — only one path runs per frame
        // =========================================

        // ---- MODE SELECT SCREEN ----
        if (menuPhase == 0) {
            window.clear(Color(20, 20, 20));
            window.draw(modeBg);
            window.draw(titleBar);
            window.draw(survivalBtn);
            window.draw(campaignBtn);
            window.draw(modeKey1);
            window.draw(modeKey2);
            window.draw(titleText);
            window.draw(survivalText);
            window.draw(campaignText);
            window.display();
            continue;
        }

        // ---- CHARACTER SELECT SCREEN ----
        if (menuPhase != 0 && !playerSelected) {
            window.clear(Color(20, 20, 20));
            window.draw(charBg);
            window.draw(charTitleBar);
            window.draw(charTitleText);
            for (int i = 0; i < 4; i++) {
                window.draw(charBtns[i]);
                window.draw(charKeys[i]);
                window.draw(charTexts[i]);
            }
            window.display();
            continue;
        }

        // ---- ACTIVE PLAY STATE ----
        // each state calls window.clear(), draws everything, window.display()
        // GameStateManager delegates update and render to the top state
        if (!stateManager.isEmpty()) {
            stateManager.update(dt);
            stateManager.render(window);
        }

    } // end game loop

    return 0;
}