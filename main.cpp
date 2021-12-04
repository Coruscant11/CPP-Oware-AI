#include <iostream>
#include "Board.h"

int main() {

    Board board = Board();
    cout << board.printBoard() << endl;
    board.distributeBlueSeed(15);
    cout << board.printBoard() << endl;
    board.pickSeed(15, 7);
    cout << board.printBoard() << endl;


    return 0;
}
