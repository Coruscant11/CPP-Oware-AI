#include "Engine.h"
#include <iostream>
#include "AI.h"

#define cRESET   "\033[0m"
#define cRED     "\033[31m"      
#define cBLUE    "\033[34m"      
#define cYELLOW  "\033[33m"
#define cGREEN   "\033[32m" 
#define cGREENTURN "\033[32;4m"
#define cYELLOWTURN "\033[33;4m"
#define cGREENPLAY "\033[32;1;4m"
#define cYELLOWPLAY "\033[33;1;4m"

using namespace std;

Engine::Engine() {
	cout << "Does the player begin ? (y/n) : ";
	char rep;
	while (rep != 'y' && rep != 'n')
		cin >> rep;
	aiPlayer = rep == 'y' ? 1 : 0;
	actualPlayer = 0;
}

void Engine::run() {
	bool isFinished = false;
	int turnNumber = 0;

	while (!isFinished) {
		cout << endl;

		if (actualPlayer == 0)
			cout << cGREENTURN << "PLAYER " << actualPlayer + 1 << " TURN " << ++turnNumber << cRESET << endl;
		else
			cout << cYELLOWTURN << "PLAYER " << actualPlayer + 1 << " TURN " << ++turnNumber << cRESET << endl;

		gameBoard.printBoard();

		if (gameBoard.checkFamine(actualPlayer)) {
			isFinished = true;
			displayWinner(Engine::getNextPlayer(actualPlayer));
		}

        struct Choice choice;

        if (actualPlayer == aiPlayer){
            struct Array2DIndex decision = AI::decisionMinMax(aiPlayer, gameBoard);
            choice = decisionMinMaxToChoice(decision);
			if (aiPlayer == 0)
            	cout << cGREENPLAY << "IA PLAY : " << choice.hole+1 << choice.color << cRESET << endl;
			else
				cout << cYELLOWPLAY << "IA PLAY : " << choice.hole+1 << choice.color << cRESET << endl;
        }
        else {
            choice = askChoice(actualPlayer, gameBoard);
        }

        gameBoard.playMove(actualPlayer, choice.hole, choice.color);

		int winner = gameBoard.checkWinWithoutFamine();
		if (winner >= 0) {
			isFinished = true;
            gameBoard.printBoard();
			displayWinner(winner);
		}

		updateActualPlayer();
		cout << endl;
	}
}

struct Choice Engine::askChoice(int player, Board b) {
	if (player == 0) { cout << cGREEN << "Choose an odd hole and a color (ex: 13 B) : " << cRESET << endl; }
	else if (player == 1) { cout << cYELLOW << "Choose an even hole and a color (ex: 12 R) : " << cRESET << endl; }

	struct Choice choice;
	choice.hole = -1;
	choice.color = 'a';
    bool alreadyPlayed = false;
	while (true)
	{
		cin >> choice.hole;
		cin >> choice.color;
        if (b.isPossibleMove(player, choice.hole - 1, choice.color)) {
            break;
        }
        else {
            cout << "Coup invalide !" << endl;
        }
	}
	
	choice.hole--;
	return choice;
}

struct Choice Engine::decisionMinMaxToChoice(struct Array2DIndex decision) {
    Choice choice;
	choice.hole = decision.holeIndex;
	choice.color = decision.colorIndex == 0 ? 'R' : 'B';
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
	if (winner == 2) {
		cout << "It's a draw !" << endl;
	}
	else if (winner == 0 || winner == 1) {
		cout << "Player " << winner + 1 << " won the game !" << endl;
	}
}

