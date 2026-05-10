#pragma once
#include <SFML/Graphics.hpp>
#include <cstdio>
#include <cstring>
using namespace sf;

class ScoreSystem {
private:
    int totalScore;
    int survivalHighScore;
    int campaignHighScore;

    Font scoreFont;
    bool fontLoaded;

    bool   showScorePopup;
    Clock  scorePopupTimer;
    const char*  lastScoreText;
    int    lastScoreValue;

    // Multi-kill tracking
    int   recentKillCount;
    Clock multiKillTimer;
    bool  flawlessActive;   // koi damage nahi liya abhi tak

    // Enemy score values
    static const int REBEL_SOLDIER_SCORE = 50;
    static const int SHIELDED_SOLDIER_SCORE = 75;
    static const int BAZOOKA_SOLDIER_SCORE = 100;
    static const int GRENADE_SOLDIER_SCORE = 100;
    static const int PARATROOPER_SCORE = 100; // base 75 + 25
    static const int MUMMY_SCORE = 150;
    static const int ZOMBIE_SCORE = 100;
    static const int MARTIAN_SCORE = 200;
    static const int NORMAL_BOSS_SCORE = 500;
    static const int ULTIMATE_BOSS_SCORE = 1500;

    // Feat score values
    static const int MELEE_KILL_SCORE = 50;
    static const int AERIAL_KILL_SCORE = 100;
    static const int MULTI_KILL_SCORE = 200;
    static const int MASSACRE_BASE_SCORE = 300;
    static const int MASSACRE_PER_KILL = 50;
    static const int SURVIVAL_CLEAR_SCORE = 1000;
    static const int CAMPAIGN_CLEAR_SCORE = 3000;
    static const int FLAWLESS_VICTORY_SCORE = 5000;
    static const int BOSS_CLEAR_SCORE = 500;

public:
    ScoreSystem();
    ~ScoreSystem();
    void loadFont(const char* fontPath);
    void onPlayerDamaged();
    void addScore(int points, const char* reason = "");
    void addEnemyKillScore(const char* enemyType, bool isBoss = false, int bossPhase = 0);
    void addFeatScore(const char* feat);
    void addMassacreScore(int killCount);
    void checkMultiKill(float playerX, float playerY, ScoreSystem* self);
    void checkFlawlessVictory();
    void resetFlawless();
    void render(RenderWindow& window, int screenX, int screenY);
    int  getTotalScore() const;
    int  getSurvivalHighScore() const;
    int  getCampaignHighScore() const;
    bool getFlawlessActive() const;
    void updateHighScore(int gameMode);
    void resetScore();
};