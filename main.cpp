#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <clocale>
#include <iostream>
#include "GameObject.hpp"
#include "Paddle.hpp"
#include "Ball.hpp"
#include "Brick.hpp"
#include "GameState.hpp"
#include "CollisionManager.hpp"

int main() {
    std::setlocale(LC_ALL, "ru_RU.utf8");
    AppMode appMode = AppMode::MainMenu;

    srand(static_cast<unsigned>(time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(800, 600), "BrickWave - Working Prototype");
    window.setFramerateLimit(60);

    Paddle paddle_obj;
    Ball ball_obj;
    GameState game_state_obj;
    CollisionManager collision_manager;

    // СИСТЕМА ЖИЗНЕЙ + СЧЕТЧИК БОНУСОВ
    enum class LifeState { Normal, RespawnWait };
    LifeState lifeState = LifeState::Normal;
    sf::Texture heartFullTexture;
    sf::Sprite heartSprites[3];
    bool heartActive[3] = { true, true, true };

    // СЧЕТЧИК БОНУСОВ
    int bonusCount = 0;
    sf::Text bonusText;

    // Переменные для экрана Game Over
    sf::Text gameOverText, scoreText, deathsText, btnNewGameText, btnMenuText;
    float gameOverTimer = 0.0f;

    ball_obj.SetStuckToPaddle(false);

    // загрузка фонового изображения
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("fon.jpg")) {
        // обработка ошибки загрузки
    }
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    // подгонка фона под размер окна
    sf::Vector2u texSize = backgroundTexture.getSize();
    sf::Vector2u winSize = window.getSize();
    if (texSize.x > 0 && texSize.y > 0) {
        float scaleX = static_cast<float>(winSize.x) / texSize.x;
        float scaleY = static_cast<float>(winSize.y) / texSize.y;
        backgroundSprite.setScale(scaleX, scaleY);
    }

    // Загрузка текстуры сердечек
    heartFullTexture.loadFromFile("hearts1.png");

    // Инициализация спрайтов сердечек (выше - Y=7)
    for (int i = 0; i < 3; i++) {
        heartSprites[i].setTexture(heartFullTexture);
        heartSprites[i].setPosition(700 - i * 45, 7);
    }

    // создание набора кирпичей
    std::vector<Brick> bricks;
    const float bricksAreaLeft = 30.0f;
    const float bricksAreaTop = 60.0f;
    const float brickW = 55.0f;
    const float brickH = 22.0f;
    const float stepX = 65.0f;
    const float stepY = 32.0f;
    const int rows = 6;
    const int cols = 11;

    auto generateBricks = [&]() {
        bricks.clear();
        for (int row = 0; row < rows; ++row) {
            float y = bricksAreaTop + row * stepY;
            int bricksInRow = 0;
            for (int col = 0; col < cols; ++col) {
                float x = bricksAreaLeft + col * stepX;
                if (x + brickW > 770.0f || y + brickH > 300.0f) continue;
                float baseChance = 0.7f;
                float rowFactor = 1.0f - row * 0.1f;
                float finalChance = baseChance * rowFactor;
                float r = static_cast<float>(rand()) / RAND_MAX;
                if (r > finalChance) {
                    if (bricksInRow < 2 && col > cols - 3) {}
                    else continue;
                }
                BrickType type = static_cast<BrickType>(rand() % 5);
                bricks.emplace_back(x, y, type);
                bricksInRow++;
            }
            if (bricksInRow == 0) {
                int col = rand() % cols;
                float x = bricksAreaLeft + col * stepX;
                float yFixed = bricksAreaTop + row * stepY;
                if (x + brickW <= 770.0f && yFixed + brickH <= 300.0f) {
                    BrickType type = static_cast<BrickType>(rand() % 5);
                    bricks.emplace_back(x, yFixed, type);
                }
            }
        }
        };
    generateBricks();

    // загрузка шрифтов
    sf::Font titleFont, uiFont;
    titleFont.loadFromFile("Typepunk-Regular.otf");
    uiFont.loadFromFile("F37BlankaCyrillic-VF.ttf");

    // Счетчик бонусов (левый верхний угол)
    bonusText.setFont(uiFont);
    bonusText.setString(L"Счет: 0");
    bonusText.setCharacterSize(24);
    bonusText.setFillColor(sf::Color::Yellow);
    bonusText.setPosition(20, 7);  // На уровне сердечек
    bonusText.setOutlineThickness(2);
    bonusText.setOutlineColor(sf::Color::Black);

    // настройки цветов интерфейса
    sf::Color titleColor(0, 0, 0);
    sf::Color buttonTextColor(0, 0, 0);
    sf::Color panelColor(255, 255, 255, 180);

    // панель меню
    sf::RectangleShape menuPanel;
    menuPanel.setSize(sf::Vector2f(400.f, 260.f));
    menuPanel.setFillColor(panelColor);
    menuPanel.setOrigin(menuPanel.getSize() / 2.f);
    menuPanel.setPosition(400.f, 340.f);

    // текст заголовка
    sf::Text titleText;
    titleText.setFont(titleFont);
    titleText.setString("BrickWave");
    titleText.setCharacterSize(64);
    titleText.setFillColor(titleColor);
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width / 2.f, titleBounds.height / 2.f);
    titleText.setPosition(400.f, 130.f);

    sf::Text titleOutlineText = titleText;
    titleOutlineText.setFillColor(sf::Color::White);
    titleOutlineText.setCharacterSize(68);

    // кнопки меню
    sf::Text btnStartText, btnSettingsText, btnRecordsText, btnExitText;
    auto setupButtonText = [&](sf::Text& txt, const std::wstring& str, float y) {
        txt.setFont(uiFont);
        txt.setString(str);
        txt.setCharacterSize(32);
        txt.setFillColor(buttonTextColor);
        sf::FloatRect bounds = txt.getLocalBounds();
        txt.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        txt.setPosition(400.f, y);
        };
    setupButtonText(btnStartText, L"Начать игру", 280.f);
    setupButtonText(btnSettingsText, L"Настройки", 330.f);
    setupButtonText(btnRecordsText, L"Рекорды", 380.f);
    setupButtonText(btnExitText, L"Выход", 430.f);

    // Game Over текст
    auto setupGameOverText = [&](sf::Text& txt, const std::wstring& str, float y, int size, bool isButton = false) {
        txt.setFont(uiFont);
        txt.setString(str);
        txt.setCharacterSize(size);
        if (isButton) txt.setFillColor(sf::Color(50, 150, 255));
        else txt.setFillColor(sf::Color::Red);
        sf::FloatRect bounds = txt.getLocalBounds();
        txt.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        txt.setPosition(400.f, y);
        };

    // курсоры
    sf::Cursor handCursor, arrowCursor;
    arrowCursor.loadFromSystem(sf::Cursor::Arrow);
    handCursor.loadFromSystem(sf::Cursor::Hand);

    sf::Clock clock;

    while (window.isOpen()) {
        float delta_time = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (appMode == AppMode::MainMenu) {
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {

                    sf::Vector2f mousePos(
                        static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));

                    auto startBounds = btnStartText.getGlobalBounds();
                    auto settingsBounds = btnSettingsText.getGlobalBounds();
                    auto recordsBounds = btnRecordsText.getGlobalBounds();
                    auto exitBounds = btnExitText.getGlobalBounds();

                    if (startBounds.contains(mousePos)) {
                        appMode = AppMode::Playing;
                        game_state_obj.ResetGameState();
                        generateBricks();

                        // СБРОС
                        bonusCount = 0;
                        for (int i = 0; i < 3; i++) {
                            heartActive[i] = true;
                            heartSprites[i].setTexture(heartFullTexture);
                            heartSprites[i].setPosition(700 - i * 45, 7);
                        }
                        lifeState = LifeState::Normal;

                        paddle_obj.SetPosition(355.0f, 550.0f);
                        ball_obj.SetPosition(400.0f, 530.0f);
                        ball_obj.SetStuckToPaddle(false);
                        ball_obj.vector_x = 210.0f;
                        ball_obj.vector_y = -240.0f;
                    }
                    else if (settingsBounds.contains(mousePos)) {}
                    else if (recordsBounds.contains(mousePos)) {}
                    else if (exitBounds.contains(mousePos)) {
                        window.close();
                    }
                }
            }
            else if (appMode == AppMode::GameOver) {
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {

                    sf::Vector2f mousePos(
                        static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));

                    if (btnNewGameText.getGlobalBounds().contains(mousePos)) {
                        appMode = AppMode::Playing;
                        game_state_obj.ResetGameState();
                        generateBricks();

                        // СБРОС
                        bonusCount = 0;
                        for (int i = 0; i < 3; i++) {
                            heartActive[i] = true;
                            heartSprites[i].setTexture(heartFullTexture);
                            heartSprites[i].setPosition(700 - i * 45, 7);
                        }
                        lifeState = LifeState::Normal;

                        paddle_obj.SetPosition(355.0f, 550.0f);
                        ball_obj.SetPosition(400.0f, 530.0f);
                        ball_obj.SetStuckToPaddle(false);
                        ball_obj.vector_x = 210.0f;
                        ball_obj.vector_y = -240.0f;
                    }
                    else if (btnMenuText.getGlobalBounds().contains(mousePos)) {
                        appMode = AppMode::MainMenu;
                        game_state_obj.ResetGameState();
                        generateBricks();
                    }
                }
            }
        }

        // обновление игры
        if (appMode == AppMode::Playing) {
            bonusText.setString(L"Счет: " + std::to_wstring(game_state_obj.GetScore()));            // Game Over по жизням
            bool allHeartsDead = true;
            for (int i = 0; i < 3; i++) {
                if (heartActive[i]) {
                    allHeartsDead = false;
                    break;
                }
            }
            if (allHeartsDead) {
                std::cout << "GAME OVER ACTIVATED!" << std::endl;
                appMode = AppMode::GameOver;
                gameOverTimer = 999.0f;

                setupGameOverText(gameOverText, L"ИГРА ОКОНЧЕНА!", 200.f, 64);
                std::wstring scoreStr = L"Счет: " + std::to_wstring(game_state_obj.GetScore());
                setupGameOverText(scoreText, scoreStr, 300.f, 32);
                std::wstring deathsStr = L"Падений: 3";
                setupGameOverText(deathsText, deathsStr, 350.f, 32);
                setupGameOverText(btnNewGameText, L"НОВАЯ ИГРА", 450.f, 40, true);
                setupGameOverText(btnMenuText, L"ГЛАВНОЕ МЕНЮ", 510.f, 40, true);
                continue;
            }

            ball_obj.UpdateObject(delta_time);
            if (lifeState != LifeState::RespawnWait) {
                paddle_obj.UpdateObject(delta_time);
            }

            collision_manager.CheckBallVsPaddle(ball_obj, paddle_obj);
            collision_manager.CheckBallVsWalls(ball_obj, game_state_obj);
            collision_manager.CheckBallVsBricks(ball_obj, bricks, game_state_obj);

            // Падение мяча
            if (ball_obj.GetBoundingBox().top > 600 && lifeState == LifeState::Normal) {
                lifeState = LifeState::RespawnWait;
                for (int i = 2; i >= 0; i--) {
                    if (heartActive[i]) {
                        heartActive[i] = false;
                        heartSprites[i].setPosition(-100, -100);
                        break;
                    }
                }
                paddle_obj.SetPosition(355.0f, 550.0f);
                ball_obj.SetPosition(400.0f, 530.0f);
                ball_obj.StartRespawn(0.7f);
            }

            if (lifeState == LifeState::RespawnWait && !ball_obj.IsRespawning()) {
                lifeState = LifeState::Normal;
            }
        }

        // курсор
        if (appMode == AppMode::MainMenu) {
            sf::Vector2i mousePosPixels = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos(static_cast<float>(mousePosPixels.x),
                static_cast<float>(mousePosPixels.y));
            auto startBounds = btnStartText.getGlobalBounds();
            auto settingsBounds = btnSettingsText.getGlobalBounds();
            auto recordsBounds = btnRecordsText.getGlobalBounds();
            auto exitBounds = btnExitText.getGlobalBounds();
            if (startBounds.contains(mousePos) || settingsBounds.contains(mousePos) ||
                recordsBounds.contains(mousePos) || exitBounds.contains(mousePos)) {
                window.setMouseCursor(handCursor);
            }
            else {
                window.setMouseCursor(arrowCursor);
            }
        }
        else {
            window.setMouseCursor(arrowCursor);
        }

        // отрисовка
        window.clear();
        window.draw(backgroundSprite);

        if (appMode == AppMode::MainMenu) {
            window.draw(titleOutlineText);
            window.draw(titleText);
            window.draw(menuPanel);
            window.draw(btnStartText);
            window.draw(btnSettingsText);
            window.draw(btnRecordsText);
            window.draw(btnExitText);
        }
        else if (appMode == AppMode::Playing) {
            // СЧЕТЧИК БОНУСОВ (ЛЕВЫЙ ВЕРХ)
            window.draw(bonusText);

            // СЕРДЕЧКИ (ПРАВЫЙ ВЕРХ)
            for (int i = 0; i < 3; i++) {
                if (heartActive[i]) {
                    window.draw(heartSprites[i]);
                }
            }

            paddle_obj.DrawObject(window);
            ball_obj.DrawObject(window);
            for (auto& brick : bricks)
                brick.DrawObject(window);
        }
        else if (appMode == AppMode::GameOver) {
            window.draw(gameOverText);
            window.draw(scoreText);
            window.draw(deathsText);
            window.draw(btnNewGameText);
            window.draw(btnMenuText);
        }

        window.display();
    }

    return 0;
}

