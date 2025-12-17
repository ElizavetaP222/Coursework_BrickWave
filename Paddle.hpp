#pragma once
#include "GameObject.hpp"
#include <SFML/Window/Keyboard.hpp>

class Paddle : public GameObject {
private:
    // скорость перемещения платформы
    float paddle_speed;
    // направление движения -1 влево, 0 стоп, 1 вправо
    int move_direction;
    // исходная ширина платформы для сброса эффектов бонусов
    float base_width_paddle;

public:
    Paddle();
    void SetPosition(float x, float y);
    float GetWidth() const { return object_width; }
    // обработка ввода пользователя клавиши A D или стрелки
    void HandleInputPaddle();
    // применение эффекта бонуса изменение ширины
    void ApplyBonusPaddle();
    // сброс размера платформы к исходному
    void ResetSizePaddle();
    void DrawObject(sf::RenderWindow& window) override;
    void UpdateObject(float delta_time) override;
    sf::FloatRect GetBoundingBox() const override;
};
