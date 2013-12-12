#include <math.h>

#include "player.h"
#include "textureManager.h"
#include "bullet.h"
#include "explosion.h"

PlayerController playerController[2];

PlayerCraft::PlayerCraft(PlayerController* controller, int type)
: GameEntity(10.0f), controller(controller), type(type)
{
    invulnerability = 1.0;
    fireCooldown = 0.4;
    if (type == 0)
        textureManager.setTexture(sprite, "player1");
    else
        textureManager.setTexture(sprite, "player2");
    sprite.setPosition(sf::Vector2f(160, 220));
}

PlayerCraft::~PlayerCraft()
{
}

void PlayerCraft::update(float delta)
{
    if (fireCooldown > 0)
        fireCooldown -= delta;
    if (invulnerability > 0)
        invulnerability -= delta;

    velocity = velocity * 0.85f;//TODO: Proper dampening with use of delta.
    if (controller->left())
        velocity = sf::Vector2f(-100.0, velocity.y);
    if (controller->right())
        velocity = sf::Vector2f( 100.0, velocity.y);
    if (controller->up())
        velocity = sf::Vector2f(velocity.x, -100);
    if (controller->down())
        velocity = sf::Vector2f(velocity.x,  100);
    
    sprite.setPosition(sprite.getPosition() + velocity * delta);

    if (sprite.getPosition().x < 20)
        sprite.setPosition(20, sprite.getPosition().y);
    if (sprite.getPosition().x > 300)
        sprite.setPosition(300, sprite.getPosition().y);
    if (sprite.getPosition().y < 10)
        sprite.setPosition(sprite.getPosition().x, 10);
    if (sprite.getPosition().y > 230)
        sprite.setPosition(sprite.getPosition().x, 230);

    if (controller->button(fireButton) && fireCooldown <= 0 && invulnerability <= 0)
    {
        if (type == 0)
        {
            new Bullet(sprite.getPosition(), -1, 0);
            fireCooldown = 0.4;
        }
        if (type == 1)
        {
            new Bullet(sprite.getPosition() + sf::Vector2f(7, 0), -2, 0);
            new Bullet(sprite.getPosition() + sf::Vector2f(-4, 0), -2, 0);
            fireCooldown = 0.8;
        }
    }
    if (!controller->button(fireButton))
    {
        if (type == 0 && fireCooldown > 0.1)
            fireCooldown = 0.1;
        if (type == 1 && fireCooldown > 0.2)
            fireCooldown = 0.2;
    }
}

void PlayerCraft::render(sf::RenderTarget& window)
{
    if (fmod(invulnerability, 4.0/60.0) > 2.0/60.0)
        return;
    sprite.setRotation(velocity.x / 10.0);
    window.draw(sprite);
}

bool PlayerCraft::takeDamage(sf::Vector2f position, int damageType, int damageAmount)
{
    if (damageType < 0 || invulnerability > 0)
        return false;
    destroy();
    new Explosion(sprite.getPosition(), 12);
    return true;
}
