#pragma once
#include <SFML/Graphics.hpp>
#include "Level.h"
#include "GameMode.h"
#include "entity.h"
#include"echo.h"
using namespace sf;


class GameMode;
class Level;
class Level1;
class Level2;
class Level3;
class Entity;
class LevelManager;
class EntityManager;


//abstract base class
class GameState {

protected:

    bool active;
    bool paused;

public:

    GameState();
    virtual ~GameState();

    //abstract
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual void update(float dt) = 0;
    virtual void render(RenderWindow& window) = 0;

    virtual void pause();
    virtual void resume();

    bool isActive() const;
    bool isPaused() const;
};


GameState::GameState() {
    active = false;
    paused = false;
}

GameState::~GameState() {
}

void GameState::pause() {
    paused = true;
}

void GameState::resume() {
    paused = false;
}

bool GameState::isActive() const {
    return active;
}

bool GameState::isPaused() const {
    return paused;
}




//Level composition manager
class LevelManager {

private:

    Level* levels[4];
    int     totalLevels;
    int     currentIndex;
    Level* current;

public:

    LevelManager();
    ~LevelManager();

    void    loadAllLevels();
    void    switchToLevel(int index);
    void    nextLevel();
    bool    isAllLevelsComplete();

    void    update(float dt);
    void    render(RenderWindow& window, float camX, float camY);
    void    resolveCollisions(float& px, float& py, float pw, float ph,
        float& velX, float& velY, bool& onGround);

    Level* getCurrentLevel();
    int     getCurrentIndex();
    int     getTotalLevels();
};


LevelManager::LevelManager() {
    totalLevels = 0;
    currentIndex = -1;
    current = nullptr;
    for (int i = 0; i < 4; i++) {
        levels[i] = nullptr;
    }
}

LevelManager::~LevelManager() {
    for (int i = 0; i < 4; i++) {
        delete levels[i];
        levels[i] = nullptr;
    }
}

void LevelManager::loadAllLevels() {
    levels[0] = new Level1();
    levels[1] = new Level2();
    levels[2] = new Level3();
    //levels[3] = new BossLevel();
    totalLevels = 3;
}

void LevelManager::switchToLevel(int index) {
    if (index < 0 || index >= totalLevels) {
        return;
    }
    currentIndex = index;
    current = levels[index];
    if (current != nullptr) {
        current->generateBiomes();
        current->spawnEnemies();
    }
}

void LevelManager::nextLevel() {
    switchToLevel(currentIndex + 1);
}

bool LevelManager::isAllLevelsComplete() {
    bool lastReached = (currentIndex >= totalLevels - 1);
    bool currentDone = (current != nullptr && current->checkLevelComplete());
    return lastReached && currentDone;
}

void LevelManager::update(float dt) {
    if (current != nullptr) {
        current->update(dt);
    }
}

void LevelManager::render(RenderWindow& window, float camX, float camY) {
    if (current != nullptr) {
        current->render(window, camX, camY);
    }
}

void LevelManager::resolveCollisions(float& px, float& py, float pw, float ph,
    float& velX, float& velY, bool& onGround) {
    if (current != nullptr) {
        current->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
    }
}

Level* LevelManager::getCurrentLevel() {
    return current;
}

int LevelManager::getCurrentIndex() {
    return currentIndex;
}

int LevelManager::getTotalLevels() {
    return totalLevels;
}




//Entity composition manager
class EntityManager {

private:

    static const int MAX_ENTITIES = 512;

    Entity* pool[MAX_ENTITIES];
    int     count;

public:

    EntityManager();
    ~EntityManager();

    void    addEntity(Entity* e);
    void    removeEntity(Entity* e);
    void    clearAll();

    void    update(float dt);
    void    render(RenderWindow& window, float camX, float camY);

    int     getCount();
    Entity* getAt(int index);
};


EntityManager::EntityManager() {
    count = 0;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        pool[i] = nullptr;
    }
}

EntityManager::~EntityManager() {
    clearAll();
}

void EntityManager::addEntity(Entity* e) {
    if (e == nullptr) {
        return;
    }
    if (count >= MAX_ENTITIES) {
        return;
    }
    pool[count] = e;
    count++;
}

void EntityManager::removeEntity(Entity* e) {
    for (int i = 0; i < count; i++) {
        if (pool[i] == e) {
            delete pool[i];
            count--;
            pool[i] = pool[count];
            pool[count] = nullptr;
            return;
        }
    }
}

void EntityManager::clearAll() {
    for (int i = 0; i < count; i++) {
        delete pool[i];
        pool[i] = nullptr;
    }
    count = 0;
}

void EntityManager::update(float dt) {
    for (int i = 0; i < count; i++) {
        if (pool[i] != nullptr) {
            pool[i]->update(dt);
        }
    }
}

void EntityManager::render(RenderWindow& window, float camX, float camY) {
    for (int i = 0; i < count; i++) {
        if (pool[i] != nullptr) {
            pool[i]->render(window, camX, camY);
        }
    }
}

int EntityManager::getCount() {
    return count;
}

Entity* EntityManager::getAt(int index) {
    if (index < 0 || index >= count) {
        return nullptr;
    }
    return pool[index];
}




//Concrete play state
class PlayState : public GameState {

private:

    GameMode* activeMode;

    LevelManager* levelManager;
    EntityManager* entityManager;



    whisper originalBGM;
    whisper muffledBGM;
    sf::SoundBuffer normalBuffer;
    sf::SoundBuffer muffledBuffer;
    sf::Sound gameSound;
    bool playerInWater;
    CharacterManager* charManager;

public:

    PlayState(GameMode* mode, CharacterManager* chars);
    ~PlayState();

    void onEnter()                    override;
    void onExit()                     override;
    void update(float dt)             override;
    void render(RenderWindow& window) override;

    GameMode* getGameMode();
    LevelManager* getLevelManager();
    EntityManager* getEntityManager();
};


PlayState::PlayState(GameMode* mode, CharacterManager* chars) {
    activeMode = mode;

    levelManager = new LevelManager();
    entityManager = new EntityManager();

    charManager = chars;
    playerInWater = false;
    originalBGM.loadingaudio("mus.wav");

}



PlayState::~PlayState() {
    delete levelManager;
    delete entityManager;
    levelManager = nullptr;
    entityManager = nullptr;
}

void PlayState::onEnter() {
    active = true;
    paused = false;
    muffledBGM = originalBGM.getMuffled();
    muffledBGM.header = originalBGM.header;
    originalBGM.toSFML(normalBuffer);
    muffledBGM.toSFML(muffledBuffer);
    originalBGM.toSFML(normalBuffer);
    muffledBGM.toSFML(muffledBuffer);

    gameSound.setBuffer(normalBuffer);
    gameSound.setLoop(true);
    gameSound.play();

    if (activeMode != nullptr) {
        activeMode->start();
    }

    if (levelManager != nullptr) {
        levelManager->loadAllLevels();
        levelManager->switchToLevel(0);
    }
    gameSound.play();
}

void PlayState::onExit() {
    active = false;
    if (activeMode != nullptr) {
        activeMode->end();
    }
    gameSound.stop();
}

void PlayState::update(float dt) {
    if (active == false || paused == true) {
        return;
    }
    if (activeMode != nullptr) {
        activeMode->update(dt);
    }
    if (levelManager != nullptr) {
        levelManager->update(dt);
    }
    if (entityManager != nullptr) {
        entityManager->update(dt);

    }
    Level* cur = levelManager->getCurrentLevel();
    PlayerSoldier* p = charManager->getActivePlayer();

    if (cur != nullptr && p != nullptr) {
        bool inWater = cur->checkWaterAt(
            p->getPlayerX(), p->getPlayerY(),
            p->getWidth(), p->getHeight()
        );

        if (inWater && !playerInWater) {
            gameSound.stop();
            gameSound.setBuffer(muffledBuffer);
            gameSound.play();
            playerInWater = true;
        }
        else if (!inWater && playerInWater) {
            gameSound.stop();
            gameSound.setBuffer(normalBuffer);
            gameSound.play();
            playerInWater = false;
        }
    }
}

void PlayState::render(RenderWindow& window) {
    if (active == false) {
        return;
    }
    if (activeMode != nullptr) {
        activeMode->render(window);
    }
}

GameMode* PlayState::getGameMode() {
    return activeMode;
}

LevelManager* PlayState::getLevelManager() {
    return levelManager;
}

EntityManager* PlayState::getEntityManager() {
    return entityManager;
}




//State stack manager
class GameStateManager {

private:

    static const int MAX_STATES = 8;

    GameState* stateStack[MAX_STATES];
    int         stackTop;

public:

    GameStateManager();
    ~GameStateManager();

    void        pushState(GameState* state);
    void        popState();
    void        changeState(GameState* state);

    void        update(float dt);
    void        render(RenderWindow& window);

    GameState* getCurrentState();
    bool        isEmpty();
};


GameStateManager::GameStateManager() {
    stackTop = -1;
    for (int i = 0; i < MAX_STATES; i++) {
        stateStack[i] = nullptr;
    }
}

GameStateManager::~GameStateManager() {
    while (isEmpty() == false) {
        popState();
    }
}

void GameStateManager::pushState(GameState* state) {
    if (state == nullptr) {
        return;
    }
    if (stackTop >= MAX_STATES - 1) {
        return;
    }
    stackTop++;
    stateStack[stackTop] = state;
    state->onEnter();
}

void GameStateManager::popState() {
    if (isEmpty()) {
        return;
    }
    stateStack[stackTop]->onExit();
    delete stateStack[stackTop];
    stateStack[stackTop] = nullptr;
    stackTop--;
}

void GameStateManager::changeState(GameState* state) {
    popState();
    pushState(state);
}

void GameStateManager::update(float dt) {
    if (isEmpty() == false) {
        stateStack[stackTop]->update(dt);
    }
}

void GameStateManager::render(RenderWindow& window) {
    if (isEmpty() == false) {
        stateStack[stackTop]->render(window);
    }
}

GameState* GameStateManager::getCurrentState() {
    if (isEmpty()) {
        return nullptr;
    }
    return stateStack[stackTop];
}

bool GameStateManager::isEmpty() {
    return stackTop < 0;
}