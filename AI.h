#ifndef AWALECUSTOM_AI_H
#define AWALECUSTOM_AI_H

#include <atomic>
#include <limits>
#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <chrono>
#include <ctime>
#include "Board.h"


using namespace std;

struct Array2DIndex {
    int colorIndex;
    int holeIndex;
};

class AI {
public:
    AI();
    ~AI();
    static struct Array2DIndex decisionMinMax(int player, Board board);
private:
    static int minimaxAlphaBeta(Board board, int maxPlayer, int player, bool isMax, int depth, int maxDepth, atomic<int> *cpt, int alpha, int beta, atomic<int> *cptCut); 
    static int negamaxAlphaBeta(Board board, int player, int depth, int maxDepth, atomic<int> *cpt, int alpha, int beta, atomic<int> *cptCut);

    static int evaluation2(Board board, int player, int depth);
    static int evaluation1(Board board, int player, int depth);
    static int basicEvaluation(Board board, int maxPlayer, int depth);

    static struct Array2DIndex indexMaxValueArray(int values[][16]);
};

#endif //AWALECUSTOM_AI_H
