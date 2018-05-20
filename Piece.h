#pragma once

#include <memory>
#include <unordered_map>


enum class PieceType : char {
	None = ' ',
	Flag = 'F',
	Rock = 'R',
	Paper = 'P',
	Scissors = 'S',
	Bomb = 'B',
<<<<<<< HEAD
	Unknown = '#',
	Joker = 'J'
=======
>>>>>>> GameManager flow fixed - simpletest work!
};

class Piece {
public:
	Piece(int player, PieceType type, bool isJoker = false) :
		_player(player),
		_type(type),
		_isJoker(isJoker) {}
	int getPlayer() const;
	PieceType getType() const;
	bool setType(PieceType type);
	bool isJoker() const;
	bool canMove() const;
	bool canKill(PieceType piece) const;
	static bool isValid(PieceType piece);
	operator char() const;
	static std::shared_ptr<Piece> Empty;
private:
	int _player;
	PieceType _type;
	bool _isJoker;
};