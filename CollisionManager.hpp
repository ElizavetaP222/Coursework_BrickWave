#pragma once
#include <vector>           
#include "Ball.hpp"
#include "Paddle.hpp"
#include "Brick.hpp"       
#include "Bonus.hpp"
#include "GameState.hpp"

class CollisionManager {
private:
    static constexpr float screen_width = 800.0f;
    static constexpr float screen_height = 600.0f;

public:
    void CheckBallVsPaddle(Ball& ball, Paddle& paddle);
    void CheckBallVsWalls(Ball& ball, GameState& game_state);
    void CheckBonusVsPaddle(Bonus& bonus, Paddle& paddle);
    void CheckBallVsBricks(Ball& ball, std::vector<Brick>& bricks, GameState& game_state);
};