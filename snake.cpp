#include <iostream>
#include <vector>
#include <ctime>
#include <thread>

#include <ncurses.h>
#include "clipp.h"

const int DELAY = 230;

int MAP_SIZE = 20;
bool FIXED_SIZE = true;
int DIFFICULTY = 1;
bool COLOR = false;
bool BLOCK = true; 
bool WIDE = true; 
bool BLOCK_FOOD = true;

//- Point system is Y, X
//? Normally it is the other way around

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};
enum class Key : wchar_t {
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

void ncursesSetup();
void GameOver(WINDOW *win, int maxY, int maxX);
void charbox(WINDOW *win, int y, int x, int height, int width);
void argumentParser(int argc, char *argv[]);

class Food {
public:
    Point pos;
    int width = 1;
public:
    Food(int MaxY, int MaxX, std::vector<Point> tail = {}) {
        if (WIDE) {
            width = 2;
        }
        Move(MaxY, MaxX, tail);
    }
    void Draw(WINDOW *win) {
        char boxChar = 'x';
        int boxWidth = 1;
        if (BLOCK_FOOD) {
            wattron(win, A_REVERSE | A_DIM);
            boxChar = ' ';
            boxWidth = width;
        }
        if (COLOR) {
            wattron(win, COLOR_PAIR(1));
        }
        mvwprintw(win, pos.y, pos.x, std::string(boxWidth, boxChar).c_str());
        mvwprintw(win, 1, 1, std::to_string(pos.x).c_str());
        wattroff(win, A_REVERSE | A_DIM | COLOR_PAIR(1));
    }

    void Move(int maxY, int maxX, std::vector<Point> tail = {}) {
        pos.y = rand() % (maxY - 2) + 1;
        pos.x = rand() % (maxX - width * 3 + 1) + width; // 3 is a magic number

        for (std::size_t i = 0; i < tail.size(); i++) {
            if (CheckCollision(tail[i])) {
                Move(maxY, maxX, tail);
                i = 0;
            }
        }
        if (BLOCK_FOOD || WIDE) {
            pos.x = pos.x + pos.x % width;
        }
    }

    bool CheckCollision(Point xy) {
        if ( (xy.y == pos.y && xy.x == pos.x) || (xy.y == pos.y && xy.x + 1 == pos.x && WIDE) ) {
            return true;
        }
        return false;
    }
};

class Snake {
private:
    Point pos;
    Direction direction;
    int width = 1;

    std::vector<Point> tail;

public:
    static const int INITIAL_TAIL_SIZE = 5;

private:
    Direction HandleKeyPress(wchar_t ch) {
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

    void HandleFoodCollision(Food &food, int maxY, int maxX, Point prev) {
        if (food.CheckCollision(pos)) {
            tail.push_back(prev);
            food.Move(maxY, maxX, tail);
        }
    }
public:
    Snake(Point xy, int tailSize = INITIAL_TAIL_SIZE)
        : pos(xy), direction(RIGHT), width(WIDE ? 2 : 1) {
        for (int i = 0; i < tailSize; i++) {
            this->tail.push_back({pos.y, pos.x - i * width});
        }
    }

    int GetTailSize() {
        return tail.size();
    }

    std::vector<Point> GetTail() {
        return tail;
    }

    void Draw(WINDOW *win) {
        char boxChar = 'o';
        if (BLOCK) {
            boxChar = ' ';
            wattron(win, A_REVERSE | A_BOLD);
        }
        if (COLOR) {
            wattron(win, COLOR_PAIR(2));
        }
        for (std::size_t i = 0; i < tail.size(); i++) {
            mvwprintw(win, tail[i].y, tail[i].x, std::string(width, boxChar).c_str());
        }
        wattroff(win, A_REVERSE | A_BOLD | COLOR_PAIR(2));

        mvwprintw(win, 2, 1, std::to_string(pos.x).c_str());
    }

    void Move(wchar_t ch) {
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

    void Update(Food &food, int maxY, int maxX) {
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

    bool CheckCollision(int maxY, int maxX) {
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
};

class Game {
private:
    int screenMaxY, screenMaxX;
    int winWidth, winHeight, winStartX, winStartY;
    int winMaxY, winMaxX;

private:
    
public:
    void Run() {
        ncursesSetup();

        getmaxyx(stdscr, screenMaxY, screenMaxX);
        winWidth = FIXED_SIZE ? MAP_SIZE * 2 : screenMaxX;
        winHeight = FIXED_SIZE ? MAP_SIZE : screenMaxY;
        winStartX = FIXED_SIZE ? screenMaxX / 2 - MAP_SIZE : 0;
        winStartY = FIXED_SIZE ? screenMaxY / 2 - MAP_SIZE / 2 : 0;
        
        WINDOW *win = newwin(winHeight, winWidth, winStartY, winStartX);
        nodelay(win, true);

        winMaxY = winHeight;
        winMaxX = winWidth;

        srand(time(0));

        Snake snake({ winMaxY / 2, winMaxX / 2 - (winMaxX / 2 % 2) });
        Food food(winMaxY, winMaxX, snake.GetTail());

        wchar_t ch, ch2 = ERR;
        
        while (true) {

            if (ch2 == ERR) {
                ch = wgetch(win);
            } else {
                ch = ch2;
            }
            ch2 = wgetch(win);
            flushinp();

            snake.Move(ch);
            snake.Update(food, winMaxY, winMaxX);

            werase(win);

            food.Draw(win);
            snake.Draw(win);
            box(win, 0, 0);

            if (FIXED_SIZE) {
                mvprintw(getbegy(win) - 1, getbegx(win), "Score: %d", snake.GetTailSize() - Snake::INITIAL_TAIL_SIZE);
            } else {
                mvwprintw(win, 0, 1, "Score: %d", snake.GetTailSize() - Snake::INITIAL_TAIL_SIZE);
            }

            wrefresh(win);
            refresh();

            if (snake.CheckCollision(winMaxY, winMaxX) || ch == (wchar_t)Key::ESCAPE || ch == (wchar_t)Key::q || ch == (wchar_t)Key::Q) {
                if (snake.CheckCollision(winMaxY, winMaxX)) {
                    GameOver(win, winMaxY, winMaxX);
                }
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(DELAY - DIFFICULTY * 30));
        }
        endwin();
    }
};

int main(int argc, char *argv[]) {
    argumentParser(argc, argv);

    Game game;
    game.Run();

    return 0;
}

void ncursesSetup() {
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, true);
    nodelay(stdscr, true);
    clear();
    refresh();

    if (has_colors() && COLOR) {
        start_color();
        use_default_colors();

        init_pair(1, COLOR_RED, -1);
        init_pair(2, COLOR_GREEN, -1);
    }

    if (!has_colors()) {
        endwin();
        std::cerr << "Your terminal does not support color" << std::endl;
        exit(1);
    }
}
void GameOver(WINDOW *win, int maxY, int maxX) {

    std::string gameOver = "  Game Over  ";
    mvwprintw(win, maxY / 2 - 1, maxX / 2 - gameOver.size() / 2, gameOver.c_str());
    charbox(win, maxY / 2 - 2, maxX / 2 - gameOver.size() / 2 - 1, 2, gameOver.size() + 1);

    box(win, 0, 0);
    wrefresh(win);
    nodelay(stdscr, false);
    getch();
}

void charbox(WINDOW *win, int y, int x, int height, int width) {
    mvwhline(win, y, x, 0, width);
    mvwhline(win, y + height, x, 0, width);
    mvwvline(win, y, x, 0, height);
    mvwvline(win, y, x + width, 0, height);
    mvwaddch(win, y, x, ACS_ULCORNER);
    mvwaddch(win, y, x + width, ACS_URCORNER);
    mvwaddch(win, y + height, x, ACS_LLCORNER);
    mvwaddch(win, y + height, x + width, ACS_LRCORNER);
}

void argumentParser(int argc, char *argv[]) {
    using namespace clipp;

    bool help = false;
    bool overrideDifficulty = false;

    group cli;
    cli = joinable(
        (
        (option("-d", "--difficulty") & integer("difficulty", DIFFICULTY)).doc("Set difficulty 1-5 (default: " + std::to_string(DIFFICULTY) + ")"),
        option("-o", "--override").set(overrideDifficulty).doc("Override difficulty limit"),
        option("-f", "--nofixed").set(FIXED_SIZE, false).doc("Use the whole terminal window"),
        (option("-s", "--size") & integer("size", MAP_SIZE)).doc("Set map size 10 - 50 (default: " + std::to_string(MAP_SIZE) + ")")
        ).doc("Game options"),
        (
        option("-c", "--color").set(COLOR).doc("Enable color mode"),
        option("-a", "--ascii").set(BLOCK, false).doc("ASCII mode (overrides -w and -b)"),
        option("-w", "--nowide").set(WIDE, false).doc("Turn off wide mode"),
        option("-b", "--no-block-food").set(BLOCK_FOOD, false).doc("Disable block food")
        ).doc("Visual options"),
        option("-h", "--help").set(help).doc("Print help")
    );

    auto ftm = doc_formatting{}
        .first_column(4)
        .doc_column(28)
        .last_column(80)
        ;

    if (!parse(argc, argv, cli) || help || (!overrideDifficulty && (DIFFICULTY < 1 || DIFFICULTY > 5) && (MAP_SIZE < 10 || MAP_SIZE > 50))) {
        std::cout << "Usage:\n" << usage_lines(cli, argv[0], ftm) << std::endl
                << "Options:\n" << documentation(cli, ftm) << std::endl;
        exit(0);
    }

    if (BLOCK == false) {
        WIDE = false;
    }
    if (WIDE == false) {
        BLOCK_FOOD = false;
    }
}

