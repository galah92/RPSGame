#pragma once

#include <unordered_map>
#include <functional>
#include <utility>
#include <string>
#include <atomic>
#include <memory>
#include <deque>
#include <mutex>
#include <map>
#include "PlayerAlgorithm.h"


class shared_lib;

class TournamentManager {
public:
    static TournamentManager& get();
    void registerAlgorithm(std::string id, std::function<std::unique_ptr<PlayerAlgorithm>()> factoryMethod);
    bool run();
    int maxThreads;
    std::string path;
private:
    TournamentManager() = default;
    std::vector<shared_lib> loadSharedLibs();
    void initGames();
    void workerThread();
    void output();
    static TournamentManager _singleton;
    std::unordered_map<std::string, std::function<std::unique_ptr<PlayerAlgorithm>()>> _algorithms;
    std::unordered_map<std::string, std::shared_ptr<std::atomic<unsigned int>>> _scores;
    std::deque<std::pair<std::string, std::string>> _games;
    std::mutex _gamesMutex;
    const static unsigned int MAX_GAMES = 30;
};