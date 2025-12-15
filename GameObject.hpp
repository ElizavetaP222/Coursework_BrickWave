#pragma once
#include <SFML/Graphics.hpp>

class GameObject {
protected:
    float pos_x;
    float pos_y;
    float object_width;
    float object_height;
    float object_radius;
    bool flag_active;

public:
    GameObject();
    virtual ~GameObject() = default;

    virtual void DrawObject(sf::RenderWindow& window) {}
    virtual void UpdateObject(float delta_time) {}
    virtual sf::FloatRect GetBoundingBox() const { return {}; }
};
