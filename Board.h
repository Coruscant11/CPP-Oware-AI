#ifndef AWALECUSTOM_BOARD_H
#define AWALECUSTOM_BOARD_H

#include <string>

using namespace std;

// Convention: (pour l'instant)
// J1 a les case impair donc pair dans le tableau et l'attic 1
// J2 a les cases pair donc impair dans le tableau et l'attic 0

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
    bool* getPossibleBlueMove(int player);
    bool* getPossibleRedMove(int player);
    bool isPossibleMove(int player, int move, char color);

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