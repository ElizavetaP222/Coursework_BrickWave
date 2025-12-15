#pragma once
#include "GameObject.hpp"
#include "Paddle.hpp"
#include "Ball.hpp"
#include "GameState.hpp"

enum BonusType { enlarge_paddle, shrink_paddle, speed_up_ball, slow_down_ball, extra_life };

class Bonus : public GameObject {
private:
    BonusType bonus_type;
    float fall_speed;
    sf::Texture texture_bonus;

public:
    Bonus(BonusType type, float start_x, float start_y);

    // применение эффекта бонуса к объектам игры
    void ApplyEffectBonus(Paddle& paddle, Ball& ball, GameState& game_state);

    // полиморфизм обязательные методы
    void DrawObject(sf::RenderWindow& window) override;
    void UpdateObject(float delta_time) override;
    sf::FloatRect GetBoundingBox() const override;
};
