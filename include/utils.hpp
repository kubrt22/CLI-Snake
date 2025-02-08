#pragma once

#include "curses.h"

const int DELAY = 230;

struct GameOptions {
    inline static int DIFFICULTY = 1;
    inline static bool FIXED_SIZE = true;
    inline static int MAP_SIZE = 20;
    inline static bool COLOR = true;
    inline static bool BLOCK = true;
    inline static bool WIDE = true;
    inline static bool BLOCK_FOOD = true;
};

//- Point system is Y, X
//? Normally it is the other way around

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};
enum class Key {
    W = 'W',
    A = 'A',
    S = 'S',
    D = 'D',
    Q = 'Q',
    UP = KEY_UP,
    DOWN = KEY_DOWN,
    LEFT = KEY_LEFT,
    RIGHT = KEY_RIGHT,
    ESCAPE = 27,
    MANUAL_MODE = 'M',
    SHOW_PATH = 'P',
    AUTOPLAY = 'N'
};

struct Point {
    int y;
    int x;

    bool operator==(Point p) {
        return y == p.y && x == p.x;
    }
};

void argumentParser(int argc, char *argv[]);