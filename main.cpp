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

    // игровые объекты
    Paddle paddle_obj;
    Ball ball_obj;
    GameState game_state_obj;
    CollisionManager collision_manager;

    // создание первого уровн€ кирпичей
    std::vector<Brick> bricks;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 12; col++) {
            float x = 40 + col * 65;
            float y = 50 + row * 35;
            BrickType type = static_cast<BrickType>(rand() % 4);
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

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // синхронизаци€ прилипшего м€ча с платформой
        if (ball_obj.flag_stuck_to_paddle) {
            ball_obj.pos_x = paddle_obj.pos_x + paddle_obj.object_width / 2;
            ball_obj.pos_y = paddle_obj.pos_y - 50;
        }

        // обновление объектов
        paddle_obj.UpdateObject(delta_time);
        ball_obj.UpdateObject(delta_time);

        // проверка столкновений
        collision_manager.CheckBallVsPaddle(ball_obj, paddle_obj);
        collision_manager.CheckBallVsWalls(ball_obj, game_state_obj);

        window.clear(sf::Color::Black);

        // отрисовка объектов
        paddle_obj.DrawObject(window);
        ball_obj.DrawObject(window);

        for (auto& brick : bricks) {
            brick.DrawObject(window);
        }

        // интерфейс
        sf::Text ui_text;
        ui_text.setFont(font);
        ui_text.setCharacterSize(24);
        ui_text.setFillColor(sf::Color::White);
        ui_text.setPosition(10, 10);
        ui_text.setString("Score: " + std::to_string(game_state_obj.score_game) +
            " Lives: " + std::to_string(game_state_obj.lives_game));
        window.draw(ui_text);

        window.display();
    }

    return 0;
}
