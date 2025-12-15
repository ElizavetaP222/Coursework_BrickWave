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
    Bonus();

    void ApplyEffectBonus(Paddle& paddle, Ball& ball, GameState& game_state);
};
