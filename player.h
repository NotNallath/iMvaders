
class PlayerController: public sf::NonCopyable
{
public:
    PlayerController()
    {
    }
    
    bool left() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Left); }
    bool right() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Right); }
    bool up() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Up); }
    bool down() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Down); }
    bool fire() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Space); }
};

class PlayerCraft: public GameEntity
{
private:
    PlayerController* controller;
    int fireCooldown;
public:
    sf::Sprite sprite;
    
    PlayerCraft(PlayerController* controller)
    : controller(controller)
    {
        fireCooldown = 20;
        sprite.setTexture(playerTexture);
        sprite.setOrigin(playerTexture.getSize().x/2, playerTexture.getSize().y/2);
        sprite.setPosition(sf::Vector2f(160, 220));
    }
    
    virtual ~PlayerCraft()
    {
    }
    
    virtual void update()
    {
        if (fireCooldown)
            fireCooldown--;
        
        if (controller->left())
            sprite.setPosition(sprite.getPosition().x - 2.0f, sprite.getPosition().y);
        if (controller->right())
            sprite.setPosition(sprite.getPosition().x + 2.0f, sprite.getPosition().y);
        if (controller->up())
            sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y - 2.0f);
        if (controller->down())
            sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y + 2.0f);
        
        if (sprite.getPosition().x < 20)
            sprite.setPosition(20, sprite.getPosition().y);
        if (sprite.getPosition().x > 300)
            sprite.setPosition(300, sprite.getPosition().y);
        if (sprite.getPosition().y < 10)
            sprite.setPosition(sprite.getPosition().x, 10);
        if (sprite.getPosition().y > 230)
            sprite.setPosition(sprite.getPosition().x, 230);
        
        if (controller->fire() && !fireCooldown)
        {
            new Bullet(sprite.getPosition(), 2, 0);
            fireCooldown = 20;
        }
    }
    
    virtual void render(sf::RenderTarget& window)
    {
        window.draw(sprite);
    }
    
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount)
    {
        if (damageType >= 2)
            return false;
        if ((position - sprite.getPosition()) > 10.0f)
            return false;
        destroy();
        return true;
    }
};
