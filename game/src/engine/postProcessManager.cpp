#include <stdio.h>

#include "postProcessManager.h"

static int powerOfTwo(int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

PostProcessor::PostProcessor(string name, RenderChain* chain)
: chain(chain)
{
    if (shader.loadFromFile("resources/" + name + ".frag", sf::Shader::Fragment))
    {
        printf(("Loaded shader: " + name + "\n").c_str());
    }else{
        printf(("Failed to load shader: " + name + "\n").c_str());
    }
}

void PostProcessor::render(sf::RenderTarget& window)
{
    sf::Vector2i virtualSize(320, 240);
    if (renderTexture.getSize().x < 1)
    {
        //Setup a backBuffer to render the game on. Then we can render the backbuffer back to the main screen with full-screen shader effects
        int w = window.getView().getViewport().width * window.getSize().x;
        int h = window.getView().getViewport().height * window.getSize().y;
        int tw = powerOfTwo(w);
        int th = powerOfTwo(h);
        sf::View view(sf::Vector2f(virtualSize.x/2,virtualSize.y/2), sf::Vector2f(virtualSize));
        view.setViewport(sf::FloatRect(0, 1.0 - float(h) / float(th), float(w) / float(tw), float(h) / float(th)));

        renderTexture.create(tw, th, false);
        renderTexture.setRepeated(true);
        renderTexture.setSmooth(true);
        renderTexture.setView(view);
    }
    
    renderTexture.clear();
    chain->render(renderTexture);
    
    renderTexture.display();
    sf::Sprite backBufferSprite(renderTexture.getTexture(), sf::IntRect(0, renderTexture.getSize().y - window.getView().getViewport().height * window.getSize().y, window.getView().getViewport().width * window.getSize().x, window.getView().getViewport().height * window.getSize().y));
    backBufferSprite.setScale(virtualSize.x/float(renderTexture.getSize().x)/renderTexture.getView().getViewport().width, virtualSize.y/float(renderTexture.getSize().y)/renderTexture.getView().getViewport().height);
    
    shader.setParameter("inputSize", sf::Vector2f(window.getSize().x, window.getSize().y));
    shader.setParameter("textureSize", sf::Vector2f(renderTexture.getSize().x, renderTexture.getSize().y));
    
    window.draw(backBufferSprite, &shader);
}
