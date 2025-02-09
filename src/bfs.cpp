#include "bfs.hpp"
#include "utils.hpp"

#include "curses.h"

#include <vector>
#include <algorithm>
#include <string>

const std::vector<Point> DIRECTIONS = {
    {1, 0}, // down
    {0, 1}, // right
    {-1, 0}, // up
    {0, -1} // left
};

BFS::BFS(int width, int height, bool block) {
    this->block = block;
    height -= 1;
    width -= 2;
    grid = std::vector<std::vector<std::pair<Tile, Point>>>(height, std::vector<std::pair<Tile, Point>>(width, {Tile::EMPTY, {-1, -1}}));
}

void BFS::updateGrid(std::vector<Point> snake, Point food) {
    this->snake = snake;
    this->food = food;
    grid = std::vector<std::vector<std::pair<Tile, Point>>>(grid.size(), std::vector<std::pair<Tile, Point>>(grid[0].size(), {Tile::EMPTY, {-1, -1}}));

    for (Point p : snake) {
        grid[p.y][p.x].first = Tile::SNAKE;
    }

    grid[food.y][food.x].first = Tile::FOOD;
}

void BFS::search() {
    Point start = snake[0];
    Point end = food;
    visited.clear();
    path.clear();
    queue.clear();

    queue.push_back(start);

    while (!queue.empty()) {
        Point current = queue.front();
        queue.erase(queue.begin());

        if (current == end) {
            Point p = current;
            while (!(p == start)) {
                path.push_back(p);
                p = grid[p.y][p.x].second;
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return;
        }

        visited.push_back(current);

        for (int i = 0; i < 4; i++) {
            Point next = {current.y + DIRECTIONS[i].y, current.x + DIRECTIONS[i].x};
            if (block && i == 1) {
                next.x++; // Right
            } else if (block && i == 3) {
                next.x--; // Left
            }

            if (next.x <= 0 || next.x >= (int)grid[0].size() || next.y <= 0 || next.y >= (int)grid.size()) {
                continue;
            }

            if (grid[next.y][next.x].first == Tile::SNAKE || grid[next.y][next.x].first == Tile::VISITED) {
                continue;
            }

            if (std::find(visited.begin(), visited.end(), next) == visited.end()) {
                grid[next.y][next.x].first = Tile::VISITED;
                grid[next.y][next.x].second = current;
                queue.push_back(next);
            }
        }
    }
}

void BFS::DrawPath(WINDOW* win) {
    char ch = 'x';
    int width = 1;
    if (GameOptions::BLOCK && GameOptions::COLOR) {
        ch = ' ';
        wattron(win, A_REVERSE);
    }
    if (GameOptions::WIDE) {
        width = 2;
    }
    for (Point p : path) {
        if (p == snake[0] || p == food) {
            continue;
        }
        mvwprintw(win, p.y, p.x, std::string(width, ch).c_str());
    }

    wattroff(win, COLOR_PAIR(COLOR_BLUE) | A_BOLD | A_REVERSE);
}