#pragma once
#include "GameObject.hpp"

class Paddle : public GameObject {
private:
    float speed;
    int moveDirection; // -1 left, 0 none, 1 right
    float baseWidth;

public:
    Paddle();

    void HandleInput();
    void ApplyBonus();
    void ResetSize();
};
