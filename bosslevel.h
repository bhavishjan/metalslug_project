#pragma once
#include "Level.h"
#include "Enemy.h"
#include "Weapon.h"
#include "PlayerSoldier.h"
#include <SFML/Graphics.hpp>

// ================================================================
//  Forward declarations (implement these classes separately)
// ================================================================
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

// ================================================================
//  BOSS BASE CLASS
//  Sab bosses isse inherit karenge - polymorphism ke liye
// ================================================================
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
    Boss() {
        x = y = 0;
        width = height = 80;
        hp = maxHp = 30;
        isAlive = true;
        hasRetreated = false;
        scoreValue = 500;
        speed = 30.f;
        bulletMgr = nullptr;
        target = nullptr;
    }

    virtual ~Boss() {}

    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window, float camX, float camY) = 0;
    virtual void attack() = 0;

    virtual void takeDamage(int dmg) {
        hp -= dmg;
        if (hp <= 0) {
            hp = 0;
            isAlive = false;
        }
    }

    // 50% HP pe retreat
    bool shouldRetreat() const {
        return (hp <= maxHp * 0.5f) && !hasRetreated;
    }

    void retreat() {
        hasRetreated = true;
        isAlive = false;  // screen se hat jao
    }

    void setPosition(float nx, float ny) { x = nx; y = ny; }
    void setPlayer(PlayerSoldier* p) { target = p; }
    void setBulletManager(BulletManager* bm) { bulletMgr = bm; }

    float getX()     const { return x; }
    float getY()     const { return y; }
    float getWidth() const { return width; }
    float getHeight()const { return height; }
    float getHp()    const { return hp; }
    float getMaxHp() const { return maxHp; }
    bool  getIsAlive()    const { return isAlive; }
    bool  getHasRetreated()const { return hasRetreated; }
    int   getScoreValue() const { return scoreValue; }
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

    sf::RectangleShape bodyShape;

public:
    UltimateBoss(float startX, float startY) {
        x = startX;
        y = startY;
        width = 100.f;
        height = 80.f;
        maxHp = 30.f;
        groundHp = aerialHp = aquaticHp = maxHp;
        currentState = 0;
        stateTimer = 12.f;  // 10-15 sec ke beech
        stateDuration = 12.f;
        isAlive = true;
        bulletMgr = nullptr;
        target = nullptr;

        // Create sub-bosses for polymorphic use
        groundBoss = new IronNokana();
        aerialBoss = new HairbusterRiberts();
        aquaticBoss = new SeaSatan();

        bodyShape.setSize(sf::Vector2f(width, height));
    }

    ~UltimateBoss() {
        delete groundBoss;
        delete aerialBoss;
        delete aquaticBoss;
    }

    void setPlayer(PlayerSoldier* p) {
        target = p;
        groundBoss->setPlayer(p);
        aerialBoss->setPlayer(p);
        aquaticBoss->setPlayer(p);
    }

    void setBulletManager(BulletManager* bm) {
        bulletMgr = bm;
        groundBoss->setBulletManager(bm);
        aerialBoss->setBulletManager(bm);
        aquaticBoss->setBulletManager(bm);
    }

    // Sync sub-boss position with ultimate boss
    void syncPositions() {
        groundBoss->setPosition(x, y);
        aerialBoss->setPosition(x, y);
        aquaticBoss->setPosition(x, y);
    }

    // Cycle state: ground -> aerial -> aquatic -> ground
    void cycleState() {
        // Skip states where that boss health is depleted
        for (int attempt = 0; attempt < 3; attempt++) {
            currentState = (currentState + 1) % 3;
            if (currentState == 0 && groundHp > 0) break;
            if (currentState == 1 && aerialHp > 0) break;
            if (currentState == 2 && aquaticHp > 0) break;
        }
        stateTimer = stateDuration;
    }

    void takeDamage(int dmg) {
        // Damage current state's health
        if (currentState == 0) groundHp -= dmg;
        if (currentState == 1) aerialHp -= dmg;
        if (currentState == 2) aquaticHp -= dmg;

        if (groundHp < 0) groundHp = 0;
        if (aerialHp < 0) aerialHp = 0;
        if (aquaticHp < 0) aquaticHp = 0;

        // Sab dead = truly dead
        if (groundHp <= 0 && aerialHp <= 0 && aquaticHp <= 0) {
            isAlive = false;
        }
    }

    void update(float dt) {
        if (!isAlive) return;

        syncPositions();

        // State timer countdown
        stateTimer -= dt;
        if (stateTimer <= 0.f) {
            cycleState();
        }

        // Move toward player (basic)
        if (target) {
            float dx = target->getPlayerX() - x;
            float speed = 50.f;
            x += (dx > 0 ? speed : -speed) * dt;
        }

        // Polymorphic attack based on current state
        if (currentState == 0 && groundHp > 0)
            groundBoss->attack();
        else if (currentState == 1 && aerialHp > 0)
            aerialBoss->attack();
        else if (currentState == 2 && aquaticHp > 0)
            aquaticBoss->attack();
    }

    void render(sf::RenderWindow& window, float camX, float camY) {
        if (!isAlive) return;

        // Sprite changes based on state (polymorphic)
        sf::Color stateColor;
        if (currentState == 0) stateColor = sf::Color(180, 80, 0);   // ground = orange
        else if (currentState == 1) stateColor = sf::Color(60, 60, 180); // aerial = blue
        else                        stateColor = sf::Color(30, 80, 60);   // aquatic = green

        bodyShape.setFillColor(stateColor);
        bodyShape.setPosition(x - camX, y - camY);
        window.draw(bodyShape);

        // Three HP bars
        float barW = width;
        float barY = y - camY - 36.f;

        // Ground HP
        sf::RectangleShape g(sf::Vector2f(barW * (groundHp / maxHp), 7.f));
        g.setFillColor(sf::Color(220, 80, 0));
        g.setPosition(x - camX, barY);
        window.draw(g);

        // Aerial HP
        sf::RectangleShape a(sf::Vector2f(barW * (aerialHp / maxHp), 7.f));
        a.setFillColor(sf::Color(80, 80, 220));
        a.setPosition(x - camX, barY + 9.f);
        window.draw(a);

        // Aquatic HP
        sf::RectangleShape aq(sf::Vector2f(barW * (aquaticHp / maxHp), 7.f));
        aq.setFillColor(sf::Color(50, 200, 100));
        aq.setPosition(x - camX, barY + 18.f);
        window.draw(aq);
    }

    bool getIsAlive()    const { return isAlive; }
    int  getCurrentState()const { return currentState; }
    float getX()         const { return x; }
    float getY()         const { return y; }
    float getWidth()     const { return width; }
    float getHeight()    const { return height; }
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
    BossLevel() : SurvivalLevel("Boss Level", 4, 83, 15)
    {
        currentPhase = 1;
        isPhase1Complete = false;
        isPhase2Complete = false;
        isPhase3Complete = false;
        isPhase4Complete = false;

        groundBoss = nullptr;
        aerialBoss = nullptr;
        aquaticBoss = nullptr;
        ultimateBoss = nullptr;

        bossMinions = new Enemy * [MAX_MINIONS];
        for (int i = 0; i < MAX_MINIONS; i++) bossMinions[i] = nullptr;
        minionCount = 0;
        minionBatchSize = 3;
        minionBatchKilled = 0;
        totalMinionBatches = 0;

        blendedBiomeActive = false;
        bulletMgr = nullptr;
        playerRef = nullptr;
        phaseMessageTimer = 0.f;

        scoreMultiplier = 3.0f;
        playerSpawnX = 400.f;
        playerSpawnY = 0.f;
        totalEnemies = 1;

        generateBiomes();
    }

    ~BossLevel() {
        delete groundBoss;
        delete aerialBoss;
        delete aquaticBoss;
        delete ultimateBoss;
        if (bossMinions) {
            for (int i = 0; i < MAX_MINIONS; i++) {
                delete bossMinions[i];
            }
            delete[] bossMinions;
        }
    }

    void setBulletManager(BulletManager* bm) { bulletMgr = bm; }
    void setPlayerRef(PlayerSoldier* p) { playerRef = p; }

    void generateBiomes() override {
        plains = new PlainsBiome(levelStart, plainsEnd);
        aerial = new AerialBiome(plainsEnd, aerialEnd);
        aquatic = new AquaticBiome(aerialEnd, aquaticEnd);

        loadTextures("Sprites/blocks/stone.png",
            "Sprites/blocks/water.png",
            "Sprites/blocks/grass.png",
            "Sprites/blocks/dirt.png");

        plains->generateTerrain(biomeWidth, biomeHeight);
        aerial->generateTerrain(biomeWidth, biomeHeight);
        aquatic->generateTerrain(biomeWidth, biomeHeight);

        isLoaded = true;
    }

    void spawnEnemies(EnemyManager& mgr, PlayerSoldier* player) override {
        playerRef = player;
        startPhase1();
    }

    // ---- PHASE 1: Iron Nokana in Plains ----
    void startPhase1() {
        currentPhase = 1;
        delete groundBoss;
        groundBoss = new IronNokana();
        // Spawn in plains biome center
        groundBoss->setPosition(levelStart + (plainsEnd - levelStart) * 0.5f, 400.f);
        if (playerRef)  groundBoss->setPlayer(playerRef);
        if (bulletMgr)  groundBoss->setBulletManager(bulletMgr);

        spawnMinionsForPhase(1);
        phaseMessageTimer = 3.f;
    }

    // ---- PHASE 2: Hairbuster Riberts in Aerial ----
    void startPhase2() {
        currentPhase = 2;
        delete aerialBoss;
        aerialBoss = new HairbusterRiberts();
        // Spawn in aerial biome
        aerialBoss->setPosition(plainsEnd + (aerialEnd - plainsEnd) * 0.5f, 200.f);
        if (playerRef) aerialBoss->setPlayer(playerRef);
        if (bulletMgr) aerialBoss->setBulletManager(bulletMgr);

        spawnMinionsForPhase(2);
        phaseMessageTimer = 3.f;
    }

    // ---- PHASE 3: Sea Satan in Aquatic ----
    void startPhase3() {
        currentPhase = 3;
        delete aquaticBoss;
        aquaticBoss = new SeaSatan();
        // Spawn in aquatic biome
        aquaticBoss->setPosition(aerialEnd + (aquaticEnd - aerialEnd) * 0.5f, 450.f);
        if (playerRef) aquaticBoss->setPlayer(playerRef);
        if (bulletMgr) aquaticBoss->setBulletManager(bulletMgr);

        spawnMinionsForPhase(3);
        phaseMessageTimer = 3.f;
    }

    // ---- PHASE 4: Ultimate Boss (fused) ----
    void startPhase4() {
        currentPhase = 4;
        blendedBiomeActive = true;

        delete ultimateBoss;
        // Spawn in middle of level
        float midX = (levelStart + levelEnd) * 0.5f;
        ultimateBoss = new UltimateBoss(midX, 350.f);
        if (playerRef) ultimateBoss->setPlayer(playerRef);
        if (bulletMgr) ultimateBoss->setBulletManager(bulletMgr);

        spawnMinionsForPhase(4);
        phaseMessageTimer = 3.f;
    }

    // Spawn minions based on phase type
    void spawnMinionsForPhase(int phase) {
        // Clear old minions
        for (int i = 0; i < MAX_MINIONS; i++) {
            delete bossMinions[i];
            bossMinions[i] = nullptr;
        }
        minionCount = 0;
        minionBatchKilled = 0;

        float spawnX = levelStart + 300.f;
        float spawnY = 300.f;
        int   batch = minionBatchSize;

        for (int i = 0; i < batch && minionCount < MAX_MINIONS; i++) {
            Enemy* m = nullptr;
            if (phase == 1) m = new RebelSoldier();    // ground minion
            if (phase == 2) m = new Paratrooper();     // aerial minion
            if (phase == 3) m = new Zombie();           // aquatic minion
            if (phase == 4) m = new Martian();          // alien minion
            if (m) {
                m->setPosition(spawnX + i * 120.f, spawnY);
                if (playerRef) m->setPlayer(playerRef);
                if (bulletMgr) m->setBulletManager(bulletMgr);
                bossMinions[minionCount++] = m;
            }
        }
        totalMinionBatches++;
    }

    // Check if player bullet hit a boss
    void checkBulletHitsOnBosses(BulletManager& bm) {
        // Phase 1
        if (currentPhase == 1 && groundBoss && groundBoss->getIsAlive()) {
            int dmg = 0; float hx, hy;
            if (bm.popPlayerBulletHit(groundBoss->getX(), groundBoss->getY(),
                groundBoss->getWidth(), groundBoss->getHeight(),
                dmg, hx, hy)) {
                groundBoss->takeDamage(dmg);
            }
        }
        // Phase 2
        if (currentPhase == 2 && aerialBoss && aerialBoss->getIsAlive()) {
            int dmg = 0; float hx, hy;
            if (bm.popPlayerBulletHit(aerialBoss->getX(), aerialBoss->getY(),
                aerialBoss->getWidth(), aerialBoss->getHeight(),
                dmg, hx, hy)) {
                aerialBoss->takeDamage(dmg);
            }
        }
        // Phase 3
        if (currentPhase == 3 && aquaticBoss && aquaticBoss->getIsAlive()) {
            int dmg = 0; float hx, hy;
            if (bm.popPlayerBulletHit(aquaticBoss->getX(), aquaticBoss->getY(),
                aquaticBoss->getWidth(), aquaticBoss->getHeight(),
                dmg, hx, hy)) {
                aquaticBoss->takeDamage(dmg);
            }
        }
        // Phase 4
        if (currentPhase == 4 && ultimateBoss && ultimateBoss->getIsAlive()) {
            int dmg = 0; float hx, hy;
            if (bm.popPlayerBulletHit(ultimateBoss->getX(), ultimateBoss->getY(),
                ultimateBoss->getWidth(), ultimateBoss->getHeight(),
                dmg, hx, hy)) {
                ultimateBoss->takeDamage(dmg);
            }
        }
    }

    void update(float dt) override {
        Level::update(dt);

        if (phaseMessageTimer > 0.f) phaseMessageTimer -= dt;

        // ---- Phase 1 update ----
        if (currentPhase == 1) {
            if (groundBoss) groundBoss->update(dt);

            // Minions update
            updateMinions(dt);

            // Phase 1 complete: boss retreated
            if (groundBoss && groundBoss->getHasRetreated() && !isPhase1Complete) {
                isPhase1Complete = true;
                startPhase2();
            }
        }

        // ---- Phase 2 update ----
        else if (currentPhase == 2) {
            if (aerialBoss) aerialBoss->update(dt);
            updateMinions(dt);

            if (aerialBoss && aerialBoss->getHasRetreated() && !isPhase2Complete) {
                isPhase2Complete = true;
                startPhase3();
            }
        }

        // ---- Phase 3 update ----
        else if (currentPhase == 3) {
            if (aquaticBoss) aquaticBoss->update(dt);
            updateMinions(dt);

            if (aquaticBoss && aquaticBoss->getHasRetreated() && !isPhase3Complete) {
                isPhase3Complete = true;
                startPhase4();
            }
        }

        // ---- Phase 4 update ----
        else if (currentPhase == 4) {
            if (ultimateBoss) ultimateBoss->update(dt);
            updateMinions(dt);

            if (ultimateBoss && !ultimateBoss->getIsAlive() && !isPhase4Complete) {
                isPhase4Complete = true;
                isComplete = true;
            }
        }
    }

    void updateMinions(float dt) {
        for (int i = 0; i < MAX_MINIONS; i++) {
            if (!bossMinions[i]) continue;
            if (!bossMinions[i]->getIsAlive()) continue;
            bossMinions[i]->update(dt);
        }
    }

    void render(sf::RenderWindow& window, float camX, float camY) override {
        Level::render(window, camX, camY);

        // Render active boss
        if (currentPhase == 1 && groundBoss)
            groundBoss->render(window, camX, camY);
        else if (currentPhase == 2 && aerialBoss)
            aerialBoss->render(window, camX, camY);
        else if (currentPhase == 3 && aquaticBoss)
            aquaticBoss->render(window, camX, camY);
        else if (currentPhase == 4 && ultimateBoss)
            ultimateBoss->render(window, camX, camY);

        // Render minions
        for (int i = 0; i < MAX_MINIONS; i++) {
            if (bossMinions[i] && bossMinions[i]->getIsAlive())
                bossMinions[i]->render(window, camX, camY);
        }

        // Phase message
        if (phaseMessageTimer > 0.f) {
            sf::Font font;
            font.loadFromFile("arial.TTF");
            sf::Text msg;
            msg.setFont(font);
            msg.setCharacterSize(60);
            msg.setFillColor(sf::Color::Red);

            if (currentPhase == 1) msg.setString("PHASE 1 - IRON NOKANA!");
            else if (currentPhase == 2) msg.setString("PHASE 2 - HAIRBUSTER RIBERTS!");
            else if (currentPhase == 3) msg.setString("PHASE 3 - SEA SATAN!");
            else if (currentPhase == 4) msg.setString("PHASE 4 - ULTIMATE BOSS!");

            sf::FloatRect bounds = msg.getLocalBounds();
            msg.setPosition(800.f - bounds.width * 0.5f, 400.f);
            window.draw(msg);
        }
    }

    bool checkLevelComplete() override {
        return isPhase4Complete;
    }

    int  getCurrentPhase()    const { return currentPhase; }
    bool getPhase1Complete()  const { return isPhase1Complete; }
    bool getPhase4Complete()  const { return isPhase4Complete; }

    // Expose minions for Game.h collision resolution
    Enemy* getMinionAt(int i) {
        if (i >= 0 && i < MAX_MINIONS) return bossMinions[i];
        return nullptr;
    }
    int getMaxMinions() const { return MAX_MINIONS; }
};