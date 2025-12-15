#pragma once
#include "GameObject.hpp"

class Ball : public GameObject {
private:
    // компоненты вектора скорости м€ча
    float vector_x;
    float vector_y;
    // базова€ скорость дл€ сброса эффектов бонусов
    float base_speed;
    // флаг прилипани€ м€ча к платформе
    bool flag_stuck_to_paddle;

public:
    Ball();

    // обновление позиции м€ча
    void MoveBall();
    // обработка отскока от объекта
    void BounceBall(std::string direction);
    // отталкивание м€ча от платформы
    void ActivateBall();
    // применение модификатора скорости
    void ApplySpeedModifier(float modifier);
    void DrawObject(sf::RenderWindow& window) override;
    void UpdateObject(float delta_time) override;
    sf::FloatRect GetBoundingBox() const override;

};
