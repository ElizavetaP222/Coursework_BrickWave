#pragma once
#include "GameObject.hpp"

enum BrickType {
    normal_brick,
    strong_brick,
    exploding_brick,
    multi_hit_brick,
    super_brick
};

class Brick : public GameObject {
private:
    // прочность кирпича количество ударов дл€ разрушени€
    int hit_points;
    // максимальна€ прочность дл€ восстановлени€
    int max_hit_points;
    // тип кирпича определ€ет поведение
    BrickType brick_type;
    // очки за разрушение кирпича
    int score_value;
    // содержит ли кирпич бонус
    bool contains_bonus;

public:
    Brick(int start_x, int start_y, BrickType type);
    int GetScoreValue() const { return score_value; }
    // обработка попадани€ м€ча
    void OnHitBrick();
    // цвет кирпича по текущей прочности
    sf::Color GetColorBrick() const;
    void DrawObject(sf::RenderWindow& window) override;
    void UpdateObject(float delta_time) override;
    sf::FloatRect GetBoundingBox() const override;
    sf::Color GetColorByHits() const;

};
