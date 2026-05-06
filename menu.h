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

    // Background textures
    Texture startBgTexture;
    Texture characterBgTexture;
    Texture modeBgTexture;
    Sprite startBgSprite;
    Sprite characterBgSprite;
    Sprite modeBgSprite;

    // Start screen elements
    CircleShape startButton;
    Text startText;
    Text titleText;
    float startButtonScale;
    float startButtonGlow;

    // Character Selection Elements
    CircleShape macroRossiBtn;
    CircleShape tarmaRovingBtn;
    CircleShape eriKasamotoBtn;
    CircleShape fiolinaGermiBtn;
    Text macroRossiText;
    Text tarmaRovingText;
    Text eriKasamotoText;
    Text fiolinaGermiText;
    Text characterTitleText;

    // Game Mode Menu Elements
    CircleShape survivalBtn;
    CircleShape campaignBtn;
    Text survivalText;
    Text campaignText;
    Text modeTitleText;

    // Arrow indicators
    ConvexShape arrowIndicator;
    int selectionIndex;
    float arrowScale;

    // Minimal instruction box
    Text instructionText;

    // Animation variables
    float animationTime;
    float glowIntensity;

    // Character preview images
    Texture macroTexture;
    Texture tarmaTexture;
    Texture eriTexture;
    Texture fiolinaTexture;
    Sprite macroSprite;
    Sprite tarmaSprite;
    Sprite eriSprite;
    Sprite fiolinaSprite;

public:
    Menu(int sx, int sy) : screen_x(sx), screen_y(sy), menuState(0), animationTime(0), 
                          glowIntensity(0), selectionIndex(0), startButtonScale(1), 
                          startButtonGlow(0), arrowScale(1) {
        if (!menuFont.loadFromFile("arial.ttf")) {
            // Fallback if font fails to load
        }
        loadBackgrounds();
        loadCharacterSprites();
        initializeStartScreen();
        initializeCharacterSelection();
        initializeModeSelection();
    }

    void loadBackgrounds() {
        if (startBgTexture.loadFromFile("start.jpg")) {
            startBgSprite.setTexture(startBgTexture);
            startBgSprite.setScale(screen_x / 1920.0f, screen_y / 1080.0f);
        }
        if (characterBgTexture.loadFromFile("choose_character.jpg")) {
            characterBgSprite.setTexture(characterBgTexture);
            characterBgSprite.setScale(screen_x / 1920.0f, screen_y / 1080.0f);
        }
        if (modeBgTexture.loadFromFile("choose_mode.jpg")) {
            modeBgSprite.setTexture(modeBgTexture);
            modeBgSprite.setScale(screen_x / 1920.0f, screen_y / 1080.0f);
        }
    }

    void loadCharacterSprites() {
        if (macroTexture.loadFromFile("Sprites/Marco Rossi 1.png")) {
            macroSprite.setTexture(macroTexture);
            macroSprite.setScale(0.3f, 0.3f);
        }
        if (tarmaTexture.loadFromFile("Sprites/Tarma Roving.png")) {
            tarmaSprite.setTexture(tarmaTexture);
            tarmaSprite.setScale(0.3f, 0.3f);
        }
        if (eriTexture.loadFromFile("Sprites/Eri Kasamoto.png")) {
            eriSprite.setTexture(eriTexture);
            eriSprite.setScale(0.3f, 0.3f);
        }
        if (fiolinaTexture.loadFromFile("Sprites/Fiolina Germi 1.png")) {
            fiolinaSprite.setTexture(fiolinaTexture);
            fiolinaSprite.setScale(0.3f, 0.3f);
        }
    }

    void initializeStartScreen() {
        // Beautiful round start button with gradient effect
        startButton.setRadius(80);
        startButton.setPosition(screen_x / 2 - 80, screen_y / 2 + 50);
        startButton.setFillColor(Color(255, 215, 0)); // Gold
        startButton.setOutlineThickness(4);
        startButton.setOutlineColor(Color(255, 255, 255));

        // Title text
        titleText.setFont(menuFont);
        titleText.setString("METAL SLUG");
        titleText.setCharacterSize(80);
        titleText.setFillColor(Color(255, 255, 255));
        titleText.setOutlineColor(Color(0, 0, 0));
        titleText.setOutlineThickness(3);
        titleText.setPosition(screen_x / 2 - 220, screen_y / 2 - 150);

        // Start button text
        startText.setFont(menuFont);
        startText.setString("START");
        startText.setCharacterSize(35);
        startText.setFillColor(Color(0, 0, 0));
        startText.setPosition(screen_x / 2 - 55, screen_y / 2 + 70);

        // Instruction text - minimal in corner
        instructionText.setFont(menuFont);
        instructionText.setString("UP, Down, Select");
        instructionText.setCharacterSize(16);
        instructionText.setFillColor(Color(255, 255, 255));
        instructionText.setOutlineColor(Color(0, 0, 0));
        instructionText.setOutlineThickness(2);
        instructionText.setPosition(screen_x - 150, screen_y - 40);

        // Arrow indicator
        arrowIndicator.setPointCount(3);
        arrowIndicator.setPoint(0, Vector2f(0, 0));
        arrowIndicator.setPoint(1, Vector2f(-25, -12));
        arrowIndicator.setPoint(2, Vector2f(-25, 12));
        arrowIndicator.setFillColor(Color(0, 255, 0));
        arrowIndicator.setOutlineThickness(2);
        arrowIndicator.setOutlineColor(Color(0, 200, 0));
    }

    void initializeCharacterSelection() {
        // Character selection - circular buttons with reflections
        float buttonRadius = 60;
        float startY = screen_y / 2 - 100;
        float spacing = 120;

        macroRossiBtn.setRadius(buttonRadius);
        macroRossiBtn.setPosition(screen_x / 2 - 250, startY);
        macroRossiBtn.setFillColor(Color(255, 165, 0)); // Orange
        macroRossiBtn.setOutlineThickness(3);
        macroRossiBtn.setOutlineColor(Color(255, 200, 100));

        tarmaRovingBtn.setRadius(buttonRadius);
        tarmaRovingBtn.setPosition(screen_x / 2 - 250, startY + spacing);
        tarmaRovingBtn.setFillColor(Color(0, 191, 255)); // Deep sky blue
        tarmaRovingBtn.setOutlineThickness(3);
        tarmaRovingBtn.setOutlineColor(Color(100, 200, 255));

        eriKasamotoBtn.setRadius(buttonRadius);
        eriKasamotoBtn.setPosition(screen_x / 2 - 250, startY + spacing * 2);
        eriKasamotoBtn.setFillColor(Color(255, 105, 180)); // Hot pink
        eriKasamotoBtn.setOutlineThickness(3);
        eriKasamotoBtn.setOutlineColor(Color(255, 150, 200));

        fiolinaGermiBtn.setRadius(buttonRadius);
        fiolinaGermiBtn.setPosition(screen_x / 2 - 250, startY + spacing * 3);
        fiolinaGermiBtn.setFillColor(Color(147, 112, 219)); // Purple
        fiolinaGermiBtn.setOutlineThickness(3);
        fiolinaGermiBtn.setOutlineColor(Color(180, 140, 250));

        // Character names
        characterTitleText.setFont(menuFont);
        characterTitleText.setString("CHOOSE YOUR HERO");
        characterTitleText.setCharacterSize(50);
        characterTitleText.setFillColor(Color(255, 255, 255));
        characterTitleText.setOutlineColor(Color(0, 0, 0));
        characterTitleText.setOutlineThickness(3);
        characterTitleText.setPosition(screen_x / 2 - 250, 100);

        macroRossiText.setFont(menuFont);
        macroRossiText.setString("Marco Rossi");
        macroRossiText.setCharacterSize(28);
        macroRossiText.setFillColor(Color(255, 255, 255));
        macroRossiText.setOutlineColor(Color(0, 0, 0));
        macroRossiText.setOutlineThickness(2);
        macroRossiText.setPosition(screen_x / 2 - 100, startY + 20);

        tarmaRovingText.setFont(menuFont);
        tarmaRovingText.setString("Tarma Roving");
        tarmaRovingText.setCharacterSize(28);
        tarmaRovingText.setFillColor(Color(255, 255, 255));
        tarmaRovingText.setOutlineColor(Color(0, 0, 0));
        tarmaRovingText.setOutlineThickness(2);
        tarmaRovingText.setPosition(screen_x / 2 - 100, startY + spacing + 20);

        eriKasamotoText.setFont(menuFont);
        eriKasamotoText.setString("Eri Kasamoto");
        eriKasamotoText.setCharacterSize(28);
        eriKasamotoText.setFillColor(Color(255, 255, 255));
        eriKasamotoText.setOutlineColor(Color(0, 0, 0));
        eriKasamotoText.setOutlineThickness(2);
        eriKasamotoText.setPosition(screen_x / 2 - 100, startY + spacing * 2 + 20);

        fiolinaGermiText.setFont(menuFont);
        fiolinaGermiText.setString("Fiolina Germi");
        fiolinaGermiText.setCharacterSize(28);
        fiolinaGermiText.setFillColor(Color(255, 255, 255));
        fiolinaGermiText.setOutlineColor(Color(0, 0, 0));
        fiolinaGermiText.setOutlineThickness(2);
        fiolinaGermiText.setPosition(screen_x / 2 - 100, startY + spacing * 3 + 20);

        // Position character sprites
        macroSprite.setPosition(screen_x / 2 + 150, startY - 20);
        tarmaSprite.setPosition(screen_x / 2 + 150, startY + spacing - 20);
        eriSprite.setPosition(screen_x / 2 + 150, startY + spacing * 2 - 20);
        fiolinaSprite.setPosition(screen_x / 2 + 150, startY + spacing * 3 - 20);
    }

    void initializeModeSelection() {
        // Mode selection - circular buttons
        float buttonRadius = 70;
        float startY = screen_y / 2 - 50;

        survivalBtn.setRadius(buttonRadius);
        survivalBtn.setPosition(screen_x / 2 - 250, startY);
        survivalBtn.setFillColor(Color(50, 205, 50)); // Lime green
        survivalBtn.setOutlineThickness(3);
        survivalBtn.setOutlineColor(Color(100, 255, 100));

        campaignBtn.setRadius(buttonRadius);
        campaignBtn.setPosition(screen_x / 2 - 250, startY + 150);
        campaignBtn.setFillColor(Color(70, 130, 180)); // Steel blue
        campaignBtn.setOutlineThickness(3);
        campaignBtn.setOutlineColor(Color(100, 160, 210));

        // Mode title
        modeTitleText.setFont(menuFont);
        modeTitleText.setString("SELECT MODE");
        modeTitleText.setCharacterSize(50);
        modeTitleText.setFillColor(Color(255, 255, 255));
        modeTitleText.setOutlineColor(Color(0, 0, 0));
        modeTitleText.setOutlineThickness(3);
        modeTitleText.setPosition(screen_x / 2 - 150, 100);

        // Mode names
        survivalText.setFont(menuFont);
        survivalText.setString("SURVIVAL");
        survivalText.setCharacterSize(30);
        survivalText.setFillColor(Color(255, 255, 255));
        survivalText.setOutlineColor(Color(0, 0, 0));
        survivalText.setOutlineThickness(2);
        survivalText.setPosition(screen_x / 2 - 90, startY + 20);

        campaignText.setFont(menuFont);
        campaignText.setString("CAMPAIGN");
        campaignText.setCharacterSize(30);
        campaignText.setFillColor(Color(255, 255, 255));
        campaignText.setOutlineColor(Color(0, 0, 0));
        campaignText.setOutlineThickness(2);
        campaignText.setPosition(screen_x / 2 - 90, startY + 170);
    }

    void updateAnimation(float dt) {
        animationTime += dt;
        glowIntensity = (sin(animationTime * 4) + 1) / 2; // 0 to 1
        startButtonGlow = (sin(animationTime * 3) + 1) / 2;
        arrowScale = 1 + sin(animationTime * 5) * 0.1f;
    }

    void moveSelectionUp() {
        if (menuState == 1 && selectionIndex > 0) {
            selectionIndex--;
        } else if (menuState == 2 && selectionIndex > 0) {
            selectionIndex--;
        }
    }

    void moveSelectionDown() {
        if (menuState == 1 && selectionIndex < 3) {
            selectionIndex++;
        } else if (menuState == 2 && selectionIndex < 1) {
            selectionIndex++;
        }
    }

    int getSelectionIndex() {
        return selectionIndex;
    }

    int getMenuState() {
        return menuState;
    }

    void setMenuState(int state) {
        menuState = state;
        selectionIndex = 0;
    }

    void resetSelection() {
        selectionIndex = 0;
    }

    void renderStartScreen(RenderWindow& window) {
        window.clear();
        
        // Draw background
        window.draw(startBgSprite);

        // Animated start button with glow
        Color startColor = Color(255, 215, 0);
        startColor.r = static_cast<Uint8>(255 + startButtonGlow * 30);
        startColor.g = static_cast<Uint8>(215 + startButtonGlow * 40);
        startButton.setFillColor(startColor);
        startButton.setScale(startButtonScale + startButtonGlow * 0.1f, startButtonScale + startButtonGlow * 0.1f);
        startButton.setPosition(screen_x / 2 - 80 * (startButtonScale + startButtonGlow * 0.1f), screen_y / 2 + 50);

        window.draw(titleText);
        window.draw(startButton);
        window.draw(startText);
        window.draw(instructionText);
        window.display();
    }

    void renderCharacterSelection(RenderWindow& window) {
        window.clear();
        
        // Draw background
        window.draw(characterBgSprite);

        // Update button colors based on selection
        Color selectedColor = Color(0, 255, 0);
        Color normalColor;

        float startY = screen_y / 2 - 100;
        float spacing = 120;

        // Macro Rossi
        if (selectionIndex == 0) {
            macroRossiBtn.setFillColor(selectedColor);
            macroRossiBtn.setScale(1.1f, 1.1f);
        } else {
            normalColor = Color(255, 165, 0);
            normalColor.r = static_cast<Uint8>(255 + glowIntensity * 20);
            macroRossiBtn.setFillColor(normalColor);
            macroRossiBtn.setScale(1, 1);
        }
        macroRossiBtn.setPosition(screen_x / 2 - 250 * macroRossiBtn.getScale().x, startY);

        // Tarma Roving
        if (selectionIndex == 1) {
            tarmaRovingBtn.setFillColor(selectedColor);
            tarmaRovingBtn.setScale(1.1f, 1.1f);
        } else {
            normalColor = Color(0, 191, 255);
            normalColor.b = static_cast<Uint8>(255 + glowIntensity * 20);
            tarmaRovingBtn.setFillColor(normalColor);
            tarmaRovingBtn.setScale(1, 1);
        }
        tarmaRovingBtn.setPosition(screen_x / 2 - 250 * tarmaRovingBtn.getScale().x, startY + spacing);

        // Eri Kasamoto
        if (selectionIndex == 2) {
            eriKasamotoBtn.setFillColor(selectedColor);
            eriKasamotoBtn.setScale(1.1f, 1.1f);
        } else {
            normalColor = Color(255, 105, 180);
            normalColor.r = static_cast<Uint8>(255 + glowIntensity * 20);
            eriKasamotoBtn.setFillColor(normalColor);
            eriKasamotoBtn.setScale(1, 1);
        }
        eriKasamotoBtn.setPosition(screen_x / 2 - 250 * eriKasamotoBtn.getScale().x, startY + spacing * 2);

        // Fiolina Germi
        if (selectionIndex == 3) {
            fiolinaGermiBtn.setFillColor(selectedColor);
            fiolinaGermiBtn.setScale(1.1f, 1.1f);
        } else {
            normalColor = Color(147, 112, 219);
            normalColor.b = static_cast<Uint8>(219 + glowIntensity * 20);
            fiolinaGermiBtn.setFillColor(normalColor);
            fiolinaGermiBtn.setScale(1, 1);
        }
        fiolinaGermiBtn.setPosition(screen_x / 2 - 250 * fiolinaGermiBtn.getScale().x, startY + spacing * 3);

        // Draw elements
        window.draw(characterTitleText);
        window.draw(macroRossiBtn);
        window.draw(tarmaRovingBtn);
        window.draw(eriKasamotoBtn);
        window.draw(fiolinaGermiBtn);
        window.draw(macroRossiText);
        window.draw(tarmaRovingText);
        window.draw(eriKasamotoText);
        window.draw(fiolinaGermiText);

        // Draw character sprites
        window.draw(macroSprite);
        window.draw(tarmaSprite);
        window.draw(eriSprite);
        window.draw(fiolinaSprite);

        // Draw arrow indicator
        float arrowY = startY + 40 + selectionIndex * spacing;
        arrowIndicator.setPosition(screen_x / 2 - 320, arrowY);
        arrowIndicator.setScale(arrowScale, arrowScale);
        window.draw(arrowIndicator);

        // Draw instruction text
        window.draw(instructionText);
        window.display();
    }

    void renderModeSelection(RenderWindow& window) {
        window.clear();
        
        // Draw background
        window.draw(modeBgSprite);

        // Update button colors based on selection
        Color selectedColor = Color(0, 255, 0);
        Color normalColor;
        float startY = screen_y / 2 - 50;

        // Survival
        if (selectionIndex == 0) {
            survivalBtn.setFillColor(selectedColor);
            survivalBtn.setScale(1.1f, 1.1f);
        } else {
            normalColor = Color(50, 205, 50);
            normalColor.g = static_cast<Uint8>(205 + glowIntensity * 30);
            survivalBtn.setFillColor(normalColor);
            survivalBtn.setScale(1, 1);
        }
        survivalBtn.setPosition(screen_x / 2 - 250 * survivalBtn.getScale().x, startY);

        // Campaign
        if (selectionIndex == 1) {
            campaignBtn.setFillColor(selectedColor);
            campaignBtn.setScale(1.1f, 1.1f);
        } else {
            normalColor = Color(70, 130, 180);
            normalColor.b = static_cast<Uint8>(180 + glowIntensity * 30);
            campaignBtn.setFillColor(normalColor);
            campaignBtn.setScale(1, 1);
        }
        campaignBtn.setPosition(screen_x / 2 - 250 * campaignBtn.getScale().x, startY + 150);

        // Draw elements
        window.draw(modeTitleText);
        window.draw(survivalBtn);
        window.draw(campaignBtn);
        window.draw(survivalText);
        window.draw(campaignText);

        // Draw arrow indicator
        float arrowY = startY + 40 + selectionIndex * 150;
        arrowIndicator.setPosition(screen_x / 2 - 320, arrowY);
        arrowIndicator.setScale(arrowScale, arrowScale);
        window.draw(arrowIndicator);

        // Draw instruction text
        window.draw(instructionText);
        window.display();
    }
};