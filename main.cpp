#include <SFML/Graphics.hpp>
#include <vector>
#include "GameObject.hpp"
#include "Paddle.hpp"
#include "Ball.hpp"
#include "Brick.hpp"
#include "GameState.hpp"
#include "CollisionManager.hpp"
#include <cstdlib>
#include <ctime>

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(800, 600), "BrickWave - Working Prototype");
    window.setFramerateLimit(60);

    Paddle paddle_obj;
    Ball ball_obj;
    GameState game_state_obj;
    CollisionManager collision_manager;

    // АВТОЗАПУСК МЯЧА
    ball_obj.SetStuckToPaddle(false);
    ball_obj.vector_x = 210.0f;  // вправо
    ball_obj.vector_y = -240.0f; // ВВЕРХ

    // создание первого уровня кирпичей
    std::vector<Brick> bricks;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 12; col++) {
            float x = 40 + col * 65;
            float y = 50 + row * 35;
            BrickType type = static_cast<BrickType>(rand() % 5);
            bricks.emplace_back(x, y, type);
        }
    }

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        // шрифт не найден используем заглушку
    }

    sf::Clock clock;

    while (window.isOpen()) {
        float delta_time = clock.restart().asSeconds();

        // события
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        // 1. ПЛАТФОРМА
        paddle_obj.UpdateObject(delta_time);

        // 2. СТОЛКНОВЕНИЯ
        collision_manager.CheckBallVsPaddle(ball_obj, paddle_obj);
        collision_manager.CheckBallVsWalls(ball_obj, game_state_obj);
        collision_manager.CheckBallVsBricks(ball_obj, bricks, game_state_obj);

        // 3. МЯЧ
        ball_obj.UpdateObject(delta_time);

        // отрисовка
        window.clear(sf::Color::Black);
        paddle_obj.DrawObject(window);
        ball_obj.DrawObject(window);
        for (auto& brick : bricks) brick.DrawObject(window);
        window.display();
    }


    return 0;
}
