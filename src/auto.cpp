#include "auto.hpp"

#include <ncurses.h>

#include <vector>

void Autoplay::start() {
    
}

void Autoplay::stop() {
    
}

void Autoplay::updatePath(std::vector<Point> path) {
    this->path = path;
}

void Autoplay::updateCharQueue() {
    charQueue.clear();
    Point prev = path[0];
    for (Point p : path) {
        Point diff = {p.y - prev.y, p.x - prev.x};
        if (diff.y == 1) {
            charQueue.push_back((int)Key::S);
        } else if (diff.y == -1) {
            charQueue.push_back((int)Key::W);
        } else if (diff.x == 1 || diff.x == 2) {
            charQueue.push_back((int)Key::D);
        } else if (diff.x == -1 || diff.x == -2) {
            charQueue.push_back((int)Key::A);
        }

        prev = p;
    }   
}