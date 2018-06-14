#pragma once

#include <iostream>
#include <array>
#include <utility>
#include "Board.h"
#include "Point.h"


template<class T>
class GameBoard : public Board {
public:
    const static std::size_t N = 10;
    const static std::size_t M = 10;
    using Entry = struct {
        T piece;
        int player;
    };
    void clear() { _arr.fill({ T(), 0 }); }
    int getPlayer(const Point& pos) const override { return _arr[getIndex(pos)].player; }
    bool isValid(const Point& pos) const { return getIndex(pos) >= 0 && getIndex(pos) < SIZE; }
    Entry& operator[](const Point& pos) { return _arr[getIndex(pos)]; }
    const Entry& operator[](const Point& pos) const { return _arr[getIndex(pos)]; }
    Entry& operator[](const std::pair<int, int> pos) { return _arr[getIndex(pos)]; }
    const Entry& operator[](const std::pair<int, int> pos) const { return _arr[getIndex(pos)]; }
private:
    std::size_t getIndex(const Point& pos) const { return pos.getX() * M + pos.getY(); }
    const static std::size_t SIZE = N * M;
    std::array<Entry, SIZE> _arr;
};