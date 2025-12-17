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

    // НИЖНЯЯ ГРАНИЦА - потеря жизни ТОЛЬКО при первом касании
    static bool wasBelowLastFrame = false;  // СТАТИЧЕСКАЯ ПЕРЕМЕННАЯ!

    if (box.top + box.height > 600 && !ball.IsRespawning()) {
        if (!wasBelowLastFrame) {  // ПЕРВЫЙ КАДР ниже экрана
            game_state.LoseLifeGame();
        }
        wasBelowLastFrame = true;
    }
    else {
        wasBelowLastFrame = false;  // Сброс флага
    }
}



void CollisionManager::CheckBallVsPaddle(Ball& ball, Paddle& paddle) {
    sf::FloatRect ball_box = ball.GetBoundingBox();
    sf::FloatRect paddle_box = paddle.GetBoundingBox();

    if (ball_box.intersects(paddle_box)) {
        // Нормализуем скорость перед отскоком
        float current_speed = sqrt(ball.vector_x * ball.vector_x + ball.vector_y * ball.vector_y);
        if (current_speed == 0) return;

        // Вычисляем точную точку удара (0.0 = левый край, 1.0 = правый край)
        float hit_position = (ball_box.top + ball_box.height / 2 - paddle_box.top) / paddle_box.height;
        hit_position = std::max(0.0f, std::min(1.0f, hit_position));

        // Закон геометрической оптики: угол падения = угол отражения
        float reflect_angle;

        if (hit_position < 0.33f) {
            // Левый край: 135° (45° от вертикали)
            reflect_angle = 135.0f * 3.14159f / 180.0f;
        }
        else if (hit_position > 0.67f) {
            // Правый край: 45° (45° от вертикали)
            reflect_angle = 45.0f * 3.14159f / 180.0f;
        }
        else {
            // Центр: 90° (вертикально вверх)
            reflect_angle = 90.0f * 3.14159f / 180.0f;
        }

        // Применяем отражённую скорость
        ball.vector_x = current_speed * cos(reflect_angle);
        ball.vector_y = -current_speed * sin(reflect_angle);  // Отрицательное = вверх

        // Дополнительный эффект: края дают больше скорости
        float speed_multiplier = 1.0f + (0.5f * fabs(hit_position - 0.5f));
        ball.vector_x *= speed_multiplier;
        ball.vector_y *= speed_multiplier;
    }
}
void CollisionManager::CheckBallVsBricks(Ball& ball, std::vector<Brick>& bricks, GameState& game_state) {
    sf::FloatRect ball_box = ball.GetBoundingBox();

    for (auto& brick : bricks) {
        if (!brick.IsActive()) continue;

        sf::FloatRect brick_box = brick.GetBoundingBox();
        if (ball_box.intersects(brick_box)) {
            brick.OnHitBrick();

            // Физика отскока от кирпича
            float overlap_left = ball_box.left + ball_box.width - brick_box.left;
            float overlap_right = brick_box.left + brick_box.width - ball_box.left;
            float overlap_top = ball_box.top + ball_box.height - brick_box.top;
            float overlap_bottom = brick_box.top + brick_box.height - ball_box.top;

            // Определяем сторону столкновения
            bool hit_left = overlap_left < overlap_right && overlap_left < overlap_top && overlap_left < overlap_bottom;
            bool hit_right = overlap_right < overlap_left && overlap_right < overlap_top && overlap_right < overlap_bottom;
            bool hit_top = overlap_top < overlap_left && overlap_top < overlap_right && overlap_top < overlap_bottom;
            bool hit_bottom = overlap_bottom < overlap_left && overlap_bottom < overlap_right && overlap_bottom < overlap_top;

            // Отражение по нормали
            if (hit_left || hit_right) {
                ball.vector_x = -ball.vector_x;
            }
            if (hit_top || hit_bottom) {
                ball.vector_y = -ball.vector_y;
            }

            // Очки при разрушении
            if (!brick.IsActive()) {
                game_state.AddPointsGame(brick.GetScoreValue());
            }
            break;
        }
    }
}



