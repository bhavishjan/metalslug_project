#pragma once

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class Menu {
private:
    int screen_x;
    int screen_y;
    Font menuFont;

    // Menu state: 0=Start, 1=Character, 2=Mode
    int menuState;
    int selectionIndex;

    // Background textures
    Texture startBgTexture;
    Texture characterBgTexture;
    Texture modeBgTexture;
    Sprite startBgSprite;
    Sprite characterBgSprite;
    Sprite modeBgSprite;

    // Animation
    float animationTime;
    float pulse;
    float blink;

    // Common UI Elements
    Text pressEnterText;
    Text instructionText;

    // Character Selection - Card-based UI
    RectangleShape charPanel;
    RectangleShape charCard[4];
    RectangleShape charCardHighlight[4];
    Text charName[4];
    Text charSubtitle[4];
    Text charTitle;
    RectangleShape charTitleBar;

    // Mode Selection - Card-based UI
    RectangleShape modePanel;
    RectangleShape modeCard[2];
    RectangleShape modeCardHighlight[2];
    Text modeName[2];
    Text modeSubtitle[2];
    Text modeTitle;
    RectangleShape modeTitleBar;

public:
    Menu(int sx, int sy) : screen_x(sx), screen_y(sy), menuState(0), selectionIndex(0),
                          animationTime(0), pulse(0), blink(0) {
        menuFont.loadFromFile("arial.ttf");
        loadBackgrounds();
        initializeStartScreen();
        initializeCharacterSelection();
        initializeModeSelection();
    }

    void loadBackgrounds() {
        if (startBgTexture.loadFromFile("start.png")) {
            startBgSprite.setTexture(startBgTexture);
            Vector2u sz = startBgTexture.getSize();
            startBgSprite.setScale((float)screen_x / sz.x, (float)screen_y / sz.y);
        }
        if (characterBgTexture.loadFromFile("choose_character.jpg")) {
            characterBgSprite.setTexture(characterBgTexture);
            Vector2u sz = characterBgTexture.getSize();
            characterBgSprite.setScale((float)screen_x / sz.x, (float)screen_y / sz.y);
        }
        if (modeBgTexture.loadFromFile("choose_mode.jpg")) {
            modeBgSprite.setTexture(modeBgTexture);
            Vector2u sz = modeBgTexture.getSize();
            modeBgSprite.setScale((float)screen_x / sz.x, (float)screen_y / sz.y);
        }
    }

    void initializeStartScreen() {
        // Just elegant pulsing text at bottom center - no Metal Slug title needed (already in image)
        pressEnterText.setFont(menuFont);
        pressEnterText.setString("PRESS ENTER TO START");
        pressEnterText.setCharacterSize(38);
        pressEnterText.setFillColor(Color(255, 230, 100));
        pressEnterText.setOutlineColor(Color(80, 30, 0));
        pressEnterText.setOutlineThickness(3);
        pressEnterText.setStyle(Text::Bold);
        FloatRect b = pressEnterText.getLocalBounds();
        pressEnterText.setPosition(screen_x / 2.0f - b.width / 2.0f, screen_y - 110);

        // Minimal instruction in corner
        instructionText.setFont(menuFont);
        instructionText.setString("UP / DOWN / ENTER");
        instructionText.setCharacterSize(15);
        instructionText.setFillColor(Color(255, 255, 255, 200));
        instructionText.setOutlineColor(Color(0, 0, 0, 220));
        instructionText.setOutlineThickness(2);
        instructionText.setStyle(Text::Bold);
        instructionText.setPosition(screen_x - 200, screen_y - 35);
    }

    void initializeCharacterSelection() {
        // Background image: characters on right, empty light-gray on left
        // UI panel goes on the LEFT side
        const float panelX = 60;
        const float panelY = 90;
        const float panelW = 480;
        const float panelH = 720;

        // Subtle frosted glass panel
        charPanel.setSize(Vector2f(panelW, panelH));
        charPanel.setPosition(panelX, panelY);
        charPanel.setFillColor(Color(15, 25, 40, 180));
        charPanel.setOutlineColor(Color(220, 50, 50, 230));
        charPanel.setOutlineThickness(3);

        // Title bar
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

        // Character cards
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

    void initializeModeSelection() {
        // Background image: characters on right, empty teal/green on left
        // UI panel goes on the LEFT side
        const float panelX = 80;
        const float panelY = 130;
        const float panelW = 520;
        const float panelH = 620;

        // Frosted panel matching the green theme
        modePanel.setSize(Vector2f(panelW, panelH));
        modePanel.setPosition(panelX, panelY);
        modePanel.setFillColor(Color(20, 50, 45, 180));
        modePanel.setOutlineColor(Color(70, 180, 150, 230));
        modePanel.setOutlineThickness(3);

        // Title bar
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

        // Mode cards
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
            modeSubtitle[i].setPosition(cardX + 30, startY + i * (cardH + gap) + 90);
        }
    }

    void updateAnimation(float dt) {
        animationTime += dt;
        pulse = (sin(animationTime * 3.0f) + 1.0f) / 2.0f;
        blink = (sin(animationTime * 2.5f) + 1.0f) / 2.0f;
    }

    void moveSelectionUp() {
        if (menuState == 0) return;
        if (selectionIndex > 0) selectionIndex--;
    }

    void moveSelectionDown() {
        int maxIdx;
        if (menuState == 1) maxIdx = 3;
        else if (menuState == 2) maxIdx = 1;
        else maxIdx = 0;
        if (selectionIndex < maxIdx) selectionIndex++;
    }

    int getSelectionIndex() { return selectionIndex; }
    int getMenuState() { return menuState; }
    void setMenuState(int s) { menuState = s; selectionIndex = 0; }
    void resetSelection() { selectionIndex = 0; }

    void renderStartScreen(RenderWindow& window) {
        window.clear();
        window.draw(startBgSprite);

        // Pulsing "PRESS ENTER" text
        Color c(255, 230, 100);
        c.a = static_cast<Uint8>(150 + blink * 105);
        pressEnterText.setFillColor(c);
        float scale = 1.0f + blink * 0.05f;
        pressEnterText.setScale(scale, scale);
        FloatRect b = pressEnterText.getLocalBounds();
        pressEnterText.setPosition(screen_x / 2.0f - (b.width * scale) / 2.0f, screen_y - 110);
        window.draw(pressEnterText);

        window.draw(instructionText);
        window.display();
    }

    void renderCharacterSelection(RenderWindow& window) {
        window.clear();
        window.draw(characterBgSprite);

        window.draw(charPanel);
        window.draw(charTitleBar);
        window.draw(charTitle);

        // Render cards with selection highlight
        for (int i = 0; i < 4; i++) {
            if (i == selectionIndex) {
                // Selected: green theme + subtle glow pulse
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

        // Minimal instruction in corner
        instructionText.setPosition(screen_x - 200, screen_y - 35);
        window.draw(instructionText);
        window.display();
    }

    void renderModeSelection(RenderWindow& window) {
        window.clear();
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
        window.display();
    }
};