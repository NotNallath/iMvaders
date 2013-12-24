#ifndef PLAYER_H
#define PLAYER_H

#define MAX_PLAYERS 2

#include "gameEntity.h"
#include "Collisionable.h"
#include "random.h"

const int fireButton = 0;
const int nukeButton = 1;

class PlayerController: public virtual PObject
{
public:
    static const int buttonCount = 6;
    sf::Keyboard::Key keyBind[4 + buttonCount];

    PlayerController()
    {
        keyBind[0] = sf::Keyboard::Left;
        keyBind[1] = sf::Keyboard::Right;
        keyBind[2] = sf::Keyboard::Up;
        keyBind[3] = sf::Keyboard::Down;
        keyBind[4] = sf::Keyboard::Space;
        keyBind[5] = sf::Keyboard::Z;
        keyBind[6] = sf::Keyboard::X;
        keyBind[7] = sf::Keyboard::C;
        keyBind[8] = sf::Keyboard::V;
        keyBind[9] = sf::Keyboard::B;
    }

    bool left() { return sf::Keyboard::isKeyPressed(keyBind[0]); }
    bool right() { return sf::Keyboard::isKeyPressed(keyBind[1]); }
    bool up() { return sf::Keyboard::isKeyPressed(keyBind[2]); }
    bool down() { return sf::Keyboard::isKeyPressed(keyBind[3]); }
    bool button(int idx) { return sf::Keyboard::isKeyPressed(keyBind[4 + idx]); }
};

class PlayerCraft: public GameEntity, public Collisionable
{
public:
    sf::Sprite nukeIcon;

    PlayerController* controller;
    sf::Vector2f velocity;
    int type;
    float fireCooldown;
    float invulnerability;
    int nukeCount;
    int health;
public:
    PlayerCraft(PlayerController* controller, int type);

    virtual ~PlayerCraft();

    virtual void update(float delta);

    virtual void render(sf::RenderTarget& window);

    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount);
};

#endif//PLAYER_H
