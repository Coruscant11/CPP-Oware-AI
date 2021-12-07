#include "AI.h"
#include "Engine.h"
#include "iostream"

AI::AI() {

}

AI::~AI() {

}

struct Array2DIndex AI::decisionMinMax(int player, Board board, int maxDepth) {
    int values[2][16]; // 0 -> Bleu; 1 -> Rouge

    for (int color = 0; color < 2; color++) {
        for (int move=0; move<16; move++) {

            if (board.isPossibleMove(player, move, color == 0 ? 'B' : 'R')) {
                values[color][move] = valueMinMax(board, player, false, 0, maxDepth);
            }
            else {
                values[color][move] = -10000;
            }
        }
    }

    return indexMaxValueArray(values);
}

int AI::valueMinMax(Board board, int player, bool aiPlay, int depth, int depthMax) {
    int tab_values[2][16];
    Board nextBoard = board;

    // Positions finales
    if (board.isWinning(player))
        return 10000;
    if (board.isLoosing(player))
        return -10000;
    if (board.draw())
        return 0;

    // Profondeur maximale
    if (depth == depthMax){
        return evaluation(board, player,aiPlay, depth);
    }

    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            char colorC = color == 0 ? 'B' : 'R';

            if (board.isPossibleMove(player, hole, colorC)) {
                Board::playMove(nextBoard, player, hole, colorC);
                tab_values[color][hole] = valueMinMax(nextBoard, Engine::getNextPlayer(player), !aiPlay, depth+1, depthMax);
            }

            else {
                if (aiPlay)
                    tab_values[color][hole] = -10000;
                else
                    tab_values[color][hole] = 10000;
            }
        }
    }
    if (aiPlay)
        return maxValueArray(tab_values);
    else
        return minValueArray(tab_values);  
}

int AI::evaluation(Board board, int player, bool aiPlay, int depth) {
    return board.getAtticPlayer(player) - board.getAtticPlayer((player + 1) % 2);
}

int AI::minValueArray(int values[][16]) {
    struct Array2DIndex indexs;
    indexs.colorIndex = 0;
    indexs.holeIndex = 0;

    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            if (values[color][hole] < values[indexs.colorIndex][indexs.holeIndex]) {
                indexs.colorIndex = color;
                indexs.holeIndex = hole;
            }
        }
    }

    return values[indexs.colorIndex][indexs.holeIndex];
}

int AI::maxValueArray(int values[][16]) {
    struct Array2DIndex indexs;
    indexs.colorIndex = 0;
    indexs.holeIndex = 0;

    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            if (values[color][hole] > values[indexs.colorIndex][indexs.holeIndex]) {
                indexs.colorIndex = color;
                indexs.holeIndex = hole;
            }
        }
    }

    return values[indexs.colorIndex][indexs.holeIndex];
}

struct Array2DIndex AI::indexMaxValueArray(int values[][16]) {
    struct Array2DIndex indexs;
    indexs.colorIndex = 0;
    indexs.holeIndex = 0;

    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            if (values[color][hole] > values[indexs.colorIndex][indexs.holeIndex]) {
                indexs.colorIndex = color;
                indexs.holeIndex = hole;
            }
        }
    }

    return indexs;
}