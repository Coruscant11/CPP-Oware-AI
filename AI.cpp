#include "AI.h"
#include "iostream"

AI::AI() {

}

AI::~AI() {

}

int AI::decisionMinMax(int player, Board board, int maxDepth) {
    bool* possibleBlueMove;
    possibleBlueMove = board.getPossibleBlueMove(player);
    bool* possibleRedMove;
    possibleRedMove = board.getPossibleRedMove(player);

    int values[32];
    // 0->15 : Blue
    // 16-> 31 : Red
    for (int move=0; move<32; move++){
        if (move < 16){
            if (possibleRedMove[move]){
                values[move] = valueMinMax(board, player, false, 0,maxDepth);
            }
            else{
                values[move] = -100;
            }
        }
        else{
            if (possibleBlueMove[move]){
                values[move] = valueMinMax(board, player, false, 0, maxDepth);
            }
            else{
                values[move] = -100;
            }
        }
    }
    return indexMaxValueArray(values, 16);
}

int AI::valueMinMax(Board board, int player, bool aiPlay, int depth, int depthMax) {
    int tab_values[16];
    Board nextBoard;

    // Positions finales
    if (board.isWinning(player)){
        return 100;
    }
    if (board.isLoosing(player)){
        return -100;
    }
    if (board.draw()){
        return 0;
    }

    // Profondeur maximale
    if (depth == depthMax){
        return evaluation(board, player,aiPlay, depth);
    }

    for (int move=0; move<32; move++){
        bool isPossibleMove;
        if (move < 16){
            isPossibleMove = board.isPossibleMove(player, move, 'B');
        } else{
            isPossibleMove = board.isPossibleMove(player, move % 16, 'R');
        }

        if (isPossibleMove){
            if (move<16){
                Board::playMove(board, player, move, 'B');

            }else {
                Board::playMove(board, player, move%16, 'R');
            }
            tab_values[move] = valueMinMax(nextBoard,(player+1)%2, !aiPlay, depth+1, depthMax);
        }
        else{
            if (player==0){
                tab_values[move] = -100;
            }
            else{
                tab_values[move] = 100;
            }
        }
    }

    if (aiPlay){
        return maxValueArray(tab_values,8);
    }
    else{
        return minValueArray(tab_values, 8);
    }
}

int AI::evaluation(Board board, int player, bool aiPlay, int depth) {
    return board.getAtticPlayer(player)-board.getAtticPlayer((player + 1) % 2);
}

int AI::minValueArray(int *tab, int tabSize) {
    int valueMin = tab[0];
    for (int i=1; i<tabSize; i++){
        if (tab[i] < valueMin){
            valueMin = tab[i];
        }
    }
    return valueMin;
}

int AI::maxValueArray(int *tab, int tabSize) {
    int valueMax = tab[0];
    for (int i=1; i<tabSize; i++){
        if (tab[i] > valueMax){
            valueMax = tab[i];
        }
    }
    return valueMax;
}

int AI::indexMaxValueArray(int *tab, int tabSize) {
    int indexMax= 0;
    int valueMax = tab[0];
    for (int i=1; i<tabSize; i++){
        if (tab[i] > valueMax){
            valueMax = tab[i];
            indexMax = i;
        }
    }
    return indexMax;
}
