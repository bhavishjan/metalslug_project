#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

class Menu {
private:
    int screen_x;
    int screen_y;
    Font menuFont;

    int menuState;
    int selectionIndex;

    bool isPauseMenu;
    bool pauseMenuVisible;

    Texture startBgTexture;
    Texture characterBgTexture;
    Texture modeBgTexture;
    Sprite startBgSprite;
    Sprite characterBgSprite;
    Sprite modeBgSprite;

    float animationTime;
    float pulse;
    float blink;

    Text pressEnterText;
    Text instructionText;

    RectangleShape charPanel;
    RectangleShape charCard[4];
    RectangleShape charCardHighlight[4];
    Text charName[4];
    Text charSubtitle[4];
    Text charTitle;
    RectangleShape charTitleBar;

    RectangleShape modePanel;
    RectangleShape modeCard[2];
    RectangleShape modeCardHighlight[2];
    Text modeName[2];
    Text modeSubtitle[2];
    Text modeTitle;
    RectangleShape modeTitleBar;

    RectangleShape pauseBox;
    RectangleShape pauseResumeButton;
    RectangleShape pauseExitButton;
    Text pauseTitle;
    Text pauseResumeText;
    Text pauseExitText;
    Text pauseInstructions;
    int pauseSelection;

public:
    Menu(bool isPause = false);
    void loadBackgrounds();
    void initializeStartScreen();
    void initializeCharacterSelection();
    void initializeModeSelection();
    void initializePauseMenu();
    void showPauseMenu();
    void hidePauseMenu();
    bool getPauseMenuVisible() const;
    int getPauseSelection() const;
    void handlePauseInput();
    void renderPauseMenu(RenderWindow& window);
    void updateAnimation(float dt);
    void moveSelectionUp();
    void moveSelectionDown();
    int getSelectionIndex();
    int getMenuState();
    void setMenuState(int s);
    void resetSelection();
    void renderStartScreen(RenderWindow& window);
    void renderCharacterSelection(RenderWindow& window);
    void renderModeSelection(RenderWindow& window);
};