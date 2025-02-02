#pragma once

#include "curses.h"
#include "Utils.hpp"

#include <vector>
#include <random>

class Food {
public:
    Point pos;
    int width = 1;
    std::mt19937 gen;
public:
    Food(int MaxY, int MaxX, std::vector<Point> tail = {});
    void Draw(WINDOW *win);
    void Move(int maxY, int maxX, std::vector<Point> tail = {});
    bool CheckCollision(Point xy);
};