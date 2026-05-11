#define _CRT_SECURE_NO_WARNINGS
#include "Game.h"
#include "DeveloperMode.h"

Game::Game()
    : screenX(1600), screenY(900),
    gameMode(0), selectedCharacter(0),
    startMenu(false), pauseMenu(true),
    survivalGame(nullptr), campaignGame(nullptr),
    jumpHeld(false), showLevelTitle(false),
    currentLevelNumber(0), showPlayerDiedMessage(false),
    camera(screenX, screenY)
{
    window.create(VideoMode(screenX, screenY), "Metal Slug", Style::Close);
    Delay.restart();
    pauseDelay.restart();
    playerDiedTimer.restart();
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    enemies.setBulletManager(&bulletManager);
    scoreSystem.loadFont("arial.TTF");
}

Game::~Game() {
    cleanup();
}

void Game::run() {
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        handleInput();
        update(dt);
        render();
    }
}

void Game::handleInput() {
    Event ev;
    while (window.pollEvent(ev)) {
        if (ev.type == Event::Closed) {
            window.close();
            return;
        }

        // handle cheat code input
        if (ev.type == Event::KeyPressed) {
            developerMode.processKeyInput(ev.key.code);
        }

        if (gameMode == 0 && ev.type == Event::KeyPressed) {
            if (ev.key.code == Keyboard::Up)
                startMenu.moveSelectionUp();
            else if (ev.key.code == Keyboard::Down)
                startMenu.moveSelectionDown();
            else if (ev.key.code == Keyboard::Enter) {
                int menuState = startMenu.getMenuState();
                if (menuState == 0) {
                    startMenu.setMenuState(1);
                }
                else if (menuState == 1) {
                    selectedCharacter = startMenu.getSelectionIndex();
                    characters.switchCharacterToIndex(selectedCharacter);
                    startMenu.setMenuState(2);
                }
                else if (menuState == 2) {
                    int sel = startMenu.getSelectionIndex();
                    if (sel == 0) startSurvivalMode();
                    else if (sel == 1) startCampaignMode();
                }
            }
            else if (ev.key.code == Keyboard::Escape)
                handleMenuEscape();
        }

        if (gameMode != 0 && ev.type == Event::KeyPressed
            && ev.key.code == Keyboard::Escape
            && pauseDelay.getElapsedTime().asSeconds() > 0.3f)
        {
            if (pauseMenu.getPauseMenuVisible()) {
                pauseMenu.handlePauseInput();
                if (!pauseMenu.getPauseMenuVisible()) {
                    int sel = pauseMenu.getPauseSelection();
                    if (sel == 1) {
                        cleanup();
                        window.close();
                        return;
                    }
                }
            }
            else {
                pauseMenu.showPauseMenu();
            }
            pauseDelay.restart();
        }
    }
}

void Game::update(float dt) {
    // update dev mode system
    developerMode.update(dt);
    
    // apply dev mode benefits
    if (developerMode.isDeveloperModeActive() && characters.getActivePlayer()) {
        developerMode.applyDeveloperBenefits(characters.getActivePlayer());
    }
    
    if (gameMode == 0) {
        startMenu.updateAnimation(dt);
        return;
    }

    if (pauseMenu.getPauseMenuVisible()) {
        pauseMenu.handlePauseInput();
        if (!pauseMenu.getPauseMenuVisible()) {
            if (pauseMenu.getPauseSelection() == 1) {
                cleanup();
                window.close();
            }
        }
        return;
    }

    if (!showPlayerDiedMessage
        && characters.getActivePlayer()
        && characters.getActivePlayer()->getHP() <= 0)
    {
        showPlayerDiedMessage = true;
        playerDiedTimer.restart();
    }

    if (showPlayerDiedMessage) {
        if (playerDiedTimer.getElapsedTime().asSeconds() > 3.0f) {
            showPlayerDiedMessage = false;
            cleanup();
            gameMode = 0;
            startMenu.setMenuState(2);
            if (characters.getActivePlayer())
                characters.getActivePlayer()->forceRespawn();
        }
        return;
    }

    characters.getActivePlayer()->update(dt);

    if (Keyboard::isKeyPressed(Keyboard::Z)
        && Delay.getElapsedTime().asSeconds() > 0.2f)
    {
        characters.switchCharacter();
        Delay.restart();
    }

    if (Keyboard::isKeyPressed(Keyboard::F)
        && playerFireTimer.getElapsedTime().asSeconds() > playerFireCooldown)
    {
        playerFire();
        playerFireTimer.restart();
    }

    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        if (characters.getActivePlayer()->isFacingRight()) {
            characters.getActivePlayer()->setVelocityX(0);
            characters.getActivePlayer()->flipToLeft();
        }
        characters.getActivePlayer()->moveLeft();
    }
    else if (Keyboard::isKeyPressed(Keyboard::Right)) {
        if (!characters.getActivePlayer()->isFacingRight()) {
            characters.getActivePlayer()->setVelocityX(0);
            characters.getActivePlayer()->flipToRight();
        }
        characters.getActivePlayer()->moveRight();
    }
    else {
        if (characters.getActivePlayer()->getIsGrounded())
            characters.getActivePlayer()->setVelocityX(
                characters.getActivePlayer()->getVelocityX()
                * characters.getActivePlayer()->getFriction());
        else
            characters.getActivePlayer()->setVelocityX(
                characters.getActivePlayer()->getVelocityX()
                * characters.getActivePlayer()->getAirFriction());
    }

    float velCap = (gameMode == 2) ? 14.f : 6.f;
    float vx = characters.getActivePlayer()->getVelocityX();
    if (vx > velCap) characters.getActivePlayer()->setVelocityX(velCap);
    if (vx < -velCap) characters.getActivePlayer()->setVelocityX(-velCap);

    if (Keyboard::isKeyPressed(Keyboard::Up)) {
        if (characters.getActivePlayer()->getIsGrounded()) {
            characters.getActivePlayer()->setVelocityY(
                characters.getActivePlayer()->getJumpPower());
            characters.getActivePlayer()->setGrounded(false);
            jumpHeld = true;
        }
    }
    else if (Keyboard::isKeyPressed(Keyboard::J)) {
        if (characters.getActivePlayer()->getIsGrounded()) {
            characters.getActivePlayer()->setVelocityY(-80.0f);
            characters.getActivePlayer()->setGrounded(false);
            jumpHeld = true;
        }
    }
    else {
        jumpHeld = false;
    }

    if (!jumpHeld && characters.getActivePlayer()->getVelocityY() < 0)
        characters.getActivePlayer()->setVelocityY(
            characters.getActivePlayer()->getVelocityY() * 0.5f);

    characters.getActivePlayer()->setVelocityY(
        characters.getActivePlayer()->getVelocityY()
        + characters.getActivePlayer()->getGravity());

    if (characters.getActivePlayer()->getVelocityY()
        > characters.getActivePlayer()->getMaxFallSpeed())
        characters.getActivePlayer()->setVelocityY(
            characters.getActivePlayer()->getMaxFallSpeed());

    if (gameMode == 1 && survivalGame)
        updateSurvival(dt);
    else if (gameMode == 2 && campaignGame)
        updateCampaign(dt);
}

void Game::render() {
    window.clear(Color(135, 206, 235));
    
    // render dev mode indicator
    developerMode.render(window);

    if (gameMode == 0) {
        int ms = startMenu.getMenuState();
        if (ms == 0) startMenu.renderStartScreen(window);
        else if (ms == 1) startMenu.renderCharacterSelection(window);
        else if (ms == 2) startMenu.renderModeSelection(window);
    }

    if (gameMode == 1 && survivalGame) {
        survivalGame->render(window);
        enemies.renderAll(window, camera.getX(), camera.getY());
        bulletManager.render(window, camera.getX(), camera.getY());
        characters.getActivePlayer()->render(window, camera.getX(), camera.getY());

        if (inBossLevel) {
            BossLevel* bl = dynamic_cast<BossLevel*>(
                survivalGame->getCurrentLevel());
            if (bl) {
                for (int i = 0; i < bl->getMaxMinions(); i++) {
                    Enemy* m = bl->getMinionAt(i);
                    if (m && m->getIsAlive())
                        m->render(window, camera.getX(), camera.getY());
                }
            }
        }

        scoreSystem.render(window, screenX, screenY);
        renderPlayerDiedOverlay();
        renderLevelTitle();

        if (pauseMenu.getPauseMenuVisible()) {
            renderPauseOverlay();
        }
        window.display();
        return;
    }

    if (gameMode == 2 && campaignGame) {
        campaignGame->render(window);
        enemies.renderAll(window, camera.getX(), camera.getY());
        bulletManager.render(window, camera.getX(), camera.getY());
        characters.getActivePlayer()->render(window, camera.getX(), camera.getY());
        scoreSystem.render(window, screenX, screenY);
        renderPlayerDiedOverlay();
        if (pauseMenu.getPauseMenuVisible())
            renderPauseOverlay();
        window.display();
        return;
    }

    window.display();
}

void Game::playerFire() {
    PlayerSoldier* player = characters.getActivePlayer();
    if (!player) return;
    float px = player->getPlayerX();
    float py = player->getPlayerY();
    bool  fr = player->isFacingRight();
    float bx = fr ? px + 30.f : px - 30.f;
    float by = py + 10.f;
    float angle = fr ? 0.f : 3.14159f;
    bulletManager.spawnBullet(bx, by, angle, 10, PLAYER,
        800.f, 600.f, Color::Yellow);
    player->shoot();
}

void Game::cleanup() {
    delete survivalGame;  survivalGame = nullptr;
    delete campaignGame;  campaignGame = nullptr;
    enemies.clearAll();
    bulletManager.clearAll();
    inBossLevel = false;
    lastCampaignSpawnX = 0.f;
    survivalClearAdded = false;
    campaignClearAdded = false;
    scoreSystem.resetScore();
}

void Game::startSurvivalMode() {
    gameMode = 1;
    inBossLevel = false;
    survivalClearAdded = false;

    if (survivalGame) { delete survivalGame; survivalGame = nullptr; }
    enemies.clearAll();
    bulletManager.clearAll();
    enemies.setBulletManager(&bulletManager);

    survivalGame = new SurvivalGame(screenX, screenY);
    survivalGame->setCharManager(&characters);
    survivalGame->start();

    Level* level = survivalGame->getCurrentLevel();
    if (level) {
        characters.getActivePlayer()->setPlayerPosition(
            level->getPlayerSpawnX(), level->getPlayerSpawnY());
        level->spawnEnemies(enemies, characters.getActivePlayer());
    }

    characters.getActivePlayer()->setVelocity(0, 0);
    characters.getActivePlayer()->setGrounded(false);
    camera.reset();
    Delay.restart();

    currentLevelNumber = 1;
    showLevelTitle = true;
    levelTitleTimer.restart();

    levelManager.loadAllLevels();
    levelManager.switchToLevel(0);
}

void Game::startCampaignMode() {
    gameMode = 2;
    campaignClearAdded = false;

    if (campaignGame) { delete campaignGame; campaignGame = nullptr; }
    enemies.clearAll();
    bulletManager.clearAll();
    enemies.setBulletManager(&bulletManager);

    campaignGame = new CampaignGame(screenX, screenY);
    campaignGame->setCharManager(&characters);
    campaignGame->start();

    CampaignLevel* cl = campaignGame->getCampaignLevel();
    if (cl) {
        for (int i = 0; i < 8; i++)
            cl->update(i * 16 * 64.0f);
    }

    characters.getActivePlayer()->setPlayerPosition(200, 50);
    characters.getActivePlayer()->setVelocity(0, 0);
    characters.getActivePlayer()->setGrounded(false);
    camera.reset();
    Delay.restart();
    lastCampaignSpawnX = 0.f;
}

void Game::handleMenuEscape() {
    int ms = startMenu.getMenuState();
    if (ms == 2) { startMenu.setMenuState(1); startMenu.resetSelection(); }
    else if (ms == 1) { startMenu.setMenuState(0); startMenu.resetSelection(); }
    else { window.close(); }
}

void Game::advanceToNextLevel() {
    if (!survivalClearAdded) {
        scoreSystem.addFeatScore("Survival Clear");
        scoreSystem.checkFlawlessVictory();
        scoreSystem.resetFlawless();
        survivalClearAdded = false;
    }

    levelManager.nextLevel();
    Level* newLevel = levelManager.getCurrentLevel();
    if (!newLevel) return;

    bulletManager.clearAll();
    enemies.clearAll();
    enemies.setBulletManager(&bulletManager);
    survivalGame->setCurrentLevel(newLevel);

    BossLevel* bl = dynamic_cast<BossLevel*>(newLevel);
    if (bl) {
        inBossLevel = true;
        bl->setBulletManager(&bulletManager);
        bl->setPlayerRef(characters.getActivePlayer());
        bl->spawnEnemies(enemies, characters.getActivePlayer());
    }
    else {
        inBossLevel = false;
        newLevel->spawnEnemies(enemies, characters.getActivePlayer());
    }

    characters.getActivePlayer()->setPlayerPosition(
        newLevel->getPlayerSpawnX(), newLevel->getPlayerSpawnY());
    characters.getActivePlayer()->setVelocity(0, 0);
    characters.getActivePlayer()->setGrounded(false);
    camera.reset();

    currentLevelNumber = levelManager.getCurrentLevelIndex() + 1;
    showLevelTitle = true;
    levelTitleTimer.restart();
}

void Game::updateSurvival(float dt) {
    Level* currentLevel = survivalGame->getCurrentLevel();
    if (!currentLevel) { gameMode = 0; return; }

    float pX = characters.getActivePlayer()->getPlayerX();
    float pY = characters.getActivePlayer()->getPlayerY();
    float pVelocityX = characters.getActivePlayer()->getVelocityX();
    float pVelocityY = characters.getActivePlayer()->getVelocityY();

    updatePlayerPhysics(currentLevel, pX, pY, pVelocityX, pVelocityY);

    if (pX < 10.f) { pX = 10.f; pVelocityX = 0.f; }

    float levelEndX = currentLevel->getLevelEnd();
    if (pX + characters.getActivePlayer()->getWidth() >= levelEndX - 10.f) {
        if (levelManager.getCurrentLevelIndex() < levelManager.getTotalLevels() - 1) {
            advanceToNextLevel();
            return;
        }
        else if (levelManager.getCurrentLevelIndex() == levelManager.getTotalLevels() - 1) {
            // This is the boss level (index 3)
            if (!inBossLevel) {
                advanceToNextLevel();
                return;
            }
        }
        else {
            if (inBossLevel) {
                BossLevel* bl = dynamic_cast<BossLevel*>(currentLevel);
                if (bl && bl->getPhase4Complete()) {
                    scoreSystem.addFeatScore("Boss Clear");
                    scoreSystem.addFeatScore("Survival Clear");
                    scoreSystem.checkFlawlessVictory();
                    cleanup();
                    gameMode = 0;
                    startMenu.setMenuState(0);
                }
            }
        }
    }

    characters.getActivePlayer()->setPlayerPosition(pX, pY);
    characters.getActivePlayer()->setVelocity(pVelocityX, pVelocityY);

    camera.follow(pX, pY);
    camera.setBounds(0.f, currentLevel->getLevelEnd() - screenX, 0.f, 0.f);
    camera.update();
    survivalGame->setCamera(camera.getX(), camera.getY());

    enemies.updateAll(dt, characters.getActivePlayer());
    updateEnemyPhysics(currentLevel, dt);

    bulletManager.update(dt);
    checkBulletEnemyCollisions();
    checkBulletPlayerCollisions();
    bulletManager.checkMultiKill(
        characters.getActivePlayer()->getPlayerX(),
        characters.getActivePlayer()->getPlayerY(),
        &scoreSystem);

    if (!inBossLevel) {
        for (int i = 0; i < enemies.getEnemyCount(); i++) {
            Enemy* e = enemies.getEnemyAt(i);
            if (e && !e->getIsAlive()) {
                SurvivalLevel* sl = dynamic_cast<SurvivalLevel*>(currentLevel);
                if (sl) sl->enemyKilled();
            }
        }
    }

    if (inBossLevel)
        updateBossLevel(dt, currentLevel);
}

void Game::updateBossLevel(float dt, Level* currentLevel) {
    BossLevel* bl = dynamic_cast<BossLevel*>(currentLevel);
    if (!bl) return;

    bl->update(dt);
    bl->checkBulletHitsOnBosses(bulletManager);

    for (int i = 0; i < bl->getMaxMinions(); i++) {
        Enemy* minion = bl->getMinionAt(i);
        if (!minion || !minion->getIsAlive()) continue;

        for (int b = 0; b < bulletManager.getBulletCount(); b++) {
            Bullet* bullet = bulletManager.getBullet(b);
            if (!bullet || !bullet->isActive()) continue;
            if (bullet->getOwner() != PLAYER)    continue;

            float bx = bullet->getX();
            float by = bullet->getY();
            float br = bullet->getRadius();

            if (bx + br > minion->getX() &&
                bx - br < minion->getX() + minion->getWidth() &&
                by + br > minion->getY() &&
                by - br < minion->getY() + minion->getHeight())
            {
                minion->takeDamage(bullet->getDamage(), bx, by, false);
                if (!minion->getIsAlive())
                    scoreSystem.addEnemyKillScore(minion->getName());
                bullet->deactivate();
                break;
            }
        }
    }

    if (bl->getPhase4Complete()) {
        scoreSystem.addFeatScore("Boss Clear");
        scoreSystem.addFeatScore("Survival Clear");
        scoreSystem.checkFlawlessVictory();
        cleanup();
        gameMode = 0;
        startMenu.setMenuState(0);
    }
}

void Game::updateCampaign(float dt) {
    CampaignLevel* campaignLevel = campaignGame->getCampaignLevel();
    if (!campaignLevel) { gameMode = 0; return; }

    float pX = characters.getActivePlayer()->getPlayerX();
    float pY = characters.getActivePlayer()->getPlayerY();
    float pVelocityX = characters.getActivePlayer()->getVelocityX();
    float pVelocityY = characters.getActivePlayer()->getVelocityY();

    campaignLevel->update(pX);
    updatePlayerPhysicsCampaign(campaignLevel, pX, pY, pVelocityX, pVelocityY);

    if (pY + characters.getActivePlayer()->getHeight() > screenY) {
        pY = screenY - characters.getActivePlayer()->getHeight();
        pVelocityY = 0.f;
        characters.getActivePlayer()->setGrounded(true);
    }
    if (pX < 0.f) { pX = 0.f; pVelocityX = 0.f; }

    characters.getActivePlayer()->setPlayerPosition(pX, pY);
    characters.getActivePlayer()->setVelocity(pVelocityX, pVelocityY);

    camera.follow(pX, pY);
    camera.setBounds(0.f, pX + (float)screenX * 2.f, 0.f, 0.f);
    camera.update();
    campaignGame->setCamera(camera.getX(), camera.getY());

    enemies.updateAll(dt, characters.getActivePlayer());

    for (int i = 0; i < enemies.getEnemyCount(); i++) {
        Enemy* e = enemies.getEnemyAt(i);
        if (!e || !e->getIsAlive()) continue;
        float ex = e->getX(), ey = e->getY();
        float evx = e->getVelocityX(), evy = e->getVelocityY();
        e->applyGravity(dt);
        evy = e->getVelocityY();
        ey += evy * dt;
        ex += evx * dt;
        bool onGround = false;
        campaignLevel->resolveCollisions(ex, ey,
            e->getWidth(), e->getHeight(), evx, evy, onGround);
        int tries = 0;
        while (campaignLevel->checkCollision(ex, ey,
            e->getWidth(), e->getHeight()) && tries < 100)
        {
            ey -= 1.f; tries++;
        }
        e->setGrounded(onGround);
        e->setVelocityY(evy);
        e->setVelocityX(evx);
        e->setPosition(ex, ey);
    }

    enemies.removeDeadEnemies();

    if (pX - lastCampaignSpawnX > 500.f) {
        campaignLevel->spawnWave(pX, enemies, characters.getActivePlayer());
        lastCampaignSpawnX = pX;
    }

    bulletManager.update(dt);
    checkBulletEnemyCollisions();
    checkBulletPlayerCollisions();
    bulletManager.checkMultiKill(
        characters.getActivePlayer()->getPlayerX(),
        characters.getActivePlayer()->getPlayerY(),
        &scoreSystem);

    campaignGame->update(dt, &characters);

    if (campaignGame->getKillQuotaReached() && !campaignClearAdded) {
        scoreSystem.addFeatScore("Campaign Clear");
        scoreSystem.checkFlawlessVictory();
        campaignClearAdded = true;
    }
}

void Game::updatePlayerPhysics(Level* level,
    float& pX, float& pY,
    float& pVelocityX, float& pVelocityY)
{
    if (!level) return;
    float pw = characters.getActivePlayer()->getWidth();
    float ph = characters.getActivePlayer()->getHeight();

    pX += pVelocityX;
    const float stepHeight = 64.0f;
    if (pVelocityX != 0 && level->checkCollision(pX, pY, pw, ph)) {
        float tempY = pY - stepHeight;
        if (!level->checkCollision(pX, tempY, pw, ph))
            pY = tempY;
        else {
            pX = characters.getActivePlayer()->getPlayerX();
            pVelocityX = 0;
        }
    }

    pY += pVelocityY;
    if (level->checkCollision(pX, pY, pw, ph)) {
        if (pVelocityY > 0) {
            characters.getActivePlayer()->setGrounded(true);
            pVelocityY = 0;
            while (level->checkCollision(pX, pY, pw, ph)) pY -= 1.f;
        }
        else {
            pVelocityY = 0;
            while (level->checkCollision(pX, pY, pw, ph)) pY += 1.f;
        }
    }
    else {
        if (pVelocityY >= 0)
            characters.getActivePlayer()->setGrounded(false);
    }
}

void Game::updatePlayerPhysicsCampaign(CampaignLevel* level,
    float& pX, float& pY,
    float& pVelocityX, float& pVelocityY)
{
    if (!level) return;
    float pw = characters.getActivePlayer()->getWidth();
    float ph = characters.getActivePlayer()->getHeight();

    pX += pVelocityX;
    const float stepHeight = 20.0f;
    if (pVelocityX != 0 && level->checkCollision(pX, pY, pw, ph)) {
        float tempY = pY - stepHeight;
        if (!level->checkCollision(pX, tempY, pw, ph))
            pY = tempY;
        else {
            pX = characters.getActivePlayer()->getPlayerX();
            pVelocityX = 0;
        }
    }

    pY += pVelocityY;
    if (level->checkCollision(pX, pY, pw, ph)) {
        if (pVelocityY > 0) {
            characters.getActivePlayer()->setGrounded(true);
            pVelocityY = 0;
            while (level->checkCollision(pX, pY, pw, ph)) pY -= 1.f;
        }
        else {
            pVelocityY = 0;
            while (level->checkCollision(pX, pY, pw, ph)) pY += 1.f;
        }
    }
    else {
        if (pVelocityY >= 0)
            characters.getActivePlayer()->setGrounded(false);
    }
}

void Game::updateEnemyPhysics(Level* currentLevel, float dt) {
    for (int i = 0; i < enemies.getEnemyCount(); i++) {
        Enemy* enemy = enemies.getEnemyAt(i);
        if (!enemy || !enemy->getIsAlive()) continue;

        float ex = enemy->getX(), ey = enemy->getY();
        float evx = enemy->getVelocityX(), evy = enemy->getVelocityY();
        const float ew = enemy->getWidth(), eh = enemy->getHeight();

        enemy->applyGravity(dt);
        evy = enemy->getVelocityY();
        ey += evy * dt;
        ex += evx * dt;

        if (ex < 0) { ex = 0; evx = -evx; enemy->setVelocityX(evx); }

        bool onGround = false;
        currentLevel->resolveCollisions(ex, ey, ew, eh, evx, evy, onGround);

        int pushAttempts = 0;
        while (currentLevel->checkCollision(ex, ey, ew, eh)
            && pushAttempts < 100)
        {
            ey -= 1.0f; pushAttempts++;
        }

        if (onGround && evx != 0.f) {
            float checkX = (evx > 0) ? (ex + ew + 5.f) : (ex - 5.f);
            bool blockAhead = currentLevel->checkCollision(checkX, ey, 5.f, eh);
            bool canJump = !currentLevel->checkCollision(checkX, ey - 40.f, 5.f, eh);
            if (blockAhead && canJump) {
                evy = -250.0f;
                enemy->setVelocityY(evy);
                onGround = false;
            }
        }

        enemy->setGrounded(onGround);
        enemy->setVelocityY(evy);
        enemy->setVelocityX(evx);
        enemy->setPosition(ex, ey);
    }
}

void Game::checkBulletEnemyCollisions() {
    for (int i = 0; i < bulletManager.getBulletCount(); i++) {
        Bullet* bullet = bulletManager.getBullet(i);
        if (!bullet || !bullet->isActive()) continue;
        if (bullet->getOwner() != PLAYER)   continue;

        float bx = bullet->getX(), by = bullet->getY();
        float br = bullet->getRadius();

        for (int j = 0; j < enemies.getEnemyCount(); j++) {
            Enemy* enemy = enemies.getEnemyAt(j);
            if (!enemy || !enemy->getIsAlive()) continue;

            float ex = enemy->getX(), ey = enemy->getY();
            float ew = enemy->getWidth(), eh = enemy->getHeight();

            if (bx + br > ex && bx - br < ex + ew &&
                by + br > ey && by - br < ey + eh)
            {
                enemy->takeDamage(bullet->getDamage(), bx, by, false);

                if (!enemy->getIsAlive()) {
                    scoreSystem.addEnemyKillScore(enemy->getName());
                    if (gameMode == 2 && campaignGame) {
                        CampaignLevel* cl = campaignGame->getCampaignLevel();
                        if (cl) cl->recordEnemyKill(enemy->getName());
                    }
                }
                bullet->deactivate();
                break;
            }
        }
    }
}

void Game::checkBulletPlayerCollisions() {
    PlayerSoldier* player = characters.getActivePlayer();
    if (!player || !player->getIsAlive()) return;

    float px = player->getPlayerX(), py = player->getPlayerY();
    float pw = player->getWidth(), ph = player->getHeight();

    for (int i = 0; i < bulletManager.getBulletCount(); i++) {
        Bullet* bullet = bulletManager.getBullet(i);
        if (!bullet || !bullet->isActive()) continue;
        if (bullet->getOwner() != ENEMY)    continue;

        float bx = bullet->getX(), by = bullet->getY();
        float br = bullet->getRadius();

        if (bx + br > px && bx - br < px + pw &&
            by + br > py && by - br < py + ph)
        {
            player->takeDamage(bullet->getDamage());
            scoreSystem.onPlayerDamaged();
            bullet->deactivate();
        }
    }
}

void Game::renderPlayerDiedOverlay() {
    if (!showPlayerDiedMessage) return;
    Font font;
    if (!font.loadFromFile("arial.TTF")) return;

    RectangleShape overlay(Vector2f((float)screenX, 150.f));
    overlay.setFillColor(Color(0, 0, 0, 150));
    overlay.setPosition(0, 0);
    window.draw(overlay);

    Text diedText;
    diedText.setFont(font);
    diedText.setString("PLAYER DIED");
    diedText.setCharacterSize(60);
    diedText.setFillColor(Color::Red);
    diedText.setStyle(Text::Bold);
    FloatRect tb = diedText.getLocalBounds();
    diedText.setPosition(screenX / 2.f - tb.width / 2.f, 30.f);
    window.draw(diedText);

    Text ret;
    ret.setFont(font);
    ret.setString("Returning to menu...");
    ret.setCharacterSize(25);
    ret.setFillColor(Color::White);
    FloatRect rb = ret.getLocalBounds();
    ret.setPosition(screenX / 2.f - rb.width / 2.f, 100.f);
    window.draw(ret);
}

void Game::renderLevelTitle() {
    if (!showLevelTitle) return;
    if (levelTitleTimer.getElapsedTime().asSeconds() < 3.0f) {
        Font font;
        if (!font.loadFromFile("arial.TTF")) return;
        Text t;
        t.setFont(font);
        char levelStr[50];
        if (inBossLevel) {
            sprintf_s(levelStr, sizeof(levelStr), "BOSS LEVEL");
        } else {
            sprintf_s(levelStr, sizeof(levelStr), "LEVEL %d", currentLevelNumber);
        }
        t.setString(levelStr);
        t.setCharacterSize(100);
        t.setFillColor(inBossLevel ? Color::Red : Color::White);
        t.setStyle(Text::Bold);
        FloatRect tb = t.getLocalBounds();
        t.setPosition(static_cast<float>(screenX) / 2.f - tb.width / 2.f, static_cast<float>(screenY) / 2.f - 50.f);
        window.draw(t);
    }
    else {
        showLevelTitle = false;
    }
}

void Game::renderPauseOverlay() {
    RectangleShape overlay;
    overlay.setSize(Vector2f((float)screenX, (float)screenY));
    overlay.setFillColor(Color(0, 0, 0, 150));
    overlay.setPosition(0, 0);
    window.draw(overlay);
    pauseMenu.renderPauseMenu(window);
}
