#pragma once


class Point;

class FightInfo {
public:
	virtual const Point& getPosition() const = 0;
	virtual char getPiece(int player) const = 0;
	virtual int getWinner() const = 0;
	virtual ~FightInfo() {}
};

class FightInfoImpl : public FightInfo {
public:
	FightInfoImpl(Point& pos, char piece1, char piece2, int winner) :
		_pos(pos),
		_piece1(piece1),
		_piece2(piece2),
		_winner(winner) {}
	const Point& getPosition() const override { return _pos; };
	char getPiece(int player) const override { return player == 1 ? _piece1 : _piece2; };
	int getWinner() const override { return _winner; };
private:
	Point& _pos;
	char _piece1;
	char _piece2;
	int _winner;
};