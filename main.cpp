#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include "TournamentManager.h"
#include "GameManager.h"
#include "AutoPlayerAlgorithm.h"
#include "FilePlayerAlgorithm.h"


// const std::string DELIMITER = "-vs-";
// const std::string USAGE = "Usage: ./ex2 <auto|file>-vs-<auto|file>";

// std::shared_ptr<PlayerAlgorithm> PlayerAlgorithmFactory(std::string str) {
//     if (str == "auto") return std::make_shared<AutoPlayerAlgorithm>();
//     if (str == "file") return std::make_shared<FilePlayerAlgorithm>();
//     return nullptr;
// }

std::map<std::string, std::string> parseArgs(int argc, const char *argv[]) {
    std::vector<std::string> vec(argv + 1, argv + argc);
    std::map<std::string, std::string> args;
    for (unsigned int i = 0; i < vec.size(); i += 2) {
        args[vec[i]] = vec[i + 1];
    }
    return args;
}

int main(int argc, const char *argv[])
{
    auto args = parseArgs(argc, argv);
    auto& manager = TournamentManager::get();
    if (args.find("-path") != args.end()) {
        manager.path = args["-path"];
    }
    if (args.find("-threads") != args.end()) {
        manager.maxThreads = std::stoi(args["-threads"]);
    }
    manager.run();

    // if (argc < 2) {
    //     std::cout << USAGE << std::endl;
    //     return -1;
    // }
    // std::string arg(argv[1]);
    // auto pos = arg.find(DELIMITER);
    // auto algo1 = PlayerAlgorithmFactory(arg.substr(0, pos));
    // arg.erase(0, pos + DELIMITER.length());
    // auto algo2 = PlayerAlgorithmFactory(arg.substr(0, pos));
    // if (!algo1 || !algo2) {
    //     std::cout << USAGE << std::endl;
    //     return -1;
    // }
    // GameManager game;
    // game.playRound(algo1, algo2);
    // return 0;
}