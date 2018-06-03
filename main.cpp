#include <iostream>
#include <memory>
#include <string>
#include "TournamentManager.h"
#include "GameManager.h"
#include "AutoPlayerAlgorithm.h"
#include "FilePlayerAlgorithm.h"


const std::string DELIMITER = "-vs-";
const std::string USAGE = "Usage: ./ex2 <auto|file>-vs-<auto|file>";

std::shared_ptr<PlayerAlgorithm> PlayerAlgorithmFactory(std::string str) {
    if (str == "auto") return std::make_shared<AutoPlayerAlgorithm>();
    if (str == "file") return std::make_shared<FilePlayerAlgorithm>();
    return nullptr;
}

int main(int argc, const char *argv[])
{
    if (argc < 2) {
        std::cout << USAGE << std::endl;
        return -1;
    }
    std::string arg(argv[1]);
    auto pos = arg.find(DELIMITER);
    auto algo1 = PlayerAlgorithmFactory(arg.substr(0, pos));
    arg.erase(0, pos + DELIMITER.length());
    auto algo2 = PlayerAlgorithmFactory(arg.substr(0, pos));
    if (!algo1 || !algo2) {
        std::cout << USAGE << std::endl;
        return -1;
    }
    GameManager game;
    game.playRound(algo1, algo2);
    return 0;
}