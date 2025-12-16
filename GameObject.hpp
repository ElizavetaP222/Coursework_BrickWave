#pragma once
#include <SFML/Graphics.hpp>

class GameObject {
protected:
    // координаты позиции объекта на экране
    float pos_x;
    float pos_y;
    // размеры прямоугольного объекта
    float object_width;
    float object_height;
    // радиус круглого объекта
    float object_radius;
    // флаг активности объекта для отрисовки
    bool flag_active;

public:
    GameObject();
    virtual ~GameObject() = default;
    virtual void Deactivate() { flag_active = false; }
    // отрисовка объекта на экране
    virtual void DrawObject(sf::RenderWindow& window) {}
    // обновление состояния объекта с учетом времени
    virtual void UpdateObject(float delta_time) {}
    // возвращает границы объекта для проверки столкновений
    virtual sf::FloatRect GetBoundingBox() const { return {}; }
};
