#include "food.hpp"
#include "utils.hpp"

#include "curses.h"

#include <string>
#include <vector>
#include <random>

Food::Food(int MaxY, int MaxX, std::vector<Point> tail) {
    if (GameOptions::WIDE) {
        width = 2;
    }

    gen.seed(std::random_device()());

    Move(MaxY, MaxX, tail);
}
void Food::Draw(WINDOW *win) {
    char boxChar = 'x';
    int boxWidth = 1;
    if (GameOptions::BLOCK_FOOD) {
        wattron(win, A_REVERSE | A_DIM);
        boxChar = ' ';
        boxWidth = width;
    }
    if (GameOptions::COLOR) {
        wattron(win, COLOR_PAIR(COLOR_RED));
    }
    mvwprintw(win, pos.y, pos.x, std::string(boxWidth, boxChar).c_str());
    wattroff(win, A_REVERSE | A_DIM | COLOR_PAIR(COLOR_RED));
}

void Food::Move(int maxY, int maxX, std::vector<Point> tail) {
    std::uniform_int_distribution<> distY(1, maxY - 2);
    std::uniform_int_distribution<> distX(width, maxX - width * 3 + 1);

    pos.y = distY(gen);
    pos.x = distX(gen);

    for (std::size_t i = 0; i < tail.size(); i++) {
        if (CheckCollision(tail[i])) {
            Move(maxY, maxX, tail);
            i = 0;
        }
    }
    if (GameOptions::BLOCK_FOOD || GameOptions::WIDE) {
        pos.x = pos.x + pos.x % width;
    }
}

bool Food::CheckCollision(Point xy) {
    if ( (xy.y == pos.y && xy.x == pos.x) || (xy.y == pos.y && xy.x + 1 == pos.x && GameOptions::WIDE) ) {
        return true;
    }
    return false;
}