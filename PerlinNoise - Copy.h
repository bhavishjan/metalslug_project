#pragma once
#include <SFML/Graphics.hpp>
#include "Block.h"
using namespace sf;

class PerlinNoise {
private:
    int seed;
    int p[512];
    int tableSize;
    // Permutation table for randomness
public:
    PerlinNoise(int seed) {
        tableSize = 256;
        this->seed = seed;

        for (int i = 0;i < 256;i++) {
            p[i] = i;
        }
        srand(seed);
        // Fisher-Yates shuffle
        for (int i = 255;i > 0;i--) {
            int n = rand() % (i + 1);
            int temp = p[i];
            p[i] = p[n];
            p[n] = temp;
        }



        // Double to avoid overflow
        for (int i = 0;i < 256;i++) {
            p[i + 256] = p[i];
        }
    }

    // Smoothness
    float fade(float num) {
        return (6 * num * num * num * num * num - 15 * num * num * num * num + 10 * num * num * num);
    }

    float LINEARINTERPOLATION(float a, float b, float l) {
        return a + (l * (b - a));
    }
    // Hash gives random number to each corner for smooth terrain
    // Grad function checks direction and distance
    float gradient(int hash, float dx, float dy) {


        switch (hash & 7) {
        case 0: return  dx + dy;
        case 1: return -dx + dy;
        case 2: return  dx - dy;
        case 3: return -dx - dy;
        case 4: return  dx;
        case 5: return -dx;
        case 6: return  dy;
        case 7: return -dy;
        default: return 0;
        }



    }
    // Floor to prevent decimals
    float noise(float x, float y) {
        // Get value between -1 and 1
        int xx = int(floor(x)) & 255;
        int yy = int(floor(y)) & 255;
        // Distance of point inside block
        float dx = x - floor(x);
        float dy = y - floor(y);
        // Fade for smoothness
        float a = fade(dx);
        float b = fade(dy);
        // Hash of 4 corners
        int aa = p[p[xx] + yy];
        int ab = p[p[xx] + yy + 1];
        int ba = p[p[xx + 1] + yy];
        int bb = p[p[xx + 1] + yy + 1];
        // Blending - need gradient values of each corner
        float gradAA = gradient(aa, dx, dy);
        float gradAB = gradient(ab, dx, dy - 1.0f);
        float gradBA = gradient(ba, dx - 1.0f, dy);
        float gradBB = gradient(bb, dx - 1.0f, dy - 1.0f);
        float lerp1 = LINEARINTERPOLATION(gradAA, gradBA, a);
        float lerp2 = LINEARINTERPOLATION(gradAB, gradBB, a);
        return LINEARINTERPOLATION(lerp1, lerp2, b);
    }

    // Fractal noise adds layers - frequency doubles, amplitude halves per octave


    // Normalize to -1 to 1
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
    }


    // Decide biome
    int getBiome(float x, float y) {
        float height = fractal(x, y, 6);

        int section = (int)(x * 10) % 3;

        if (section == 0) {
            return BIOME_PLAINS;
        }
        if (section == 1) {
            return BIOME_AERIAL;
        }
        return BIOME_AQUATIC;
    }

};

// Fractal noise is multiple perlin layers for realistic terrain
class NoiseProfile {
public:
    virtual float getScale() = 0;
    virtual int getOctaves() = 0;
    // Defined in child classes


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



// Factory class to decide which profile to use
class NoiseProfileFactory {
public:
    NoiseProfile* createProfile(int choice) {
        if (choice == 1) {
            return new AmplifiedProfile;
        }
        if (choice == 2) {
            return new FlatProfile;
        }
        return new NormalProfile;
    }
};











