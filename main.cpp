#include <SFML/Graphics.hpp>
#include "GameObject.hpp"
#include "Paddle.hpp"
#include "Ball.hpp"
#include "Brick.hpp"
#include "Bonus.hpp"
#include "GameState.hpp"
#include "Level.hpp"
#include "CollisionManager.hpp"
#include "Screen.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "BrickWave - Core Classes");
    Paddle paddle_obj;
    Ball ball_obj;
    GameState game_state_obj;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        paddle_obj.HandleInputPaddle();
        window.clear(sf::Color::Black);
        window.display();
    }

    return 0;
}
