#include "Engine.h"
#include <iostream>

Engine::Engine() {
	actualPlayer = 0;
}

void Engine::run() {
	bool isFinished = false;

	while (!isFinished) {
		cout << "PLAYER " << actualPlayer + 1 << " TURN" << endl;
		gameBoard.printBoard();

		struct Choice choice = askChoice(actualPlayer);
		int lastHole;

		if (choice.color == 'B') {
			lastHole = gameBoard.distributeBlueSeed(choice.hole);
		}
		else if (choice.color == 'R') {
			lastHole = gameBoard.distributeRedSeed(choice.hole);
		}
	
		gameBoard.pickSeed(lastHole, choice.hole);
		updateActualPlayer();
		cout << endl;
	}
}

struct Choice Engine::askChoice(int player) {
	if (player == 0) { cout << "Choose an odd hole and a color (ex: 13 B) : " << endl; }
	else if (player == 1) { cout << "Chose an even hole and a color (ex: 12 R) : " << endl; }

	struct Choice choice;
	choice.hole = -1;
	choice.color = 'a';

	while (!((choice.hole > 0 && choice.hole <= 16) &&
		((player == 0 && choice.hole % 2 == 1) ||
		 (player == 1 && choice.hole % 2 == 0)) &&
		(choice.color == 'B' || choice.color == 'R')))
	{
		cin >> choice.hole;
		cin >> choice.color;
	}
	
	choice.hole--;
	return choice;
}

void Engine::updateActualPlayer() {
	actualPlayer = (actualPlayer + 1) % 2;
}

int Engine::getNextPlayer() {
	return (actualPlayer + 1) % 2;
}

int Engine::getNextPlayer(int previousPlayer) {
	return (previousPlayer + 1) % 2;
}