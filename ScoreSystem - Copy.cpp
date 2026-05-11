#include "ScoreSystem.h"

ScoreSystem::ScoreSystem() : totalScore(0), survivalHighScore(0), campaignHighScore(0),
    showScorePopup(false), lastScoreValue(0), fontLoaded(false),
    recentKillCount(0), flawlessActive(true) {
    lastScoreText = "";
}

ScoreSystem::~ScoreSystem() {}

void ScoreSystem::loadFont(const char* fontPath) {
    fontLoaded = scoreFont.loadFromFile(fontPath);
}

void ScoreSystem::onPlayerDamaged() {
    flawlessActive = false;
}

void ScoreSystem::addScore(int points, const char* reason) {
    if (points <= 0) return;
    totalScore += points;
    lastScoreValue = points;
    if (reason[0] == '\0') {
        lastScoreText = "+50";
    } else {
        lastScoreText = reason;
    }
    showScorePopup = true;
    scorePopupTimer.restart();
}

void ScoreSystem::addEnemyKillScore(const char* enemyType,
    bool isBoss,
    int  bossPhase)
{
    int    points = 0;
    const char* reason;

    if (isBoss) {
        if (bossPhase == 4) {
            points = ULTIMATE_BOSS_SCORE;
            reason = "Ultimate Boss Defeated!";
        } else {
            points = NORMAL_BOSS_SCORE;
            reason = "Boss Defeated!";
        }
    } else {
        reason = enemyType;
        if (enemyType == "Rebel Soldier") points = REBEL_SOLDIER_SCORE;
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

void ScoreSystem::addFeatScore(const char* feat) {
    int    points = 0;
    const char* reason = feat;

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

void ScoreSystem::addMassacreScore(int killCount) {
    if (killCount < 2) return;

    if (killCount == 2) {
        addScore(MULTI_KILL_SCORE, "Multi-Kill!");
    }
    else {
        int bonus = (killCount - 3) * MASSACRE_PER_KILL;
        int points = MASSACRE_BASE_SCORE + (bonus > 0 ? bonus : 0);
        const char* temp = "MASSACRE!";
        addScore(points, temp);
    }
}

void ScoreSystem::checkMultiKill(float playerX, float playerY, ScoreSystem* self) {
}

void ScoreSystem::checkFlawlessVictory() {
    if (flawlessActive) {
        addScore(FLAWLESS_VICTORY_SCORE, "FLAWLESS VICTORY!");
    }
}

void ScoreSystem::resetFlawless() {
    flawlessActive = true;
}

void ScoreSystem::render(RenderWindow& window, int screenX, int screenY) {
    if (!fontLoaded) return;

    Text scoreText;
    char scoreStr[50];
    sprintf_s(scoreStr, sizeof(scoreStr), "SCORE: %d", totalScore);
    scoreText.setFont(scoreFont);
    scoreText.setString(scoreStr);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::White);
    scoreText.setOutlineColor(Color::Black);
    scoreText.setOutlineThickness(2);
    scoreText.setPosition(10, 10);
    window.draw(scoreText);

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
            static_cast<float>(screenX) / 2.f - bounds.width / 2.f,
            static_cast<float>(screenY) / 2.f - 120.f);
        window.draw(popupText);
    }
    else {
        showScorePopup = false;
    }
}

int ScoreSystem::getTotalScore() const {
    return totalScore;
}

int ScoreSystem::getSurvivalHighScore() const {
    return survivalHighScore;
}

int ScoreSystem::getCampaignHighScore() const {
    return campaignHighScore;
}

bool ScoreSystem::getFlawlessActive() const {
    return flawlessActive;
}

void ScoreSystem::updateHighScore(int gameMode) {
    if (gameMode == 1 && totalScore > survivalHighScore)
        survivalHighScore = totalScore;
    else if (gameMode == 2 && totalScore > campaignHighScore)
        campaignHighScore = totalScore;
}

void ScoreSystem::resetScore() {
    totalScore = 0;
    showScorePopup = false;
    flawlessActive = true;
    recentKillCount = 0;
}
