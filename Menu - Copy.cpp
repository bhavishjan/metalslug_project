#include "Menu.h"

Menu::Menu(bool isPause) : screen_x(1600), screen_y(900), menuState(0), selectionIndex(0),
    isPauseMenu(isPause), pauseMenuVisible(false), pauseSelection(0),
    animationTime(0), pulse(0), blink(0) {
    if (!menuFont.loadFromFile("arial.ttf")) {
        if (!menuFont.loadFromFile("arial.TTF")) {
            menuFont.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");
        }
    }
    if (isPauseMenu) {
        initializePauseMenu();
    }
    else {
        loadBackgrounds();
        initializeStartScreen();
        initializeCharacterSelection();
        initializeModeSelection();
    }
}

void Menu::loadBackgrounds() {
    if (startBgTexture.loadFromFile("Sprites/start.png")) {
        startBgSprite.setTexture(startBgTexture);
        Vector2u sz = startBgTexture.getSize();
        startBgSprite.setScale((float)screen_x / sz.x, (float)screen_y / sz.y);
    }
    if (characterBgTexture.loadFromFile("Sprites/choose_character.jpg")) {
        characterBgSprite.setTexture(characterBgTexture);
        Vector2u sz = characterBgTexture.getSize();
        characterBgSprite.setScale((float)screen_x / sz.x, (float)screen_y / sz.y);
    }
    if (modeBgTexture.loadFromFile("Sprites/choose_mode.jpg")) {
        modeBgSprite.setTexture(modeBgTexture);
        Vector2u sz = modeBgTexture.getSize();
        modeBgSprite.setScale((float)screen_x / sz.x, (float)screen_y / sz.y);
    }
}

void Menu::initializeStartScreen() {
    pressEnterText.setFont(menuFont);
    pressEnterText.setString("PRESS ENTER TO START");
    pressEnterText.setCharacterSize(38);
    pressEnterText.setFillColor(Color(255, 230, 100));
    pressEnterText.setOutlineColor(Color(80, 30, 0));
    pressEnterText.setOutlineThickness(3);
    pressEnterText.setStyle(Text::Bold);
    FloatRect b = pressEnterText.getLocalBounds();
    pressEnterText.setPosition(screen_x / 2.0f - b.width / 2.0f, screen_y - 110);

    instructionText.setFont(menuFont);
    instructionText.setString("UP / DOWN / ENTER");
    instructionText.setCharacterSize(15);
    instructionText.setFillColor(Color(255, 255, 255, 200));
    instructionText.setOutlineColor(Color(0, 0, 0, 220));
    instructionText.setOutlineThickness(2);
    instructionText.setStyle(Text::Bold);
    instructionText.setPosition(screen_x - 200, screen_y - 35);
}

void Menu::initializeCharacterSelection() {
    const float panelX = 60;
    const float panelY = 90;
    const float panelW = 480;
    const float panelH = 720;

    charPanel.setSize(Vector2f(panelW, panelH));
    charPanel.setPosition(panelX, panelY);
    charPanel.setFillColor(Color(15, 25, 40, 180));
    charPanel.setOutlineColor(Color(220, 50, 50, 230));
    charPanel.setOutlineThickness(3);

    charTitleBar.setSize(Vector2f(panelW, 70));
    charTitleBar.setPosition(panelX, panelY);
    charTitleBar.setFillColor(Color(220, 50, 50, 230));

    charTitle.setFont(menuFont);
    charTitle.setString("SELECT CHARACTER");
    charTitle.setCharacterSize(34);
    charTitle.setFillColor(Color(255, 255, 255));
    charTitle.setStyle(Text::Bold);
    FloatRect tb = charTitle.getLocalBounds();
    charTitle.setPosition(panelX + panelW / 2.0f - tb.width / 2.0f, panelY + 12);

    const char* names[4] = { "MARCO ROSSI", "TARMA ROVING", "ERI KASAMOTO", "FIOLINA GERMI" };
    const char* subs[4] = { "Balanced Soldier", "Heavy Demolition", "Stealth Operator", "Tactical Expert" };
    float cardX = panelX + 30;
    float cardW = panelW - 60;
    float cardH = 110;
    float startY = panelY + 100;
    float gap = 30;

    for (int i = 0; i < 4; i++) {
        charCard[i].setSize(Vector2f(cardW, cardH));
        charCard[i].setPosition(cardX, startY + i * (cardH + gap));
        charCard[i].setFillColor(Color(30, 40, 60, 220));
        charCard[i].setOutlineColor(Color(100, 110, 130, 200));
        charCard[i].setOutlineThickness(2);

        charCardHighlight[i].setSize(Vector2f(8, cardH));
        charCardHighlight[i].setPosition(cardX, startY + i * (cardH + gap));
        charCardHighlight[i].setFillColor(Color(60, 70, 90, 200));

        charName[i].setFont(menuFont);
        charName[i].setString(names[i]);
        charName[i].setCharacterSize(26);
        charName[i].setFillColor(Color(255, 255, 255));
        charName[i].setStyle(Text::Bold);
        charName[i].setPosition(cardX + 25, startY + i * (cardH + gap) + 18);

        charSubtitle[i].setFont(menuFont);
        charSubtitle[i].setString(subs[i]);
        charSubtitle[i].setCharacterSize(16);
        charSubtitle[i].setFillColor(Color(180, 190, 210));
        charSubtitle[i].setPosition(cardX + 25, startY + i * (cardH + gap) + 60);
    }
}

void Menu::initializeModeSelection() {
    const float panelX = 80;
    const float panelY = 130;
    const float panelW = 520;
    const float panelH = 620;

    modePanel.setSize(Vector2f(panelW, panelH));
    modePanel.setPosition(panelX, panelY);
    modePanel.setFillColor(Color(20, 50, 45, 180));
    modePanel.setOutlineColor(Color(70, 180, 150, 230));
    modePanel.setOutlineThickness(3);

    modeTitleBar.setSize(Vector2f(panelW, 75));
    modeTitleBar.setPosition(panelX, panelY);
    modeTitleBar.setFillColor(Color(70, 180, 150, 230));

    modeTitle.setFont(menuFont);
    modeTitle.setString("CHOOSE MODE");
    modeTitle.setCharacterSize(36);
    modeTitle.setFillColor(Color(255, 255, 255));
    modeTitle.setStyle(Text::Bold);
    FloatRect tb = modeTitle.getLocalBounds();
    modeTitle.setPosition(panelX + panelW / 2.0f - tb.width / 2.0f, panelY + 14);

    const char* names[2] = { "SURVIVAL", "CAMPAIGN" };
    const char* subs[2] = { "Endless waves of enemies\nFight to the last breath", "Story driven battles\nMaster every level" };
    float cardX = panelX + 35;
    float cardW = panelW - 70;
    float cardH = 200;
    float startY = panelY + 110;
    float gap = 50;

    for (int i = 0; i < 2; i++) {
        modeCard[i].setSize(Vector2f(cardW, cardH));
        modeCard[i].setPosition(cardX, startY + i * (cardH + gap));
        modeCard[i].setFillColor(Color(30, 60, 55, 220));
        modeCard[i].setOutlineColor(Color(100, 140, 130, 200));
        modeCard[i].setOutlineThickness(2);

        modeCardHighlight[i].setSize(Vector2f(10, cardH));
        modeCardHighlight[i].setPosition(cardX, startY + i * (cardH + gap));
        modeCardHighlight[i].setFillColor(Color(70, 100, 95, 200));

        modeName[i].setFont(menuFont);
        modeName[i].setString(names[i]);
        modeName[i].setCharacterSize(40);
        modeName[i].setFillColor(Color(255, 255, 255));
        modeName[i].setStyle(Text::Bold);
        modeName[i].setPosition(cardX + 30, startY + i * (cardH + gap) + 25);

        modeSubtitle[i].setFont(menuFont);
        modeSubtitle[i].setString(subs[i]);
        modeSubtitle[i].setCharacterSize(18);
        modeSubtitle[i].setFillColor(Color(190, 210, 200));
        modeSubtitle[i].setPosition(cardX + 30, startY + i * (cardH + gap) + 75);
    }
}

void Menu::initializePauseMenu() {
    float boxWidth = 400.0f;
    float boxHeight = 300.0f;

    pauseBox.setSize(Vector2f(boxWidth, boxHeight));
    pauseBox.setFillColor(Color(50, 50, 50, 230));
    pauseBox.setOutlineColor(Color(255, 255, 255));
    pauseBox.setOutlineThickness(3.0f);

    float boxX = (screen_x - boxWidth) / 2.0f;
    float boxY = (screen_y - boxHeight) / 2.0f;
    pauseBox.setPosition(boxX, boxY);

    pauseResumeButton.setSize(Vector2f(300.0f, 50.0f));
    pauseResumeButton.setFillColor(Color(50, 200, 50));
    pauseResumeButton.setPosition(boxX + 50.0f, boxY + 100.0f);

    pauseExitButton.setSize(Vector2f(300.0f, 50.0f));
    pauseExitButton.setFillColor(Color(100, 100, 100));
    pauseExitButton.setPosition(boxX + 50.0f, boxY + 180.0f);

    pauseTitle.setFont(menuFont);
    pauseTitle.setString("PAUSED");
    pauseTitle.setCharacterSize(48);
    pauseTitle.setFillColor(Color::White);
    pauseTitle.setPosition(boxX + boxWidth / 2.0f - 90.0f, boxY + 20.0f);

    pauseInstructions.setFont(menuFont);
    pauseInstructions.setString("UP / DOWN / ENTER");
    pauseInstructions.setCharacterSize(16);
    pauseInstructions.setFillColor(Color(255, 255, 255, 200));
    pauseInstructions.setPosition(boxX + 50.0f, boxY + 260.0f);

    pauseResumeText.setFont(menuFont);
    pauseResumeText.setString("Resume");
    pauseResumeText.setCharacterSize(32);
    pauseResumeText.setFillColor(Color::White);
    pauseResumeText.setPosition(boxX + boxWidth / 2.0f - 60.0f, boxY + 105.0f);

    pauseExitText.setFont(menuFont);
    pauseExitText.setString("Exit");
    pauseExitText.setCharacterSize(32);
    pauseExitText.setFillColor(Color::White);
    pauseExitText.setPosition(boxX + boxWidth / 2.0f - 40.0f, boxY + 185.0f);
}

void Menu::showPauseMenu() {
    pauseMenuVisible = true;
    pauseSelection = 0;
}

void Menu::hidePauseMenu() {
    pauseMenuVisible = false;
}

bool Menu::getPauseMenuVisible() const {
    return pauseMenuVisible;
}

int Menu::getPauseSelection() const {
    return pauseSelection;
}

void Menu::handlePauseInput() {
    if (!pauseMenuVisible) {
        return;
    }

    if (Keyboard::isKeyPressed(Keyboard::Up)) {
        pauseSelection = 0;
    }

    if (Keyboard::isKeyPressed(Keyboard::Down)) {
        pauseSelection = 1;
    }

    if (Keyboard::isKeyPressed(Keyboard::Enter)) {
        hidePauseMenu();
    }

    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
        hidePauseMenu();
    }

    if (pauseSelection == 0) {
        pauseResumeButton.setFillColor(Color(50, 200, 50));
        pauseExitButton.setFillColor(Color(100, 100, 100));
    }
    else {
        pauseResumeButton.setFillColor(Color(100, 100, 100));
        pauseExitButton.setFillColor(Color(50, 200, 50));
    }
}

void Menu::renderPauseMenu(RenderWindow& window) {
    if (!pauseMenuVisible) {
        return;
    }

    window.draw(pauseBox);
    window.draw(pauseResumeButton);
    window.draw(pauseExitButton);
    window.draw(pauseTitle);
    window.draw(pauseResumeText);
    window.draw(pauseExitText);
    window.draw(pauseInstructions);
}

void Menu::updateAnimation(float dt) {
    animationTime += dt;
    pulse = (sin(animationTime * 3.0f) + 1.0f) / 2.0f;
    blink = (sin(animationTime * 2.5f) + 1.0f) / 2.0f;
}

void Menu::moveSelectionUp() {
    if (menuState == 0) {
        return;
    }
    if (selectionIndex > 0) {
        selectionIndex--;
    }
}

void Menu::moveSelectionDown() {
    int maxIdx;
    if (menuState == 1) {
        maxIdx = 3;
    }
    else if (menuState == 2) {
        maxIdx = 1;
    }
    else {
        maxIdx = 0;
    }
    if (selectionIndex < maxIdx) {
        selectionIndex++;
    }
}

int Menu::getSelectionIndex() {
    return selectionIndex;
}

int Menu::getMenuState() {
    return menuState;
}

void Menu::setMenuState(int s) {
    menuState = s;
    selectionIndex = 0;
}

void Menu::resetSelection() {
    selectionIndex = 0;
}

void Menu::renderStartScreen(RenderWindow& window) {
    window.draw(startBgSprite);

    Color c(255, 230, 100);
    c.a = static_cast<Uint8>(150 + blink * 105);
    pressEnterText.setFillColor(c);
    float scale = 1.0f + blink * 0.05f;
    pressEnterText.setScale(scale, scale);
    FloatRect b = pressEnterText.getLocalBounds();
    pressEnterText.setPosition(screen_x / 2.0f - (b.width * scale) / 2.0f, screen_y - 110);
    window.draw(pressEnterText);

    window.draw(instructionText);
}

void Menu::renderCharacterSelection(RenderWindow& window) {
    window.draw(characterBgSprite);

    window.draw(charPanel);
    window.draw(charTitleBar);
    window.draw(charTitle);

    for (int i = 0; i < 4; i++) {
        if (i == selectionIndex) {
            Color sel(20, 130, 60, 240);
            sel.g = static_cast<Uint8>(130 + pulse * 60);
            charCard[i].setFillColor(sel);
            charCard[i].setOutlineColor(Color(80, 255, 130, 240));
            charCard[i].setOutlineThickness(4);
            charCardHighlight[i].setFillColor(Color(80, 255, 130, 255));
            charName[i].setFillColor(Color(255, 255, 255));
            charSubtitle[i].setFillColor(Color(220, 255, 230));
        }
        else {
            charCard[i].setFillColor(Color(30, 40, 60, 220));
            charCard[i].setOutlineColor(Color(100, 110, 130, 200));
            charCard[i].setOutlineThickness(2);
            charCardHighlight[i].setFillColor(Color(60, 70, 90, 200));
            charName[i].setFillColor(Color(220, 220, 230));
            charSubtitle[i].setFillColor(Color(150, 160, 180));
        }
        window.draw(charCard[i]);
        window.draw(charCardHighlight[i]);
        window.draw(charName[i]);
        window.draw(charSubtitle[i]);
    }

    instructionText.setPosition(screen_x - 200, screen_y - 35);
    window.draw(instructionText);
}

void Menu::renderModeSelection(RenderWindow& window) {
    window.draw(modeBgSprite);

    window.draw(modePanel);
    window.draw(modeTitleBar);
    window.draw(modeTitle);

    for (int i = 0; i < 2; i++) {
        if (i == selectionIndex) {
            Color sel(20, 130, 90, 240);
            sel.g = static_cast<Uint8>(130 + pulse * 60);
            modeCard[i].setFillColor(sel);
            modeCard[i].setOutlineColor(Color(80, 255, 160, 240));
            modeCard[i].setOutlineThickness(4);
            modeCardHighlight[i].setFillColor(Color(80, 255, 160, 255));
            modeName[i].setFillColor(Color(255, 255, 255));
            modeSubtitle[i].setFillColor(Color(220, 255, 240));
        }
        else {
            modeCard[i].setFillColor(Color(30, 60, 55, 220));
            modeCard[i].setOutlineColor(Color(100, 140, 130, 200));
            modeCard[i].setOutlineThickness(2);
            modeCardHighlight[i].setFillColor(Color(70, 100, 95, 200));
            modeName[i].setFillColor(Color(220, 230, 225));
            modeSubtitle[i].setFillColor(Color(150, 180, 170));
        }
        window.draw(modeCard[i]);
        window.draw(modeCardHighlight[i]);
        window.draw(modeName[i]);
        window.draw(modeSubtitle[i]);
    }

    instructionText.setPosition(screen_x - 200, screen_y - 35);
    window.draw(instructionText);
}
