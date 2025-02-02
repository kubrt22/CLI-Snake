#pragma once

#include "curses.h"

const int DELAY = 230;

struct GameOptions {
    inline static int DIFFICULTY = 1;
    inline static bool FIXED_SIZE = true;
    inline static int MAP_SIZE = 20;
    inline static bool COLOR = false;
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
    w = 'w',
    a = 'a',
    s = 's',
    d = 'd',
    q = 'q',
    W = 'W',
    A = 'A',
    S = 'S',
    D = 'D',
    Q = 'Q',
    UP = KEY_UP,
    DOWN = KEY_DOWN,
    LEFT = KEY_LEFT,
    RIGHT = KEY_RIGHT,
    ESCAPE = 27
};

struct Point {
    int y;
    int x;
};

void argumentParser(int argc, char *argv[]);