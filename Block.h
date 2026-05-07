#pragma once
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;


// Biome types
static const int BIOME_AERIAL = 0;
static  const int BIOME_PLAINS = 1;
static const int BIOME_AQUATIC = 2;


//campaign level
//perlin noise
class PerlinNoise {
private:
    int seed;
    int p[512];      // 256 double = 512
    int tableSize;   //  256
    //permutatiipntable for randmoness
public:
    PerlinNoise(int seed) {
        tableSize = 256;
        this->seed = seed;

        for (int i = 0;i < 256;i++) {
            p[i] = i;
        }
        srand(seed);//1st val-230//220th =12
        //fisher yates alog
        for (int i = 255;i > 0;i--) {
            int n = rand() % (i + 1);
            //bubble sort
            int temp = p[i];
            p[i] = p[n];
            p[n] = temp;
        }



        //double to avoid overflow
        for (int i = 0;i < 256;i++) {
            p[i + 256] = p[i];
        }
    }

    //smmothness
    float fade(float num) {
        return (6 * num * num * num * num * num - 15 * num * num * num * num + 10 * num * num * num);
    }

    float LINEARINTERPOLATION(float a, float b, float l) {
        return a + (l * (b - a));
    }
    //hash give random number to each corner so terrian is smooth and varient
    //grad function to check direction and analyze ke point us direction se kitna distance per ha
    float gradient(int hash, float dx, float dy) {


        switch (hash & 7) {          // 8 direction
        case 0: return  dx + dy;
        case 1: return -dx + dy;
        case 2: return  dx - dy;
        case 3: return -dx - dy;
        case 4: return  dx;       // pure horizontal
        case 5: return -dx;
        case 6: return  dy;       // pure vertical
        case 7: return -dy;
        default: return 0;
        }



    }
    //floor for intefer prevent dec
    float noise(float x, float y) {
        //to get val btw 1 and -1
        int xx = int(floor(x)) & 255;
        int yy = int(floor(y)) & 255;
        //to find how much distant is point inside block;
        float dx = x - floor(x);
        float dy = y - floor(y);
        //fade for smoothness
        float a = fade(dx);
        float b = fade(dy);
        //hash of 4 corners
        int aa = p[p[xx] + yy];
        int ab = p[p[xx] + yy + 1];
        int ba = p[p[xx + 1] + yy];
        int bb = p[p[xx + 1] + yy + 1];
        //blending
        //but firts i need gradient val of eac corner
        float gradAA = gradient(aa, dx, dy);
        float gradAB = gradient(ab, dx, dy - 1.0f);
        float gradBA = gradient(ba, dx - 1.0f, dy);
        float gradBB = gradient(bb, dx - 1.0f, dy - 1.0f);
        float lerp1 = LINEARINTERPOLATION(gradAA, gradBA, a);
        float lerp2 = LINEARINTERPOLATION(gradAB, gradBB, a);
        return LINEARINTERPOLATION(lerp1, lerp2, b);
    }

    //factual noise to add layers Frequency = Zoom level more frequency = more detai amplitude =effect more amplitude=more height octave is one layer
    //in every layer fre doubles and amp halves to add more detail but not too much


    //normalize to 1 to -1
    float fractal(float x, float y, int octaves) {
        float sum = 0;
        float f = 1.0f;
        float a = 1.0f;
        float max = 0.0f;
        for (int i = 0;i < octaves;i++) {
            sum += noise(x * f, y * f) * a;
            max += a;
            f = 2 * f;
            a = a / 2;
        }
        return sum / max;
        //so normalization is done by dividing sum by max which is the total amplitude of all octaves combined this way we get a value between -1 and 1
    }


    //decid biome
    int getBiome(float x, float y) {
        float height = fractal(x, y, 6);

        int section = (int)(x * 10) % 3;

        if (section == 0) return BIOME_PLAINS;
        if (section == 1) return BIOME_AERIAL;
        return BIOME_AQUATIC;
    }

};

//fractual nosie is multiple perlin noises //perlin is a single layer
//realsitic terrians with  small detail and large feature
class NoiseProfile {
public:
    virtual float getScale() = 0;//how much to zoom in and out
    virtual int getOctaves() = 0;//how many layers of noise
    //will define in childs


};


class AmplifiedProfile : public NoiseProfile {
public:
    float getScale() override {
        return 2.0;
    }
    int getOctaves() override {
        return 8;
    }
};




class FlatProfile : public NoiseProfile {
public:
    float getScale() override {
        return 0.5;
    }
    int getOctaves() override {
        return 4;
    }
};




class NormalProfile : public NoiseProfile {
public:
    float getScale() override {
        return 1.0;
    }
    int getOctaves() override {
        return 6;
    }
};



//fac class decide which profile is require
class NoiseProfileFactory {
public:
    NoiseProfile* createProfile(int choice) {
        if (choice == 1) return new AmplifiedProfile;
        if (choice == 2) return new FlatProfile;
        return new NormalProfile;  // default Normal
    }
};












// Block types
static const int BLOCK_EMPTY = 0;
static const int BLOCK_SOLID = 1;
static const int BLOCK_WATER = 2;
static const int BLOCK_INDESTRUCTIBLE = 3;



static const int SCREEN_H = 900;
class Block {
protected:
    float x, y;
    float width = 64;
    float height = 64;
    int   blockType;
    int   biomeType;
    bool  isSolid;
    bool  isWater;
    bool  isIndestructible;
    bool  isVisible;
    int   hp;
    Sprite sprite;

public:
    // Default constructor
    Block() {
        x = 0; y = 0;
        blockType = BLOCK_EMPTY;
        biomeType = BIOME_PLAINS;
        isSolid = false;
        isWater = false;
        isIndestructible = false;
        isVisible = true;
        hp = 3;
    }

    // Parameterized constructor
    Block(float x, float y, int blockType, int biomeType) {
        this->x = x;
        this->y = y;
        this->blockType = blockType;
        this->biomeType = biomeType;
        isSolid = (blockType == BLOCK_SOLID);
        isWater = (blockType == BLOCK_WATER);
        isIndestructible = false;
        isVisible = true;
        hp = 3;
    }

    virtual ~Block() {}

    // Getters
    float getX() { return x; }
    float getY() { return y; }
    float getWidth() { return width; }
    float getHeight() { return height; }
    int   getBlockType() { return blockType; }
    bool  getIsSolid() { return isSolid; }
    bool  getIsWater() { return isWater; }
    bool  getIsIndestructible() { return isIndestructible; }

    // Setters
    void setTexture(Texture& tex) { sprite.setTexture(tex); }
    void setIsSolid(bool val) { isSolid = val; }

    // Collision check
    bool checkCollision(float px, float py, float pw, float ph) {
        if (!isSolid) return false;
        if (px + pw <= x) return false;
        if (px >= x + width) return false;
        if (py + ph <= y) return false;
        if (py >= y + height) return false;
        return true;
    }

    void resolveCollision(float& px, float& py, float pw, float ph,
        float& velX, float& velY, bool& onGround) {

        if (!isSolid) return;

        if (px + pw <= x) return;
        if (px >= x + width) return;
        if (py + ph <= y) return;
        if (py >= y + height) return;

        float overlapLeft = (x + width) - px;
        float overlapRight = (px + pw) - x;
        float overlapTop = (y + height) - py;
        float overlapBottom = (py + ph) - y;

        float minX = min(overlapLeft, overlapRight);
        float minY = min(overlapTop, overlapBottom);

        if (minY < minX) {
            // Y collision
            if (velY > 0) {
                // landing on ground
                py = y - ph;
                velY = 0;
                onGround = true;
            }
            else if (velY < 0) {
                // hitting ceiling
                py = y + height;
                velY = 0;
            }
        }
        else {
            // X collision
            if (velX > 0) {
                px = x - pw;
            }
            else if (velX < 0) {
                px = x + width;
            }
            velX = 0;
        }
    }


    virtual void update(float dt) {}

    virtual void render(RenderWindow& window, float camX, float camY) {
        if (!isVisible) return;
        if (blockType == BLOCK_EMPTY) return;
        sprite.setPosition(x - camX, y - camY);
        window.draw(sprite);
    }

    virtual void takeDamage(int amount) {
        if (isIndestructible) return;
        hp -= amount;
        if (hp <= 0) {
            blockType = BLOCK_EMPTY;
            isSolid = false;
            isVisible = false;
        }
    }

    bool isDestroyed() { return blockType == BLOCK_EMPTY; }
};

//childblock
class IndestructibleBlock : public Block {
public:
    IndestructibleBlock(float x, float y, int biomeType, int layerIndex)
        : Block(x, y, BLOCK_SOLID, biomeType)
    {
        isIndestructible = true;
        isSolid = true;
        hp = 99999;
    }

    void takeDamage(int amount) override {}

    void render(RenderWindow& window, float camX, float camY) override {
        sprite.setPosition(x - camX, y - camY);
        window.draw(sprite);
    }
};


//if speed is less = gentle wave
//if speed is more = more peaks
//if size is less = smaller hill
//if size is more = bigger hill
class Biome {
protected:
    const char* name;
    int    biomeType;
    float  startX, endX;
    float  startY, endY;
    Block** blocks;//agggregation with block
    int    blockCount;
    Texture solidTex;
    Texture waterTex;

public:
    Biome() {
        name = "";
        biomeType = BIOME_PLAINS;
        startX = endX = 0;
        startY = 0; endY = 900;
        blocks = nullptr;
        blockCount = 0;
    }

    Biome(const char* name, int biomeType, float startX, float endX) {
        this->name = name;
        this->biomeType = biomeType;
        this->startX = startX;
        this->endX = endX;
        startY = 0; endY = 900;
        blocks = nullptr;
        blockCount = 0;
    }

    virtual ~Biome() {
        if (blocks) {
            for (int i = 0; i < blockCount; i++)
                delete blocks[i];
            delete[] blocks;
        }
    }

    // Pure virtual so every biome can make her terrian
    virtual void generateTerrain(int widthInBlocks, int heightInBlocks) = 0;

    void loadTextures(const char* solidPath, const char* waterPath) {
        solidTex.loadFromFile(solidPath);
        waterTex.loadFromFile(waterPath);
    }

    void update(float dt) {
        for (int i = 0; i < blockCount; i++)
            if (blocks[i]) blocks[i]->update(dt);
    }
    void render(RenderWindow& window, float camX, float camY) {
        for (int i = 0; i < blockCount; i++)
            if (blocks[i]) blocks[i]->render(window, camX, camY);
    }


    bool isInBiome(float playerX) {
        return (playerX >= startX - 2000 && playerX <= endX + 2000);
    }
    bool checkCollision(float px, float py, float pw, float ph) {

        // check is playerin  biome 
        if (!isInBiome(px)) return false;

        // then  block check 
        for (int i = 0; i < blockCount; i++)
            if (blocks[i] && blocks[i]->checkCollision(px, py, pw, ph))
                return true;

        return false;
    }
    void resolveCollisions(float& px, float& py, float pw, float ph,
        float& velX, float& velY, bool& onGround) {
        if (!isInBiome(px)) return;
        for (int i = 0; i < blockCount; i++)
            if (blocks[i])
                blocks[i]->resolveCollision(px, py, pw, ph, velX, velY, onGround);
    }
    float getStartX() { return startX; }
    float getEndX() { return endX; }
};


class PlainsBiome : public Biome {
protected:
    bool useperlin;
private:
    float hillSpeed;  // sin wave speed
    int   hillSize;   // sin wave amplitude
public:

    PlainsBiome(float startX, float endX, bool per = false)
        : Biome("Plains", BIOME_PLAINS, startX, endX) {
        useperlin = per;
    }

    void generateTerrain(int widthInBlocks, int heightInBlocks) override {
        blocks = new Block * [widthInBlocks * heightInBlocks];
        blockCount = 0;

        //3 ROWS ABOVE BOTTOM TO CREATE GENTLE HILLS
        int surfaceRow = heightInBlocks - 4;


        PerlinNoise perlin(42);





        float speed = 0.03f;
        int hillSize = 4;

        for (int col = 0; col < widthInBlocks; col++) {
            float x = startX + col * 64.0f;
            int sr;
            // Gentle wave
            if (useperlin) {
                float globalCol = (startX / 64.0f) + col;
                float noiseVal = perlin.fractal((globalCol) * 0.005f, 0, 4);
                sr = (int)(heightInBlocks * 0.65f) + (int)(noiseVal * hillSize);
            }
            else {
                // Survival: sin wave
                sr = surfaceRow + (int)(sin(col * speed) * hillSize);
            }








            if (sr < 1) sr = 1;
            if (sr >= heightInBlocks - 1) sr = heightInBlocks - 2;

            for (int row = sr; row < heightInBlocks; row++) {
                //GEN BLOCKS AT BOTTOM OF SUR
                float y = row * 64.0f;
                Block* b;

                if (row == heightInBlocks - 1)
                    b = new IndestructibleBlock(x, y, BIOME_PLAINS, row);
                else {
                    b = new Block(x, y, BLOCK_SOLID, BIOME_PLAINS);
                    b->setTexture(solidTex);
                }
                blocks[blockCount++] = b;
            }
        }
    }



};

//ariel biome use two sin waves to create more peak variant mountains
class AerialBiome : public Biome {
protected:
    bool useperlin;
public:
    AerialBiome(float startX, float endX, bool perlin = false)
        : Biome("Aerial", BIOME_AERIAL, startX, endX), useperlin(perlin) {
    }

    void generateTerrain(int widthInBlocks, int heightInBlocks) override {
        blocks = new Block * [widthInBlocks * heightInBlocks];
        blockCount = 0;
        PerlinNoise perlin(42);
        // Mountains are above  divide by 3 to get highh peak
        int topRow = 2;

        for (int col = 0; col < widthInBlocks; col++) {
            float x = startX + col * 64.0f;
            int surfaceRow;

            if (useperlin) {
                float globalCol = (startX / 64.0f) + col;

                // 2 layer  large feature + detail
                float n1 = perlin.fractal(globalCol * 0.003f, 0.0f, 5);
                float n2 = perlin.fractal(globalCol * 0.09f, 99.0f, 3) * 0.4f;
                float combined = (n1 + n2) / 1.4f;
                surfaceRow = (int)(heightInBlocks * 0.5f) + (int)(combined * 6.0f);
            }
            // hillRows = 5 in aerial 

            else {
                // Survival: 2 sin waves
                surfaceRow = topRow
                    + (int)(sin(col * 0.08f) * 5)
                    + (int)(sin(col * 0.20f) * 3);
            }


            //// 2 sin waves
            //// 1st wave =  slow wave 
            //// 2md wave =  fast wave 
            //int surfaceRow = topRow
            //    + (int)(sin(col * 0.08f) * 5)   // slow
            //    + (int)(sin(col * 0.20f) * 3);  // jagged wave

            if (surfaceRow < 1)                  surfaceRow = 1;
            if (surfaceRow >= heightInBlocks - 1) surfaceRow = heightInBlocks - 2;

            for (int row = surfaceRow; row < heightInBlocks; row++) {
                float y = row * 64.0f;
                Block* b;

                if (row == heightInBlocks - 1) {
                    b = new IndestructibleBlock(x, y, BIOME_AERIAL, row);
                }
                else {
                    b = new Block(x, y, BLOCK_SOLID, BIOME_AERIAL);
                    b->setTexture(solidTex);
                }
                blocks[blockCount++] = b;
            }
        }
    }
};

//water on top and floor at bottom with very gentle wave
class AquaticBiome : public Biome {
protected:
    bool useperlin;
public:
    AquaticBiome(float startX, float endX, bool perlin = false)
        : Biome("Aquatic", BIOME_AQUATIC, startX, endX), useperlin(perlin) {

    }

    void generateTerrain(int widthInBlocks, int heightInBlocks) override {
        blocks = new Block * [widthInBlocks * heightInBlocks];
        blockCount = 0;

        // Sea level water start
        int seaLevelRow = heightInBlocks / 3;
        // Sea floor solid ground start
        int floorMidRow = (int)(heightInBlocks * 0.75f);

        PerlinNoise perlin(42);





        for (int col = 0; col < widthInBlocks; col++) {
            float x = startX + col * 64.0f;

            int floorRow;



            //GENTLE WAVE IN Sea floor 
            // speed = 0.05 THATS SLOW
            if (useperlin) {
                float globalCol = (startX / 64.0f) + col;
                float noiseVal = perlin.fractal(globalCol * 0.004f, 55.0f, 4);
                floorRow = floorMidRow + (int)(noiseVal * 3);
            }
            else {
                // Survival has gentle sin
                floorRow = floorMidRow + (int)(sin(col * 0.03f) * 2);
            }



            if (floorRow < seaLevelRow + 2)       floorRow = seaLevelRow + 2;
            if (floorRow >= heightInBlocks - 1)    floorRow = heightInBlocks - 2;

            for (int row = seaLevelRow; row < heightInBlocks; row++) {
                float y = row * 64.0f;
                Block* b;

                if (row < floorRow) {
                    // water blocks
                    b = new Block(x, y, BLOCK_WATER, BIOME_AQUATIC);
                    b->setTexture(waterTex);
                    b->setIsSolid(false);
                }
                else if (row == heightInBlocks - 1) {
                    // bottom most indestructible
                    b = new IndestructibleBlock(x, y, BIOME_AQUATIC, row);
                }
                else {
                    // Sea floor solid
                    b = new Block(x, y, BLOCK_SOLID, BIOME_AQUATIC);
                    b->setTexture(solidTex);
                }
                blocks[blockCount++] = b;
            }
        }
    }
};










//infinite terrian
class CampaignLevel {
private:
    bool isInfinite;
    int maxGeneratedWidth;
    int currentGeneratedWidth;
    int generationChunkSize;

    int* enemiesKilledPerType;
    int* vehiclesDestroyedPerType;
    int killQuotaPerType;      // 5 enemies per type
    int vehicleDestroyQuota;   // 3 vehicles per type

    bool isKillQuotaReached;
    bool isDynamicSpawning;
    float spawnRadius;

    float fusionCooldownTimer;
    float fusionCooldownDuration;  // 3 minutes

    int currentChunkX;
    bool generatedLeft;
    bool generatedRight;

    PerlinNoise* perlin;
    NoiseProfile* profile;


    Texture solidTex;
    Texture waterTex;


    Biome** activeBiomes;
    int biomeCount;
    int maxBiomes;
public:




    CampaignLevel(int profileChoice) {

        // Basic settings
        isInfinite = true;
        generationChunkSize = 16;
        killQuotaPerType = 5;
        vehicleDestroyQuota = 3;
        fusionCooldownDuration = 180.0f;
        fusionCooldownTimer = 0.0f;
        spawnRadius = 500.0f;

        // Chunk tracking
        currentChunkX = 0;
        generatedLeft = false;
        generatedRight = false;
        currentGeneratedWidth = 0;
        maxGeneratedWidth = 1000000; // 1 million blocks

        // Quota tracking
        isKillQuotaReached = false;
        isDynamicSpawning = true;

        // Arrays  8 enemy types
        enemiesKilledPerType = new int[8];
        vehiclesDestroyedPerType = new int[3]; // 3 vehicle types

        for (int i = 0; i < 8; i++)
            enemiesKilledPerType[i] = 0;

        for (int i = 0; i < 3; i++)
            vehiclesDestroyedPerType[i] = 0;

        // Perlin setup
        NoiseProfileFactory factory;
        profile = factory.createProfile(profileChoice);
        perlin = new PerlinNoise(42);


        maxBiomes = 5000000;
        biomeCount = 0;
        activeBiomes = new Biome * [maxBiomes];

    }



    bool checkCollision(float px, float py, float pw, float ph) {
        for (int i = 0; i < biomeCount; i++)
            if (activeBiomes[i]->checkCollision(px, py, pw, ph))
                return true;
        return false;
    }

    void resolveCollisions(float& px, float& py, float pw, float ph,
        float& velX, float& velY, bool& onGround) {
        for (int i = 0; i < biomeCount; i++)
            activeBiomes[i]->resolveCollisions(px, py, pw, ph, velX, velY, onGround);
    }

    //one chunk is 16 blocks wide when player more forwarrd new chunk create
    //first x pos;then height from perlin and dec biome from height; then create block bottom to top and add in block array


    void generateChunk(int startCol) {
        float scale = profile->getScale();
        int octaves = profile->getOctaves();

        // Biome decide karo noise se
        float noiseX = startCol * 0.03f;




        int biomeType = perlin->getBiome(noiseX, 0);

        float startX = startCol * 64.0f;
        float endX = (startCol + generationChunkSize) * 64.0f;

        // POLYMORPHISM - Biome* pointer se child class banao
        Biome* b = nullptr;
        if (biomeType == BIOME_AERIAL)
            b = new AerialBiome(startX, endX, true);
        else if (biomeType == BIOME_AQUATIC)
            b = new AquaticBiome(startX, endX, true);
        else
            b = new PlainsBiome(startX, endX, true);

        // Texture aur terrain - virtual functions call honge
        b->loadTextures("Sprites/blocks/grass.png", "Sprites/blocks/water.png");
        b->generateTerrain(generationChunkSize, 20);

        if (biomeCount < maxBiomes)
            activeBiomes[biomeCount++] = b;
    }
    //drae block stored in chunsk array
    void render(RenderWindow& window, float camX, float camY) {
        for (int i = 0; i < biomeCount; i++)
            activeBiomes[i]->render(window, camX, camY);
    }


    void update(float playerX) {
        //cal on which chunk player is
        int plaChunk = (int)(playerX / (generationChunkSize * 64.0f));
        //genrate forward
        if (plaChunk + 5 > currentChunkX) {
            generateChunk(currentChunkX * generationChunkSize);
            currentChunkX++;
            generatedRight = true;
        }


        //generete backward
        if (plaChunk - 2 < 0) {
            generateChunk((currentChunkX - 1) * generationChunkSize);
            generatedLeft = true;
        }
    }

};