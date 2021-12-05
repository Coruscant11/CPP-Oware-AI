#include "Engine.h"
#include <iostream>

#define cRESET   "\033[0m"
#define cRED     "\033[31m"      
#define cBLUE    "\033[34m"      
#define cYELLOW  "\033[33m"
#define cGREEN   "\033[32m" 

Engine::Engine() {
	actualPlayer = 0;
}

void Engine::run() {
	bool isFinished = false;

	while (!isFinished) {
		if (actualPlayer == 0)
			cout << cGREEN << "PLAYER " << actualPlayer + 1 << " TURN" << cRESET << endl;
		else
			cout << cYELLOW << "PLAYER " << actualPlayer + 1 << " TURN" << cRESET << endl;

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
		int winner = gameBoard.checkWin();
		if (winner >= 0) {
			isFinished = true;
			displayWinner(winner);
		}

		updateActualPlayer();
		cout << endl;
	}
}

struct Choice Engine::askChoice(int player) {
	if (player == 0) { cout << cGREEN << "Choose an odd hole and a color (ex: 13 B) : " << cRESET << endl; }
	else if (player == 1) { cout << cYELLOW << "Chose an even hole and a color (ex: 12 R) : " << cRESET << endl; }

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
	return getNextPlayer(actualPlayer);
}

int Engine::getNextPlayer(int previousPlayer) {
	return (previousPlayer + 1) % 2;
}

void Engine::displayWinner(int winner) {
	cout << endl << endl;
	if (winner == 3) {
		cout << "It's a draw !" << endl;
	}
	else if (winner == 0 || winner == 1) {
		cout << "Player " << winner << " won the game !" << endl;
	}
}