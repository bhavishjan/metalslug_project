#define _CRT_SECURE_NO_WARNINGS
#include "PlayerSoldier.h"

void UndeadState::applyEffects(PlayerSoldier* s) {
    s->setVelocity(s->getVelocityX() * 0.7f, s->getVelocityY());
}

void MummyState::applyEffects(PlayerSoldier* s) {
    s->setVelocity(s->getVelocityX() * 0.5f, s->getVelocityY());
    s->pickupWeapon(nullptr);
}