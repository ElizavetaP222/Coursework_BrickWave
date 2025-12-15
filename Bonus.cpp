#include "Bonus.hpp"

Bonus::Bonus(BonusType type, float start_x, float start_y) {
    bonus_type = type;
    pos_x = start_x;
    pos_y = start_y;
    object_width = 30.0f;
    object_height = 30.0f;
    fall_speed = 200.0f;
    flag_active = true;
}

void Bonus::ApplyEffectBonus(Paddle& paddle, Ball& ball, GameState& game_state) {
    switch (bonus_type) {
    case enlarge_paddle:
        paddle.ApplyBonusPaddle();
        break;
    case extra_life:
        game_state.GainLifeGame();
        break;
    case speed_up_ball:
        ball.ApplySpeedModifier(1.5f);
        break;
    }
    flag_active = false;
}

void Bonus::DrawObject(sf::RenderWindow& window) {
    if (!flag_active) return;

    sf::RectangleShape bonus_shape(sf::Vector2f(object_width, object_height));
    bonus_shape.setPosition(pos_x, pos_y);
    bonus_shape.setFillColor(sf::Color::Magenta);
    window.draw(bonus_shape);
}

void Bonus::UpdateObject(float delta_time) {
    pos_y += fall_speed * delta_time;
    if (pos_y > 600) flag_active = false;
}

sf::FloatRect Bonus::GetBoundingBox() const {
    return sf::FloatRect(pos_x, pos_y, object_width, object_height);
}
