#ifndef AWALECUSTOM_BOARD_H
#define AWALECUSTOM_BOARD_H

using namespace std;

// Convention: (pour l'instant)
// J1 a les case impair donc pair dans le tableau et l'attic 1
// J2 a les cases pair donc impair dans le tableau et l'attic 0

class Board {
public:
    Board();
    Board(const Board &b);
    ~Board();

    // Getter
    int getAtticPlayer(int player);

    // Print the board
    void printBoard();

    // Distribute and steal seed
    int distributeBlueSeed(int chosenHole);
    int distributeRedSeed(int chosenHole);
    int pickSeed(int lastHole, int chosenHole, int player);
    void playMove(int player, int hole, char color);

    // Possible Move
    bool isPossibleMove(int player, int move, char color);

    // Check the end of the game
    bool positionFinale();
    int checkWin();
    bool checkFamine(int player);
    bool checkHasMoreThanHalfSeeds(int player);
    bool checkLessHeightSeed();
    void giveAllSeedsToPlayer(int player);

    bool isWinning(int player);
    bool isLoosing(int player);
    bool draw();

    int redHoles[16];
    int blueHoles[16];

    int playersAttic[2];
};


#endif //AWALECUSTOM_BOARD_H