#pragma once
#include "GameObject.hpp"

class Ball : public GameObject {
private:
    float vector_x;
    float vector_y;
    float base_speed;
    bool flag_stuck_to_paddle;

public:
    Ball();

    void MoveBall();
    void BounceBall(std::string direction);
    void ActivateBall();
    void ApplySpeedModifier(float modifier);
};
