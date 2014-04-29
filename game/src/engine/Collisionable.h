#ifndef COLLISIONABLE_H
#define COLLISIONABLE_H

#include "P.h"
#include "Box2D/Box2D.h"

enum ECollisionType
{
    CT_Circle,
    CT_Line
};

class Collisionable;
class CollisionManager
{
public:
    static void initialize();
    static void handleCollisions(float delta);
private:
    static b2World* world;
    
    friend class Collisionable;
    friend class CollisionDebugDraw;
};

class Collisionable: public virtual PObject
{
private:
    float rotation;
    b2Body* body;
    bool enablePhysics;
    bool staticPhysics;
    
    void createBody(b2Shape* shape);
public:
    Collisionable(float radius);
    Collisionable(sf::Vector2f boxSize, sf::Vector2f boxOrigin = sf::Vector2f(0, 0));
    Collisionable(const std::vector<sf::Vector2f>& shape);
    virtual ~Collisionable();
    virtual void collision(Collisionable* target);
    
    void setCollisionRadius(float radius);
    void setCollisionBox(sf::Vector2f boxSize, sf::Vector2f boxOrigin = sf::Vector2f(0, 0));
    void setCollisionShape(const std::vector<sf::Vector2f>& shape);
    void setCollisionPhysics(bool enablePhysics, bool staticPhysics);
    
    void setPosition(sf::Vector2f v);
    sf::Vector2f getPosition();
    void setRotation(float angle);
    float getRotation();
    void setVelocity(sf::Vector2f velocity);
    sf::Vector2f getVelocity();
    
    sf::Vector2f toLocalSpace(sf::Vector2f v);
    sf::Vector2f toWorldSpace(sf::Vector2f v);
    
    friend class CollisionManager;
};

#ifdef DEBUG
#include "Renderable.h"

class CollisionDebugDraw : public Renderable, public b2Draw
{
    sf::RenderTarget* renderTarget;
public:
    CollisionDebugDraw(RenderLayer* layer);
    
    virtual void render(sf::RenderTarget& window);

	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	virtual void DrawTransform(const b2Transform& xf);
};
#endif//DEBUG

#endif // COLLISIONABLE_H
