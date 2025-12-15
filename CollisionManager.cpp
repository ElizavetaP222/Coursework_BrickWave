#include "CollisionManager.hpp"
#include "GameState.hpp"

const float CollisionManager::screen_width = 800.0f;
const float CollisionManager::screen_height = 600.0f;

void CollisionManager::CheckBallVsPaddle(Ball& ball, Paddle& paddle) {
    if (ball.GetBoundingBox().intersects(paddle.GetBoundingBox())) {  // полиморфный вызов
        float hit_pos = (ball.pos_x - paddle.pos_x) / paddle.object_width;
        ball.vector_x = (hit_pos - 0.5f) * 400.0f;
        ball.vector_y = -abs(ball.vector_y);

        if (ball.flag_stuck_to_paddle) {
            ball.ActivateBall();
        }
    }
}

void CollisionManager::CheckBallVsWalls(Ball& ball, GameState& game_state) {
    if (ball.pos_x <= 0 || ball.pos_x + ball.object_radius * 2 >= screen_width) {
        ball.BounceBall("left");
    }

    if (ball.pos_y <= 0) {
        ball.BounceBall("top");
    }

    if (ball.pos_y >= screen_height) {
        game_state.LoseLifeGame();
        // возврат м€ча к платформе
        ball.flag_stuck_to_paddle = true;
        ball.pos_x = 400;
        ball.pos_y = 500;
        ball.vector_x = 0;
        ball.vector_y = 0;
    }
}

void CollisionManager::CheckBonusVsPaddle(Bonus& bonus, Paddle& paddle) {
    if (bonus.GetBoundingBox().intersects(paddle.GetBoundingBox())) {  // полиморфный вызов
        bonus.ApplyEffectBonus(paddle, Ball(), GameState());
    }
}
