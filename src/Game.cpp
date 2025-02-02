#include "Game.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "Utils.hpp"
#include <ctime>
#include <cstdlib>
#include "curses.h"
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

void Game::Run() {
    int ch, ch2 = ERR;

#ifdef _DEBUG
    int debugWidth = 16;
    debugWin = newwin(winHeight, debugWidth, winStartY, winStartX - debugWidth);
    box(debugWin, 0, 0);
    wrefresh(debugWin);
#endif
    
    while (true) {

        if (ch2 == ERR) {
            ch = wgetch(gameWin);
        } else {
            ch = ch2;
        }
        ch2 = wgetch(gameWin);
        flushinp();

        snake->Move(ch);
        snake->Update(*food, winMaxY, winMaxX);

        werase(gameWin);

        food->Draw(gameWin);
        snake->Draw(gameWin);
        box(gameWin, 0, 0);

        if (GameOptions::FIXED_SIZE) {
            mvprintw(getbegy(gameWin) - 1, getbegx(gameWin), "Score: %d", snake->GetTail().size() - Snake::INITIAL_TAIL_SIZE);
        } else {
            mvwprintw(gameWin, 0, 1, "Score: %d", snake->GetTail().size() - Snake::INITIAL_TAIL_SIZE);
        }

        wrefresh(gameWin);
        refresh();

#ifdef _DEBUG
        // Update debug window with snake and food coordinates.
        werase(debugWin);
        box(debugWin, 0, 0);
        // Assuming snake.GetTail()[0] holds the head coordinate.
        Point snakeHead = snake->GetTail().front();
        mvwprintw(debugWin, 1, 1, "Snake: %2d, %2d", snakeHead.y, snakeHead.x);
        mvwprintw(debugWin, 2, 1, "Food:  %2d, %2d", food->pos.y, food->pos.x);
        // Room for additional debug info can be added here.
        wrefresh(debugWin);
#endif

        if (snake->CheckCollision(winMaxY, winMaxX) || ch == (int)Key::ESCAPE || ch == (int)Key::q || ch == (int)Key::Q) {
            if (snake->CheckCollision(winMaxY, winMaxX)) {
                GameOverScreen(screenMaxY, screenMaxX);
            }
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY - GameOptions::DIFFICULTY * 30));
    }
}

void Game::Init() {
    initscr(); // Init ncurses
    cbreak(); // Disable line buffering
    noecho(); // Disable echoing of characters
    curs_set(0); // Hide cursor
    clear();
    refresh();

    if (has_colors() && GameOptions::COLOR) {
        start_color();
        use_default_colors();

        init_pair(1, COLOR_RED, -1);
        init_pair(2, COLOR_GREEN, -1);
    }

    if (!has_colors() && GameOptions::COLOR) {
        endwin();
        std::cerr << "Your terminal does not support color" << std::endl;
        exit(1);
    }

    getmaxyx(stdscr, screenMaxY, screenMaxX);
    winWidth = GameOptions::FIXED_SIZE ? GameOptions::MAP_SIZE * 2 : screenMaxX;
    winHeight = GameOptions::FIXED_SIZE ? GameOptions::MAP_SIZE : screenMaxY;
    winStartX = GameOptions::FIXED_SIZE ? screenMaxX / 2 - GameOptions::MAP_SIZE : 0;
    winStartY = GameOptions::FIXED_SIZE ? screenMaxY / 2 - GameOptions::MAP_SIZE / 2 : 0;
    
    gameWin = newwin(winHeight, winWidth, winStartY, winStartX);
    nodelay(gameWin, true);

    winMaxY = winHeight;
    winMaxX = winWidth;

    snake = std::make_unique<Snake>(Point{winHeight / 2, winWidth / 2});
    food = std::make_unique<Food>(winHeight, winWidth, snake->GetTail());
}

void Game::Exit() {
    if (gameWin) {
        delwin(gameWin);
    }
    if (debugWin) {
        delwin(debugWin);
    }
    endwin();
}

void Game::GameOverScreen(int maxY, int maxX) {
    const std::string gameOver = "Game Over";
    const int height = 3;
    const int width = gameOver.size() + 4;
    WINDOW *win = newwin(height, width, maxY / 2 - height / 2, maxX / 2 - width / 2);

    mvwprintw(win, 1, 2, gameOver.c_str());
    box(win, 0, 0);

    wrefresh(win);
    nodelay(stdscr, false);
    getch();
    delwin(win);
}