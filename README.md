## Terminal snake game
Classic snake game made using C++ and ncurses, it has multiple difficulties and visual options and autoplay mode.

## Dependencies
 - C++ compiler
 - Cmake
 - Ncurses library (Linux)

The cmake will build pdcurses on windows and automaticly add it.
## Build
```bash
git clone https://github.com/kubrt22/cli-Snake.git
cd cli-Snake
mkdir build
cd build
cmake ..
make
```
```bash
git clone https://github.com/kubrt22/cli-Snake.git && cd cli-Snake && mkdir build && cd build && cmake .. && make
```
On linux install it using cmake, on windows u have to add it to your path
```bash
cmake --install .
```
### Alternative
```bash
g++ -o snake src/*.cpp -std=c++17 -Iinclude -s -O3 -lncurses
```

## Keybinds
- **p** - shows shortest path to the food
- **n** - enable autoplay (using the pathfinding)

## Usage
```
Usage:
    snake [-d <difficulty>] [-o] [-f] [-s <size>] [-c] [-a] [-w] [-b] [-h]
Options:
    Game options
        -d, --difficulty <difficulty>
                            Set difficulty 1-5 (default: 1)

        -o, --override      Override difficulty limit
        -f, --nofixed       Use the whole terminal window
        -s, --size <size>   Set map size 10 - 50 (default: 20)

    Visual options
        -c, --no-color      Disable color mode
        -a, --ascii         ASCII mode (overrides -w and -b)
        -w, --nowide        Turn off wide mode
        -b, --no-block-food Disable block food

    -h, --help              Print help
```

## Screenshots
![image](https://github.com/user-attachments/assets/34a604e1-7435-47e7-a0eb-e46127afd358)
![image](https://github.com/user-attachments/assets/99841a18-950f-4d90-89fc-9d8e084ff59f)

