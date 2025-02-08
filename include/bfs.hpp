#pragma once

#include <utils.hpp>

#include <vector>

enum class Tile {
    EMPTY,
    SNAKE,
    FOOD,
    VISITED,
    QUEUED,
    PATH
};

class BFS {
private:
    std::vector<Point> snake;
    Point food;
    bool block;
public:
    std::vector<std::vector<std::pair<Tile, Point>>> grid;
    std::vector<Point> visited;
    std::vector<Point> path;
    std::vector<Point> queue;

    BFS(int width, int height, bool block = true);
    void updateGrid(std::vector<Point> snake, Point food);
    void search();
    void DrawPath(WINDOW* win);
};