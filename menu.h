#pragma once

#include <SFML/Graphics.hpp>
#include <cstring>

using namespace sf;

class Menu {
private:
    int screen_x;
    int screen_y;
    Font menuFont;

    // Game Mode Menu Elements
    RectangleShape menuBg;
    RectangleShape titleBar;
    RectangleShape survivalBtn;
    RectangleShape campaignBtn;
    RectangleShape instructionBox;
    Text titleText;
    Text survivalText;
    Text campaignText;
    Text instructionText;

    // Arrow indicator for game mode menu
    ConvexShape arrowIndicator;
    int gameModeSelection; // 0 = survival, 1 = campaign
    float arrowOffset;

    // Player Selection Menu Elements
    RectangleShape playerSelectMenuBg;
    RectangleShape playerSelectTitleBar;
    RectangleShape macroRossiBtn;
    RectangleShape tarmaRovingBtn;
    RectangleShape eriKasamotoBtn;
    RectangleShape fiolinaGermiBtn;
    RectangleShape playerInstructionBox;
    Text playerSelectTitleText;
    Text macroRossiText;
    Text tarmaRovingText;
    Text eriKasamotoText;
    Text fiolinaGermiText;
    Text playerInstructionText;

    // Arrow indicator for player selection
    ConvexShape playerArrowIndicator;
    int playerSelectionIndex; // 0-3 for characters
    float playerArrowOffset;

    // Animation variables
    float animationTime;
    float pulseIntensity;

public:
    Menu(int sx, int sy) : screen_x(sx), screen_y(sy), animationTime(0), pulseIntensity(0), 
                          gameModeSelection(0), arrowOffset(0), playerSelectionIndex(0), playerArrowOffset(0) {
        if (!menuFont.loadFromFile("arial.ttf")) {
            // Fallback if font fails to load
        }
        initializeGameModeMenu();
        initializePlayerSelectionMenu();
    }

    void initializeGameModeMenu() {
        // Modern gradient-like background
        menuBg.setSize(Vector2f(screen_x, screen_y));
        menuBg.setFillColor(Color(15, 23, 42)); // Dark slate

        // Styled title bar with gradient effect
        titleBar.setSize(Vector2f(600, 100));
        titleBar.setFillColor(Color(99, 102, 241)); // Indigo
        titleBar.setPosition(500, 120);
        titleBar.setOutlineThickness(3);
        titleBar.setOutlineColor(Color(165, 180, 252));

        // Modern survival button
        survivalBtn.setSize(Vector2f(500, 80));
        survivalBtn.setFillColor(Color(34, 197, 94)); // Green
        survivalBtn.setPosition(550, 320);
        survivalBtn.setOutlineThickness(2);
        survivalBtn.setOutlineColor(Color(74, 222, 128));

        // Modern campaign button
        campaignBtn.setSize(Vector2f(500, 80));
        campaignBtn.setFillColor(Color(59, 130, 246)); // Blue
        campaignBtn.setPosition(550, 440);
        campaignBtn.setOutlineThickness(2);
        campaignBtn.setOutlineColor(Color(96, 165, 250));

        // Instruction box
        instructionBox.setSize(Vector2f(400, 80));
        instructionBox.setFillColor(Color(30, 41, 59));
        instructionBox.setPosition(600, 750);
        instructionBox.setOutlineThickness(2);
        instructionBox.setOutlineColor(Color(148, 163, 184));

        // Title text with shadow effect
        titleText.setFont(menuFont);
        titleText.setString("SELECT MODE");
        titleText.setCharacterSize(55);
        titleText.setFillColor(Color(255, 255, 255));
        titleText.setPosition(580, 135);

        // Survival button text
        survivalText.setFont(menuFont);
        survivalText.setString("SURVIVAL MODE");
        survivalText.setCharacterSize(32);
        survivalText.setFillColor(Color(255, 255, 255));
        survivalText.setPosition(640, 340);

        // Campaign button text
        campaignText.setFont(menuFont);
        campaignText.setString("CAMPAIGN MODE");
        campaignText.setCharacterSize(32);
        campaignText.setFillColor(Color(255, 255, 255));
        campaignText.setPosition(640, 460);

        // Instruction text
        instructionText.setFont(menuFont);
        instructionText.setString("Use UP/DOWN arrows to navigate, ENTER to select");
        instructionText.setCharacterSize(18);
        instructionText.setFillColor(Color(148, 163, 184));
        instructionText.setPosition(620, 765);

        // Initialize arrow indicator
        arrowIndicator.setPointCount(3);
        arrowIndicator.setPoint(0, Vector2f(0, 0));
        arrowIndicator.setPoint(1, Vector2f(-30, -15));
        arrowIndicator.setPoint(2, Vector2f(-30, 15));
        arrowIndicator.setFillColor(Color(255, 255, 0));
        arrowIndicator.setOutlineThickness(2);
        arrowIndicator.setOutlineColor(Color(255, 200, 0));
        updateArrowPosition();
    }

    void initializePlayerSelectionMenu() {
        // Background
        playerSelectMenuBg.setSize(Vector2f(screen_x, screen_y));
        playerSelectMenuBg.setFillColor(Color(15, 23, 42)); // Dark slate

        // Title bar
        playerSelectTitleBar.setSize(Vector2f(700, 100));
        playerSelectTitleBar.setFillColor(Color(168, 85, 247)); // Purple
        playerSelectTitleBar.setPosition(450, 120);
        playerSelectTitleBar.setOutlineThickness(3);
        playerSelectTitleBar.setOutlineColor(Color(192, 132, 252));

        // Character buttons with distinct colors
        macroRossiBtn.setSize(Vector2f(500, 80));
        macroRossiBtn.setFillColor(Color(234, 88, 12)); // Orange
        macroRossiBtn.setPosition(550, 320);
        macroRossiBtn.setOutlineThickness(2);
        macroRossiBtn.setOutlineColor(Color(251, 146, 60));

        tarmaRovingBtn.setSize(Vector2f(500, 80));
        tarmaRovingBtn.setFillColor(Color(20, 184, 166)); // Teal
        tarmaRovingBtn.setPosition(550, 440);
        tarmaRovingBtn.setOutlineThickness(2);
        tarmaRovingBtn.setOutlineColor(Color(45, 212, 191));

        eriKasamotoBtn.setSize(Vector2f(500, 80));
        eriKasamotoBtn.setFillColor(Color(236, 72, 153)); // Pink
        eriKasamotoBtn.setPosition(550, 560);
        eriKasamotoBtn.setOutlineThickness(2);
        eriKasamotoBtn.setOutlineColor(Color(244, 114, 182));

        fiolinaGermiBtn.setSize(Vector2f(500, 80));
        fiolinaGermiBtn.setFillColor(Color(139, 92, 246)); // Violet
        fiolinaGermiBtn.setPosition(550, 680);
        fiolinaGermiBtn.setOutlineThickness(2);
        fiolinaGermiBtn.setOutlineColor(Color(167, 139, 250));

        // Instruction box
        playerInstructionBox.setSize(Vector2f(400, 80));
        playerInstructionBox.setFillColor(Color(30, 41, 59));
        playerInstructionBox.setPosition(600, 800);
        playerInstructionBox.setOutlineThickness(2);
        playerInstructionBox.setOutlineColor(Color(148, 163, 184));

        // Text elements
        playerSelectTitleText.setFont(menuFont);
        playerSelectTitleText.setString("CHOOSE CHARACTER");
        playerSelectTitleText.setCharacterSize(50);
        playerSelectTitleText.setFillColor(Color(255, 255, 255));
        playerSelectTitleText.setPosition(520, 135);

        macroRossiText.setFont(menuFont);
        macroRossiText.setString("Macro Rossi");
        macroRossiText.setCharacterSize(32);
        macroRossiText.setFillColor(Color(255, 255, 255));
        macroRossiText.setPosition(640, 340);

        tarmaRovingText.setFont(menuFont);
        tarmaRovingText.setString("Tarma Roving");
        tarmaRovingText.setCharacterSize(32);
        tarmaRovingText.setFillColor(Color(255, 255, 255));
        tarmaRovingText.setPosition(640, 460);

        eriKasamotoText.setFont(menuFont);
        eriKasamotoText.setString("Eri Kasamoto");
        eriKasamotoText.setCharacterSize(32);
        eriKasamotoText.setFillColor(Color(255, 255, 255));
        eriKasamotoText.setPosition(640, 580);

        fiolinaGermiText.setFont(menuFont);
        fiolinaGermiText.setString("Fiolina Germi");
        fiolinaGermiText.setCharacterSize(32);
        fiolinaGermiText.setFillColor(Color(255, 255, 255));
        fiolinaGermiText.setPosition(640, 700);

        // Instruction text
        playerInstructionText.setFont(menuFont);
        playerInstructionText.setString("Use UP/DOWN arrows to navigate, ENTER to select");
        playerInstructionText.setCharacterSize(18);
        playerInstructionText.setFillColor(Color(148, 163, 184));
        playerInstructionText.setPosition(620, 815);

        // Initialize player arrow indicator
        playerArrowIndicator.setPointCount(3);
        playerArrowIndicator.setPoint(0, Vector2f(0, 0));
        playerArrowIndicator.setPoint(1, Vector2f(-30, -15));
        playerArrowIndicator.setPoint(2, Vector2f(-30, 15));
        playerArrowIndicator.setFillColor(Color(255, 255, 0));
        playerArrowIndicator.setOutlineThickness(2);
        playerArrowIndicator.setOutlineColor(Color(255, 200, 0));
        updatePlayerArrowPosition();
    }

    void updateAnimation(float dt) {
        animationTime += dt;
        pulseIntensity = (sin(animationTime * 3) + 1) / 2; // 0 to 1
    }

    void updateArrowPosition() {
        float yPos = 0;
        if (gameModeSelection == 0) {
            yPos = 360; // Survival button position
        } else if (gameModeSelection == 1) {
            yPos = 480; // Campaign button position
        }
        arrowIndicator.setPosition(500, yPos);
    }

    void updatePlayerArrowPosition() {
        float yPos = 0;
        switch (playerSelectionIndex) {
            case 0: yPos = 360; break; // Macro Rossi
            case 1: yPos = 480; break; // Tarma Roving
            case 2: yPos = 600; break; // Eri Kasamoto
            case 3: yPos = 720; break; // Fiolina Germi
        }
        playerArrowIndicator.setPosition(500, yPos);
    }

    // Game mode menu navigation
    void moveGameModeSelectionUp() {
        if (gameModeSelection > 0) {
            gameModeSelection--;
            updateArrowPosition();
        }
    }

    void moveGameModeSelectionDown() {
        if (gameModeSelection < 1) {
            gameModeSelection++;
            updateArrowPosition();
        }
    }

    int getGameModeSelection() {
        return gameModeSelection;
    }

    // Player selection menu navigation
    void movePlayerSelectionUp() {
        if (playerSelectionIndex > 0) {
            playerSelectionIndex--;
            updatePlayerArrowPosition();
        }
    }

    void movePlayerSelectionDown() {
        if (playerSelectionIndex < 3) {
            playerSelectionIndex++;
            updatePlayerArrowPosition();
        }
    }

    int getPlayerSelectionIndex() {
        return playerSelectionIndex;
    }

    void resetPlayerSelection() {
        playerSelectionIndex = 0;
        updatePlayerArrowPosition();
    }

    void renderGameModeMenu(RenderWindow& window) {
        window.clear(Color(15, 23, 42));
        
        // Apply pulse effect to buttons
        Color survivalColor = Color(34, 197, 94);
        Color campaignColor = Color(59, 130, 246);
        
        survivalColor.r = static_cast<Uint8>(34 + pulseIntensity * 20);
        survivalColor.g = static_cast<Uint8>(197 + pulseIntensity * 20);
        survivalBtn.setFillColor(survivalColor);
        
        campaignColor.b = static_cast<Uint8>(246 + pulseIntensity * 9);
        campaignBtn.setFillColor(campaignColor);

        window.draw(menuBg);
        window.draw(titleBar);
        window.draw(survivalBtn);
        window.draw(campaignBtn);
        window.draw(instructionBox);
        window.draw(arrowIndicator);

        window.draw(titleText);
        window.draw(survivalText);
        window.draw(campaignText);
        window.draw(instructionText);

        window.display();
    }

    void renderPlayerSelectionMenu(RenderWindow& window) {
        window.clear(Color(15, 23, 42));
        
        // Apply pulse effect to buttons
        Color macroColor = Color(234, 88, 12);
        Color tarmaColor = Color(20, 184, 166);
        Color eriColor = Color(236, 72, 153);
        Color fiolinaColor = Color(139, 92, 246);
        
        macroColor.r = static_cast<Uint8>(234 + pulseIntensity * 21);
        macroColor.g = static_cast<Uint8>(88 + pulseIntensity * 10);
        macroRossiBtn.setFillColor(macroColor);
        
        tarmaColor.g = static_cast<Uint8>(184 + pulseIntensity * 20);
        tarmaRovingBtn.setFillColor(tarmaColor);
        
        eriColor.r = static_cast<Uint8>(236 + pulseIntensity * 19);
        eriColor.b = static_cast<Uint8>(153 + pulseIntensity * 20);
        eriKasamotoBtn.setFillColor(eriColor);
        
        fiolinaColor.b = static_cast<Uint8>(246 + pulseIntensity * 9);
        fiolinaGermiBtn.setFillColor(fiolinaColor);

        window.draw(playerSelectMenuBg);
        window.draw(playerSelectTitleBar);
        window.draw(macroRossiBtn);
        window.draw(tarmaRovingBtn);
        window.draw(eriKasamotoBtn);
        window.draw(fiolinaGermiBtn);
        window.draw(playerInstructionBox);
        window.draw(playerArrowIndicator);

        window.draw(playerSelectTitleText);
        window.draw(macroRossiText);
        window.draw(tarmaRovingText);
        window.draw(eriKasamotoText);
        window.draw(fiolinaGermiText);
        window.draw(playerInstructionText);

        window.display();
    }
};