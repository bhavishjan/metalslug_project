#pragma once
#include <SFML/Graphics.hpp>
#include <string>
using namespace sf;
using namespace std;

class ScoreSystem {
private:
    int totalScore;
    int survivalHighScore;
    int campaignHighScore;

    Font scoreFont;
    bool fontLoaded;

    bool   showScorePopup;
    Clock  scorePopupTimer;
    string lastScoreText;
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
    ScoreSystem() : totalScore(0), survivalHighScore(0), campaignHighScore(0),
        showScorePopup(false), lastScoreValue(0), fontLoaded(false),
        recentKillCount(0), flawlessActive(true) {
    }

    void loadFont(const string& fontPath) {
        fontLoaded = scoreFont.loadFromFile(fontPath);
    }

    // Player damaged — flawless broken
    void onPlayerDamaged() {
        flawlessActive = false;
    }

    void addScore(int points, const string& reason = "") {
        if (points <= 0) return;
        totalScore += points;
        lastScoreValue = points;
        lastScoreText = reason.empty()
            ? "+" + to_string(points)
            : reason + "  +" + to_string(points);
        showScorePopup = true;
        scorePopupTimer.restart();
    }

    void addEnemyKillScore(const string& enemyType,
        bool isBoss = false,
        int  bossPhase = 0)
    {
        int    points = 0;
        string reason = enemyType + " Kill";

        if (isBoss) {
            if (bossPhase == 4) {
                points = ULTIMATE_BOSS_SCORE;
                reason = "Ultimate Boss Defeated!";
            }
            else {
                points = NORMAL_BOSS_SCORE;
                reason = "Boss Defeated!";
            }
        }
        else {
            if (enemyType == "Rebel Soldier")    points = REBEL_SOLDIER_SCORE;
            else if (enemyType == "Shielded Soldier") points = SHIELDED_SOLDIER_SCORE;
            else if (enemyType == "Bazooka Soldier")  points = BAZOOKA_SOLDIER_SCORE;
            else if (enemyType == "Grenade Soldier")  points = GRENADE_SOLDIER_SCORE;
            else if (enemyType == "Paratrooper")      points = PARATROOPER_SCORE;
            else if (enemyType == "Mummy Warrior")    points = MUMMY_SCORE;
            else if (enemyType == "Zombie")           points = ZOMBIE_SCORE;
            else if (enemyType == "Martian")          points = MARTIAN_SCORE;
            else points = 50;
        }

        addScore(points, reason);
    }

    void addFeatScore(const string& feat) {
        int    points = 0;
        string reason = feat;

        if (feat == "Melee Kill")       points = MELEE_KILL_SCORE;
        else if (feat == "Aerial Kill")      points = AERIAL_KILL_SCORE;
        else if (feat == "Multi-Kill")       points = MULTI_KILL_SCORE;
        else if (feat == "Survival Clear")   points = SURVIVAL_CLEAR_SCORE;
        else if (feat == "Campaign Clear")   points = CAMPAIGN_CLEAR_SCORE;
        else if (feat == "Boss Clear")       points = BOSS_CLEAR_SCORE;
        else if (feat == "Flawless Victory") points = FLAWLESS_VICTORY_SCORE;

        if (points > 0)
            addScore(points, reason);
    }

    // Massacre scoring — 3+ kills with one grenade/rocket
    // killCount = kitne enemies ek saath mare
    void addMassacreScore(int killCount) {
        if (killCount < 2) return;

        if (killCount == 2) {
            // Multi-Kill
            addScore(MULTI_KILL_SCORE, "Multi-Kill!");
        }
        else {
            // Massacre: 300 base + (n-3)*50 extra
            int bonus = (killCount - 3) * MASSACRE_PER_KILL;
            int points = MASSACRE_BASE_SCORE + (bonus > 0 ? bonus : 0);
            addScore(points, "MASSACRE x" + to_string(killCount) + "!");
        }
    }

    // Game.h ke updateSurvival() mein call karo har frame
    // player ke paas kitne recent kills hain track karta hai
    void checkMultiKill(float playerX, float playerY, ScoreSystem* self) {
        // Multi-kill window 0.5 second — agar 2+ kills ek saath hote hain
        // Yeh Game.h mein explosion/grenade kill count se call hoga
        // Directly addMassacreScore(killCount) use karo jab grenade explode ho
    }

    // Flawless check — level end par call karo
    void checkFlawlessVictory() {
        if (flawlessActive) {
            addScore(FLAWLESS_VICTORY_SCORE, "FLAWLESS VICTORY!");
        }
    }

    void resetFlawless() {
        flawlessActive = true;
    }

    void render(RenderWindow& window, int screenX, int screenY) {
        if (!fontLoaded) return;

        // Score — top left
        Text scoreText;
        scoreText.setFont(scoreFont);
        scoreText.setString("SCORE: " + to_string(totalScore));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(Color::White);
        scoreText.setOutlineColor(Color::Black);
        scoreText.setOutlineThickness(2);
        scoreText.setPosition(10, 10);
        window.draw(scoreText);

        // Popup — 2 second tak dikhao
        if (showScorePopup &&
            scorePopupTimer.getElapsedTime().asSeconds() < 2.0f)
        {
            Text popupText;
            popupText.setFont(scoreFont);
            popupText.setString(lastScoreText);
            popupText.setCharacterSize(30);
            popupText.setFillColor(Color::Yellow);
            popupText.setOutlineColor(Color::Black);
            popupText.setOutlineThickness(2);
            FloatRect bounds = popupText.getLocalBounds();
            popupText.setPosition(
                screenX / 2.f - bounds.width / 2.f,
                screenY / 2.f - 120.f);
            window.draw(popupText);
        }
        else {
            showScorePopup = false;
        }
    }

    int  getTotalScore()         const { return totalScore; }
    int  getSurvivalHighScore()  const { return survivalHighScore; }
    int  getCampaignHighScore()  const { return campaignHighScore; }
    bool getFlawlessActive()     const { return flawlessActive; }

    void updateHighScore(int gameMode) {
        if (gameMode == 1 && totalScore > survivalHighScore)
            survivalHighScore = totalScore;
        else if (gameMode == 2 && totalScore > campaignHighScore)
            campaignHighScore = totalScore;
    }

    void resetScore() {
        totalScore = 0;
        showScorePopup = false;
        flawlessActive = true;
        recentKillCount = 0;
    }
};