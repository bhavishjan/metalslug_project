#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "AllHeaders.h"

using namespace sf;
using namespace std;

int screen_x = 1600;
int screen_y = 900;

int main() {
    RenderWindow window(VideoMode(screen_x, screen_y), "Metal Slug", Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    float cameraX = 0, cameraY = 0;

    // -------------------------
    // Level 1 — sirf ek line!
    // -------------------------
    Level1* level = new Level1();
    level->generateBiomes();

    // -------------------------
    // Player
    // -------------------------
    float player_x = level->getPlayerSpawnX();
    float player_y = level->getPlayerSpawnY();
    float velocityX = 0;
    float velocityY = 0;
    float gravity = 0.8f;
    float moveSpeed = 5.0f;
    bool  onGround = false;
    bool  facingRight = true;

    float scale_x = 0.5f;
    float scale_y = 0.5f;
    int Pwidth = (int)(115 * scale_x);
    int Pheight = (int)(158 * scale_y);

    Texture playerTexLeft, playerTexRight;
    Sprite  playerSprite;
    playerTexLeft.loadFromFile("Sprites/character_facing_right.png");
    playerTexRight.loadFromFile("Sprites/character_facing_left.png");
    playerSprite.setTexture(playerTexRight);
    playerSprite.setScale(scale_x, scale_y);

    Clock clock;
    Event ev;

    // -------------------------
    // GAME LOOP
    // -------------------------
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // Events
        while (window.pollEvent(ev))
            if (ev.type == Event::Closed)
                window.close();
        if (Keyboard::isKeyPressed(Keyboard::Escape))
            window.close();

        // -------------------------
        // Movement
        // -------------------------
        velocityX = 0;

        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            velocityX = -moveSpeed;
            if (facingRight) {
                playerSprite.setTexture(playerTexLeft);
                facingRight = false;
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Right)) {
            velocityX = moveSpeed;
            if (!facingRight) {
                playerSprite.setTexture(playerTexRight);
                facingRight = true;
            }
        }

        // Jump
        if (Keyboard::isKeyPressed(Keyboard::Up) && onGround) {
            velocityY = -15.0f;
            onGround = false;
        }

        // Gravity
        velocityY += gravity;
        onGround = false;

        // -------------------------
        // X move + resolve
        // -------------------------
        player_x += velocityX;
        level->resolveCollisions(player_x, player_y,
            Pwidth, Pheight,
            velocityX, velocityY,
            onGround);

        // -------------------------
        // Y move + resolve
        // -------------------------
        player_y += velocityY;
        level->resolveCollisions(player_x, player_y,
            Pwidth, Pheight,
            velocityX, velocityY,
            onGround);

        // -------------------------
        // Bounds
        // -------------------------
        if (player_x < 0)
            player_x = 0;
        if (player_x + Pwidth > level->getLevelEnd())
            player_x = level->getLevelEnd() - Pwidth;

        // -------------------------
        // Level Complete Check
        // -------------------------
        if (level->checkLevelComplete()) {
            // Baad mein — Level 2 load karein
            // Abhi sirf window band karo
            window.close();
        }

        // -------------------------
        // Camera LERP
        // -------------------------
        float targetCamX = player_x - screen_x / 2.0f;
        float targetCamY = player_y - screen_y / 2.0f + 100;
        cameraX += (targetCamX - cameraX) * 0.12f;

        if (cameraX < 0) cameraX = 0;
        if (cameraX > level->getLevelEnd() - screen_x)
            cameraX = level->getLevelEnd() - screen_x;
        cameraY = 0;//y fix

        // -------------------------
        // Update
        // -------------------------
        level->update(dt);

        // -------------------------
        // Render
        // -------------------------
        window.clear(Color(135, 206, 235));
        level->render(window, cameraX, cameraY);

        // Player draw
        playerSprite.setPosition(player_x - cameraX,
            player_y - cameraY);
        window.draw(playerSprite);

        window.display();
    }

    // -------------------------
    // Cleanup
    // -------------------------
    delete level;
    return 0;
}