#pragma once
#include "Brick.hpp"
#include <vector>

class Level {
private:
    int level_number;
    std::vector<Brick*> bricks_list;
    int bricks_remaining;
    sf::Sprite background_level;

public:
    Level();
    ~Level();

    void LoadLevel(int level_number);
    bool IsCompletedLevel() const;
    void CheckBrickCollisionBall(Ball& ball);
};
