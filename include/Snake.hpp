#pragma once
#include <vector>
#include "curses.h"
#include "Utils.hpp"
#include "Food.hpp"

class Snake {
private:
    Point pos;
    Direction direction;
    int width = 1;

    std::vector<Point> tail;

public:
    static const int INITIAL_TAIL_SIZE = 5;

private:
    Direction HandleKeyPress(int ch);

    void HandleFoodCollision(Food &food, int maxY, int maxX, Point prev);
public:
    Snake(Point xy, int tailSize = INITIAL_TAIL_SIZE);

    int GetTailSize();

    std::vector<Point> GetTail();

    void Draw(WINDOW *win);

    void Move(int ch);

    void Update(Food &food, int maxY, int maxX);

    bool CheckCollision(int maxY, int maxX);
};
