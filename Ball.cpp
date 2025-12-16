#include "Ball.hpp"
#include <cmath>

Ball::Ball() {
    pos_x = 400.0f;
    pos_y = 500.0f;
    object_radius = 8.0f;
    base_speed = 300.0f;
    vector_x = base_speed * 0.7f;      
    vector_y = -base_speed * 0.8f;     
    flag_stuck_to_paddle = false;      
    flag_active = true;
}


void Ball::MoveBall(float delta_time) { 
    if (flag_stuck_to_paddle) return;
    pos_x += vector_x * delta_time;     
    pos_y += vector_y * delta_time;
}

void Ball::BounceBall(std::string direction) {
    if (direction == "left" || direction == "right") {
        vector_x = -vector_x;
    }
    else if (direction == "top" || direction == "bottom") {
        vector_y = -vector_y;
    }
}

void Ball::ActivateBall() {
    if (!flag_stuck_to_paddle) return;  // уже активен!

    flag_stuck_to_paddle = false;
    vector_x = base_speed * 0.7f;
    vector_y = -base_speed * 0.8f;
}


void Ball::ApplySpeedModifier(float modifier) {
    float current_speed = sqrt(vector_x * vector_x + vector_y * vector_y);
    if (current_speed > 0) {  // защита от деления на 0
        float new_speed = base_speed * modifier;
        vector_x = (vector_x / current_speed) * new_speed;
        vector_y = (vector_y / current_speed) * new_speed;
    }
}

// полиморфный метод переопределение из GameObject
void Ball::DrawObject(sf::RenderWindow& window) {
    if (!flag_active) return;

    sf::CircleShape ball_shape(object_radius);
    ball_shape.setPosition(pos_x - object_radius, pos_y - object_radius);
    ball_shape.setFillColor(sf::Color::Cyan);
    ball_shape.setOutlineThickness(2);
    ball_shape.setOutlineColor(sf::Color::Blue);
    window.draw(ball_shape);
}

// полиморфный метод переопределение из GameObject
void Ball::UpdateObject(float delta_time) {
    MoveBall(delta_time);  
}

// полиморфный метод переопределение из GameObject
sf::FloatRect Ball::GetBoundingBox() const {
    return sf::FloatRect(pos_x - object_radius, pos_y - object_radius,
        object_radius * 2, object_radius * 2);
}

bool Ball::IsStuckToPaddle() const {
    return flag_stuck_to_paddle;
}

void Ball::SetStuckToPaddle(bool stuck) {
    flag_stuck_to_paddle = stuck;
}
void Ball::SetPosition(float new_x, float new_y) {
    pos_x = new_x;
    pos_y = new_y;
}

