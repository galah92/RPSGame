#include <memory>
#include <vector>
#include <cstdlib>
#include "GameManager.h"


GameManager::GameManager(PlayerAlgorithm &player1, PlayerAlgorithm &player2) :
    _players({ std::ref(player1), std::ref(player2) }) { }

void GameManager::play() {
	std::vector<std::unique_ptr<PiecePosition>> positions;
	std::vector<std::unique_ptr<FightInfo>> fights;
    _players[0].get().getInitialPositions(1, positions);
    // bool result = populate(1, positions);
    // TODO: handle populate fail
    _players[1].get().getInitialPositions(2, positions);
    // result = populate(2, positions);
    // TODO: handle populate fail
    //
    int player = 0;
    while (true) {
        auto move = _players[player].get().getMove();
        if (!isValidMove(move, player)) return;
        _players[!player].get().notifyOnOpponentMove(*move);
        //auto fightInfo = fight(move, player);
        //if (fightInfo) {
        //    _players[0].get().notifyFightResult(*fightInfo);
        //    _players[1].get().notifyFightResult(*fightInfo);
        //}
        auto change = _players[player].get().getJokerChange();
        if (change) {
            if (!isValidJokerChange(change, player)) return;
            auto piece = _board.getPiece(change->getJokerChangePosition());
            piece->setJokerRep(change->getJokerNewRep());
        }
        player = !player; // swap player
    }
}

bool GameManager::populate(int player, std::vector<std::unique_ptr<PiecePosition>> &positions, std::vector<std::unique_ptr<FightInfo>> &fights) {
    GameBoard tmpBoard;
    for (const auto &pos : positions) {
        if (!tmpBoard.getPlayer(pos->getPosition())) return false;
        tmpBoard.setPiece(pos->getPosition(), std::make_shared<GamePiece>(player, pos->getPiece(), pos->getJokerRep()));
        numFlags[player] += pos->getPiece() == 'J';
        // TODO: increment numMovablePieces if needed
    }
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int j = 0; j < M; j++) {
            // const auto piece = tmpBoard.getPiece(GamePoint(i, j));
            (void)fights;
        }
    }
    return false;
}

bool GameManager::isValidMove(std::unique_ptr<Move>& move, int player) {
    if (!move) return false;
    auto &from = move->getFrom();
    auto &to = move->getTo();
    if (!_board.isValidPosition(from) || !_board.isValidPosition(to)) return false;
    auto xDiff = std::abs(from.getX() - to.getX());
    auto yDiff = std::abs(from.getY() - to.getY());
    if ((xDiff > 1 || yDiff > 1) || (xDiff == 0 && yDiff == 0)) return false;
    if (!_board.getPiece(from)->canMove()) return false;
    if (_board.getPiece(from)->getPlayer() != player) return false;
    if (_board.getPiece(to)->getPlayer() == player) return false;
    return true;
}

bool GameManager::isValidJokerChange(std::unique_ptr<JokerChange>& change, int player) {
    auto &pos = change->getJokerChangePosition();
    if (!_board.isValidPosition(pos)) return false;
    auto piece = _board.getPiece(pos);
    if (piece->getPiece() != 'J' || piece->getPlayer() != player) return false;
    // TODO check change->getJokerNewRep() is valid piece char
    return true;
}

std::shared_ptr<GamePiece> GameManager::fight(std::shared_ptr<GamePiece> piece1, std::shared_ptr<GamePiece> piece2) {
    (void)piece1;
    (void)piece2;
    return nullptr;
}