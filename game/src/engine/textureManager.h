#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SFML/Graphics.hpp>

#include <map>
#include <vector>
#include "stringImproved.h"

class TextureManager;
extern TextureManager textureManager;
struct TextureData
{
    sf::Texture texture;
    std::vector<sf::IntRect> sprites;
};
class TextureManager
{
private:
    std::map<string, TextureData> textureMap;
public:
    TextureManager();
    ~TextureManager();
    
    void setTexture(sf::Sprite& sprite, string name, unsigned int spriteIndex = 0);
    const sf::IntRect getSpriteRect(string name, unsigned int spriteIndex = 0);
    void setSpriteRect(string name, unsigned int spriteIndex, const sf::IntRect rect);

    sf::Texture* getTexture(string name, sf::Vector2i subDiv = sf::Vector2i(0, 0));
private:
    void loadTexture(string name, sf::Vector2i subDiv);
};

#endif//TEXTURE_MANAGER_H
