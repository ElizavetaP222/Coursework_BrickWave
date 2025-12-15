#include "Brick.hpp"
#include <cstdlib>

Brick::Brick(int start_x, int start_y, BrickType type) {
    pos_x = start_x;
    pos_y = start_y;
    object_width = 60.0f;
    object_height = 25.0f;
    brick_type = type;

    switch (type) {
    case normal_brick:
        hit_points = 1; max_hit_points = 1; score_value = 10; break;
    case strong_brick:
        hit_points = 3; max_hit_points = 3; score_value = 30; break;
    case exploding_brick:
        hit_points = 1; max_hit_points = 1; score_value = 50; break;
    case multi_hit_brick:
        hit_points = 5; max_hit_points = 5; score_value = 100; break;
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
    float health_ratio = static_cast<float>(hit_points) / max_hit_points;
    if (health_ratio >= 0.7f) return sf::Color::Red;
    if (health_ratio >= 0.4f) return sf::Color::Yellow;
    return sf::Color::Green;
}

// полиморфный метод переопределение из GameObject
void Brick::DrawObject(sf::RenderWindow& window) {
    if (!flag_active) return;

    sf::RectangleShape brick_shape(sf::Vector2f(object_width, object_height));
    brick_shape.setPosition(pos_x, pos_y);
    brick_shape.setFillColor(GetColorBrick());
    brick_shape.setOutlineThickness(1);
    brick_shape.setOutlineColor(sf::Color::White);
    window.draw(brick_shape);
}

// полиморфный метод переопределение из GameObject
void Brick::UpdateObject(float delta_time) {
    // кирпичи статичны не двигаются
}

// полиморфный метод переопределение из GameObject
sf::FloatRect Brick::GetBoundingBox() const {
    return sf::FloatRect(pos_x, pos_y, object_width, object_height);
}
