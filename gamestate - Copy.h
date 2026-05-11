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

    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual void update(float dt) = 0;
    virtual void render(RenderWindow& window) = 0;

    virtual void pause();
    virtual void resume();

    bool isActive() const;
    bool isPaused() const;
};




//Level composition manager
// LevelManager is defined in Level.h to avoid circular dependencies
// LevelManager implementations moved to Level.h




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




//Concrete play state
class PlayState : public GameState {

private:

    GameMode* activeMode;

    LevelManager* levelManager;
    EntityManager* entityManager;



    whisper originalBGM;
    whisper muffledBGM;
    SoundBuffer normalBuffer;
    SoundBuffer muffledBuffer;
    Sound gameSound;
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