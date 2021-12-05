#include "AI.h"

#define DEPTH_MAX 5

AI::AI() {

}

AI::~AI() {

}

int AI::decisionMinMax(int player, Board *board, int maxDepth) {
    bool* possibleMove = board->getPossibleMove(player);
    int values[16];
    for (int move=0; move<16; move++){
        if (possibleMove[move]){
            values[move] = valueMinMax(board, player, false, DEPTH);
        }
    }
    return indexMaxValueArray(values, 16);
}

int AI::valueMinMax(Board *board, int player, bool aiPlay, int depth, int depthMax) {
    int tab_values[8];
    Board nextBoard;

    // Positions finales
    if (board->isWinning(player)){
        return 100;
    }
    if (board->isLoosing(player)){
        return -100;
    }
    if (board->draw()){
        return 0;
    }

    // Profondeur maximale
    if (depth == depthMax){
        return evaluation(board, player,aiPlay, depth);
    }

    for (int move=0; move<6; move++){
        if (board->isPossibleMove(player, move)){
            // fonction playMove(&nextBoard, board, player, move, iaPlay);
            tab_values[move] = valueMinMax(&nextBoard,(player+1)%2, !aiPlay, depth+1, depthMax);
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
    int res;
    if (aiPlay){
        return maxValueArray(tab_values,8);
    }
    else{
        return minValueArray(tab_values, 8);
    }
    return res;


}

int AI::evaluation(Board *board, int player, bool aiPlay, int depth) {

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
