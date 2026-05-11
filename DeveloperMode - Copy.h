#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class DeveloperMode {
private:
    bool isActive;
    bool cheatCodeActivated;
    char cheatSequence[10] = {}; // buffer for cheat code
    int sequenceIndex = 0;
    Clock cheatCodeTimer;
    Clock messageTimer;
    const float CHEAT_CODE_TIMEOUT = 2.0f;
    const float MESSAGE_DURATION = 3.0f;
    static constexpr char CHEAT_CODE[] = "dd"; // cheat code
    static constexpr char ACTIVATION_MESSAGE[] = "DEVELOPER MODE ACTIVATED";
    static constexpr char DEACTIVATION_MESSAGE[] = "DEVELOPER MODE DEACTIVATED";
    
    // visual indicator
    Font font;
    Text indicatorText;
    bool showIndicator;
    Clock indicatorTimer;
    const float INDICATOR_DURATION = 3.0f;

public:
    DeveloperMode();
    
    // msg display system
    void displayMessage(const std::string& message);
    void update(float dt);
    
    // cheat code detection
    void processKeyInput(Keyboard::Key key);
    bool isCheatCodeActivated() const { return cheatCodeActivated; }
    
    // dev mode features
    void activate();
    void deactivate();
    void toggle();
    bool isDeveloperModeActive() const { return isActive; }
    
    // apply dev benefits to player
    void applyDeveloperBenefits(class PlayerSoldier* player);
    void giveAllWeapons(class PlayerSoldier* player);
    void setImmortal(class PlayerSoldier* player);
    
    // visual indicator
    void render(RenderWindow& window);
    void showActivationMessage();
};
