#pragma once
#include "Level.h"       
#include "Enemy.h"
#include "Weapon.h"
#include "PlayerSoldier.h"
#include <SFML/Graphics.hpp>
using namespace sf;

//  Forward declarations 
class SupplyCrate;
class SlugMariner;
class EnemyManager;
class IronNokana;
class HairbusterRiberts;
class SeaSatan;
class Paratrooper;
class Martian;
class Zombie;
class MummyWarrior;
class FlyingTara;


//  BOSS BASE CLASS
//  Sab bosses isse inherit karenge - polymorphism ke liye
class Boss {
protected:
    float x, y;
    float width, height;
    float hp;
    float maxHp;
    bool  isAlive;
    bool  hasRetreated;
    int   scoreValue;
    float speed;
    BulletManager* bulletMgr;
    PlayerSoldier* target;

public:
    Boss();
    virtual ~Boss();

    virtual void update(float dt) = 0;
    virtual void render(RenderWindow& window, float camX, float camY) = 0;
    virtual void attack() = 0;

    virtual void takeDamage(int dmg);
    bool shouldRetreat() const;
    void retreat();

    void setPosition(float nx, float ny);
    void setPlayer(PlayerSoldier* p);
    void setBulletManager(BulletManager* bm);

    float getX()     const;
    float getY()     const;
    float getWidth() const;
    float getHeight()const;
    float getHp()    const;
    float getMaxHp() const;
    bool  getIsAlive()    const;
    bool  getHasRetreated()const;
    int   getScoreValue() const;
};



// ================================================================
//  ULTIMATE BOSS — Phase 4
//  Teeno bosses fuse - cycles through states every 10-15 sec
//  Polymorphically uses each boss's attack/sprite
// ================================================================
class UltimateBoss {
private:
    // Separate health for each fused boss
    float groundHp;
    float aerialHp;
    float aquaticHp;
    float maxHp;

    // Current state
    int   currentState;   // 0=ground, 1=aerial, 2=aquatic
    float stateTimer;
    float stateDuration;  // 10-15 sec

    // Position
    float x, y;
    float width, height;
    bool  isAlive;

    // Bosses for polymorphic attacks
    IronNokana* groundBoss;
    HairbusterRiberts* aerialBoss;
    SeaSatan* aquaticBoss;

    BulletManager* bulletMgr;
    PlayerSoldier* target;

    RectangleShape bodyShape;

public:
    UltimateBoss(float startX, float startY);
    ~UltimateBoss();

    void setPlayer(PlayerSoldier* p);
    void setBulletManager(BulletManager* bm);
    void syncPositions();
    void cycleState();
    void takeDamage(int dmg);
    void update(float dt);
    void render(RenderWindow& window, float camX, float camY);

    bool getIsAlive()    const;
    int  getCurrentState()const;
    float getX()         const;
    float getY()         const;
    float getWidth()     const;
    float getHeight()    const;
};


// ================================================================
//  BOSS LEVEL — Full implementation
// ================================================================
class BossLevel : public SurvivalLevel {
private:
    int  currentPhase;
    bool isPhase1Complete;
    bool isPhase2Complete;
    bool isPhase3Complete;
    bool isPhase4Complete;

    IronNokana* groundBoss;
    HairbusterRiberts* aerialBoss;
    SeaSatan* aquaticBoss;
    UltimateBoss* ultimateBoss;

    // Minion system
    Enemy** bossMinions;
    int          minionCount;
    static const int MAX_MINIONS = 20;
    int          minionBatchSize;
    int          minionBatchKilled;
    int          totalMinionBatches;

    bool blendedBiomeActive;

    BulletManager* bulletMgr;
    PlayerSoldier* playerRef;

    // Phase transition message timer
    float phaseMessageTimer;

public:
    BossLevel();
    ~BossLevel();

    void setBulletManager(BulletManager* bm);
    void setPlayerRef(PlayerSoldier* p);

    void generateBiomes() override;
    void spawnEnemies(EnemyManager& mgr, PlayerSoldier* player) override;

    void startPhase1();
    void startPhase2();
    void startPhase3();
    void startPhase4();
    void spawnMinionsForPhase(int phase);
    void checkBulletHitsOnBosses(BulletManager& bm);
    void update(float dt) override;
    void updateMinions(float dt) const;
    void render(RenderWindow& window, float camX, float camY);
    bool checkLevelComplete() override;

    int  getCurrentPhase()    const;
    bool getPhase1Complete()  const;
    bool getPhase4Complete()  const;
    Enemy* getMinionAt(int i) const;
    int getMaxMinions() const;
};