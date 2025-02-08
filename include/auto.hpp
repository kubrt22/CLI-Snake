#pragma once

#include <vector>
#include <utils.hpp>

class Autoplay {

public:
    std::vector<int> charQueue;
    std::vector<Point> path;

    void start();
    void stop();
    void updatePath(std::vector<Point> path);
    void updateCharQueue();
};