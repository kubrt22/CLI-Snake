#include <iostream>
#include <cstdlib>
#include <ncurses.h>
#include <vector>
#include <unistd.h>

void charbox(int y, int x, int height, int width);

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

class Food {
private:
    int x;
    int y;
public:
    void Draw() {
        mvprintw(y, x, "x");
    }

    void Move(int maxX, int maxY) {
        x = rand() % maxX;
        y = rand() % maxY;
    }

    bool CheckCollision(int x, int y) {
        return this->x == x && this->y == y;
    }
};

class Snake {
private:
    int x;
    int y;
    Direction direction;
    int tailSize;
    static const int INITIAL_TAIL_SIZE = 5;

    std::vector<std::pair<int, int>> tail;
public:
    Snake(int x, int y, int tailSize = INITIAL_TAIL_SIZE) {
        this->x = x;
        this->y = y;
        this->direction = RIGHT;
        this->tailSize = tailSize;
        for (int i = 0; i < tailSize; i++) {
            this->tail.push_back(std::make_pair(x - i, y));
        }
    }

    void Draw() {
        for (std::size_t i = 0; i < tail.size(); i++) {
            mvprintw(tail[i].second, tail[i].first, "o");
        }
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
                break; // handle invalid key inputs
        }
        switch (direction) {
            case UP:
                y--;
                break;
            case DOWN:
                y++;
                break;
            case LEFT:
                x--;
                break;
            case RIGHT:
                x++;
                break;
        }
    }

    void Update(Food &food, int maxX, int maxY) {
        int prevX = tail[0].first;
        int prevY = tail[0].second;
        int prev2X, prev2Y;
        tail[0].first = x;
        tail[0].second = y;
        for (std::size_t i = 1; i < tail.size(); i++) {
            prev2X = tail[i].first;
            prev2Y = tail[i].second;
            tail[i].first = prevX;
            tail[i].second = prevY;
            prevX = prev2X;
            prevY = prev2Y;
        }

        if (food.CheckCollision(x, y)) {
            tail.push_back(std::make_pair( prevX, prevY));
            food.Move(maxX, maxY);
        }
    }

    bool CheckCollision(int maxX, int maxY) {
        for (std::size_t i = 1; i < tail.size(); i++) {
            if (x == tail[i].first && y == tail[i].second) {
                return true;
            }
        }

        if (x >= maxX || x < 0 || y >= maxY || y < 0) {
            return true;
        }

        return false;
    }
    
};

void GameOver() {
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);
    

    std::string gameOver = "  Game Over  ";
    mvprintw(maxY / 2, maxX / 2 - gameOver.size() / 2, gameOver.c_str());
    charbox(maxY / 2 - 1, maxX / 2 - gameOver.size() / 2 - 1, 3, gameOver.size() + 1);

    refresh();
    nodelay(stdscr, false);
    getch();
}

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, true);
    nodelay(stdscr, true);
    refresh();

    clear();
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);
    int x = maxX / 2;
    int y = maxY / 2;
    Snake snake(x, y);
    Food food;
    food.Move(maxX, maxY);
    refresh();


    while (true) {
        clear();
        wchar_t ch = getch();

        snake.Move(ch);
        snake.Update(food, maxX, maxY);
        if (snake.CheckCollision(maxX, maxY) || ch == (wchar_t)Key::ESCAPE || ch == (wchar_t)Key::q || ch == (wchar_t)Key::Q) {
            food.Draw();
            snake.Draw();
            GameOver();
            break;
        }

        food.Draw();
        snake.Draw();

        refresh();
        usleep(100000);
    }
    endwin();
    return 0;
}

void charbox(int y, int x, int height, int width) {
    mvhline(y, x, 0, width);
    mvhline(y + height, x, 0, width);
    mvvline(y, x, 0, height);
    mvvline(y, x + width, 0, height);
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + width, ACS_URCORNER);
    mvaddch(y + height, x, ACS_LLCORNER);
    mvaddch(y + height, x + width, ACS_LRCORNER);
}