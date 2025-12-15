#pragma once
#include "GameObject.hpp"

class Paddle : public GameObject {
private:
    float paddle_speed;
    int move_direction; // -1 left, 0 none, 1 right
    float base_width_paddle;

public:
    Paddle();

    void HandleInputPaddle();
    void ApplyBonusPaddle();
    void ResetSizePaddle();
};
