#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <clocale>
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "GameObject.hpp"
#include "Paddle.hpp"
#include "Ball.hpp"
#include "Brick.hpp"
#include "GameState.hpp"
#include "CollisionManager.hpp"

int main() {
    // настройка локали для русского языка
    std::setlocale(LC_ALL, "ru_RU.utf8");
    AppMode appMode = AppMode::MainMenu;

    // инициализация генератора случайных чисел
    srand(static_cast<unsigned>(time(nullptr)));

    // создание окна игры
    sf::RenderWindow window(sf::VideoMode(800, 600), "BrickWave");
    window.setFramerateLimit(60);

    // создание игровых объектов
    Paddle paddle_obj;
    Ball ball_obj;
    GameState game_state_obj;
    CollisionManager collision_manager;

    // история всех результатов игр - загружаем из файла
    std::vector<int> scoreHistory;
    auto loadScores = [&]() {
        scoreHistory.clear();
        std::ifstream file("scores.txt");
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                int score;
                if (iss >> score) {
                    scoreHistory.push_back(score);
                }
            }
            file.close();
        }
        };
    auto saveScores = [&]() {
        std::ofstream file("scores.txt");
        if (file.is_open()) {
            for (int score : scoreHistory) {
                file << score << std::endl;
            }
            file.close();
        }
        };

    // загружаем рекорды при запуске
    loadScores();

    // система жизней
    enum class LifeState { Normal, RespawnWait };
    LifeState lifeState = LifeState::Normal;
    sf::Texture heartFullTexture;
    sf::Sprite heartSprites[3];
    bool heartActive[3] = { true, true, true };

    // счет
    sf::Text scoreTextUI;

    // таймер для новых строк кирпичей (каждые 20 секунд)
    sf::Clock gameTimer;
    float timeSinceNewRow = 0.0f;
    const float row_interval = 20.0f;

    // мяч не прилипает к платформе в начале
    ball_obj.SetStuckToPaddle(false);

    // фон
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("fon.jpg")) {}
    sf::Sprite backgroundSprite(backgroundTexture);
    sf::Vector2u texSize = backgroundTexture.getSize();
    sf::Vector2u winSize = window.getSize();
    if (texSize.x > 0 && texSize.y > 0) {
        float scaleX = static_cast<float>(winSize.x) / texSize.x;
        float scaleY = static_cast<float>(winSize.y) / texSize.y;
        backgroundSprite.setScale(scaleX, scaleY);
    }

    // сердечки для отображения жизней
    heartFullTexture.loadFromFile("hearts1.png");
    for (int i = 0; i < 3; i++) {
        heartSprites[i].setTexture(heartFullTexture);
        heartSprites[i].setPosition(700 - i * 45, 7);
    }

    // кирпичи
    std::vector<Brick> bricks;
    const float bricksAreaLeft = 30.0f, bricksAreaTop = 60.0f;
    const float brickW = 55.0f, brickH = 22.0f;
    const float stepX = 65.0f, stepY = 32.0f;
    const int cols = 11;
    const float PADDLE_LEVEL = 550.0f;

    // генерация начальных кирпичей
    auto generateBricks = [&]() {
        bricks.clear();
        timeSinceNewRow = 0.0f;
        gameTimer.restart();
        const int rows = 6;
        for (int row = 0; row < rows; ++row) {
            float y = bricksAreaTop + row * stepY;
            for (int col = 0; col < cols; ++col) {
                float x = bricksAreaLeft + col * stepX;
                if (x + brickW > 770.0f || y + brickH > 300.0f) continue;
                float baseChance = 0.7f;
                float rowFactor = 1.0f - row * 0.1f;
                float finalChance = baseChance * rowFactor;
                float r = static_cast<float>(rand()) / RAND_MAX;
                if (r > finalChance) continue;
                BrickType type = static_cast<BrickType>(rand() % 5);
                bricks.emplace_back(static_cast<int>(x), static_cast<int>(y), type);
            }
        }
        };

    // загрузка шрифтов
    sf::Font titleFont, uiFont;
    titleFont.loadFromFile("Typepunk-Regular.otf");
    uiFont.loadFromFile("F37BlankaCyrillic-VF.ttf");

    // структура для неоновых кнопок
    struct NeonButton {
        sf::RectangleShape bg, glow;
        sf::Text text;
        bool hovered = false;
        float pulse = 0.0f;
    };
    NeonButton menuButtons[3]; 
    NeonButton goButtons[2];
    NeonButton pauseButtons[2];
    NeonButton newRecordButtons[2];

    // счет в верхнем левом углу
    scoreTextUI.setFont(uiFont);
    scoreTextUI.setString(L"Счет: 0");
    scoreTextUI.setCharacterSize(24);
    scoreTextUI.setFillColor(sf::Color(0, 255, 255));
    scoreTextUI.setPosition(70, 7);
    scoreTextUI.setOutlineThickness(2);
    scoreTextUI.setOutlineColor(sf::Color::Black);

    // кнопка паузы
    sf::RectangleShape pauseButtonBg;
    pauseButtonBg.setSize({ 32.f, 32.f });
    pauseButtonBg.setFillColor(sf::Color(10, 10, 30, 200));
    pauseButtonBg.setOutlineThickness(2);
    pauseButtonBg.setOutlineColor(sf::Color(0, 255, 255, 180));
    pauseButtonBg.setPosition(20.f, 6.f);

    sf::RectangleShape pauseBar1, pauseBar2;
    pauseBar1.setSize({ 6.f, 18.f });
    pauseBar2.setSize({ 6.f, 18.f });
    pauseBar1.setFillColor(sf::Color(0, 255, 255));
    pauseBar2.setFillColor(sf::Color(0, 255, 255));
    pauseBar1.setPosition(pauseButtonBg.getPosition().x + 8.f, pauseButtonBg.getPosition().y + 7.f);
    pauseBar2.setPosition(pauseButtonBg.getPosition().x + 18.f, pauseButtonBg.getPosition().y + 7.f);

    // цвет неонового голубого
    sf::Color neonCyan(0, 255, 255);

    std::wstring menuBtnTexts[3] = { L" НАЧАТЬ ИГРУ", L" РУКОВОДСТВО", L" ВЫХОД" };
    for (int i = 0; i < 3; i++) {
        menuButtons[i].bg.setSize({ 360, 50 });
        menuButtons[i].bg.setFillColor(sf::Color(10, 10, 30, 200));
        menuButtons[i].bg.setOutlineThickness(3);
        menuButtons[i].bg.setOutlineColor(sf::Color(0, 255, 255, 0));
        menuButtons[i].bg.setOrigin(175, 30);
        menuButtons[i].bg.setPosition(400, 260 + i * 70);

        menuButtons[i].glow.setSize({ 370, 60 });
        menuButtons[i].glow.setFillColor(sf::Color::Transparent);
        menuButtons[i].glow.setOutlineThickness(5);
        menuButtons[i].glow.setOutlineColor(sf::Color(0, 255, 255, 80));
        menuButtons[i].glow.setOrigin(180, 35);
        menuButtons[i].glow.setPosition(400, 260 + i * 70);

        menuButtons[i].text.setFont(uiFont);
        menuButtons[i].text.setString(menuBtnTexts[i]);
        menuButtons[i].text.setCharacterSize(28);
        menuButtons[i].text.setFillColor(neonCyan);
        menuButtons[i].text.setOutlineThickness(2);
        menuButtons[i].text.setOutlineColor(sf::Color::Black);
        sf::FloatRect tb = menuButtons[i].text.getLocalBounds();
        menuButtons[i].text.setOrigin(tb.width / 2, tb.height / 2);
        menuButtons[i].text.setPosition(400, 250 + i * 70);
    }

    // заголовок игры
    sf::Text titleText, titleGlow;
    titleText.setFont(titleFont);
    titleText.setString("BRICKWAVE");
    titleText.setCharacterSize(72);
    titleText.setFillColor(neonCyan);
    titleText.setOutlineThickness(4);
    titleText.setOutlineColor(sf::Color::Black);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
    titleText.setPosition(400, 130);

    titleGlow = titleText;
    titleGlow.setCharacterSize(80);
    titleGlow.setFillColor(sf::Color(0, 255, 255, 120));
    titleGlow.setOutlineThickness(0);

    // экран game over
    sf::Text goTitleText, goResultText, goRecordText, goLineTop, goLineBottom;
    auto setupText = [&](sf::Text& t, const std::wstring& s, unsigned size, float x, float y) {
        t.setFont(uiFont);
        t.setString(s);
        t.setCharacterSize(size);
        t.setFillColor(sf::Color::Black);
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(b.width / 2, b.height / 2);
        t.setPosition(x, y);
        };

    setupText(goTitleText, L"ИГРА ОКОНЧЕНА", 40, 400.f, 80.f);
    goTitleText.setFillColor(sf::Color(255, 50, 50));
    setupText(goLineTop, L"______________________________", 20, 400.f, 120.f);
    setupText(goResultText, L"Результат: 0 очков", 25, 400.f, 170.f);
    setupText(goRecordText, L"Рекорд: 0 очков", 25, 400.f, 220.f);
    setupText(goLineBottom, L"______________________________", 20, 400.f, 270.f);

    goLineTop.setFillColor(neonCyan);
    goLineBottom.setFillColor(neonCyan);
    goResultText.setFillColor(neonCyan);
    goRecordText.setFillColor(neonCyan);

    auto centerText = [&](sf::Text& t) {
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(b.width / 2, b.height / 2);
        };

    // кнопки game over
    std::wstring goBtnTexts[2] = { L" ИГРАТЬ СНОВА", L" ВЫЙТИ" };
    for (int i = 0; i < 2; i++) {
        goButtons[i].bg.setSize({ 260, 40 });
        goButtons[i].bg.setFillColor(sf::Color(10, 10, 30, 200));
        goButtons[i].bg.setOutlineThickness(3);
        goButtons[i].bg.setOutlineColor(sf::Color(0, 255, 255, 0));
        goButtons[i].bg.setOrigin(130, 20);
        goButtons[i].bg.setPosition(400, 360 + i * 60);

        goButtons[i].glow.setSize({ 270, 50 });
        goButtons[i].glow.setFillColor(sf::Color::Transparent);
        goButtons[i].glow.setOutlineThickness(5);
        goButtons[i].glow.setOutlineColor(sf::Color(0, 255, 255, 80));
        goButtons[i].glow.setOrigin(135, 25);
        goButtons[i].glow.setPosition(400, 360 + i * 60);

        goButtons[i].text.setFont(uiFont);
        goButtons[i].text.setString(goBtnTexts[i]);
        goButtons[i].text.setCharacterSize(24);
        goButtons[i].text.setFillColor(neonCyan);
        goButtons[i].text.setOutlineThickness(2);
        goButtons[i].text.setOutlineColor(sf::Color::Black);
        sf::FloatRect tb = goButtons[i].text.getLocalBounds();
        goButtons[i].text.setOrigin(tb.width / 2, tb.height / 2);
        goButtons[i].text.setPosition(400, 354 + i * 60);
    }

    // окно нового рекорда
    sf::Text newRecordTitleText, newRecordResultText, newRecordOldRecordText;
    sf::Text newRecordLineTop, newRecordLineBottom;

    setupText(newRecordTitleText, L"НОВЫЙ РЕКОРД!", 45, 400.f, 80.f);
    newRecordTitleText.setFillColor(sf::Color(255, 255, 0)); // золотой
    setupText(newRecordLineTop, L"===============================", 22, 400.f, 130.f);
    setupText(newRecordResultText, L"Результат: 0 очков", 28, 400.f, 180.f);
    setupText(newRecordOldRecordText, L"Старый рекорд: 0 очков", 28, 400.f, 230.f);
    setupText(newRecordLineBottom, L"===============================", 22, 400.f, 290.f);

    newRecordLineTop.setFillColor(neonCyan);
    newRecordLineBottom.setFillColor(neonCyan);
    newRecordResultText.setFillColor(sf::Color(255, 255, 0));
    newRecordOldRecordText.setFillColor(neonCyan);

    // кнопки нового рекорда (копия game over)
    std::wstring newRecordBtnTexts[2] = { L" ИГРАТЬ СНОВА", L" ВЫЙТИ" };
    for (int i = 0; i < 2; i++) {
        newRecordButtons[i] = goButtons[i];
        newRecordButtons[i].text.setString(newRecordBtnTexts[i]);
    }

    // оверлей паузы
    sf::RectangleShape pauseOverlay;
    pauseOverlay.setSize(sf::Vector2f(800.f, 600.f));
    pauseOverlay.setFillColor(sf::Color(0, 0, 0, 150));

    // заголовок паузы
    sf::Text pauseTitle;
    pauseTitle.setFont(uiFont);
    pauseTitle.setString(L"ИГРА НА ПАУЗЕ");
    pauseTitle.setCharacterSize(30);
    pauseTitle.setFillColor(neonCyan);
    sf::FloatRect pBounds = pauseTitle.getLocalBounds();
    pauseTitle.setOrigin(pBounds.width / 2, pBounds.height / 2);
    pauseTitle.setPosition(400.f, 150.f);

    // оверлей game over и рекорда
    sf::RectangleShape gameOverOverlay;
    gameOverOverlay.setSize(sf::Vector2f(800.f, 600.f));
    gameOverOverlay.setFillColor(sf::Color(0, 0, 0, 150));

    // кнопки паузы
    std::wstring pauseBtnTexts[2] = { L" ПРОДОЛЖИТЬ", L" В ГЛАВНОЕ МЕНЮ" };
    for (int i = 0; i < 2; i++) {
        pauseButtons[i].bg.setSize({ 260, 40 });
        pauseButtons[i].bg.setFillColor(sf::Color(10, 10, 30, 200));
        pauseButtons[i].bg.setOutlineThickness(3);
        pauseButtons[i].bg.setOutlineColor(sf::Color(0, 255, 255, 0));
        pauseButtons[i].bg.setOrigin(130, 20);
        pauseButtons[i].bg.setPosition(400, 230 + i * 70);

        pauseButtons[i].glow.setSize({ 270, 50 });
        pauseButtons[i].glow.setFillColor(sf::Color::Transparent);
        pauseButtons[i].glow.setOutlineThickness(5);
        pauseButtons[i].glow.setOutlineColor(sf::Color(0, 255, 255, 80));
        pauseButtons[i].glow.setOrigin(135, 25);
        pauseButtons[i].glow.setPosition(400, 230 + i * 70);

        pauseButtons[i].text.setFont(uiFont);
        pauseButtons[i].text.setString(pauseBtnTexts[i]);
        pauseButtons[i].text.setCharacterSize(24);
        pauseButtons[i].text.setFillColor(neonCyan);
        pauseButtons[i].text.setOutlineThickness(2);
        pauseButtons[i].text.setOutlineColor(sf::Color::Black);
        sf::FloatRect tb = pauseButtons[i].text.getLocalBounds();
        pauseButtons[i].text.setOrigin(tb.width / 2, tb.height / 2);
        pauseButtons[i].text.setPosition(400, 224 + i * 70);
    }

    // ЭКРАН НАСТРОЕК (информация о разработчике и правила)
    sf::Text settingsTitle;
    settingsTitle.setFont(uiFont);
    settingsTitle.setString(L"РУКОВОДСТВО");
    settingsTitle.setCharacterSize(36);
    settingsTitle.setFillColor(neonCyan);
    sf::FloatRect stBounds = settingsTitle.getLocalBounds();
    settingsTitle.setOrigin(stBounds.width / 2, stBounds.height / 2);
    settingsTitle.setPosition(400.f, 60.f);

    sf::Text settingsInfo;
    settingsInfo.setFont(uiFont);
    settingsInfo.setCharacterSize(20);
    settingsInfo.setFillColor(sf::Color(255, 255, 255));
    settingsInfo.setPosition(50.f, 100.f);
    settingsInfo.setOutlineThickness(1);
    settingsInfo.setOutlineColor(sf::Color::Black);

    NeonButton settingsBackButton;
    settingsBackButton.bg.setSize({ 260, 40 });
    settingsBackButton.bg.setFillColor(sf::Color(10, 10, 30, 200));
    settingsBackButton.bg.setOutlineThickness(3);
    settingsBackButton.bg.setOutlineColor(sf::Color(0, 255, 255, 0));
    settingsBackButton.bg.setOrigin(130, 20);
    settingsBackButton.bg.setPosition(400, 500);

    settingsBackButton.glow.setSize({ 270, 50 });
    settingsBackButton.glow.setFillColor(sf::Color::Transparent);
    settingsBackButton.glow.setOutlineThickness(5);
    settingsBackButton.glow.setOutlineColor(sf::Color(0, 255, 255, 80));
    settingsBackButton.glow.setOrigin(135, 25);
    settingsBackButton.glow.setPosition(400, 500);

    settingsBackButton.text.setFont(uiFont);
    settingsBackButton.text.setString(L" НАЗАД В МЕНЮ");
    settingsBackButton.text.setCharacterSize(24);
    settingsBackButton.text.setFillColor(neonCyan);
    settingsBackButton.text.setOutlineThickness(2);
    settingsBackButton.text.setOutlineColor(sf::Color::Black);
    sf::FloatRect sbtb = settingsBackButton.text.getLocalBounds();
    settingsBackButton.text.setOrigin(sbtb.width / 2, sbtb.height / 2);
    settingsBackButton.text.setPosition(400, 494);

    // текст для экрана настроек
    std::wstring settingsText = L"РАЗРАБОТЧИК:\n"
        L"Полянская Е.О, студентка группы ПИ-42 Алтайского государственного \nтехнического университета им. И.И. Ползунова\nБарнаул, Алтайский край\n"
        L"ПРАВИЛА ИГРЫ:\n"
        L"Управление: стрелки влево/вправо\n"
        L"Цель: уничтожить как можно больше кирпичей, набирая очки\n"
        L"Каждые 20 секунд сверху появляется новый ряд кирпичей\n"
        L"3 жизни (сердечки). Потеря жизни - мяч упал ниже платформы\n"
        L"Игра окончена, если:\n"
        L"- кирпичи достигли платформы\n"
        L"- закончились жизни\n"
        L"Удачи!";
    settingsInfo.setString(settingsText);

    // курсоры мыши
    sf::Cursor handCursor, arrowCursor;
    arrowCursor.loadFromSystem(sf::Cursor::Arrow);
    handCursor.loadFromSystem(sf::Cursor::Hand);

    // основные таймеры и флаги
    sf::Clock clock;
    bool allowGameOver = true;
    sf::Clock gameOverTimer;

    // главный игровой цикл
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // обработка событий
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                saveScores();
                window.close();
            }

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            // обработка кликов мыши
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (appMode == AppMode::MainMenu) {
                    for (int i = 0; i < 3; i++) {
                        if (menuButtons[i].bg.getGlobalBounds().contains(mousePos)) {
                            if (i == 0) {
                                // начать игру
                                appMode = AppMode::Playing;
                                game_state_obj.ResetGameState();
                                generateBricks();
                                for (int j = 0; j < 3; j++) {
                                    heartActive[j] = true;
                                    heartSprites[j].setTexture(heartFullTexture);
                                    heartSprites[j].setPosition(700 - j * 45, 7);
                                }
                                lifeState = LifeState::Normal;
                                paddle_obj.SetPosition(355, 550);
                                ball_obj.SetPosition(400, 530);
                                ball_obj.SetStuckToPaddle(false);
                                ball_obj.vector_x = 210;
                                ball_obj.vector_y = -240;
                                allowGameOver = true;
                            }
                            else if (i == 1) {
                                // настройки
                                appMode = AppMode::Settings;
                            }
                            else if (i == 2) {
                                saveScores();
                                window.close();
                            }
                            break;
                        }
                    }
                }
                else if (appMode == AppMode::Playing) {
                    if (pauseButtonBg.getGlobalBounds().contains(mousePos)) {
                        appMode = AppMode::Pause;
                    }
                }
                else if (appMode == AppMode::GameOver || appMode == AppMode::NewRecord) {
                    // обработка одинаковых кнопок для обоих экранов
                    NeonButton* buttons = (appMode == AppMode::GameOver) ? goButtons : newRecordButtons;
                    for (int i = 0; i < 2; i++) {
                        if (buttons[i].bg.getGlobalBounds().contains(mousePos)) {
                            if (i == 0) {
                                // играть снова
                                appMode = AppMode::Playing;
                                game_state_obj.ResetGameState();
                                generateBricks();
                                for (int j = 0; j < 3; j++) {
                                    heartActive[j] = true;
                                    heartSprites[j].setTexture(heartFullTexture);
                                    heartSprites[j].setPosition(700 - j * 45, 7);
                                }
                                lifeState = LifeState::Normal;
                                paddle_obj.SetPosition(355, 550);
                                ball_obj.SetPosition(400, 530);
                                ball_obj.SetStuckToPaddle(false);
                                ball_obj.vector_x = 210;
                                ball_obj.vector_y = -240;
                                allowGameOver = true;
                            }
                            else {
                                appMode = AppMode::MainMenu;
                            }
                            break;
                        }
                    }
                }
                else if (appMode == AppMode::Pause) {
                    for (int i = 0; i < 2; i++) {
                        if (pauseButtons[i].bg.getGlobalBounds().contains(mousePos)) {
                            if (i == 0) appMode = AppMode::Playing;
                            else appMode = AppMode::MainMenu;
                            break;
                        }
                    }
                }
                else if (appMode == AppMode::Settings) {
                    if (settingsBackButton.bg.getGlobalBounds().contains(mousePos)) {
                        appMode = AppMode::MainMenu;
                    }
                }
            }
        }

        // анимация кнопок и смена курсора
        sf::Vector2f mouseF = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        bool anyButtonHovered = false;

        // главное меню анимация (3 кнопки)
        if (appMode == AppMode::MainMenu) {
            for (int i = 0; i < 3; i++) {
                bool hovered = menuButtons[i].bg.getGlobalBounds().contains(mouseF);
                menuButtons[i].hovered = hovered;
                if (hovered) anyButtonHovered = true;
                menuButtons[i].pulse += dt * 2.0f;
                float glowAlpha = 80 + 50 * sinf(menuButtons[i].pulse);
                if (hovered) {
                    menuButtons[i].bg.setFillColor(sf::Color(20, 100, 200, 240));
                    menuButtons[i].bg.setOutlineColor(sf::Color(0, 255, 255, 255));
                    menuButtons[i].glow.setOutlineColor(sf::Color(0, 255, 255, glowAlpha + 100));
                    menuButtons[i].text.setFillColor(sf::Color::White);
                }
                else {
                    menuButtons[i].bg.setFillColor(sf::Color(10, 10, 30, 200));
                    menuButtons[i].bg.setOutlineColor(sf::Color(0, 255, 255, 150));
                    menuButtons[i].glow.setOutlineColor(sf::Color(0, 255, 255, glowAlpha));
                    menuButtons[i].text.setFillColor(neonCyan);
                }
            }
        }

        // остальные анимации кнопок (game over, new record, pause, settings)
        if (appMode == AppMode::GameOver) {
            for (int i = 0; i < 2; i++) {
                bool hovered = goButtons[i].bg.getGlobalBounds().contains(mouseF);
                goButtons[i].hovered = hovered;
                if (hovered) anyButtonHovered = true;
                goButtons[i].pulse += dt * 2.0f;
                float glowAlpha = 80 + 50 * sinf(goButtons[i].pulse);
                if (hovered) {
                    goButtons[i].bg.setFillColor(sf::Color(20, 100, 200, 240));
                    goButtons[i].bg.setOutlineColor(sf::Color(0, 255, 255, 255));
                    goButtons[i].glow.setOutlineColor(sf::Color(0, 255, 255, glowAlpha + 100));
                    goButtons[i].text.setFillColor(sf::Color::White);
                }
                else {
                    goButtons[i].bg.setFillColor(sf::Color(10, 10, 30, 200));
                    goButtons[i].bg.setOutlineColor(sf::Color(0, 255, 255, 150));
                    goButtons[i].glow.setOutlineColor(sf::Color(0, 255, 255, glowAlpha));
                    goButtons[i].text.setFillColor(neonCyan);
                }
            }
        }

        if (appMode == AppMode::NewRecord) {
            for (int i = 0; i < 2; i++) {
                bool hovered = newRecordButtons[i].bg.getGlobalBounds().contains(mouseF);
                newRecordButtons[i].hovered = hovered;
                if (hovered) anyButtonHovered = true;
                newRecordButtons[i].pulse += dt * 2.0f;
                float glowAlpha = 80 + 50 * sinf(newRecordButtons[i].pulse);
                if (hovered) {
                    newRecordButtons[i].bg.setFillColor(sf::Color(20, 100, 200, 240));
                    newRecordButtons[i].bg.setOutlineColor(sf::Color(0, 255, 255, 255));
                    newRecordButtons[i].glow.setOutlineColor(sf::Color(0, 255, 255, glowAlpha + 100));
                    newRecordButtons[i].text.setFillColor(sf::Color::White);
                }
                else {
                    newRecordButtons[i].bg.setFillColor(sf::Color(10, 10, 30, 200));
                    newRecordButtons[i].bg.setOutlineColor(sf::Color(0, 255, 255, 150));
                    newRecordButtons[i].glow.setOutlineColor(sf::Color(0, 255, 255, glowAlpha));
                    newRecordButtons[i].text.setFillColor(neonCyan);
                }
            }
        }

        if (appMode == AppMode::Pause) {
            for (int i = 0; i < 2; i++) {
                bool hovered = pauseButtons[i].bg.getGlobalBounds().contains(mouseF);
                pauseButtons[i].hovered = hovered;
                if (hovered) anyButtonHovered = true;
                pauseButtons[i].pulse += dt * 2.0f;
                float glowAlpha = 80 + 50 * sinf(pauseButtons[i].pulse);
                if (hovered) {
                    pauseButtons[i].bg.setFillColor(sf::Color(20, 100, 200, 240));
                    pauseButtons[i].bg.setOutlineColor(sf::Color(0, 255, 255, 255));
                    pauseButtons[i].glow.setOutlineColor(sf::Color(0, 255, 255, glowAlpha + 100));
                    pauseButtons[i].text.setFillColor(sf::Color::White);
                }
                else {
                    pauseButtons[i].bg.setFillColor(sf::Color(10, 10, 30, 200));
                    pauseButtons[i].bg.setOutlineColor(sf::Color(0, 255, 255, 150));
                    pauseButtons[i].glow.setOutlineColor(sf::Color(0, 255, 255, glowAlpha));
                    pauseButtons[i].text.setFillColor(neonCyan);
                }
            }
        }

        if (appMode == AppMode::Settings) {
            bool hovered = settingsBackButton.bg.getGlobalBounds().contains(mouseF);
            settingsBackButton.hovered = hovered;
            if (hovered) anyButtonHovered = true;
            settingsBackButton.pulse += dt * 2.0f;
            float glowAlpha = 80 + 50 * sinf(settingsBackButton.pulse);
            if (hovered) {
                settingsBackButton.bg.setFillColor(sf::Color(20, 100, 200, 240));
                settingsBackButton.bg.setOutlineColor(sf::Color(0, 255, 255, 255));
                settingsBackButton.glow.setOutlineColor(sf::Color(0, 255, 255, glowAlpha + 100));
                settingsBackButton.text.setFillColor(sf::Color::White);
            }
            else {
                settingsBackButton.bg.setFillColor(sf::Color(10, 10, 30, 200));
                settingsBackButton.bg.setOutlineColor(sf::Color(0, 255, 255, 150));
                settingsBackButton.glow.setOutlineColor(sf::Color(0, 255, 255, glowAlpha));
                settingsBackButton.text.setFillColor(neonCyan);
            }
        }

        // кнопка паузы в игре
        if (appMode == AppMode::Playing && pauseButtonBg.getGlobalBounds().contains(mouseF)) {
            anyButtonHovered = true;
        }

        // смена курсора мыши
        if (anyButtonHovered) window.setMouseCursor(handCursor);
        else window.setMouseCursor(arrowCursor);

        // игровая логика
        if (appMode == AppMode::Playing) {
            // обновление текста счета
            scoreTextUI.setString(L"Счет: " + std::to_wstring(game_state_obj.GetScore()));

            static bool justGeneratedBricks = false;

            // генерация нового ряда кирпичей каждые 20 секунд
            timeSinceNewRow = gameTimer.getElapsedTime().asSeconds();
            if (timeSinceNewRow >= row_interval && !justGeneratedBricks) {
                justGeneratedBricks = true;

                // копируем активные кирпичи
                std::vector<Brick> liveBricksCopy;
                for (const auto& brick : bricks) {
                    if (brick.IsActive()) {
                        liveBricksCopy.push_back(brick);
                    }
                }

                // очищаем и создаем новый ряд сверху
                bricks.clear();
                for (int col = 0; col < cols; ++col) {
                    float x = bricksAreaLeft + col * stepX;
                    if (x + brickW > 770.0f) continue;
                    float baseChance = 0.7f * (1.0f - col * 0.05f);
                    if (static_cast<float>(rand()) / RAND_MAX < baseChance) {
                        BrickType type = static_cast<BrickType>(rand() % 5);
                        bricks.emplace_back(static_cast<int>(x), static_cast<int>(bricksAreaTop), type);
                    }
                }

                // сдвигаем старые кирпичи вниз
                for (auto& brickCopy : liveBricksCopy) {
                    brickCopy.pos_y += stepY;
                    bricks.push_back(brickCopy);
                }

                gameTimer.restart();
                gameOverTimer.restart();
                allowGameOver = false;
            }
            else if (justGeneratedBricks) {
                justGeneratedBricks = false;
            }

            // проверка условий окончания игры
            bool bricksReachedPaddle = false;
            for (const auto& brick : bricks) {
                if (brick.GetBoundingBox().top >= 496) {
                    bricksReachedPaddle = true;
                    break;
                }
            }
            bool allHeartsDead = true;
            for (int i = 0; i < 3; i++) {
                if (heartActive[i]) {
                    allHeartsDead = false;
                    break;
                }
            }

            if ((bricksReachedPaddle || allHeartsDead) && allowGameOver) {
                int score = game_state_obj.GetScore();
                scoreHistory.push_back(score);
                saveScores();

                // находим текущий рекорд
                int record = 0;
                if (!scoreHistory.empty()) {
                    record = *std::max_element(scoreHistory.begin(), scoreHistory.end());
                }

                // проверка на новый рекорд
                if (score == record) { // новый рекорд (последний результат = максимум)
                    appMode = AppMode::NewRecord;
                    newRecordResultText.setString(L"Результат: " + std::to_wstring(score) + L" очков");
                    int oldRecord = 0;
                    // ищем предпоследний максимум для старого рекорда
                    for (int s : scoreHistory) {
                        if (s != score && s > oldRecord) oldRecord = s;
                    }
                    newRecordOldRecordText.setString(L"Старый рекорд: " + std::to_wstring(oldRecord) + L" очков");
                    centerText(newRecordResultText);
                    centerText(newRecordOldRecordText);
                }
                else {
                    // обычный game over
                    appMode = AppMode::GameOver;
                    goResultText.setString(L"Результат: " + std::to_wstring(score) + L" очков");
                    goRecordText.setString(L"Рекорд: " + std::to_wstring(record) + L" очков");
                    centerText(goResultText);
                    centerText(goRecordText);
                }
            }
            else {
                // обычная физика игры
                ball_obj.UpdateObject(dt);
                if (lifeState != LifeState::RespawnWait) paddle_obj.UpdateObject(dt);

                collision_manager.CheckBallVsPaddle(ball_obj, paddle_obj);
                collision_manager.CheckBallVsWalls(ball_obj, game_state_obj);
                collision_manager.CheckBallVsBricks(ball_obj, bricks, game_state_obj);

                // логика потери жизни
                if (ball_obj.GetBoundingBox().top > 600 && lifeState == LifeState::Normal) {
                    lifeState = LifeState::RespawnWait;
                    for (int i = 2; i >= 0; i--) {
                        if (heartActive[i]) {
                            heartActive[i] = false;
                            heartSprites[i].setPosition(-100, -100);
                            break;
                        }
                    }
                    paddle_obj.SetPosition(355, 550);
                    ball_obj.SetPosition(400, 530);
                    ball_obj.StartRespawn(0.7f);
                }
                if (lifeState == LifeState::RespawnWait && !ball_obj.IsRespawning()) {
                    lifeState = LifeState::Normal;
                }
            }
        }

        // таймер задержки game over после генерации рядов
        if (!allowGameOver && gameOverTimer.getElapsedTime().asSeconds() > 1.5f) {
            allowGameOver = true;
        }

        // отрисовка
        window.clear();
        window.draw(backgroundSprite);

        if (appMode == AppMode::MainMenu) {
            window.draw(titleGlow);
            window.draw(titleText);
            for (int i = 0; i < 3; i++) {
                window.draw(menuButtons[i].glow);
                window.draw(menuButtons[i].bg);
                window.draw(menuButtons[i].text);
            }
        }
        else if (appMode == AppMode::Playing) {
            window.draw(scoreTextUI);
            window.draw(pauseButtonBg);
            window.draw(pauseBar1);
            window.draw(pauseBar2);
            for (int i = 0; i < 3; i++)
                if (heartActive[i]) window.draw(heartSprites[i]);
            paddle_obj.DrawObject(window);
            ball_obj.DrawObject(window);
            for (auto& brick : bricks) brick.DrawObject(window);
        }
        else if (appMode == AppMode::Pause) {
            // игра заморожена
            window.draw(scoreTextUI);
            window.draw(pauseButtonBg);
            window.draw(pauseBar1);
            window.draw(pauseBar2);
            for (int i = 0; i < 3; i++)
                if (heartActive[i]) window.draw(heartSprites[i]);
            paddle_obj.DrawObject(window);
            ball_obj.DrawObject(window);
            for (auto& brick : bricks) brick.DrawObject(window);

            window.draw(pauseOverlay);
            window.draw(pauseTitle);
            for (int i = 0; i < 2; i++) {
                window.draw(pauseButtons[i].glow);
                window.draw(pauseButtons[i].bg);
                window.draw(pauseButtons[i].text);
            }
        }
        else if (appMode == AppMode::GameOver) {
            // игра заморожена под оверлеем
            window.draw(scoreTextUI);
            window.draw(pauseButtonBg);
            window.draw(pauseBar1);
            window.draw(pauseBar2);
            for (int i = 0; i < 3; i++)
                if (heartActive[i]) window.draw(heartSprites[i]);
            paddle_obj.DrawObject(window);
            ball_obj.DrawObject(window);
            for (auto& brick : bricks) brick.DrawObject(window);

            window.draw(gameOverOverlay);
            window.draw(goTitleText);
            window.draw(goLineTop);
            window.draw(goResultText);
            window.draw(goRecordText);
            window.draw(goLineBottom);
            for (int i = 0; i < 2; i++) {
                window.draw(goButtons[i].glow);
                window.draw(goButtons[i].bg);
                window.draw(goButtons[i].text);
            }
        }
        else if (appMode == AppMode::NewRecord) {
            // игра заморожена под оверлеем нового рекорда
            window.draw(scoreTextUI);
            window.draw(pauseButtonBg);
            window.draw(pauseBar1);
            window.draw(pauseBar2);
            for (int i = 0; i < 3; i++)
                if (heartActive[i]) window.draw(heartSprites[i]);
            paddle_obj.DrawObject(window);
            ball_obj.DrawObject(window);
            for (auto& brick : bricks) brick.DrawObject(window);

            window.draw(gameOverOverlay);
            window.draw(newRecordTitleText);
            window.draw(newRecordLineTop);
            window.draw(newRecordResultText);
            window.draw(newRecordOldRecordText);
            window.draw(newRecordLineBottom);
            for (int i = 0; i < 2; i++) {
                window.draw(newRecordButtons[i].glow);
                window.draw(newRecordButtons[i].bg);
                window.draw(newRecordButtons[i].text);
            }
        }
        else if (appMode == AppMode::Settings) {
            // экран настроек
            window.draw(settingsTitle);
            window.draw(settingsInfo);
            window.draw(settingsBackButton.glow);
            window.draw(settingsBackButton.bg);
            window.draw(settingsBackButton.text);
        }

        window.display();
    }

    // сохранение рекордов при выходе
    saveScores();
    return 0;
}
