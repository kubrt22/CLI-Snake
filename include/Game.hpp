#pragma once

#include "curses.h"
#include "Snake.hpp"
#include "Food.hpp"

#include <memory>

class Game {
private:
    int screenMaxY, screenMaxX;
    int winWidth, winHeight, winStartX, winStartY;
    int winMaxY, winMaxX;

    WINDOW* gameWin = nullptr;
    WINDOW* debugWin = nullptr;

    std::unique_ptr<Snake> snake = nullptr;
    std::unique_ptr<Food> food = nullptr;

private:
    void GameOverScreen(int maxY, int maxX);
public:
    void Init();
    void Run();
    void Exit();
};
