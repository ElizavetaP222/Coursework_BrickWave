#pragma once
#include "GameObject.hpp"

class Ball : public GameObject {
public:
    // компоненты вектора скорости м€ча
    float vector_x;
    float vector_y;
    // базова€ скорость дл€ сброса эффектов бонусов
    float base_speed;

private:  
    bool flag_stuck_to_paddle;  

public:
    Ball();

    // обновление позиции м€ча
    void MoveBall(float delta_time);
    void BounceBall(std::string direction);
    void ActivateBall();
    void ApplySpeedModifier(float modifier);

    void DrawObject(sf::RenderWindow& window) override;
    void UpdateObject(float delta_time) override;
    sf::FloatRect GetBoundingBox() const override;

    bool IsStuckToPaddle() const;
    void SetPosition(float new_x, float new_y);
    void SetStuckToPaddle(bool stuck);
};
