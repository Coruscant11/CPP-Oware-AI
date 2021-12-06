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
	int getNextPlayer();
	static int getNextPlayer(int previousPlayer);
private:
	Board gameBoard;
	int actualPlayer;
    int aiPlayer;

	struct Choice askChoice(int player);
    struct Choice decisionMinMaxToChoice(int decision);
	void updateActualPlayer();
	void displayWinner(int winner);
};