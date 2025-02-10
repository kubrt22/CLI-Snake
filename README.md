## Terminal snake game
Classic snake game made using C++ and ncurses, it has multiple difficulties and visual options and autoplay mode.

## Dependencies
 - C++ compiler
 - Ncurses library


## Build
```bash
git clone https://github.com/kubrt22/CLI-Snake.git
cd CLI-Snake
make
```

U need to have the ncurses library on Path or in the same folder as snake

Then copy binary to path
```bash
sudo cp ./snake /usr/local/bin
```

## Usage
```
Usage:
    snake [-d <difficulty>] [-o] [-f] [-c] [-a] [-w] [-b] [-h]
Options:
    Game options
        -d, --difficulty <difficulty>
                            Set difficulty 1-5 (default: 1)

        -o, --override      Override difficulty limit
        -f, --nofixed       Use the whole terminal window

    Visual options
        -c, --color         Enable color mode
        -a, --ascii         ASCII mode (overrides -w and -b)
        -w, --nowide        Turn off wide mode
        -b, --no-block-food Disable block food

    -h, --help              Print help


```

## Screenshots
![image](https://github.com/user-attachments/assets/1da28d31-759f-44e8-b119-3f6fb3ea552f)
