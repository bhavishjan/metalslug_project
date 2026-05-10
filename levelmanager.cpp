// Game.h ke include section mein — DONO include karo
#define _CRT_SECURE_NO_WARNINGS
#include "Level.h"
#include "BossLevel.h"   // ab BossLevel ka pata hai

// Phir yahan define karo
 void LevelManager::loadAllLevels() {
    levels[0] = new Level1();
    levels[1] = new Level2();
    levels[2] = new Level3();
    levels[3] = new BossLevel();
    totalLevels = 4;  // ← 3 tha, 4 hona chahiye!
}