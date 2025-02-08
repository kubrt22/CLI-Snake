#pragma once

#include "snake.hpp"
#include "food.hpp"
#include "bfs.hpp"

#include "curses.h"

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
    std::unique_ptr<BFS> bfs = nullptr;

    bool manual = false;
    bool show_path = false;
    bool autoplay = false;

private:
    void GameOverScreen(int maxY, int maxX);
    bool HandleKeyPress(int ch);
public:
    void Init();
    void Run();
    void Exit();
};
