#pragma once
#include "GameObject.hpp"

enum BrickType { normal_brick, strong_brick, exploding_brick, multi_hit_brick };

class Brick : public GameObject {
private:
    int hit_points;
    int max_hit_points;
    BrickType brick_type;
    int score_value;
    bool contains_bonus;

public:
    Brick();

    void OnHitBrick();
    sf::Color GetColorBrick() const;
};
