#include "Snake.hpp"
#include "Food.hpp"
#include "Utils.hpp" // For globals like WIDE
#include "curses.h"
#include <string>

Direction Snake::HandleKeyPress(int ch) {
    switch ((Key)ch) {
        case Key::w:
        case Key::W:
        case Key::UP:
            return UP;
        case Key::s:
        case Key::S:
        case Key::DOWN:
            return DOWN;
        case Key::a:
        case Key::A:
        case Key::LEFT:
            return LEFT;
        case Key::d:
        case Key::D:
        case Key::RIGHT:
            return RIGHT;
        default:
            return direction;
    }
}

void Snake::HandleFoodCollision(Food &food, int maxY, int maxX, Point prev) {
    if (food.CheckCollision(pos)) {
        tail.push_back(prev);
        food.Move(maxY, maxX, tail);
    }
}

Snake::Snake(Point xy, int tailSize)
    : pos(xy), direction(RIGHT), width(GameOptions::WIDE ? 2 : 1) {
    for (int i = 0; i < tailSize; i++) {
        this->tail.push_back({pos.y, pos.x - i * width});
    }
}

int Snake::GetTailSize() {
    return tail.size();
}

std::vector<Point> Snake::GetTail() {
    return tail;
}

void Snake::Draw(WINDOW *win) {
    char boxChar = 'o';
    if (GameOptions::BLOCK) {
        boxChar = ' ';
        wattron(win, A_REVERSE | A_BOLD);
    }
    if (GameOptions::COLOR) {
        wattron(win, COLOR_PAIR(2));
    }
    for (std::size_t i = 0; i < tail.size(); i++) {
        mvwprintw(win, tail[i].y, tail[i].x, std::string(width, boxChar).c_str());
    }
    wattroff(win, A_REVERSE | A_BOLD | COLOR_PAIR(2));
}

void Snake::Move(int ch) {
    direction = HandleKeyPress(ch);
    switch (direction) {
        case UP:
            pos.y--;
            break;
        case DOWN:
            pos.y++;
            break;
        case LEFT:
            pos.x -= width;
            break;
        case RIGHT:
            pos.x += width;
            break;
    }
}

void Snake::Update(Food &food, int maxY, int maxX) {
    Point prev, prev2;
    prev = tail[0]; // Save prev pos of the head in vector
    tail[0] = pos; // Update head pos

    for (std::size_t i = 1; i < tail.size(); i++) {
        prev2 = tail[i]; // Save prev pos of the tail in vector
        tail[i] = prev; // Update tail pos
        prev = prev2; // Update prev pos
    }

    HandleFoodCollision(food, maxY, maxX, prev);
}

bool Snake::CheckCollision(int maxY, int maxX) {
    for (std::size_t i = 1; i < tail.size(); i++) {
        if (pos.y == tail[i].y && pos.x == tail[i].x) {
            return true;
        }
    }

    if (pos.y <= 0 || pos.y >= maxY - 1 || pos.x <= 0 || pos.x >= maxX - width) {
        return true;
    }

    return false;
}