#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <string>

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
    RectangleShape key1;
    RectangleShape key2;
    Text titleText;
    Text survivalText;
    Text campaignText;

    // Player Selection Menu Elements
    RectangleShape playerSelectMenuBg;
    RectangleShape playerSelectTitleBar;
    RectangleShape macroRossiBtn;
    RectangleShape tarmaRovingBtn;
    RectangleShape eriKasamotoBtn;
    RectangleShape fiolinaGermiBtn;
    RectangleShape playerSelectKey1;
    RectangleShape playerSelectKey2;
    RectangleShape playerSelectKey3;
    RectangleShape playerSelectKey4;
    Text playerSelectTitleText;
    Text macroRossiText;
    Text tarmaRovingText;
    Text eriKasamotoText;
    Text fiolinaGermiText;

    // Animation variables
    float animationTime;
    float pulseIntensity;

public:
    Menu(int sx, int sy) : screen_x(sx), screen_y(sy), animationTime(0), pulseIntensity(0) {
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

        // Styled key indicators
        key1.setSize(Vector2f(50, 50));
        key1.setFillColor(Color(239, 68, 68)); // Red
        key1.setPosition(570, 335);
        key1.setOutlineThickness(2);
        key1.setOutlineColor(Color(248, 113, 113));

        key2.setSize(Vector2f(50, 50));
        key2.setFillColor(Color(239, 68, 68)); // Red
        key2.setPosition(570, 455);
        key2.setOutlineThickness(2);
        key2.setOutlineColor(Color(248, 113, 113));

        // Title text with shadow effect
        titleText.setFont(menuFont);
        titleText.setString("SELECT MODE");
        titleText.setCharacterSize(55);
        titleText.setFillColor(Color(255, 255, 255));
        titleText.setPosition(580, 135);

        // Survival button text
        survivalText.setFont(menuFont);
        survivalText.setString("1 - SURVIVAL MODE");
        survivalText.setCharacterSize(32);
        survivalText.setFillColor(Color(255, 255, 255));
        survivalText.setPosition(640, 340);

        // Campaign button text
        campaignText.setFont(menuFont);
        campaignText.setString("2 - CAMPAIGN MODE");
        campaignText.setCharacterSize(32);
        campaignText.setFillColor(Color(255, 255, 255));
        campaignText.setPosition(640, 460);
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

        // Key indicators
        playerSelectKey1.setSize(Vector2f(50, 50));
        playerSelectKey1.setFillColor(Color(239, 68, 68));
        playerSelectKey1.setPosition(570, 335);
        playerSelectKey1.setOutlineThickness(2);
        playerSelectKey1.setOutlineColor(Color(248, 113, 113));

        playerSelectKey2.setSize(Vector2f(50, 50));
        playerSelectKey2.setFillColor(Color(239, 68, 68));
        playerSelectKey2.setPosition(570, 455);
        playerSelectKey2.setOutlineThickness(2);
        playerSelectKey2.setOutlineColor(Color(248, 113, 113));

        playerSelectKey3.setSize(Vector2f(50, 50));
        playerSelectKey3.setFillColor(Color(239, 68, 68));
        playerSelectKey3.setPosition(570, 575);
        playerSelectKey3.setOutlineThickness(2);
        playerSelectKey3.setOutlineColor(Color(248, 113, 113));

        playerSelectKey4.setSize(Vector2f(50, 50));
        playerSelectKey4.setFillColor(Color(239, 68, 68));
        playerSelectKey4.setPosition(570, 695);
        playerSelectKey4.setOutlineThickness(2);
        playerSelectKey4.setOutlineColor(Color(248, 113, 113));

        // Text elements
        playerSelectTitleText.setFont(menuFont);
        playerSelectTitleText.setString("CHOOSE CHARACTER");
        playerSelectTitleText.setCharacterSize(50);
        playerSelectTitleText.setFillColor(Color(255, 255, 255));
        playerSelectTitleText.setPosition(520, 135);

        macroRossiText.setFont(menuFont);
        macroRossiText.setString("1 - Macro Rossi");
        macroRossiText.setCharacterSize(32);
        macroRossiText.setFillColor(Color(255, 255, 255));
        macroRossiText.setPosition(640, 340);

        tarmaRovingText.setFont(menuFont);
        tarmaRovingText.setString("2 - Tarma Roving");
        tarmaRovingText.setCharacterSize(32);
        tarmaRovingText.setFillColor(Color(255, 255, 255));
        tarmaRovingText.setPosition(640, 460);

        eriKasamotoText.setFont(menuFont);
        eriKasamotoText.setString("3 - Eri Kasamoto");
        eriKasamotoText.setCharacterSize(32);
        eriKasamotoText.setFillColor(Color(255, 255, 255));
        eriKasamotoText.setPosition(640, 580);

        fiolinaGermiText.setFont(menuFont);
        fiolinaGermiText.setString("4 - Fiolina Germi");
        fiolinaGermiText.setCharacterSize(32);
        fiolinaGermiText.setFillColor(Color(255, 255, 255));
        fiolinaGermiText.setPosition(640, 700);
    }

    void updateAnimation(float dt) {
        animationTime += dt;
        pulseIntensity = (sin(animationTime * 3) + 1) / 2; // 0 to 1
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
        window.draw(key1);
        window.draw(key2);

        window.draw(titleText);
        window.draw(survivalText);
        window.draw(campaignText);

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
        window.draw(playerSelectKey1);
        window.draw(playerSelectKey2);
        window.draw(playerSelectKey3);
        window.draw(playerSelectKey4);

        window.draw(playerSelectTitleText);
        window.draw(macroRossiText);
        window.draw(tarmaRovingText);
        window.draw(eriKasamotoText);
        window.draw(fiolinaGermiText);

        window.display();
    }
};

#endif
