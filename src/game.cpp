#include "game.hpp"
#include "snake.hpp"
#include "food.hpp"
#include "utils.hpp"
#include "bfs.hpp"
#include "auto.hpp"

#include "curses.h"

#include <string>
#include <thread>
#include <chrono>
#include <iostream>

void Game::Run() {
    int ch, ch2 = ERR;
    int charGlobal = ERR;
    int buff = ERR;

#ifdef _DEBUG
    int debugWidth = 16;
    debugWin = newwin(winHeight, debugWidth, winStartY, winStartX - debugWidth);
    box(debugWin, 0, 0);
    wrefresh(debugWin);
#endif

    Autoplay autoPlay;
    
    uint32_t iter = 0;
    while (++iter) {
        //- Somewhat working code
        buff = ERR;
        bool skipUpdate = false;
        charGlobal = wgetch(gameWin);
        if (!HandleKeyPress(charGlobal)) {
            buff = charGlobal;
        } else {
            skipUpdate = true;
        }

        if (!skipUpdate) {
            if (autoplay) {
                ch = autoPlay.charQueue.front();
                autoPlay.charQueue.erase(autoPlay.charQueue.begin());
            } else if (manual && iter != 1) {
                ch = getch();
            } else {
                ch = wgetch(gameWin);
                if (buff != ERR) {
                    ch = buff;
                }
            }
        }
        flushinp();

        if (!manual || !HandleKeyPress(ch) || skipUpdate) {
            snake->Move(ch);
            snake->Update(*food, winMaxY, winMaxX);   

            bfs->updateGrid(snake->GetTail(), food->pos);
            bfs->search();

            autoPlay.updatePath(bfs->path);
            autoPlay.updateCharQueue();

            skipUpdate = false;
        }

        werase(gameWin);


        snake->Draw(gameWin);
        food->Draw(gameWin);
        if (show_path) {
            bfs->DrawPath(gameWin);
        }

        box(gameWin, 0, 0);

        if (GameOptions::FIXED_SIZE) {
            mvprintw(getbegy(gameWin) - 1, getbegx(gameWin), "Score: %d", (int)snake->GetTail().size() - Snake::INITIAL_TAIL_SIZE);
        } else {
            mvwprintw(gameWin, 0, 1, "Score: %d", (int)snake->GetTail().size() - Snake::INITIAL_TAIL_SIZE);
        }

        wrefresh(gameWin);
        refresh();

#ifdef _DEBUG
        werase(debugWin);
        box(debugWin, 0, 0);

        Point snakeHead = snake->GetTail().front();
        mvwprintw(debugWin, 1, 1, "Snake: %2d, %2d", snakeHead.y, snakeHead.x);
        mvwprintw(debugWin, 2, 1, "Food:  %2d, %2d", food->pos.y, food->pos.x);

        mvwprintw(debugWin, 4, 1, "Moves:");
        for (int i = 0; i < autoPlay.charQueue.size(); i++) {
            int y = 5 + i / 7;
            int x = 1 + i * 2 % 14;
            mvwprintw(debugWin, y, x, "%c", autoPlay.charQueue[i]);
        }

        if (GameOptions::COLOR) {
            wmove(debugWin, winHeight - 2, 1);
            wattron(debugWin, A_REVERSE );
            wattron(debugWin, COLOR_PAIR(COLOR_RED));
            wprintw(debugWin, "  ");
            wattron(debugWin, COLOR_PAIR(COLOR_GREEN));
            wprintw(debugWin, "  ");
            wattron(debugWin, COLOR_PAIR(COLOR_YELLOW));
            wprintw(debugWin, "  ");
            wattron(debugWin, COLOR_PAIR(COLOR_BLUE));
            wprintw(debugWin, "  ");
            wattron(debugWin, COLOR_PAIR(COLOR_MAGENTA));
            wprintw(debugWin, "  ");
            wattron(debugWin, COLOR_PAIR(COLOR_CYAN));
            wprintw(debugWin, "  ");
            wattron(debugWin, COLOR_PAIR(COLOR_WHITE));
            wprintw(debugWin, "  ");
            wstandend(debugWin);
        }
        if (manual && !autoplay) {
            mvwprintw(debugWin, winHeight - 3, 1, "Manual");
        }
        if (show_path) {
            mvwprintw(debugWin, winHeight - 4, 1, "Show Path");
        }
        if (autoplay) {
            mvwprintw(debugWin, winHeight - 5, 1, "Autoplay");
        }

        wrefresh(debugWin);
#endif

        if (snake->CheckCollision(winMaxY, winMaxX)) {
            GameOverScreen(screenMaxY, screenMaxX);
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

        init_pair(COLOR_RED, COLOR_RED, -1);
        init_pair(COLOR_GREEN, COLOR_GREEN, -1);
        init_pair(COLOR_YELLOW, COLOR_YELLOW, -1);
        init_pair(COLOR_BLUE, COLOR_BLUE, -1);
        init_pair(COLOR_MAGENTA, COLOR_MAGENTA, -1);
        init_pair(COLOR_CYAN, COLOR_CYAN, -1);
        init_pair(COLOR_WHITE, COLOR_WHITE, -1);
        init_pair(COLOR_BLACK, COLOR_BLACK, -1);
    }

    if (!has_colors() && GameOptions::COLOR) {
        endwin();
        std::cerr << "Your terminal does not support color" << std::endl;
        std::cerr << "Try disabling color with the '--no-color' parameter" << std::endl;
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
    bfs = std::make_unique<BFS>(winWidth, winHeight, GameOptions::BLOCK);
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

bool Game::HandleKeyPress(int ch) {
    ch = std::toupper(ch);
    switch ((Key)ch) {
        case Key::MANUAL_MODE:
            manual = !manual;
            return true;
        case Key::SHOW_PATH:
            show_path = !show_path;
            return true;
        case Key::AUTOPLAY:
            autoplay = !autoplay;
            return true;
        case Key::ESCAPE:
        case Key::Q:
            Exit();
            exit(0);
        default:
            return false;
    }
}