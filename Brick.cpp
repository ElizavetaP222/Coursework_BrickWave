#include "Brick.hpp"
#include <cstdlib>

Brick::Brick(int start_x, int start_y, BrickType type) {
    pos_x = start_x;
    pos_y = start_y;
    object_width = 59.0f;
    object_height = 26.0f;
    brick_type = type;

    switch (type) {
    case exploding_brick:   // синий кирпич - 1 удар
        hit_points = 1; max_hit_points = 1; score_value = 50; break;
    case normal_brick:      // зеленый кирпич - 2 удара  
        hit_points = 2; max_hit_points = 2; score_value = 10; break;
    case strong_brick:      // желтый кирпич - 3 удара
        hit_points = 3; max_hit_points = 3; score_value = 30; break;
    case multi_hit_brick:   // оранжевый кирпич - 4 удара
        hit_points = 4; max_hit_points = 4; score_value = 100; break;
    case super_brick:       // красный кирпич - 5 ударов
        hit_points = 5; max_hit_points = 5; score_value = 200; break;
    }

    flag_active = true;
    contains_bonus = (rand() % 3 == 0);
}

void Brick::OnHitBrick() {
    hit_points--;
    if (hit_points <= 0) {
        flag_active = false;
    }
}

sf::Color Brick::GetColorBrick() const {
    switch (brick_type) {
    case normal_brick: return sf::Color::Green;
    case strong_brick: return sf::Color::Yellow;
    case exploding_brick: return sf::Color::Blue;
    case super_brick: return sf::Color::Red;
    default: return sf::Color::White;
    }
}

// отрисовка кирпича с учетом текущей прочности
void Brick::DrawObject(sf::RenderWindow& window) {
    if (!flag_active) return;

    sf::RectangleShape brick_shape(sf::Vector2f(object_width, object_height));
    brick_shape.setPosition(pos_x, pos_y);
    brick_shape.setFillColor(GetColorByHits()); 
    brick_shape.setOutlineThickness(1);
    brick_shape.setOutlineColor(sf::Color::White);
    window.draw(brick_shape);
}

// кирпичи не двигаются
void Brick::UpdateObject(float delta_time) {
}

// возвращает границы кирпича для проверки столкновений
sf::FloatRect Brick::GetBoundingBox() const {
    return sf::FloatRect(pos_x, pos_y, object_width, object_height);
}

// возвращает цвет кирпича в зависимости от оставшихся ударов
sf::Color Brick::GetColorByHits() const {
    int remaining_hits = hit_points;

    switch (brick_type) {
    case super_brick: {  // красный кирпич - 5 ударов
        if (remaining_hits == 5) return sf::Color(255, 0, 0);      // красный
        if (remaining_hits == 4) return sf::Color(255, 140, 0);    // оранжевый
        if (remaining_hits == 3) return sf::Color(255, 255, 0);    // желтый
        if (remaining_hits == 2) return sf::Color(0, 255, 0);      // зеленый
        return sf::Color(0, 150, 255);                             // синий
    }

    case multi_hit_brick: {  // оранжевый кирпич - 4 удара
        if (remaining_hits == 4) return sf::Color(255, 140, 0);    // оранжевый
        if (remaining_hits == 3) return sf::Color(255, 255, 0);    // желтый
        if (remaining_hits == 2) return sf::Color(0, 255, 0);      // зеленый
        return sf::Color(0, 150, 255);                             // синий
    }

    case strong_brick: {  // желтый кирпич - 3 удара
        if (remaining_hits == 3) return sf::Color(255, 255, 0);    // желтый
        if (remaining_hits == 2) return sf::Color(0, 255, 0);      // зеленый
        return sf::Color(0, 150, 255);                             // синий
    }

    case normal_brick: {  // зеленый кирпич - 2 удара
        if (remaining_hits == 2) return sf::Color(0, 255, 0);      // зеленый
        return sf::Color(0, 150, 255);                             // синий
    }

    case exploding_brick:  // синий кирпич - 1 удар
        return sf::Color(0, 150, 255);                             // синий
    }

    return sf::Color::White;
}
