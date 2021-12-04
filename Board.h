#ifndef AWALECUSTOM_BOARD_H
#define AWALECUSTOM_BOARD_H

#include <string>

using namespace std;

class Board {
public:
    Board();
    ~Board();

    // Print the board
    string printBoard();

    // Distribute and steal seed
    int distributeBlueSeed(int chosenHole);
    int distributeRedSeed(int chosenHole);
    int pickSeed(int lastHole, int chosenHole);

    //
    bool* getPossibleMove(int player);

    bool checkFamine(int player);
    bool checkHasMoreThanHalfSeeds(int player);
    bool checkLessHeightSeed();

    bool isWinning(int player);
    bool isLoosing(int player);
    bool draw();

private:
    int redHoles[16];
    int blueHoles[16];

    int playersAttic[2];





};


#endif //AWALECUSTOM_BOARD_H