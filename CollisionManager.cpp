#include "CollisionManager.hpp"
#include <cmath>

constexpr float CollisionManager::screen_width;
constexpr float CollisionManager::screen_height;

void CollisionManager::CheckBallVsPaddle(Ball& ball, Paddle& paddle) {
    if (ball.GetBoundingBox().intersects(paddle.GetBoundingBox())) {
        sf::FloatRect ball_box = ball.GetBoundingBox();
        sf::FloatRect paddle_box = paddle.GetBoundingBox();
        float hit_pos = (ball_box.left - paddle_box.left) / paddle_box.width;
        ball.vector_x = (hit_pos - 0.5f) * 400.0f;
        ball.vector_y = -std::abs(ball.vector_y);

        if (ball.IsStuckToPaddle()) {
            ball.ActivateBall();
        }
    }
}

void CollisionManager::CheckBallVsWalls(Ball& ball, GameState& game_state) {
    sf::FloatRect ball_box = ball.GetBoundingBox();

    // боковые стены
    if (ball_box.left <= 0 || ball_box.left + ball_box.width >= screen_width) {
        ball.BounceBall("left");
    }

    // верхняя стена
    if (ball_box.top <= 0) {
        ball.BounceBall("top");
    }

    // нижняя граница - потеря жизни
    if (ball_box.top + ball_box.height >= screen_height) {
        game_state.LoseLifeGame();
        ball.SetStuckToPaddle(true);
    }
}

void CollisionManager::CheckBonusVsPaddle(Bonus& bonus, Paddle& paddle) {
    if (bonus.GetBoundingBox().intersects(paddle.GetBoundingBox())) {
        bonus.Deactivate();  // публичный метод
    }
}
