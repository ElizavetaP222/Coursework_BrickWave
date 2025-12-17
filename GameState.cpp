#include "GameState.hpp"
GameState::GameState() {
    score_game = 0;
    lives_game = 3;      // 3 попыток
    deaths = 0;
    flag_game_over = false;
    high_score = 0;
}

void GameState::LoseLifeGame() {
    deaths++;           // ещё одно падение
    lives_game--;        // ещё одна потерянная жизнь

    if (deaths >= 3) {  // game over ровно после 3 падений
        flag_game_over = true;
    }
}

void GameState::ResetGameState() {
    score_game = 0;
    lives_game = 3;
    deaths = 0;
    flag_game_over = false;
}

void GameState::AddPointsGame(int points) {
    score_game += points;
    if (score_game > high_score) {
        high_score = score_game;
    }
}

bool GameState::IsGameOverGame() const {
    return flag_game_over;
}

void GameState::GainLifeGame() {
    lives_game++;
}

