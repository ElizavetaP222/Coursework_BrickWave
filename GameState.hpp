#pragma once

class GameState {
private:
    // текущий счет игрока
    int score_game;
    // количество жизней
    int lives_game;
    // текущий уровень
    int current_level;
    // флаг окончания игры
    bool flag_game_over;
    // рекордный счет
    int high_score;

public:
    GameState();

    // добавление очков проверка рекорда
    void AddPointsGame(int points);
    // потеря жизни проверка game over
    void LoseLifeGame();
    // проверка окончания игры
    bool IsGameOverGame() const;
    // добавление жизни
    void GainLifeGame();
    // сброс состояния для новой игры
    void ResetGameState();
    // загрузка уровня
    void LoadLevelGame(int level);

    int GetScore() const { return score_game; }
    int GetLives() const { return lives_game; }
};
