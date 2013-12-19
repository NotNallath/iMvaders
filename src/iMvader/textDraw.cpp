#include "textDraw.h"
#include "textureManager.h"

static int textWidth(sf::String str)
{
    int ret = 0;
    for(unsigned int i=0; i<str.getSize(); i++)
    {
        int c = str[i];
        if (c >= 'A' && c <= 'Z')
            ret += textureManager.getSpriteRect("abc", c-'A').width;
        else if (c >= '0' && c <= '9')
            ret += textureManager.getSpriteRect("nums", c-'0').width;
        else
            ret += 8;
    }
    return ret;
}

void drawText(sf::RenderTarget& window, int x, int y, sf::String str, textAlign align)
{
    sf::Sprite letter;
    if (align == align_center)
        x -= textWidth(str)/2;
    if (align == align_right)
        x -= textWidth(str);
    int xStart = x;
    for(unsigned int i=0; i<str.getSize(); i++)
    {
        int c = str[i];
        if (c >= 'A' && c <= 'Z')
        {
            c -= 'A';
            textureManager.setTexture(letter, "abc", c);
            letter.setOrigin(0, 0);
            letter.setPosition(x, y);
            x += letter.getTextureRect().width + 1;
            window.draw(letter);
        }
        else if (c >= '0' && c <= '9')
        {
            textureManager.setTexture(letter, "nums", c-'0');
            letter.setOrigin(0, 0);
            letter.setPosition(x, y);
            x += letter.getTextureRect().width + 1;
            window.draw(letter);
        }
        else if (c == '|')
        {
            x = xStart;
            y += 15;
        }
        else
        {
            x += 8;
        }
    }
}
