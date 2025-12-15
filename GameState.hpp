#pragma once

class GameState {
private:
    int score_game;
    int lives_game;
    int current_level;
    bool flag_game_over;
    int high_score;

public:
    GameState();

    void AddPointsGame(int points);
    void LoseLifeGame();
    bool IsGameOverGame() const;
    void GainLifeGame();
    void ResetGameState();
    void LoadLevelGame(int level);
};
