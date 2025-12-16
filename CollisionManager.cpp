#include "CollisionManager.hpp"  
#include <cmath>                
#include <vector>                
#include "Brick.hpp"  

constexpr float CollisionManager::screen_width;
constexpr float CollisionManager::screen_height;

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
        // ВОССТАНОВИТЬ мяч только при проигрыше
        ball.SetStuckToPaddle(true);
        ball.SetPosition(400, 500);  // центр экрана
    }

}

void CollisionManager::CheckBallVsPaddle(Ball& ball, Paddle& paddle) {
    if (ball.GetBoundingBox().intersects(paddle.GetBoundingBox())) {

        // ГАРАНТИРОВАННЫЙ ОТСКОК ВВЕРХ
        ball.vector_y = -350.0f;  // ВВЕРХ!
        ball.vector_x = 200.0f;   // ЧУТЬ ВПРАВО

        // АКТИВАЦИЯ только ПРИ ПЕРВОМ касании
        if (ball.IsStuckToPaddle()) {
            ball.ActivateBall();
        }
    }
}



void CollisionManager::CheckBallVsBricks(Ball& ball, std::vector<Brick>& bricks, GameState& game_state) {
    sf::FloatRect ball_box = ball.GetBoundingBox();
    for (auto& brick : bricks) {
        if (!brick.IsActive()) continue;
        if (ball_box.intersects(brick.GetBoundingBox())) {
            brick.OnHitBrick();
            if (!brick.IsActive()) {
                game_state.AddPointsGame(brick.GetScoreValue());
            }
            ball.BounceBall("top");
            break;
        }
    }
}


