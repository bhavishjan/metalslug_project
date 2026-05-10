//#pragma once
//// ============================================================
//// ABSTRACT BASE: Vehicle  [A]
//// Virtual base to solve diamond problem for multi-inheritance
//// ============================================================
//class Vehicle {
//protected:
//    string name;
//    float x, y;
//    float width, height;
//    int hp, maxHp;
//    float speed, maxSpeed;
//    bool isActive;
//    bool isDestroyed;
//    float velocityX, velocityY;
//    bool isMuffled;
//    Player* pilot;
//    bool hasPilot;
//    float gravity;
//    bool isAffectedByGravity;
//    float collisionBox;
//    int ownerType;
//
//public:
//    Vehicle(string n = "Vehicle", float w = 100, float h = 80,
//        int hp_ = 100, float spd = 3.0f, float maxSpd = 5.0f)
//        : name(n), x(0), y(0), width(w), height(h),
//        hp(hp_), maxHp(hp_), speed(spd), maxSpeed(maxSpd),
//        isActive(false), isDestroyed(false),
//        velocityX(0), velocityY(0), isMuffled(false),
//        pilot(nullptr), hasPilot(false),
//        gravity(0.5f), isAffectedByGravity(true),
//        collisionBox(w), ownerType(0)
//    {
//    }
//
//    virtual ~Vehicle() {}
//
//    // Pure virtual interface
//    virtual void update(float dt) = 0;
//    virtual void render()  const = 0;
//    virtual void fire() = 0;
//
//    virtual void takeDamage(int dmg) {
//        if (isDestroyed) return;
//        hp -= dmg;
//        if (hp <= 0) { hp = 0; destroy(); }
//    }
//
//    virtual void destroy() {
//        isDestroyed = true;
//        isActive = false;
//        if (hasPilot) ejectPilot();
//        cout << "  [" << name << "] destroyed!\n";
//    }
//
//    void move(float dt) {
//        x += velocityX * dt;
//        y += velocityY * dt;
//        if (isAffectedByGravity) velocityY += gravity * dt;
//    }
//
//    void mountPilot(Player* p) { pilot = p; hasPilot = true; }
//    void ejectPilot() { pilot = nullptr; hasPilot = false; }
//
//    void setMuffled(bool m) {
//        isMuffled = m;
//        if (m) cout << "  [Echo Chambers] FFT filter applied - muffled\n";
//    }
//
//    // Setters
//    void setPosition(float px, float py) { x = px; y = py; }
//    void setVelocityX(float vx) { velocityX = (vx > maxSpeed) ? maxSpeed : (vx < -maxSpeed) ? -maxSpeed : vx; }
//    void setVelocityY(float vy) { velocityY = vy; }
//    void setActive(bool a) { isActive = a; }
//    void setOwnerType(int o) { ownerType = o; }
//    void setGravityEnabled(bool en) { isAffectedByGravity = en; }
//    void setName(const string& n) { name = n; }
//
//    // Getters
//    bool   isAlive()        const { return !isDestroyed && hp > 0; }
//    int    getHp()          const { return hp; }
//    int    getMaxHp()       const { return maxHp; }
//    float  getX()           const { return x; }
//    float  getY()           const { return y; }
//    string getName()        const { return name; }
//    bool   getIsDestroyed() const { return isDestroyed; }
//    int    getOwnerType()   const { return ownerType; }
//
//protected:
//    void printStatus() const {
//        cout << "HP=" << hp << "/" << maxHp
//            << (isDestroyed ? " [DESTROYED]" : "")
//            << (isMuffled ? " [MUFFLED]" : "");
//    }
//};
//
//// ============================================================
//// ABSTRACT MID-TIER 1: GroundVehicle  [A]
//// ============================================================
//class GroundVehicle : public virtual Vehicle {
//protected:
//    bool  isGrounded;
//    bool  canTraverseBlocks;
//    float tiltAngle, maxTiltAngle;
//
//public:
//    GroundVehicle() : Vehicle(), isGrounded(true),
//        canTraverseBlocks(true), tiltAngle(0), maxTiltAngle(45) {
//    }
//
//    virtual void jump() {
//        if (isGrounded) {
//            velocityY = -10.0f;
//            isGrounded = false;
//            cout << "  [" << name << "] jumps!\n";
//        }
//    }
//
//    void landOnGround(float groundY) {
//        if (y >= groundY) { y = groundY; velocityY = 0; isGrounded = true; }
//    }
//
//    void tilt(float deg) {
//        tiltAngle = max(-maxTiltAngle, min(maxTiltAngle, tiltAngle + deg));
//    }
//
//    bool getIsGrounded() const { return isGrounded; }
//};
//
//// ============================================================
//// ABSTRACT MID-TIER 2: AerialVehicle  [A]
//// ============================================================
//class AerialVehicle : public virtual Vehicle {
//protected:
//    float altitude, maxAltitude, minAltitude;
//    bool  isFlying, hoverCapability;
//
//public:
//    AerialVehicle() : Vehicle(), altitude(200), maxAltitude(500),
//        minAltitude(50), isFlying(true), hoverCapability(true)
//    {
//        isAffectedByGravity = false;
//    }
//
//    virtual void ascend() {
//        if (altitude < maxAltitude) { altitude += 5; y -= 5; }
//    }
//    virtual void descend() {
//        if (altitude > minAltitude) { altitude -= 5; y += 5; }
//    }
//    void hover() { velocityX = 0; velocityY = 0; }
//
//    float getAltitude() const { return altitude; }
//};
//
//// ============================================================
//// ABSTRACT MID-TIER 3: AquaticVehicle  [A]
//// ============================================================
//class AquaticVehicle : public virtual Vehicle {
//protected:
//    float depth, maxDepth, surfaceLevel;
//    bool  isSubmerged;
//
//public:
//    AquaticVehicle() : Vehicle(), depth(0), maxDepth(300),
//        surfaceLevel(400), isSubmerged(false)
//    {
//        isAffectedByGravity = false;
//    }
//
//    virtual void dive() {
//        if (depth < maxDepth) {
//            depth += 10; y += 10; isSubmerged = true;
//            cout << "  [" << name << "] diving - depth=" << depth << "\n";
//        }
//    }
//    virtual void surface() {
//        if (depth > 0) {
//            depth -= 10; y -= 10;
//            if (depth <= 0) { depth = 0; isSubmerged = false; }
//        }
//    }
//
//    float getDepth()      const { return depth; }
//    bool  getIsSubmerged()const { return isSubmerged; }
//};
//
//// ============================================================
//// PLAYER VEHICLE MIXIN — damage states, immunity, Tarma spec
//// ============================================================
//class PlayerVehicleMixin {
//protected:
//    int   damageState;
//    float screenRedHue;
//    float injuryTimer;
//    bool  isImmuneToExplosions;
//    float immunityTimer;
//    float immunityDuration;
//    bool  tarmaProtection;
//    int   ammo, maxAmmo;
//    float fireRate, fireTimer;
//
//public:
//    PlayerVehicleMixin()
//        : damageState(0), screenRedHue(0), injuryTimer(0),
//        isImmuneToExplosions(false), immunityTimer(0),
//        immunityDuration(20.0f), tarmaProtection(false),
//        ammo(100), maxAmmo(100), fireRate(0.2f), fireTimer(0)
//    {
//    }
//
//    void updateDamageState(int hp_, int maxHp_) {
//        float pct = (float)hp_ / maxHp_;
//        if (pct > 0.6f) { damageState = 0; screenRedHue = 0.0f; }
//        else if (pct > 0.3f) { damageState = 1; screenRedHue = 0.3f; }
//        else { damageState = 2; screenRedHue = 0.6f; }
//        cout << "  [DmgState=" << damageState << " RedHue=" << screenRedHue << "]\n";
//    }
//
//    bool canFire() const { return fireTimer <= 0 && ammo > 0; }
//
//    void activateImmunity() {
//        isImmuneToExplosions = true;
//        immunityTimer = immunityDuration;
//        cout << "  [Tarma Special] Immunity ON for " << immunityDuration << "s\n";
//    }
//
//    void setTarmaProtection(bool t) { tarmaProtection = t; }
//    bool isImmune()    const { return isImmuneToExplosions; }
//    int  getDamageState() const { return damageState; }
//    int  getAmmo()     const { return ammo; }
//
//    void updateMixin(float dt) {
//        if (fireTimer > 0) { fireTimer -= dt; }
//        if (immunityTimer > 0) { immunityTimer -= dt; if (immunityTimer <= 0) isImmuneToExplosions = false; }
//        if (injuryTimer > 0) { injuryTimer -= dt; }
//    }
//};
//
//// ============================================================
//// ENEMY VEHICLE MIXIN — grudge, boost, attack timer
//// ============================================================
//class EnemyVehicleMixin {
//protected:
//    float detectionRange, attackRange;
//    float attackTimer, attackRate;
//    float targetX, targetY;
//    bool  isAggressive;
//    int   scoreValue;
//    bool  isGrudgeful;
//    float grudgeTimer;
//    float powerMultiplier;
//    bool  hasBeenPassed;
//
//public:
//    EnemyVehicleMixin()
//        : detectionRange(300), attackRange(200),
//        attackTimer(0), attackRate(2.0f), targetX(0), targetY(0),
//        isAggressive(true), scoreValue(500), isGrudgeful(false),
//        grudgeTimer(0), powerMultiplier(1.0f), hasBeenPassed(false)
//    {
//    }
//
//    void onPlayerPassed() {
//        hasBeenPassed = true;
//        isGrudgeful = true;
//        grudgeTimer = 10.0f;
//        powerMultiplier = 1.5f;
//        cout << "  [GRUDGE] Enemy returning BOOSTED! x" << powerMultiplier << "\n";
//    }
//
//    void updateEnemy(float dt) {
//        if (attackTimer > 0) attackTimer -= dt;
//        if (grudgeTimer > 0) grudgeTimer -= dt;
//    }
//
//    bool canAttack() const { return attackTimer <= 0; }
//    int  getScore()  const { return scoreValue; }
//    void setTarget(float tx, float ty) { targetX = tx; targetY = ty; }
//};
//
//// ============================================================
//// CONCRETE PLAYER VEHICLES [C]
//// ============================================================
//
//// C: MetalSlug (SV-001)
//class MetalSlug : public GroundVehicle, public PlayerVehicleMixin {
//private:
//    int   cannonAmmo, cannonDamage, bulletDamage;
//    float cannonBlastRadius;
//    bool  isCrouching, isWarning;
//    float warningTimer, warningDuration;
//
//public:
//    MetalSlug() : GroundVehicle(), PlayerVehicleMixin(),
//        cannonAmmo(10), cannonDamage(50), bulletDamage(10),
//        cannonBlastRadius(50), isCrouching(false),
//        isWarning(false), warningTimer(0), warningDuration(2.0f)
//    {
//        name = "MetalSlug(SV-001)";
//        hp = maxHp = 150; width = 120; height = 90;
//        maxAmmo = ammo = 200; fireRate = 0.1f;
//        ownerType = 0;
//    }
//
//    void fire() override {
//        if (isImmuneToExplosions) { cout << "  [MetalSlug] IMMUNE\n"; return; }
//        if (canFire()) {
//            cout << "  [MetalSlug] Rapid bullet DMG=" << bulletDamage << " ammo=" << ammo - 1 << "\n";
//            ammo--; fireTimer = fireRate;
//        }
//    }
//
//    void fireCannon() {
//        if (cannonAmmo > 0) {
//            cout << "  [MetalSlug] CANNON fired! DMG=" << cannonDamage
//                << " BlastR=" << cannonBlastRadius << " left=" << cannonAmmo - 1 << "\n";
//            cannonAmmo--;
//        }
//    }
//
//    void throwGrenade() { cout << "  [MetalSlug] Grenade (ballistic arc)\n"; }
//    void crouch() { isCrouching = true; height = 60; cout << "  [MetalSlug] Crouching\n"; }
//
//    void suicideAttack() {
//        isWarning = true; warningTimer = warningDuration;
//        cout << "  [MetalSlug] WARNING - suicide attack!\n";
//    }
//
//    void takeDamage(int dmg) override {
//        if (isImmuneToExplosions) { cout << "  [MetalSlug] IMMUNE - no damage\n"; return; }
//        hp -= dmg; if (hp < 0) hp = 0;
//        updateDamageState(hp, maxHp);
//        if (hp <= 0) destroy();
//    }
//
//    void destroy() override {
//        isDestroyed = true; isActive = false;
//        if (hasPilot) {
//            if (tarmaProtection)
//                cout << "  [MetalSlug] DESTROYED - Tarma survives! (Totem of Undying)\n";
//            else
//                cout << "  [MetalSlug] DESTROYED - pilot ejected\n";
//            ejectPilot();
//        }
//        else {
//            cout << "  [MetalSlug] DESTROYED\n";
//        }
//    }
//
//    void update(float dt) override {
//        if (isDestroyed) return;
//        move(dt); landOnGround(700); updateMixin(dt);
//        if (warningTimer > 0) { warningTimer -= dt; if (warningTimer <= 0) isWarning = false; }
//    }
//
//    void render() const override {
//        cout << "  [MetalSlug] pos=(" << x << "," << y << ") ";
//        printStatus();
//        cout << " Cannon=" << cannonAmmo << " Bullets=" << ammo
//            << " DmgState=" << damageState
//            << " Grounded=" << isGrounded << "\n";
//    }
//};
//
//// C: SlugFlyer
//class SlugFlyer : public AerialVehicle, public PlayerVehicleMixin {
//private:
//    int missileAmmo, missileDamage, bulletDamage;
//
//public:
//    SlugFlyer() : AerialVehicle(), PlayerVehicleMixin(),
//        missileAmmo(4), missileDamage(30), bulletDamage(8)
//    {
//        name = "SlugFlyer"; hp = maxHp = 120;
//        width = 140; height = 80;
//        maxAmmo = ammo = 200; fireRate = 0.15f;
//        ownerType = 0;
//    }
//
//    void fire() override {
//        if (canFire()) {
//            cout << "  [SlugFlyer] Bullet DMG=" << bulletDamage << " ammo=" << ammo - 1 << "\n";
//            ammo--; fireTimer = fireRate;
//        }
//    }
//
//    void fireMissile() {
//        if (missileAmmo > 0) {
//            cout << "  [SlugFlyer] MISSILE DMG=" << missileDamage << " left=" << missileAmmo - 1 << "\n";
//            missileAmmo--;
//        }
//        else {
//            cout << "  [SlugFlyer] No missiles left!\n";
//        }
//    }
//
//    void takeDamage(int dmg) override {
//        if (isImmuneToExplosions) { cout << "  [SlugFlyer] IMMUNE\n"; return; }
//        hp -= dmg; if (hp < 0) hp = 0;
//        updateDamageState(hp, maxHp);
//        if (hp <= 0) destroy();
//    }
//
//    void update(float dt) override {
//        if (isDestroyed) return;
//        hover(); updateMixin(dt);
//    }
//
//    void render() const override {
//        cout << "  [SlugFlyer] pos=(" << x << "," << y << ") ";
//        printStatus();
//        cout << " Missiles=" << missileAmmo << " Bullets=" << ammo
//            << " Alt=" << altitude << "\n";
//    }
//};
//
//// C: SlugMariner
//class SlugMariner : public AquaticVehicle, public PlayerVehicleMixin {
//private:
//    int hMissile, vMissile, rMissile;
//
//public:
//    SlugMariner() : AquaticVehicle(), PlayerVehicleMixin(),
//        hMissile(3), vMissile(3), rMissile(3)
//    {
//        name = "SlugMariner"; hp = maxHp = 120;
//        width = 130; height = 70; ownerType = 0;
//    }
//
//    void fire()         override { fireHorizontal(); }
//    void fireHorizontal() {
//        if (hMissile > 0) { cout << "  [SlugMariner] Horizontal missile! left=" << hMissile - 1 << "\n"; hMissile--; }
//        else { cout << "  [SlugMariner] H-missiles empty!\n"; }
//    }
//    void fireVertical() {
//        if (vMissile > 0) { cout << "  [SlugMariner] Vertical missile (enemies above)! left=" << vMissile - 1 << "\n"; vMissile--; }
//        else { cout << "  [SlugMariner] V-missiles empty!\n"; }
//    }
//    void fireReverse() {
//        if (rMissile > 0) { cout << "  [SlugMariner] Reverse-arc missile! left=" << rMissile - 1 << "\n"; rMissile--; }
//        else { cout << "  [SlugMariner] R-missiles empty!\n"; }
//    }
//
//    void takeDamage(int dmg) override {
//        if (isImmuneToExplosions) { cout << "  [SlugMariner] IMMUNE\n"; return; }
//        hp -= dmg; if (hp < 0) hp = 0;
//        updateDamageState(hp, maxHp);
//        if (hp <= 0) destroy();
//    }
//
//    void update(float dt) override {
//        if (isDestroyed) return;
//        move(dt); updateMixin(dt);
//    }
//
//    void render() const override {
//        cout << "  [SlugMariner] pos=(" << x << "," << y << ") ";
//        printStatus();
//        cout << " H=" << hMissile << " V=" << vMissile
//            << " R=" << rMissile << " Depth=" << depth << "\n";
//    }
//};
//
//// C: AmphibiousSlug — multiple inheritance, all 3 mid-tiers
//class AmphibiousSlug : public GroundVehicle,
//    public AerialVehicle,
//    public AquaticVehicle,
//    public PlayerVehicleMixin
//{
//private:
//    enum Mode { GROUND = 0, AIR = 1, WATER = 2 };
//    Mode  currentMode;
//    bool  isTransitioning;
//    float transitionTimer;
//
//public:
//    AmphibiousSlug()
//        : GroundVehicle(), AerialVehicle(), AquaticVehicle(),
//        PlayerVehicleMixin(),
//        currentMode(GROUND), isTransitioning(false), transitionTimer(0)
//    {
//        name = "AmphibiousSlug"; hp = maxHp = 160;
//        width = 130; height = 90; ownerType = 0;
//    }
//
//    void onGroundContact() { switchMode(GROUND); isAffectedByGravity = true; }
//    void onWaterContact() { switchMode(WATER);  isAffectedByGravity = false; }
//    void onAirborne() { switchMode(AIR);    isAffectedByGravity = false; }
//
//    void switchMode(Mode m) {
//        if (currentMode == m) return;
//        currentMode = m; isTransitioning = true; transitionTimer = 1.0f;
//        const char* modes[] = { "GROUND","AIR","WATER" };
//        cout << "  [AmphibiousSlug] -> " << modes[m] << " mode\n";
//    }
//
//    void fire() override {
//        const char* modes[] = { "Ground:rapid-bullets","Air:bullets+missile","Water:horizontal-missile" };
//        cout << "  [AmphibiousSlug/" << modes[currentMode] << "]\n";
//    }
//
//    void takeDamage(int dmg) override {
//        if (isImmuneToExplosions) { cout << "  [AmphibiousSlug] IMMUNE\n"; return; }
//        hp -= dmg; if (hp < 0) hp = 0;
//        updateDamageState(hp, maxHp);
//        if (hp <= 0) destroy();
//    }
//
//    void update(float dt) override {
//        if (isDestroyed) return;
//        if (isTransitioning) { transitionTimer -= dt; if (transitionTimer <= 0) isTransitioning = false; }
//        switch (currentMode) {
//        case GROUND: move(dt); landOnGround(700); break;
//        case AIR:    hover(); break;
//        case WATER:  move(dt); break;
//        }
//        updateMixin(dt);
//    }
//
//    void render() const override {
//        const char* modes[] = { "GROUND","AIR","WATER" };
//        cout << "  [AmphibiousSlug] Mode=" << modes[currentMode]
//            << " pos=(" << x << "," << y << ") ";
//        printStatus();
//        cout << " DmgState=" << damageState << "\n";
//    }
//};
//
//// ============================================================
//// CONCRETE ENEMY VEHICLES [C]
//// ============================================================
//
//// C: FlyingTara
//class FlyingTara : public AerialVehicle, public EnemyVehicleMixin {
//private:
//    float hoverHeight, grenadeDamage, grenadeBlastRadius;
//    float grenadeTimer, grenadeRate;
//    bool  isOnTopOfPlayer;
//    float horizontalSpeed;
//    int   patrolDirection;
//
//public:
//    FlyingTara() : AerialVehicle(), EnemyVehicleMixin(),
//        hoverHeight(150), grenadeDamage(25), grenadeBlastRadius(40),
//        grenadeTimer(0), grenadeRate(2.0f),
//        isOnTopOfPlayer(false), horizontalSpeed(2.0f), patrolDirection(1)
//    {
//        name = "FlyingTara"; hp = maxHp = 5;
//        width = 100; height = 60;
//        scoreValue = 300; ownerType = 1; altitude = hoverHeight;
//    }
//
//    void fire() override {
//        if (isOnTopOfPlayer && canAttack()) {
//            cout << "  [FlyingTara] Drops grenade! DMG=" << grenadeDamage
//                << " BlastR=" << grenadeBlastRadius << "\n";
//            attackTimer = grenadeRate;
//        }
//    }
//
//    void patrol() {
//        velocityX = horizontalSpeed * patrolDirection * powerMultiplier;
//        if (x > 1500) patrolDirection = -1;
//        else if (x < 100) patrolDirection = 1;
//    }
//
//    void update(float dt) override {
//        if (isDestroyed) return;
//        y = hoverHeight; velocityY = 0;
//        patrol(); move(dt); fire(); updateEnemy(dt);
//    }
//
//    void render() const override {
//        cout << "  [FlyingTara] pos=(" << x << "," << y << ") ";
//        printStatus();
//        cout << " Alt=" << altitude
//            << (isGrudgeful ? " [GRUDGE x1.5]" : "") << "\n";
//    }
//};
//
//// C: M15ABradley
//class M15ABradley : public GroundVehicle, public EnemyVehicleMixin {
//private:
//    float missileBlastRadius;
//    int   missileDamage;
//    bool  isOnFlatTerrain;
//    float approachSpeed;
//
//public:
//    M15ABradley() : GroundVehicle(), EnemyVehicleMixin(),
//        missileBlastRadius(60), missileDamage(40),
//        isOnFlatTerrain(false), approachSpeed(2.0f)
//    {
//        name = "M15ABradley"; hp = maxHp = 7;
//        width = 150; height = 100;
//        scoreValue = 600; ownerType = 1;
//    }
//
//    void fire() override {
//        if (canAttack()) {
//            cout << "  [M15ABradley] Arc missile! DMG=" << missileDamage
//                << " BlastR=" << missileBlastRadius << "\n";
//            attackTimer = attackRate;
//        }
//    }
//
//    void setOnFlatTerrain(bool f) { isOnFlatTerrain = f; }
//
//    void update(float dt) override {
//        if (isDestroyed) return;
//        if (isOnFlatTerrain) { velocityX = approachSpeed * powerMultiplier; move(dt); }
//        fire(); updateEnemy(dt);
//    }
//
//    void render() const override {
//        cout << "  [M15ABradley] pos=(" << x << "," << y << ") ";
//        printStatus();
//        cout << " FlatTerrain=" << isOnFlatTerrain
//            << (isGrudgeful ? " [GRUDGE x1.5]" : "") << "\n";
//    }
//};
//
//// C: EnemySub
//class EnemySub : public AquaticVehicle, public EnemyVehicleMixin {
//private:
//    int   rocketDamage;
//    float rocketBlastRadius, rocketArcAngle;
//
//public:
//    EnemySub() : AquaticVehicle(), EnemyVehicleMixin(),
//        rocketDamage(35), rocketBlastRadius(50), rocketArcAngle(60)
//    {
//        name = "EnemySub"; hp = maxHp = 7;
//        width = 160; height = 70;
//        scoreValue = 700; ownerType = 1;
//        depth = 200; isSubmerged = true;
//    }
//
//    void fire() override {
//        if (canAttack()) {
//            cout << "  [EnemySub] Rocket! DMG=" << rocketDamage
//                << " Arc=" << rocketArcAngle << "deg\n";
//            attackTimer = attackRate;
//        }
//    }
//
//    void update(float dt) override {
//        if (isDestroyed) return;
//        move(dt); fire(); updateEnemy(dt);
//    }
//
//    void render() const override {
//        cout << "  [EnemySub] pos=(" << x << "," << y << ") ";
//        printStatus();
//        cout << " Depth=" << depth
//            << (isSubmerged ? " [SUBMERGED]" : "")
//            << (isGrudgeful ? " [GRUDGE x1.5]" : "") << "\n";
//    }
//};
//
//// ============================================================
//// BOSS BASE [A]
//// ============================================================
//class BossVehicle : public virtual Vehicle {
//protected:
//    bool hasRetreated;
//
//public:
//    BossVehicle() : Vehicle(), hasRetreated(false) { ownerType = 1; }
//
//    virtual void retreat() {
//        hasRetreated = true;
//        cout << "  [" << name << "] retreats at 50% HP!\n";
//    }
//
//    virtual void spawnMinions() {
//        cout << "  [" << name << "] spawns minion batch! (crate spawns after kill)\n";
//    }
//
//    void checkRetreat() { if (hp <= maxHp / 2 && !hasRetreated) retreat(); }
//    bool getHasRetreated() const { return hasRetreated; }
//};
//
//// C: IronNokana (Ground Boss, Phase 1)
//class IronNokana : public GroundVehicle, public BossVehicle {
//private:
//    bool flamethrowerActive;
//    int  missileDamage;
//
//public:
//    IronNokana() : GroundVehicle(), BossVehicle(),
//        flamethrowerActive(false), missileDamage(15)
//    {
//        name = "IronNokana"; hp = maxHp = 30; width = 200; height = 150;
//    }
//
//    void fire()    override { cout << "  [IronNokana] Arc missile! DMG=" << missileDamage << "\n"; }
//    void activateFlamethrower() {
//        flamethrowerActive = true;
//        cout << "  [IronNokana] Flamethrower ON! (5-block range, 2HP/s)\n";
//    }
//    void throwFireBombGrenade() {
//        cout << "  [IronNokana] Fire bomb grenade! (fire pool 10s)\n";
//    }
//
//    void takeDamage(int dmg) override { hp -= dmg; if (hp < 0) hp = 0; checkRetreat(); if (hp <= 0) destroy(); }
//    void update(float dt)    override { if (isDestroyed) return; move(dt); checkRetreat(); }
//    void render() const      override {
//        cout << "  [IronNokana-GroundBoss] "; printStatus();
//        cout << (hasRetreated ? " [RETREATED]" : "") << "\n";
//    }
//};
//
//// C: HairBusterRiberts (Aerial Boss, Phase 2)
//class HairBusterRiberts : public AerialVehicle, public BossVehicle {
//private:
//    int rocketDamage, fireBombDamage;
//
//public:
//    HairBusterRiberts() : AerialVehicle(), BossVehicle(),
//        rocketDamage(20), fireBombDamage(15)
//    {
//        name = "HairBusterRiberts"; hp = maxHp = 30; altitude = 350.0f;
//    }
//
//    void fire()          override { cout << "  [HairBusterRiberts] Rockets! DMG=" << rocketDamage << "\n"; }
//    void dropFireBomb() { cout << "  [HairBusterRiberts] Fire bomb grenade!\n"; }
//
//    void takeDamage(int dmg) override { hp -= dmg; if (hp < 0) hp = 0; checkRetreat(); if (hp <= 0) destroy(); }
//    void update(float dt)    override { if (isDestroyed) return; hover(); checkRetreat(); (void)dt; }
//    void render() const      override {
//        cout << "  [HairBusterRiberts-AerialBoss] "; printStatus();
//        cout << " Alt=" << altitude << (hasRetreated ? " [RETREATED]" : "") << "\n";
//    }
//};
//
//// C: SeaSatan (Aquatic Boss, Phase 3)
//class SeaSatan : public AquaticVehicle, public BossVehicle {
//private:
//    int hmgDamage, rocketDamage;
//
//public:
//    SeaSatan() : AquaticVehicle(), BossVehicle(),
//        hmgDamage(3), rocketDamage(20)
//    {
//        name = "SeaSatan"; hp = maxHp = 30; width = 300; height = 150;
//    }
//
//    void fire()       override { cout << "  [SeaSatan] HMG both sides! DMG=" << hmgDamage << "/bullet\n"; }
//    void fireRocket() { cout << "  [SeaSatan] Rocket! DMG=" << rocketDamage << "\n"; }
//
//    void takeDamage(int dmg) override { hp -= dmg; if (hp < 0) hp = 0; checkRetreat(); if (hp <= 0) destroy(); }
//    void update(float dt)    override { if (isDestroyed) return; move(dt); checkRetreat(); }
//    void render() const      override {
//        cout << "  [SeaSatan-AquaticBoss] "; printStatus();
//        cout << " Depth=" << depth << (hasRetreated ? " [RETREATED]" : "") << "\n";
//    }
//};
//
//// ============================================================
//// C: UltimateBoss — Phase 4, polymorphic fusion
//// Inherits all 3 mid-tiers + BossVehicle
//// ============================================================
//class UltimateBoss : public GroundVehicle,
//    public AerialVehicle,
//    public AquaticVehicle,
//    public BossVehicle
//{
//public:
//    enum BossState { ON_GROUND = 0, IN_AIR = 1, IN_WATER = 2 };
//
//private:
//    BossState currentState;
//    float stateTimer, stateDuration;
//    int groundHp, airHp, waterHp;
//
//    // Pointers to sub-boss instances for polymorphic ability dispatch
//    IronNokana* groundBoss;
//    HairBusterRiberts* aerialBoss;
//    SeaSatan* aquaticBoss;
//
//public:
//    UltimateBoss(IronNokana* g, HairBusterRiberts* a, SeaSatan* s)
//        : GroundVehicle(), AerialVehicle(), AquaticVehicle(), BossVehicle(),
//        currentState(ON_GROUND), stateTimer(0), stateDuration(12.0f),
//        groundHp(30), airHp(30), waterHp(30),
//        groundBoss(g), aerialBoss(a), aquaticBoss(s)
//    {
//        name = "UltimateBoss(Fusion)"; hp = maxHp = 90;
//        width = 250; height = 180;
//        cout << "\n  *** EXPLOSION *** All three bosses FUSE!\n";
//    }
//
//    // Spec requirement: each boss ability used POLYMORPHICALLY
//    void fire() override {
//        cout << "  [UltimateBoss] Polymorphic fire -> ";
//        switch (currentState) {
//        case ON_GROUND: groundBoss->fire(); break;  // IronNokana::fire()
//        case IN_AIR:    aerialBoss->fire(); break;  // HairBusterRiberts::fire()
//        case IN_WATER:  aquaticBoss->fire(); break; // SeaSatan::fire()
//        }
//    }
//
//    void cycleState() {
//        currentState = (BossState)((currentState + 1) % 3);
//        stateTimer = 0;
//        const char* s[] = { "GROUND","AIR","WATER" };
//        cout << "  [UltimateBoss] State -> " << s[currentState] << "\n";
//    }
//
//    void takeDamage(int dmg) override {
//        switch (currentState) {
//        case ON_GROUND: groundHp -= dmg; if (groundHp < 0) groundHp = 0; break;
//        case IN_AIR:    airHp -= dmg; if (airHp < 0)    airHp = 0;    break;
//        case IN_WATER:  waterHp -= dmg; if (waterHp < 0)  waterHp = 0;  break;
//        }
//        hp = groundHp + airHp + waterHp;
//        cout << "  [UltimateBoss] Hit! G=" << groundHp
//            << " A=" << airHp << " W=" << waterHp << "\n";
//        if (hp <= 0) destroy();
//    }
//
//    void update(float dt) override {
//        if (isDestroyed) return;
//        stateTimer += dt;
//        if (stateTimer >= stateDuration) cycleState();
//        switch (currentState) {
//        case ON_GROUND: move(dt);  break;
//        case IN_AIR:    hover();   break;
//        case IN_WATER:  move(dt);  break;
//        }
//    }
//
//    // Spec: uses matching sprite per active state
//    void render() const override {
//        const char* s[] = { "GROUND","AIR","WATER" };
//        const char* sp[] = { "IronNokana-sprite","HairBuster-sprite","SeaSatan-sprite" };
//        cout << "  [UltimateBoss] State=" << s[currentState]
//            << " Sprite=" << sp[currentState]
//            << " G=" << groundHp << " A=" << airHp << " W=" << waterHp;
//        cout << (isDestroyed ? " [DEFEATED]" : "") << "\n";
//    }
//};