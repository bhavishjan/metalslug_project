#define _CRT_SECURE_NO_WARNINGS
#include "GameState.h"
#include "PlayerSoldier.h"

// GameState stuff
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

// EntityManager stuff
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

// PlayState implementations
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

// GameStateManager implementations
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