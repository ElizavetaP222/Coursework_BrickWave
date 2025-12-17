#pragma once
#include "GameObject.hpp"

class Ball : public GameObject {
public:
    // компоненты вектора скорости мяча
    float vector_x;
    float vector_y;
    // базовая скорость для сброса эффектов бонусов
    float base_speed;
    float respawnTimeLeft = 0.0f;
    bool respawnBlink = false;

private:
    bool flag_stuck_to_paddle;

public:
    Ball();
    void SetActive(bool active);
    // обновление позиции мяча
    void MoveBall(float delta_time);
    void BounceBall(std::string direction);
    void ActivateBall();
    void ApplySpeedModifier(float modifier);
    void DrawObject(sf::RenderWindow& window) override;
    virtual void UpdateObject(float delta_time) override;
    sf::FloatRect GetBoundingBox() const override;
    bool IsStuckToPaddle() const;
    void SetPosition(float new_x, float new_y);
    void SetStuckToPaddle(bool stuck);
    void StartRespawn(float time);
    bool IsRespawning() const { return respawnTimeLeft > 0.0f; }
};
