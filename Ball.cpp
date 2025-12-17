#include "Ball.hpp"
#include <cmath>

Ball::Ball() {
    pos_x = 400.0f;
    pos_y = 500.0f;
    object_radius = 10.0f;
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
    if (!flag_stuck_to_paddle) return;
    flag_stuck_to_paddle = false;
    vector_x = base_speed * 0.7f;
    vector_y = -base_speed * 0.8f;
}

void Ball::ApplySpeedModifier(float modifier) {
    float current_speed = sqrt(vector_x * vector_x + vector_y * vector_y);
    if (current_speed > 0) {
        float new_speed = base_speed * modifier;
        vector_x = (vector_x / current_speed) * new_speed;
        vector_y = (vector_y / current_speed) * new_speed;
    }
}

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

void Ball::SetActive(bool active) {
    flag_active = active;
}

void Ball::StartRespawn(float time) {
    respawnTimeLeft = time;  
    respawnBlink = true;
    flag_active = true;
    flag_stuck_to_paddle = true;
}


void Ball::UpdateObject(float delta_time) {
    if (respawnTimeLeft > 0.0f) {
        respawnTimeLeft -= delta_time;
        if (respawnTimeLeft <= 0.0f) {
            respawnBlink = false;
            flag_stuck_to_paddle = false;
            vector_x = base_speed * 0.7f;
            vector_y = -base_speed * 0.8f;
        }
    }
    MoveBall(delta_time);
    if (fabs(vector_x) < 1.0f && fabs(vector_y) < 1.0f) {
        vector_x = base_speed * 0.7f;
        vector_y = -base_speed * 0.8f;
    }
}

void Ball::DrawObject(sf::RenderWindow& window) {
    if (!flag_active) return;

    // Мигание при респауне
    if (respawnBlink && respawnTimeLeft > 0.0f) {
        // Простое мигание: вкл 0.15с → выкл 0.05с
        float blinkPhase = fmod(respawnTimeLeft, 0.2f);
        if (blinkPhase > 0.15f) {  // выкл только последние 0.05с
            return;
        }
    }

    // Рисуем мяч
    sf::CircleShape ballShape(object_radius);
    ballShape.setPosition(pos_x - object_radius, pos_y - object_radius);
    ballShape.setFillColor(sf::Color::White);
    ballShape.setOutlineThickness(2);
    ballShape.setOutlineColor(sf::Color(100, 100, 100));
    window.draw(ballShape);
}



