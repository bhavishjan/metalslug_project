#pragma once

#include <SFML/Graphics.hpp>
using namespace sf;

// Abstract entity class
class Entity {
protected:
    float x, y;
    float width, height;
    bool  isActive;

public:
    Entity() : x(0), y(0), width(32), height(48), isActive(true) {}
    virtual ~Entity() {}

    virtual void update(float dt) = 0;
    virtual void render(RenderWindow& window, float camX, float camY) = 0;

    float getX() const { return x; }
    float getY() const { return y; }
    bool  getIsActive() const { return isActive; }
    void  setPosition(float px, float py) { x = px; y = py; }
};

class DamagableEntity : public Entity {
protected:
    int   hp;
    int   maxHp;
    int   damage;
    bool  isAlive;

public:
    DamagableEntity() : hp(1), maxHp(1), damage(0), isAlive(true) {}
    virtual ~DamagableEntity() {}

    virtual void takeDamage(int dmg) {
        hp -= dmg;
        if (hp <= 0) {
            die();
        }
    }
    virtual void die() { isAlive = false; isActive = false; }

    bool getIsAlive() const { return isAlive; }
    int  getHP()      const { return hp; }
    int  getMaxHP()   const { return maxHp; }
};

// Soldier between DamagableEntity and PlayerSoldier
class Soldier : public DamagableEntity {
protected:
    bool  facingRight;
    float velocityX, velocityY;

public:
    Soldier() : facingRight(true), velocityX(0), velocityY(0) {}
    virtual ~Soldier() {}

    virtual void move(float dt) = 0;
    virtual void attack() = 0;
};
