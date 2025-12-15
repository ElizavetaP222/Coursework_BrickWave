#include "GameState.hpp"

GameState::GameState() {
    score_game = 0;
    lives_game = 3;
    current_level = 1;
    flag_game_over = false;
    high_score = 0;
}

void GameState::AddPointsGame(int points) {
    score_game += points;
    if (score_game > high_score) {
        high_score = score_game;
    }
}

void GameState::LoseLifeGame() {
    lives_game--;
    if (lives_game <= 0) {
        flag_game_over = true;
    }
}

bool GameState::IsGameOverGame() const {
    return flag_game_over;
}

void GameState::GainLifeGame() {
    lives_game++;
}

void GameState::ResetGameState() {
    score_game = 0;
    lives_game = 3;
    current_level = 1;
    flag_game_over = false;
}

void GameState::LoadLevelGame(int level) {
    current_level = level;
}
