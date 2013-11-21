#ifndef STAR_BACKGROUND_H
#define STAR_BACKGROUND_H

#include <SFML/Graphics.hpp>
#include "Updatable.h"
#include "Renderable.h"

class Star
{
public:
    float x, y;
    float depth;
};

class StarBackground : public Updatable, Renderable
{
public:
    const static int starCount = 256;
    Star stars[starCount];
    StarBackground();
    ~StarBackground();
    virtual void update(float delta);
    virtual void preRender(sf::RenderTarget& window);
    virtual void render(sf::RenderTarget& window) {}
    virtual void postRender(sf::RenderTarget& window) {};
};

#endif//STAR_BACKGROUND_H
