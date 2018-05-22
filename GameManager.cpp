#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "GameManager.h"
#include "Piece.h"
#include "Point.h"
#include "Move.h"
#include "JokerChange.h"
#include "PiecePosition.h"
#include "GameContainers.h"


const unsigned int FIGHTS_THRESHOLD = 100;

GameManager::GameManager(std::shared_ptr<PlayerAlgorithm> algo1, std::shared_ptr<PlayerAlgorithm> algo2) {
	_players[0] = std::make_unique<Player>(1, algo1);
	_players[1] = std::make_unique<Player>(2, algo2);
}

int GameManager::playRound() {
	// init
	_board.clear();
	_numFights = 0;
	// positioning
    std::vector<std::unique_ptr<FightInfo>> fights;
	position(0, fights);
	position(1, fights);
    _players[0]->algo->notifyOnInitialBoard(_board, fights);
    _players[1]->algo->notifyOnInitialBoard(_board, fights);
	// moves
	auto i = 0;
	while (_numFights < FIGHTS_THRESHOLD) {
		if (!isGameOn()) break;
		doMove(i);
		if (!isGameOn()) break;
		changeJoker(i);
		i = 1 - i; // switch player
	}
	return output();
}

void GameManager::position(int i, std::vector<std::unique_ptr<FightInfo>>& fights) {
    auto& player = _players[i];
	BoardImpl tmpBoard;
	std::vector<std::unique_ptr<PiecePosition>> positions;
	player->algo->getInitialPositions(player->index, positions);
	// populate tmpBoard & player piece map
	for (const auto& piecePos : positions) {
		if (!isValid(piecePos, tmpBoard)) {
			player->status = PlayerStatus::InvalidPos;
			return;
		}
		const auto& pos = piecePos->getPosition();
		auto type = (PieceType)piecePos->getPiece();
		auto jokerType = (PieceType)piecePos->getJokerRep();
		tmpBoard[pos] = std::make_shared<Piece>(player->index, type, jokerType);
		player->numPieces[type]++;
		if (tmpBoard[pos]->getType() == PieceType::Flag) player->numFlags++;
		if (tmpBoard[pos]->canMove()) player->numMovable++;
	}
	if (!isValid(player)) {
		player->status = PlayerStatus::InvalidPos;
		return;
	}
	// merge tmpBoard and main board
	for (unsigned int i = 0; i < N; i++) {
		for (unsigned int j = 0; j < N; j++) {
			PointImpl pos(i, j);
			auto fightInfo = fight(pos, tmpBoard[pos]);
			if (fightInfo) fights.push_back(std::move(fightInfo));
		}
	}
}

void GameManager::doMove(int i) {
	const auto move = _players[i]->algo->getMove();
	if (!isValid(move, i)) {
		_players[i]->status = PlayerStatus::InvalidMove;
		return;
	}
	auto fightInfo = fight(move->getTo(), _board[move->getFrom()]);
	if (fightInfo) {
		_players[0]->algo->notifyFightResult(*fightInfo);
		_players[1]->algo->notifyFightResult(*fightInfo);
		_numFights = 0;
	} else {
		_numFights++;
	}
	_board[move->getFrom()] = Piece::Empty;
	_players[1 - i]->algo->notifyOnOpponentMove(*move);
}

void GameManager::changeJoker(int i) {
	auto& player = _players[i];
	const auto jokerChange = player->algo->getJokerChange();
	if (!isValid(jokerChange, i)) {
		player->status = PlayerStatus::InvalidMove;
		return;
	}
	const auto& piece = _board[jokerChange->getJokerChangePosition()];
	piece->setJokerType((PieceType)jokerChange->getJokerNewRep());
}

int GameManager::output() {
	std::ofstream fout("rps.output");
	if (_players[0]->status == PlayerStatus::Playing || _players[1]->status == PlayerStatus::Playing) {
		int winner = _players[0]->status == PlayerStatus::Playing ? 0 : 1;
		int loser = 1 - winner;
		fout << "Winner: " << winner + 1 << std::endl << "Reason: ";
		switch (_players[loser]->status) {
		case PlayerStatus::InvalidPos:
			fout << "Bad positioning input for player " << loser + 1 << std::endl;
			break;
		case PlayerStatus::InvalidMove:
			fout << "Bad move input for player " << loser + 1 << std::endl;
			break;
		case PlayerStatus::NoFlags:
			fout << "All flags of the opponent are captured" << std::endl;
			break;
		case PlayerStatus::CantMove:
			fout << "All moving PIECEs of the opponent are eaten" << std::endl;
			break;
		default:
			break;
		}
        fout << std::endl << _board;
        return winner;
	} else { // tie
		fout << "Winner:" << 0 << std::endl << "Reason: ";
		if (_players[0]->status == PlayerStatus::InvalidPos) {
			fout << "Bad positioning input for both players" << std::endl;
		} else { // _players[0].status == PlayerStatus::NoFlags
			fout << "Both players cannot play (no flags / cannot move)" << std::endl;
		}
		// TODO: handle numFights exceeded
        fout << std::endl << _board;
        return 0;
	}
}

std::unique_ptr<FightInfo> GameManager::fight(const Point& pos, const std::shared_ptr<Piece> piece1) {
	auto piece2 = _board[pos];
	auto killPiece1 = piece2->canKill(*piece1);
	auto killPiece2 = piece1->canKill(*piece2);
	if (killPiece1 && piece1 != Piece::Empty) kill(piece1);
	if (killPiece2 && piece2 != Piece::Empty) kill(piece2);
	_board[pos] = killPiece1 && killPiece2 ? Piece::Empty : (killPiece1 ? piece2 : piece1);
	if (piece1 == Piece::Empty || piece2 == Piece::Empty) return nullptr;
	auto winner = killPiece1 && killPiece2 ? 0 : (killPiece1 ? 2 : 1);
	auto ch1 = piece1->getPlayer() == 1 ? piece1 : piece2;
	auto ch2 = piece1->getPlayer() == 2 ? piece1 : piece2;
	return std::make_unique<FightInfoImpl>((const PointImpl&)pos, *ch1, *ch2, winner);
}

void GameManager::kill(std::shared_ptr<Piece> piece) {
	auto& player = _players[piece->getPlayer() - 1];
	player->numPieces[piece->getType()]--;
	if (piece->getType() == PieceType::Flag) {
		player->numFlags--;
		if (player->numFlags == 0) player->status = PlayerStatus::NoFlags;
	}
	if (piece->canMove()) {
		player->numMovable--;
		if (player->numMovable == 0) player->status = PlayerStatus::CantMove;
	}
}

bool GameManager::isValid(const std::unique_ptr<Move>& move, int i) const {
	if (!move) return false;
	const auto& from = move->getFrom();
	const auto& to = move->getTo();
	// check that points on board
	if (!_board.isValidPosition(to)) return false;
	if (!_board.isValidPosition(from)) return false;
	// check  that points are next to each other
	auto horizontal = std::abs(from.getX() - to.getX());
	auto vertical = std::abs(from.getY() - to.getY());
	if (horizontal > 1 || vertical > 1) return false;
	if (horizontal == 0 && vertical == 0) return false;
	// check that that piece is the player's piece and that it can move
	if (_board[from]->getPlayer() != i + 1) return false;
	if (!_board[from]->canMove()) return false;
	// check that the destination doesn't contain a player's piece
	if (_board[to]->getPlayer() == i + 1) return false;
	return true;
}

bool GameManager::isValid(const std::unique_ptr<JokerChange>& jokerChange, int i) const {
	if (!jokerChange) return false;
	const auto rep = (PieceType)jokerChange->getJokerNewRep();
	const auto& pos = jokerChange->getJokerChangePosition();
	// check that point on board
	if (!_board.isValidPosition(pos)) return false;
	// check that rep is valid
	if (!Piece::isValid(rep)) return false;
	// check that that piece is the player's piece and that it's a Joker
	if (_board[pos]->getPlayer() != i + 1) return false;
	if (_board[pos]->getType() != PieceType::Joker) return false;
	return true;
}

bool GameManager::isValid(const std::unique_ptr<PiecePosition>& piecePos, const BoardImpl& board) const {
	if (!piecePos) return false;
	// check that pos is empty
	if (board[piecePos->getPosition()]->getPlayer() != 0) return false;
	// check that it's a valid piece
	if (!Piece::isValid((PieceType)piecePos->getPiece(), (PieceType)piecePos->getJokerRep())) return false;
	return true;
}

bool GameManager::isValid(std::unique_ptr<Player>& player) const {
	if (player->numFlags == 0 || player->numMovable == 0) return false;
	for (const auto& type : player->numPieces) {
		if (type.second > Piece::maxCapacity.at(type.first)) return false;
	}
	return true;
}

bool GameManager::isGameOn() {
	return _players[0]->status == PlayerStatus::Playing && _players[1]->status == PlayerStatus::Playing;
}