#include "utils.hpp"

#include "curses.h"
#include "clipp.h"

#include <iostream>


void argumentParser(int argc, char *argv[]) {
    using namespace clipp;

    bool help = false;
    bool overrideDifficulty = false;

    group cli;
    cli = joinable(
        (
        (option("-d", "--difficulty") & integer("difficulty", GameOptions::DIFFICULTY)).doc("Set difficulty 1-5 (default: " + std::to_string(GameOptions::DIFFICULTY) + ")"),
        option("-o", "--override").set(overrideDifficulty).doc("Override difficulty limit"),
        option("-f", "--nofixed").set(GameOptions::FIXED_SIZE, false).doc("Use the whole terminal window"),
        (option("-s", "--size") & integer("size", GameOptions::MAP_SIZE)).doc("Set map size 10 - 50 (default: " + std::to_string(GameOptions::MAP_SIZE) + ")")
        ).doc("Game options"),
        (
        option("-c", "--no-color").set(GameOptions::COLOR, false).doc("Disable color mode"),
        option("-a", "--ascii").set(GameOptions::BLOCK, false).doc("ASCII mode (overrides -w and -b)"),
        option("-w", "--nowide").set(GameOptions::WIDE, false).doc("Turn off wide mode"),
        option("-b", "--no-block-food").set(GameOptions::BLOCK_FOOD, false).doc("Disable block food")
        ).doc("Visual options"),
        option("-h", "--help").set(help).doc("Print help")
    );

    auto ftm = doc_formatting{}
        .first_column(4)
        .doc_column(28)
        .last_column(80)
        ;

    if (!parse(argc, argv, cli) || help || (!overrideDifficulty && (GameOptions::DIFFICULTY < 1 || GameOptions::DIFFICULTY > 5) && (GameOptions::MAP_SIZE < 10 || GameOptions::MAP_SIZE > 50))) {
        std::cout << "Usage:\n" << usage_lines(cli, argv[0], ftm) << std::endl
                << "Options:\n" << documentation(cli, ftm) << std::endl;
        exit(0);
    }

    if (GameOptions::BLOCK == false) {
        GameOptions::WIDE = false;
    }
    if (GameOptions::WIDE == false) {
        GameOptions::BLOCK_FOOD = false;
    }
}