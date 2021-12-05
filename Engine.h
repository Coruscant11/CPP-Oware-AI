#pragma once

#include "Board.h"

using namespace std;

struct Choice {
	int hole;
	char color;
};

class Engine {
public:
	Engine();
	
	void run();
private:
	Board gameBoard;
	int actualPlayer;

	struct Choice askChoice(int player);
	void updateActualPlayer();
	int getNextPlayer();
	int getNextPlayer(int previousPlayer);
};