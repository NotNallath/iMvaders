#ifndef NUKE_H
#define NUKE_H

#include <SFML/Audio.hpp>

#include "engine.h"

class Nuke: public GameEntity, public Collisionable
{
private:
    sf::Vector2f velocity;
    float size;
    int owner;
public:
    Nuke(sf::Vector2f position, sf::Vector2f velocity, float size, int owner);
    virtual ~Nuke() {}
    
    virtual void update(float delta) override;
    
    virtual void render(sf::RenderTarget& window) override;
    
    virtual void collide(Collisionable* other, float force) override;
    
    void explode();
};

#endif//NUKE_H
