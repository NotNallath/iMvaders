#include <math.h>

#include "transmission.h"
#include "textDraw.h"
#include "textureManager.h"

Transmission::Transmission()
{
    faceName = "Jaime1";
    text = "WARNING INCOMMING|MAKERBOTS";
    transmissionTime = 0;
}

void Transmission::postRender(sf::RenderTarget& window)
{
    sf::RectangleShape transmissionShape(sf::Vector2f(280, 62));
    transmissionShape.setPosition(20, 160);
    transmissionShape.setFillColor(sf::Color::Black);
    transmissionShape.setOutlineColor(sf::Color::White);
    transmissionShape.setOutlineThickness(1);
    window.draw(transmissionShape);
    face.setPosition(21+30, 161+30);
    window.draw(face);
    
    drawText(window, 23 + 60, 161, text.substr(0, int(transmissionTime * charsPerSecond)), align_left);
}

void Transmission::update(float delta)
{
    transmissionTime += delta;
    if (transmissionTime * charsPerSecond < text.length())
        textureManager.setTexture(face, faceName.c_str(), fmodf(transmissionTime * charsPerSecond, 2.0f) < 1.0f ? 1 : 0);
    else
        textureManager.setTexture(face, faceName.c_str(), 0);
}
