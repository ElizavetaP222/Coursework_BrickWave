#pragma once
#include <SFML/Graphics.hpp>

class GameObject {
protected:
    float x, y;
    float width, height;
    float radius;
    bool flagActive;

public:
    GameObject();
    virtual ~GameObject() = default;

    virtual void Draw(sf::RenderWindow& window) {}
    virtual void Update(float deltaTime) {}
    virtual sf::FloatRect GetBoundingBox() const { return {}; }
};
