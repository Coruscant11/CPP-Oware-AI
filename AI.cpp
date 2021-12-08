#include "AI.h"
#include "Engine.h"
#include "iostream"

AI::AI() {

}

AI::~AI() {

}

struct Array2DIndex AI::decisionMinMax(int player, Board board, int maxDepth) {
    int cpt = 0;
    int values[2][16]; // 0 -> Bleu; 1 -> Rouge

    for (int color = 0; color < 2; color++) {
        for (int move=0; move<16; move++) {
            if (board.isPossibleMove(player, move, color == 0 ? 'R' : 'B')) {
                Board nextBoard = board;
                nextBoard.playMove(player, move, color == 0 ? 'R' : 'B');
                values[color][move] = valueMinMax(nextBoard, (player+1)%2, false, 0, maxDepth, cpt);
            }
            else {
                values[color][move] = -10000;
            }
        }
    }

    std::cout << cpt << " appels de minmax" << std::endl;
    for (int x = 0; x < 2; x++) { for (int y = 0; y < 16; y++) { std::cout << values[x][y] << " "; } std::cout << endl;}
    std::cout << std::endl;
    return indexMaxValueArray(values);
}

int AI::valueMinMax(Board board, int player, bool isAI, int depth, int depthMax, int &cpt) {
    cpt+=1;
    int tab_values[2][16];
    for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 16; y++) {
            tab_values[x][y] = 0;
        }
    }
    Board nextBoard = board;

    //Positions finales
    if ((isAI && nextBoard.isWinning(player) )|| (!isAI && nextBoard.isLoosing(player)))
        return 64;
    if ((isAI && nextBoard.isLoosing(player)) || (!isAI && nextBoard.isWinning(player)))
        return -64;
    if (nextBoard.draw())
        return 0;

    // Profondeur maximale
    if (depth == depthMax){
        return evaluation(nextBoard, player, isAI, depth);
    }

    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            char colorC = color == 0 ? 'R' : 'B';

            if (nextBoard.isPossibleMove(player, hole, colorC)) {
                Board pos_next = nextBoard;
                pos_next.playMove(player, hole, colorC);
                tab_values[color][hole] = valueMinMax(pos_next, Engine::getNextPlayer(player), !isAI, depth+1, depthMax, cpt);
            }

            else {
                if (isAI){
                    tab_values[color][hole] = -1000;
                }
                else{
                    tab_values[color][hole] = 1000;
                }
            }
        }
    }
    if (isAI){
        return maxValueArray(tab_values);
    }
    else{
        return minValueArray(tab_values);
    }
}

int AI::evaluation(Board board, int player, bool isAI, int depth) {
    int ai = isAI ? player : (player+1)%2;
    int quality = 0;
    quality += (board.getAtticPlayer(ai) - board.getAtticPlayer((ai+1)%2));
/*
    for (int i = 0; i < 8; i++) {
        int idx = i * 2 + player;
        if (board.blueHoles[idx] + board.redHoles[idx] == 2 && board.blueHoles[idx] + board.redHoles[idx] == 3) quality - 1;
        if (board.blueHoles[idx] > 3) quality += 2;
        if (board.redHoles[idx] > 3) quality += 2;
    }

    for (int i = 0; i < 8; i++) {
        int idx = i * 2 + Engine::getNextPlayer(player);
        if (board.blueHoles[idx] + board.redHoles[idx] == 2 && board.blueHoles[idx] + board.redHoles[idx] == 3) quality += 2;
        if (board.blueHoles[idx] > 3) quality -= 1;
        if (board.redHoles[idx] > 3) quality -= 1;
    }
*/
    return quality;
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