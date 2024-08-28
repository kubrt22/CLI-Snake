#include <iostream>
#include <vector>
#include <ctime>

#include <ncurses.h>
#include "clipp.h"

const int MAP_SIZE = 60;
const int DELAY = 230;

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
    Food(int MaxX, int MaxY) {
        if (WIDE) {
            width = 2;
        }
        Move(MaxX, MaxY);
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
        wattroff(win, A_REVERSE | A_DIM | COLOR_PAIR(1));
    }

    void Move(int maxX, int maxY) {
        pos.y = rand() % (maxY - 2) + 1;
        pos.x = rand() % (maxX - width * 3 + 1) + width; // 3 is a magic number
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
public:
    Point pos;
    Direction direction;
    int tailSize;
    int width = 1;
    static const int INITIAL_TAIL_SIZE = 5;

    std::vector<Point> tail;
public:
    Snake(Point xy, int tailSize = INITIAL_TAIL_SIZE) {
        if (WIDE) {
            width = 2;
        }

        this->pos = xy;
        this->direction = RIGHT;
        this->tailSize = tailSize;
        for (int i = 0; i < tailSize; i++) {
            this->tail.push_back({pos.y, pos.x - i * width});
        }
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
    }

    void Move(wchar_t ch) {
        switch ((Key)ch) {
            case Key::w:
            case Key::W:
            case Key::UP:
                if (direction != DOWN) {
                    direction = UP;
                }
                break;
            case Key::s:
            case Key::S:
            case Key::DOWN:
                if (direction != UP) {
                    direction = DOWN;
                }
                break;
            case Key::a:
            case Key::A:
            case Key::LEFT:
                if (direction != RIGHT) {
                    direction = LEFT;
                }
                break;
            case Key::d:
            case Key::D:
            case Key::RIGHT:
                if (direction != LEFT) {
                    direction = RIGHT;
                }
                break;
            case Key::ESCAPE:
            case Key::q:
            case Key::Q:
                break;
            default:
                break;
        }
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

    void Update(Food &food, int maxX, int maxY) {
        Point prev, prev2;
        prev = tail[0]; // Save prev pos of the head in vector
        tail[0] = pos; // Update head pos

        for (std::size_t i = 1; i < tail.size(); i++) {
            prev2 = tail[i]; // Save prev pos of the tail in vector
            tail[i] = prev; // Update tail pos
            prev = prev2; // Update prev pos
        }

        if (food.CheckCollision(pos)) {
            tail.push_back(prev); // Add a new tail
            food.Move(maxX, maxY);
            for (std::size_t i = 0; i < tail.size(); i++) {
                if (food.CheckCollision(tail[i])) {
                    food.Move(maxX, maxY);
                    i = 0;
                }
            }
        }
    }

    bool CheckCollision(int maxX, int maxY) {
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
public:
    void Run() {
        ncursesSetup();

        int maxY, maxX;
        getmaxyx(stdscr, maxY, maxX);

        int width = FIXED_SIZE ? MAP_SIZE : maxX;
        int height = FIXED_SIZE ? MAP_SIZE / 2 : maxY;
        int start_x = FIXED_SIZE ? maxX / 2 - MAP_SIZE / 2 : 0;
        int start_y = FIXED_SIZE ? maxY / 2 - MAP_SIZE / 4 : 0;
        
        WINDOW *win = newwin(height, width, start_y, start_x);

        getmaxyx(win, maxY, maxX);

        srand(time(0));

        Snake snake({ maxY / 2, maxX / 2 - (maxX % 2) });
        Food food(maxX, maxY);

        wchar_t ch, ch2 = ERR;

        while (true) {
            wclear(win);
            clear();

            if (ch2 == ERR) {
                ch = getch();
            } else {
                ch = ch2;
            }
            ch2 = getch();
            flushinp();


            snake.Move(ch);
            snake.Update(food, maxX, maxY);

            food.Draw(win);
            snake.Draw(win);

            box(win, 0, 0);

            if (FIXED_SIZE) {
                mvprintw(getbegy(win) - 1, getbegx(win), "Score: %ld", snake.tail.size() - Snake::INITIAL_TAIL_SIZE);
            } else {
                mvwprintw(win, 0, 1, "Score: %ld", snake.tail.size() - Snake::INITIAL_TAIL_SIZE);
            }

            refresh();
            wrefresh(win);

            if (snake.CheckCollision(maxX, maxY) || ch == (wchar_t)Key::ESCAPE || ch == (wchar_t)Key::q || ch == (wchar_t)Key::Q) {
                if (snake.CheckCollision(maxX, maxY)) {
                    GameOver(win, maxY, maxX);
                }
                break;
            }

            if (ch == 'p') {
                while (true) {
                    ch = getch();
                    if (ch == 'p') {
                        break;
                    }
                }
            }
            napms(DELAY - DIFFICULTY * 30);
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
    mvwprintw(win, maxY / 2, maxX / 2 - gameOver.size() / 2, gameOver.c_str());
    charbox(win, maxY / 2 - 1, maxX / 2 - gameOver.size() / 2 - 1, 3, gameOver.size() + 1);

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
        (option("-d", "--difficulty") & integer("difficulty", DIFFICULTY)).doc("Set difficulty 1-5 (default: 1)"),
        option("-o", "--override").set(overrideDifficulty).doc("Override difficulty limit"),
        option("-f", "--nofixed").set(FIXED_SIZE, false).doc("Use the whole terminal window")
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

    if (!parse(argc, argv, cli) || help || (!overrideDifficulty && (DIFFICULTY < 1 || DIFFICULTY > 5))) {
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

