#include "Paddle.hpp"
#include <SFML/Window/Keyboard.hpp>

Paddle::Paddle() {
    pos_x = 350.0f;
    pos_y = 550.0f;
    object_width = 90.0f;
    object_height = 20.0f;
    paddle_speed = 400.0f;
    move_direction = 0;
    base_width_paddle = 100.0f;
    flag_active = true;
}

void Paddle::HandleInputPaddle() {
    move_direction = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        move_direction = -1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        move_direction = 1;
    }

    pos_x += move_direction * paddle_speed * 0.016f;
    if (pos_x < 0) pos_x = 0;
    if (pos_x + object_width > 800) pos_x = 800 - object_width;
}

void Paddle::ApplyBonusPaddle() {
    object_width = base_width_paddle * 1.5f;
}

void Paddle::ResetSizePaddle() {
    object_width = base_width_paddle;
}

// Реализация переопределенных методов
void Paddle::DrawObject(sf::RenderWindow& window) {
    if (!flag_active) return;

    sf::RectangleShape paddle_shape(sf::Vector2f(object_width, object_height));
    paddle_shape.setPosition(pos_x, pos_y);
    paddle_shape.setFillColor(sf::Color::White);
    paddle_shape.setOutlineThickness(2);
    paddle_shape.setOutlineColor(sf::Color::Yellow);
    window.draw(paddle_shape);
}

void Paddle::UpdateObject(float delta_time) {
    HandleInputPaddle();
}

sf::FloatRect Paddle::GetBoundingBox() const {
    return sf::FloatRect(pos_x, pos_y, object_width, object_height);
}
void Paddle::SetPosition(float x, float y) {
    pos_x = x;
    pos_y = y;
}