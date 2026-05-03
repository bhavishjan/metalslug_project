#pragma once
#include "Header.h"
#include "Block.h"
#include <SFML/Graphics.hpp>

class Weapon;
class Vehicle;
class Pistol;
class Knife;

// One sprite-sheet animation: texture + a uniform horizontal strip of frames
class Animation {
private:
    static const int MAX_FRAMES = 32;
    Texture texture;
    IntRect frames[MAX_FRAMES];
    int frameCount;
    int currentFrame;
    float timer;
    float frameTime;
    
    // Secondary layer for legs (optional)
    Texture legsTexture;
    IntRect legsFrames[MAX_FRAMES];
    int legsFrameCount;
    int legsCurrentFrame;
    float legsTimer;
    float legsFrameTime;
    int legsOffsetY;  // Vertical offset from torso to draw legs
    int torsoOffsetY; // Vertical offset for torso (raise above ground)

public:
    Animation() : frameCount(0), currentFrame(0), timer(0.0f), frameTime(0.08f),
                   legsFrameCount(0), legsCurrentFrame(0), legsTimer(0.0f), legsFrameTime(0.08f), legsOffsetY(0), torsoOffsetY(0) {}

    // Build frames in a loop along a horizontal strip on the sheet
    void load(const char* path, int x, int y, int frameW, int frameH, int count, int stride, float seconds) {
        texture.loadFromFile(path);
        frameTime = seconds;
        frameCount = count > MAX_FRAMES ? MAX_FRAMES : count;
        for (int i = 0; i < frameCount; i++) {
            frames[i] = IntRect(x + i * stride, y, frameW, frameH);
        }
    }
    
    // Load secondary legs layer
    void loadLegs(const char* path, int x, int y, int frameW, int frameH, int count, int stride, float seconds, int offsetY) {
        legsTexture.loadFromFile(path);
        legsFrameTime = seconds;
        legsFrameCount = count > MAX_FRAMES ? MAX_FRAMES : count;
        legsOffsetY = offsetY;
        for (int i = 0; i < legsFrameCount; i++) {
            legsFrames[i] = IntRect(x + i * stride, y, frameW, frameH);
        }
    }

    void update(float dt) {
        if (frameCount <= 0) 
            return;
        timer += dt;
        while (timer >= frameTime) {
            timer -= frameTime;
            currentFrame = (currentFrame + 1) % frameCount;
        }
        
        // Update legs layer if present
        if (legsFrameCount > 0) {
            legsTimer += dt;
            while (legsTimer >= legsFrameTime) {
                legsTimer -= legsFrameTime;
                legsCurrentFrame = (legsCurrentFrame + 1) % legsFrameCount;
            }
        }
    }

    void reset() { currentFrame = 0; timer = 0.0f; legsCurrentFrame = 0; legsTimer = 0.0f; }

    bool hasFrames() const { 
        return frameCount > 0; 
    }
    
    bool hasLegs() const {
        return legsFrameCount > 0;
    }

    void setTorsoOffset(int offset) {
        torsoOffsetY = offset;
    }

    const Texture& getTexture() const { 
        return texture; 
    }
    
    const Texture& getLegsTexture() const {
        return legsTexture;
    }

    IntRect currentRect() const {
        return frameCount > 0 ? frames[currentFrame] : IntRect();
    }
    
    IntRect currentLegsRect() const {
        return legsFrameCount > 0 ? legsFrames[legsCurrentFrame] : IntRect();
    }
    
    int getLegsOffsetY() const {
        return legsOffsetY;
    }

    int getTorsoOffsetY() const {
        return torsoOffsetY;
    }
};

class Player {
protected:
    // Animation slots (add more later: idle, jump, shoot, ...)
    enum { ANIM_WALK = 0, ANIM_IDLE = 1, ANIM_JUMP = 2, ANIM_COUNT = 8 };
    const char* name;
    float player_x;
    float player_y;
    float width;
    float height;
    float velocityX;
    float velocityY;
    int hp;
    int maxHp;
    int lives;
    bool isGrounded;
    bool isCrouching;
    bool isJumping;
    int grenadeCount;
    int grenadeType;
    Weapon* currentWeapon;
    Pistol* pistol;
    Knife* knife;
    bool facingRight;
    float aimAngle;
    int state;
    int damageState;
    float damageTimer;
    float stateTimer;
    float meleeCooldown;
    float meleeTimer;
    bool inVehicle;
    Vehicle* currentVehicle;
    bool isAlive;
    bool isImmortal;
    bool isFat;
    bool isInWater;
    int score;
    int saturation;
    bool specialPowerActive;
    float specialPowerTimer;
    float specialPowerDuration;
    Weapon* weaponSlots[2];
    int activeWeaponIndex;
    float scale_x;
    float scale_y;
    float moveAcceleration;
    Animation anims[ANIM_COUNT];
    Sprite sprite;
    int currentAnim;

    // Pick the right anim for current state, advance its timer
    void updateAnimations(float dt) {
        bool walking = velocityX > 0.1f || velocityX < -0.1f;
        currentAnim = walking ? ANIM_WALK : ANIM_WALK;
        if (!walking) {
            anims[ANIM_WALK].reset();
            return;
        }
        anims[currentAnim].update(dt);
    }

    // Draw the current animation at player's bottom-center, offset by camera
    void renderCurrent(RenderWindow& window, float camX, float camY) {
        Animation& a = anims[currentAnim];
        if (!a.hasFrames()) return;
        
        // Draw legs first if they exist
        if (a.hasLegs()) {
            IntRect legsR = a.currentLegsRect();
            sprite.setTexture(a.getLegsTexture());
            sprite.setTextureRect(legsR);
            sprite.setOrigin(legsR.width / 2.0f, (float)legsR.height);
            sprite.setPosition(player_x + width / 2.0f - camX,
                               player_y + height - camY + a.getLegsOffsetY() * scale_y);
            sprite.setScale(facingRight ? scale_x : -scale_x, scale_y);
            window.draw(sprite);
        }
        
        // Draw torso on top (raised by torsoOffset above legs baseline)
        IntRect r = a.currentRect();
        sprite.setTexture(a.getTexture());
        sprite.setTextureRect(r);
        sprite.setOrigin(r.width / 2.0f, (float)r.height);
        sprite.setPosition(player_x + width / 2.0f - camX,
                           player_y + height - camY - a.getTorsoOffsetY() * scale_y);
        sprite.setScale(facingRight ? scale_x : -scale_x, scale_y);
        window.draw(sprite);
    }
public:

    Player() {
        player_x = 100.0f;
        player_y = 500.0f;
        scale_x = 0.5f;
        scale_y = 0.5f;
        width = (int)(115 * scale_x);
        height = (int)(158 * scale_y);
        velocityX = 0.0f;
        velocityY = 0.0f;
        hp = 100;
        maxHp = 100;
        lives = 3;
        isGrounded = false;
        isCrouching = false;
        isJumping = false;
        grenadeCount = 10;
        grenadeType = 0;
        currentWeapon = nullptr;
        pistol = nullptr;
        knife = nullptr;
        facingRight = true;
        aimAngle = 0;
        state = 0;
        damageState = 0;
        damageTimer = 0;
        stateTimer = 0;
        meleeCooldown = 0.5f;
        meleeTimer = 0;
        inVehicle = false;
        currentVehicle = nullptr;
        isAlive = true;
        isImmortal = false;
        isFat = false;
        isInWater = false;
        score = 0;
        saturation = 100;
        specialPowerActive = false;
        specialPowerTimer = 0;
        specialPowerDuration = 10.0f;
        weaponSlots[0] = nullptr;
        weaponSlots[1] = nullptr;
        activeWeaponIndex = 0;
        moveAcceleration = 0.6f;
        currentAnim = ANIM_WALK;
    }

    ~Player() {
    }

    void jump() {
        if (isGrounded && !inVehicle) {
            velocityY = -12.0f;
            isJumping = true;
            isGrounded = false;
        }
    }

    void moveLeft() {
        velocityX -= moveAcceleration;
    }

    void moveRight() {
        velocityX += moveAcceleration;
    }

    virtual void flipToLeft() = 0;
    virtual void flipToRight() = 0;

    bool isFacingRight() const {
        return facingRight;
    }

    void crouch() {
        if (!inVehicle) {
            isCrouching = true;
            height = 50.0f;
        }
    }

    void shoot() {
        if (currentWeapon && !inVehicle) {
            cout << "Player shoots" << endl;
        }
    }

    void meleeAttack() {
        if (meleeTimer <= 0 && !inVehicle) {
            cout << "Player melee attack" << endl;
            meleeTimer = meleeCooldown;
        }
    }

    void throwGrenade() {
        if (grenadeCount > 0 && !inVehicle) {
            cout << "Player throws grenade" << endl;
            grenadeCount--;
        }
    }

    void takeDamage(int damage) {
        if (!isImmortal && isAlive) {
            hp -= damage;
            if (hp <= 0) {
                hp = 0;
                die();
            }
            updateDamageState();
        }
    }

    void takeMeleeDamage(int damage) {
        takeDamage(damage);
    }

    void die() {
        isAlive = false;
        lives--;
        if (lives > 0) {
            respawn();
        }
    }

    void respawn() {
        hp = maxHp;
        isAlive = true;
        player_x = 100.0f;
        player_y = 500.0f;
        velocityX = 0;
        velocityY = 0;
    }

    void pickupWeapon(Weapon* weapon) {
        currentWeapon = weapon;
    }

    void pickupFood(int saturationValue) {
        saturation += saturationValue;
        if (saturation > 100) {
            saturation = 100;
        }
    }

    void enterVehicle(Vehicle* vehicle) {
        currentVehicle = vehicle;
        inVehicle = true;
    }

    void exitVehicle() {
        currentVehicle = nullptr;
        inVehicle = false;
    }

    void switchWeapon() {
        activeWeaponIndex = (activeWeaponIndex + 1) % 2;
        currentWeapon = weaponSlots[activeWeaponIndex];
    }

    void updateDamageState() {
        float healthPercent = (float)hp / maxHp;
        if (healthPercent > 0.6f) {
            damageState = 0;
        }
        else if (healthPercent > 0.3f) {
            damageState = 1;
        }
        else {
            damageState = 2;
        }
    }

    void updateState() {
        if (stateTimer > 0) {
            stateTimer -= 0.016f;
        }
    }

    void becomeUndead() {
        state = 1;
        stateTimer = 10.0f;
        velocityX *= 0.7f;
    }

    void becomeMummy() {
        state = 2;
        stateTimer = 10.0f;
        currentWeapon = nullptr;
    }

    void revertState() {
        state = 0;
        stateTimer = 0;
    }

    void freePrisoner() {
        cout << "Player frees prisoner" << endl;
    }

    void aim(float angle) {
        aimAngle = angle;
    }

    void applyFannumTax() {
        if (isFat) {
            width *= 1.5f;
            height *= 1.5f;
        }
    }

    void applyGravity() {
        if (!isGrounded && !inVehicle) {
            velocityY += 0.5f;
        }
    }

    void checkGrounded() {
        if (player_y >= 700) {
            player_y = 700;
            velocityY = 0;
            isGrounded = true;
            isJumping = false;
        }
        else {
            isGrounded = false;
        }
    }

    void checkCollision() {
    }

    void enterWater() {
        isInWater = true;
        velocityX *= 0.5f;
        velocityY *= 0.5f;
    }

    void exitWater() {
        isInWater = false;
        velocityX *= 2.0f;
        velocityY *= 2.0f;
    }

    float getDistanceFrom(float targetX, float targetY) {
        float dx = player_x - targetX;
        float dy = player_y - targetY;
        return sqrt(dx * dx + dy * dy);
    }

    bool isNearEnemy() {
        return false;
    }

    bool isNearPrisoner() {
        return false;
    }

    void updateInputHistory() {
    }

    virtual void updateAnimation(float dt) {
        updateAnimations(dt);
    }

    void update() {
        applyGravity();
        checkGrounded();
        updateState();

        if (meleeTimer > 0) {
            meleeTimer -= 0.016f;
        }

        if (specialPowerTimer > 0) {
            specialPowerTimer -= 0.016f;
            if (specialPowerTimer <= 0) {
                specialPowerActive = false;
            }
        }
    }

    virtual void render(RenderWindow& window, float camX = 0, float camY = 0) = 0;

    void applyVehicleBonus() {
    }

    void removeVehicleBonus() {
    }

    //GETTERS & SETTERS
    float getFireRate() {
        return 0.2f;
    }

    float getBlastRadius() {
        return 50.0f;
    }

    int getGrenadeCount() {
        return grenadeCount;
    }

    float getMeleeRange() {
        return 30.0f;
    }

    int getMeleeDamage() {
        return 20;
    }

    bool isShielded() {
        return false;
    }

    bool getIsAlive() const {
        return isAlive;
    }

    float getPlayerX() const {
        return player_x;
    }

    float getPlayerY() const {
        return player_y;
    }

    void setPlayerX(float x) {
        player_x = x;
    }

    void setPlayerY(float y) {
        player_y = y;
    }

    void setPlayerPosition(float x, float y) {
        player_x = x;
        player_y = y;
    }

    float getVelocityX() const {
        return velocityX;
    }

    float getVelocityY() const {
        return velocityY;
    }

    void setVelocityX(float vx) {
        velocityX = vx;
    }
    void setVelocityY(float vy) {
        velocityY = vy;
    }

    void setVelocity(float vx, float vy) {
        velocityX = vx;
        velocityY = vy;
    }

    int getHP() const {
        return hp;
    }
    int getMaxHP() const {
        return maxHp;
    }
    int getLives() const {
        return lives;
    }
    int getWidth() const {
        return width;
    }
    int getHeight() const {
        return height;
    }
};

class Marco : public Player {
private:
    bool dualFireActive;
    float dualFireTimer;
    float dualFireDuration;

public:
    Marco() : Player() {
        name = "Marco";
        dualFireActive = false;
        dualFireTimer = 0;
        dualFireDuration = 10.0f;
        scale_x = 2.0f;
        scale_y = 2.0f;
        width = 60.0f;
        // Sprite is 29 source px tall * 2 scale = 58 screen px
        height = 58.0f;

        // Top-left walk-with-pistol strip (band 9): full-body, 15 frames, 30x29, stride 35
        anims[ANIM_WALK].load("Sprites/Marco Rossi 1.png", 10, 422, 30, 29, 15, 35, 0.08f);
    }

    void flipToLeft() override {
        facingRight = false;
    }

    void flipToRight() override {
        facingRight = true;
    }

    void render(RenderWindow& window, float camX = 0, float camY = 0) override {
        renderCurrent(window, camX, camY);
    }
    void shoot() {
        if (dualFireActive) {
            cout << "Marco dual fires!" << endl;
        }
        else {
            shoot();
        }
    }

    void meleeAttack() {
        cout << "Marco melee attack" << endl;
        meleeAttack();
    }

    void activateSpecialPower() {
        dualFireActive = true;
        dualFireTimer = dualFireDuration;
        specialPowerActive = true;
        specialPowerTimer = dualFireDuration;
    }

    void updateDualFire() {
        if (dualFireTimer > 0) {
            dualFireTimer -= 0.016f;
            if (dualFireTimer <= 0) {
                dualFireActive = false;
            }
        }
    }

    float getFireRate() {
        return dualFireActive ? 0.1f : 0.2f;
    }

    int getGrenadeCount() {
        return grenadeCount * 2;
    }

    void applyVehicleBonus() {
        velocityX += 2.0f;
    }
};

class Tarma : public Player {
private:
    bool immunityActive;
    float immunityTimer;
    float immunityDuration;
    float vehicleFireRateBonus;
    float vehicleDurabilityBonus;

public:
    Tarma() : Player() {
        name = "Tarma";
        immunityActive = false;
        immunityTimer = 0;
        immunityDuration = 5.0f;
        vehicleFireRateBonus = 1.5f;
        vehicleDurabilityBonus = 1.3f;
        scale_x = 2.0f;
        scale_y = 2.0f;
        width = 60.0f;
        height = 82.0f;

        // Torso: y=385, 36px tall, 8 frames at x=7, stride 34, width 29
        anims[ANIM_WALK].load("Sprites/Tarma Roving.png", 7, 385, 29, 36, 8, 34, 0.08f);
        // Legs: y=481, 29px tall, 15 frames at x=8, stride 38, width 30, offset 32
        anims[ANIM_WALK].loadLegs("Sprites/Tarma Roving.png", 8, 481, 30, 29, 15, 38, 0.08f, 32);
    }

    void flipToLeft() override {
        facingRight = false;
    }

    void flipToRight() override {
        facingRight = true;
    }

    void render(RenderWindow& window, float camX = 0, float camY = 0) override {
        renderCurrent(window, camX, camY);
    }

    void takeDamage(int damage) {
        if (immunityActive) {
            cout << "Tarma is immune!" << endl;
            return;
        }
        takeDamage(damage);
    }

    void enterVehicle(Vehicle* vehicle) {
        enterVehicle(vehicle);
        applyVehicleBonus();
    }

    void exitVehicle() {
        removeVehicleBonus();
        exitVehicle();
    }

    void onVehicleDestroyed() {
        activateSpecialPower();
    }

    void activateSpecialPower() {
        immunityActive = true;
        immunityTimer = immunityDuration;
        specialPowerActive = true;
        specialPowerTimer = immunityDuration;
    }

    void updateImmunity() {
        if (immunityTimer > 0) {
            immunityTimer -= 0.016f;
            if (immunityTimer <= 0) {
                immunityActive = false;
            }
        }
    }

    bool isInVehicle() {
        return inVehicle;
    }

    float getVehicleFireRate() {
        return 0.2f * vehicleFireRateBonus;
    }

    float getVehicleDurability() {
        return vehicleDurabilityBonus;
    }
};

class Eri : public Player {
private:
    bool doubleGrenadeActive;
    float doubleGrenadeTimer;
    float doubleGrenadeDuration;

public:
    Eri() : Player() {
        name = "Eri";
        doubleGrenadeActive = false;
        doubleGrenadeTimer = 0;
        doubleGrenadeDuration = 10.0f;
        scale_x = 2.0f;
        scale_y = 2.0f;
        width = 60.0f;
        height = 82.0f;

        // Full-body walking: y=387, 40px tall, 16 frames at x=5, stride 34, width 30
        anims[ANIM_WALK].load("Sprites/Eri Kasamoto.png", 5, 387, 30, 40, 16, 34, 0.08f);
    }

    void flipToLeft() override {
        facingRight = false;
    }

    void flipToRight() override {
        facingRight = true;
    }

    void render(RenderWindow& window, float camX = 0, float camY = 0) override {
        renderCurrent(window, camX, camY);
    }
    void throwGrenade() {
        if (doubleGrenadeActive && grenadeCount >= 2) {
            cout << "Eri throws double grenades!" << endl;
            grenadeCount -= 2;
            spawnFirePool();
        }
        else {
            throwGrenade();
        }
    }

    void activateSpecialPower() {
        doubleGrenadeActive = true;
        doubleGrenadeTimer = doubleGrenadeDuration;
        specialPowerActive = true;
        specialPowerTimer = doubleGrenadeDuration;
    }

    void updateDoubleGrenade() {
        if (doubleGrenadeTimer > 0) {
            doubleGrenadeTimer -= 0.016f;
            if (doubleGrenadeTimer <= 0) {
                doubleGrenadeActive = false;
            }
        }
    }

    void spawnFirePool() {
        cout << "Fire pool created!" << endl;
    }

    int getGrenadeCount() {
        return doubleGrenadeActive ? grenadeCount * 2 : grenadeCount;
    }

    float getBlastRadius() {
        return doubleGrenadeActive ? 80.0f : 50.0f;
    }

    void meleeAttack() {
        cout << "Eri melee attack" << endl;
        meleeAttack();
    }

    float getFireRate() {
        return 0.25f;
    }
};

class Fiolina : public Player {
private:
    bool superchargedActive;
    float superchargedTimer;
    float superchargedDuration;
public:
    Fiolina() : Player() {
        name = "Fiolina";
        superchargedActive = false;
        superchargedTimer = 0;
        superchargedDuration = 12.0f;
        scale_x = 2.0f;
        scale_y = 2.0f;
        width = 60.0f;
        height = 82.0f;

        // Full-body walking: y=420, 38px tall, 16 frames at x=10, stride 31, width 26
        anims[ANIM_WALK].load("Sprites/Fiolina Germi 1.png", 10, 420, 26, 38, 16, 31, 0.08f);
    }

    void flipToLeft() override {
        facingRight = false;
    }

    void flipToRight() override {
        facingRight = true;
    }

    void render(RenderWindow& window, float camX = 0, float camY = 0) override {
        renderCurrent(window, camX, camY);
    }

    void pickupWeapon(Weapon* weapon) {
        pickupWeapon(weapon);
        if (superchargedActive) {
            cout << "Fiolina gets supercharged ammo!" << endl;
        }
    }

    void activateSpecialPower() {
        superchargedActive = true;
        superchargedTimer = superchargedDuration;
        specialPowerActive = true;
        specialPowerTimer = superchargedDuration;
    }

    void updateSupercharged() {
        if (superchargedTimer > 0) {
            superchargedTimer -= 0.016f;
            if (superchargedTimer <= 0) {
                superchargedActive = false;
            }
        }
    }

    float getFireRate() {
        return superchargedActive ? 0.1f : 0.2f;
    }

    void meleeAttack() {
        cout << "Fiolina melee attack" << endl;
        meleeAttack();
    }

    int getGrenadeCount() {
        return superchargedActive ? grenadeCount + 5 : grenadeCount;
    }
};

class FusionCompanion {
private:
    bool isActive;
    bool isSpawned;
    bool isImmortal;
    float x;
    float y;
    float width;
    float height;
    int hp;
    int maxHp;
    float speed;
    float fireRate;
    int damage;
    int grenadeCount;
    float blastRadius;
    Player* fusedCharacters[4];
    int fusedCount;
    float spawnDuration;
    float spawnTimer;
    float cooldownTimer;
    float cooldownDuration;
    int spriteID;
    float followDistance;
    float averagedFireRate;
    float averagedSpeed;
    int averagedDamage;
    int averagedGrenadeCount;
    float averagedBlastRadius;

public:
    FusionCompanion() {
        isActive = false;
        isSpawned = false;
        isImmortal = true;
        x = 0;
        y = 0;
        width = 60.0f;
        height = 90.0f;
        hp = 200;
        maxHp = 200;
        speed = 4.0f;
        fireRate = 0.15f;
        damage = 25;
        grenadeCount = 20;
        blastRadius = 60.0f;
        fusedCount = 0;
        spawnDuration = 30.0f;
        spawnTimer = 0;
        cooldownTimer = 0;
        cooldownDuration = 60.0f;
        spriteID = 0;
        followDistance = 100.0f;
        averagedFireRate = 0;
        averagedSpeed = 0;
        averagedDamage = 0;
        averagedGrenadeCount = 0;
        averagedBlastRadius = 0;

        for (int i = 0; i < 4; i++) {
            fusedCharacters[i] = nullptr;
        }
    }

    ~FusionCompanion() {
    }

    void computeAveragedStats() {
        averageStats();
    }

    void inheritBuffsWeaknesses() {
    }

    void followPlayer() {
        cout << "Fusion companion follows player" << endl;
    }

    void attackNearestEnemy() {
        cout << "Fusion companion attacks enemy" << endl;
    }

    void findNearestEnemy() {
    }

    void moveTowardsPlayer() {
        followPlayer();
    }

    void moveTowardsEnemy() {
    }

    bool isExpired() {
        return spawnTimer <= 0;
    }

    void update() {
        if (isActive && isSpawned) {
            spawnTimer -= 0.016f;
            if (isExpired()) {
                despawn();
            }
            followPlayer();
            attackNearestEnemy();
        }

        if (cooldownTimer > 0) {
            cooldownTimer -= 0.016f;
        }
    }

    void render(RenderWindow& window) {
        //window.draw(/*fusion companion sprite*/);
    }

    void spawn() {
        isActive = true;
        isSpawned = true;
        spawnTimer = spawnDuration;
    }

    void despawn() {
        isActive = false;
        isSpawned = false;
        startCooldown();
    }

    void fuseCharacters(Player** characters, int count) {
        fusedCount = count;
        for (int i = 0; i < count && i < 4; i++) {
            fusedCharacters[i] = characters[i];
        }
        averageStats();
    }

    void averageStats() {
        if (fusedCount == 0) return;

        float totalFireRate = 0;
        float totalSpeed = 0;
        int totalDamage = 0;
        int totalGrenades = 0;
        float totalBlastRadius = 0;

        for (int i = 0; i < fusedCount; i++) {
            if (fusedCharacters[i]) {
                totalFireRate += fusedCharacters[i]->getFireRate();
                totalGrenades += fusedCharacters[i]->getGrenadeCount();
                totalBlastRadius += fusedCharacters[i]->getBlastRadius();
            }
        }

        averagedFireRate = totalFireRate / fusedCount;
        averagedGrenadeCount = totalGrenades / fusedCount;
        averagedBlastRadius = totalBlastRadius / fusedCount;
    }

    void inheritBuffs() {
    }

    void inheritWeaknesses() {
    }

    void fire() {
        cout << "Fusion companion fires" << endl;
    }

    void throwGrenade() {
        if (grenadeCount > 0) {
            cout << "Fusion companion throws grenade" << endl;
            grenadeCount--;
        }
    }

    bool canSpawn() {
        return !isActive && cooldownTimer <= 0;
    }

    void startCooldown() {
        cooldownTimer = cooldownDuration;
    }

    void updateCooldown() {
        if (cooldownTimer > 0) {
            cooldownTimer -= 0.016f;
        }
    }

    bool isCooldownExpired() {
        return cooldownTimer <= 0;
    }

    bool isSpawnExpired() {
        return spawnTimer <= 0;
    }

    void resetForNewLevel() {
        despawn();
        cooldownTimer = 0;
    }

    float getSurvivalCooldown() {
        return 60.0f;
    }

    float getCampaignCooldown() {
        return 120.0f;
    }

    void renderFollowPath() {
    }

};

class CharacterManager {
private:
    Player* characters[4];
    int activeIndex;
    FusionCompanion* fusionCompanion;
    float fusionCooldownTimer;
    int activeCharacterCount;

    void switchToIndexKeepingPosition(int index) {
        if (!isCharacterAvailable(index) || index == activeIndex)
            return;

        Player* oldPlayer = characters[activeIndex];
        Player* newPlayer = characters[index];

        if (oldPlayer && newPlayer) {
            newPlayer->setPlayerPosition(oldPlayer->getPlayerX(), oldPlayer->getPlayerY());
            newPlayer->setVelocity(oldPlayer->getVelocityX(), oldPlayer->getVelocityY());

            if (oldPlayer->isFacingRight()) {
                newPlayer->flipToRight();
            }
            else {
                newPlayer->flipToLeft();
            }
        }

        activeIndex = index;
    }

public:
    CharacterManager() {
        characters[0] = new Marco();
        characters[1] = new Tarma();
        characters[2] = new Eri();
        characters[3] = new Fiolina();
        activeIndex = 0;
        fusionCompanion = new FusionCompanion();
        fusionCooldownTimer = 0;
        activeCharacterCount = 4;
    }

    void switchCharacter() {
        int nextIndex = getNextAvailableIndex();
        if (nextIndex != -1) {
            switchToIndexKeepingPosition(nextIndex);
        }
    }

    void switchCharacterToIndex(int index) {
        if (isCharacterAvailable(index)) {
            switchToIndexKeepingPosition(index);
        }
    }

    Player* getActivePlayer() {
        return characters[activeIndex];
    }

    bool isCharacterAvailable(int index) {
        if (index < 0 || index >= 4)
            return false;
        return characters[index] && characters[index]->getIsAlive();
    }

    int getNextAvailableIndex() {
        for (int i = 0; i < 4; i++) {
            int nextIndex = (activeIndex + i + 1) % 4;
            if (isCharacterAvailable(nextIndex)) {
                return nextIndex;
            }
        }
        return -1;
    }

    bool allDead() {
        for (int i = 0; i < 4; i++) {
            if (characters[i] && characters[i]->getIsAlive()) {
                return false;
            }
        }
        return true;
    }

    void resetAll() {
        for (int i = 0; i < 4; i++) {
            if (characters[i]) {
                characters[i]->respawn();
            }
        }
        activeIndex = 0;
    }

    void spawnFusion() {
        if (fusionCompanion->canSpawn()) {
            Player* aliveCharacters[4];
            int count = 0;
            for (int i = 0; i < 4; i++) {
                if (characters[i] && characters[i]->getIsAlive()) {
                    aliveCharacters[count++] = characters[i];
                }
            }
            fusionCompanion->fuseCharacters(aliveCharacters, count);
            fusionCompanion->spawn();
        }
    }

    void update() {
        if (characters[activeIndex]) {
            characters[activeIndex]->update();
        }

        if (fusionCompanion) {
            fusionCompanion->update();
        }
    }

    void render(RenderWindow& window, float camX = 0, float camY = 0) {
        if (characters[activeIndex]) {
            characters[activeIndex]->render(window, camX, camY);
        }

        if (fusionCompanion) {
            fusionCompanion->render(window);
        }
    }
};
