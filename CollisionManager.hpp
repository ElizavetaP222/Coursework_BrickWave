#pragma once
#include "Ball.hpp"
#include "Paddle.hpp"
#include "Bonus.hpp"

class CollisionManager {
private:
    static const float screen_width = 800.0f;
    static const float screen_height = 600.0f;

public:
    void CheckBallVsPaddle(Ball& ball, Paddle& paddle);
    void CheckBallVsWalls(Ball& ball);
    void CheckBonusVsPaddle(Bonus& bonus, Paddle& paddle);
};
