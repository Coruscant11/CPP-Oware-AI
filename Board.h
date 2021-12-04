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

private:
    int redHoles[16];
    int blueHoles[16];

    int playersAttic[2];

    bool checkFamine(int player);
    bool checkHasHalfSeeds(int player);
    bool checkLessHeightSeed()



};


#endif //AWALECUSTOM_BOARD_H