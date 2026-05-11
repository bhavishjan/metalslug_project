#include "DeveloperMode.h"
#include "PlayerSoldier.h"
#include "Weapon.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

DeveloperMode::DeveloperMode() : isActive(false), cheatCodeActivated(false), showIndicator(false), sequenceIndex(0) {
    // Initialize cheat sequence buffer
    for (int i = 0; i < 10; i++) {
        cheatSequence[i] = '\0';
    }
    // Initialize cheat sequence buffer to prevent any garbage values
    for (int i = 0; i < 10; i++) {
        cheatSequence[i] = '\0';
    }
    
    // load font for dev mode, try other font if arial fails
    if (!font.loadFromFile("arial.ttf")) {
        // try alternative fonts if arial is not available
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cout << "Warning: Could not load font for developer mode indicator" << std::endl;
        }
    }
    
    // setup indicator text
    indicatorText.setFont(font);
    indicatorText.setCharacterSize(24u);
    indicatorText.setFillColor(Color::Yellow);
    indicatorText.setString(ACTIVATION_MESSAGE);
    indicatorText.setPosition(10.0f, 10.0f);
}

void DeveloperMode::processKeyInput(Keyboard::Key key) {
    // reset timer on keypress
    cheatCodeTimer.restart();
    
    // add key to sequence
    if (key == Keyboard::D && sequenceIndex < 10) {
        cheatSequence[sequenceIndex++] = 'd';
    } else {
        // reset if wrong key
        sequenceIndex = 0;
        for (int i = 0; i < 10; i++) {
            cheatSequence[i] = '\0';
        }
        return;
    }
    
    // check if code complete
    bool codeComplete = true;
    for (int i = 0; i < 2; i++) {
        if (cheatSequence[i] != CHEAT_CODE[i]) {
            codeComplete = false;
            break;
        }
    }
    
    if (codeComplete && sequenceIndex >= 2) {
        activate();
        sequenceIndex = 0;
        for (int i = 0; i < 10; i++) {
            cheatSequence[i] = '\0';
        }
        showActivationMessage();
        std::cout << "Developer Mode Activated!" << std::endl;
    }
}

void DeveloperMode::activate() {
    if (!isActive) {
        isActive = true;
        cheatCodeActivated = true;
        showActivationMessage();
        std::cout << "Developer Mode Activated!" << std::endl;
    }
}

void DeveloperMode::deactivate() {
    isActive = false;
    cheatCodeActivated = false;
    std::cout << "Developer Mode Deactivated" << std::endl;
}

void DeveloperMode::toggle() {
    if (isActive) {
        deactivate();
    } else {
        activate();
    }
}

void DeveloperMode::applyDeveloperBenefits(PlayerSoldier* player) {
    if (!player || !isActive) return;
    
    // Set player to immortal
    setImmortal(player);
    
    // give all weapons to player
    giveAllWeapons(player);
}

void DeveloperMode::giveAllWeapons(PlayerSoldier* player) {
    if (!player || !isActive) return;
    
    // create and give all weapons
    Pistol* pistol = new Pistol();
    HeavyMachineGun* rifle = new HeavyMachineGun();
    RocketLauncher* rocketLauncher = new RocketLauncher();
    HandGrenade* grenadeLauncher = new HandGrenade();
    FlameShot* flameThrower = new FlameShot();
    LaserGun* laser = new LaserGun();
    
    // add weapons to player inv
    player->setWeapon(pistol);
    // more weapon stuff here
}

void DeveloperMode::setImmortal(PlayerSoldier* player) {
    if (!player || !isActive) return;
    
    // Set player to immortal
    // depends on PlayerSoldier having isImmortal
    // might need to change this based on your PlayerSoldier
    player->setImmortal(true);
}

void DeveloperMode::render(RenderWindow& window) {
    if (isActive) {
        if (showIndicator) {
            // show activation msg
            Font font;
            if (!font.loadFromFile("arial.TTF")) {
                if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
                    return;
                }
            }
            
            Vector2u windowSize = window.getSize();
            float centerX = windowSize.x / 2.0f;
            float centerY = windowSize.y / 2.0f;
            
            // make bg rect
            RectangleShape background;
            background.setSize(Vector2f(500, 80));
            background.setFillColor(Color(0, 0, 0, 180));
            background.setPosition(centerX - 250, centerY - 40);
            window.draw(background);
            
            Text activationText;
            activationText.setFont(font);
            activationText.setString("DEVELOPER MODE ACTIVATED");
            activationText.setCharacterSize(50);
            activationText.setFillColor(Color::Red);
            activationText.setStyle(Text::Bold);
            
            FloatRect textBounds = activationText.getLocalBounds();
            activationText.setPosition(centerX - textBounds.width / 2.0f, centerY - 25.0f);
            window.draw(activationText);
        } else {
            // show dev mode indicator in corner
            indicatorText.setString("DEV MODE");
            indicatorText.setFillColor(Color::Yellow);
            indicatorText.setCharacterSize(20);
            window.draw(indicatorText);
        }
    }
}

void DeveloperMode::displayMessage(const std::string& message) {
    std::cout << message << std::endl;
}

void DeveloperMode::showActivationMessage() {
    showIndicator = true;
    indicatorTimer.restart();
    displayMessage(ACTIVATION_MESSAGE);
}

void DeveloperMode::update(float dt) {
    // update msg timer
    if (messageTimer.getElapsedTime().asSeconds() > MESSAGE_DURATION) {
        // clear msg after time
        messageTimer.restart();
    }
    
    // check cheat code timeout
    if (cheatCodeTimer.getElapsedTime().asSeconds() > CHEAT_CODE_TIMEOUT) {
        sequenceIndex = 0;
        for (int i = 0; i < 10; i++) {
            cheatSequence[i] = '\0';
        }
        cheatCodeTimer.restart();
    }
    
    // hide activation msg after time
    if (showIndicator && isActive && indicatorTimer.getElapsedTime().asSeconds() > INDICATOR_DURATION) {
        showIndicator = false;
    }
}
