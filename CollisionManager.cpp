#include "CollisionManager.hpp"  
#include <cmath>                
#include <vector>                
#include "Brick.hpp"  

constexpr float CollisionManager::screen_width;
constexpr float CollisionManager::screen_height;

void CollisionManager::CheckBallVsWalls(Ball& ball, GameState& game_state) {
    sf::FloatRect box = ball.GetBoundingBox();

    // Боковые стены
    if (box.left <= 0 || box.left + box.width >= 800) {
        ball.vector_x = -ball.vector_x;
    }

    // Верхняя стена
    if (box.top <= 0) {
        ball.vector_y = -ball.vector_y;
    }

    static bool wasBelowLastFrame = false;  

    if (box.top + box.height > 600 && !ball.IsRespawning()) {
        if (!wasBelowLastFrame) {  
            game_state.LoseLifeGame();
        }
        wasBelowLastFrame = true;
    }
    else {
        wasBelowLastFrame = false;  // Сброс флага
    }
}



void CollisionManager::CheckBallVsPaddle(Ball& ball, Paddle& paddle) {
    sf::FloatRect ballBox = ball.GetBoundingBox();
    sf::FloatRect paddleBox = paddle.GetBoundingBox();

    // Проверка пересечения
    if (ballBox.intersects(paddleBox)) {

        // Центр платформы
        float paddleCenter = paddleBox.left + paddle.GetWidth() / 2.0f;
        float ballCenter = ballBox.left + ballBox.width / 2.0f;
        float hitPosition = (ballCenter - paddleCenter) / (paddle.GetWidth() / 2.0f);
        hitPosition = std::max(-1.0f, std::min(1.0f, hitPosition));  

        // Базовая скорость мяча
        float ballSpeed = sqrt(ball.vector_x * ball.vector_x + ball.vector_y * ball.vector_y);

        float angleMultiplier = hitPosition * 0.7f; 

        // Новая скорость с учетом угла
        ball.vector_x = ballSpeed * angleMultiplier;
        ball.vector_y = -ballSpeed * sqrt(1.0f - angleMultiplier * angleMultiplier);

        // Отталкиваем мяч вверх от платформы
        ball.pos_y = paddleBox.top - ballBox.height;
    }
}
void CollisionManager::CheckBallVsBricks(Ball& ball, std::vector<Brick>& bricks, GameState& game_state) {
    sf::FloatRect ball_box = ball.GetBoundingBox();

    for (int i = bricks.size() - 1; i >= 0; --i) {
        if (!bricks[i].IsActive()) {
            bricks.erase(bricks.begin() + i);  
            continue;
        }

        sf::FloatRect brick_box = bricks[i].GetBoundingBox();
        if (ball_box.intersects(brick_box)) {
            bricks[i].OnHitBrick();

            // Физика отскока (остается как есть)
            float overlap_left = ball_box.left + ball_box.width - brick_box.left;
            float overlap_right = brick_box.left + brick_box.width - ball_box.left;
            float overlap_top = ball_box.top + ball_box.height - brick_box.top;
            float overlap_bottom = brick_box.top + brick_box.height - ball_box.top;

            bool hit_left = overlap_left < overlap_right && overlap_left < overlap_top && overlap_left < overlap_bottom;
            bool hit_right = overlap_right < overlap_left && overlap_right < overlap_top && overlap_right < overlap_bottom;
            bool hit_top = overlap_top < overlap_left && overlap_top < overlap_right && overlap_top < overlap_bottom;
            bool hit_bottom = overlap_bottom < overlap_left && overlap_bottom < overlap_right && overlap_bottom < overlap_top;

            if (hit_left || hit_right) {
                ball.vector_x = -ball.vector_x;
            }
            if (hit_top || hit_bottom) {
                ball.vector_y = -ball.vector_y;
            }

            if (!bricks[i].IsActive()) {
                game_state.AddPointsGame(bricks[i].GetScoreValue());
                bricks.erase(bricks.begin() + i);
            }
        }
    }
}
