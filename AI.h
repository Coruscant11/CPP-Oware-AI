#ifndef AWALECUSTOM_AI_H
#define AWALECUSTOM_AI_H

#include "Board.h"

struct Array2DIndex {
    int colorIndex;
    int holeIndex;
};

class AI {
public:
    AI();
    ~AI();
    static struct Array2DIndex decisionMinMax(int player, Board board, int maxDepth);
    static struct Array2DIndex decisionAlphaBeta(int player, Board board, int maxDepth);

private:
    static int valueMinMax(Board board, int player, int AI, int depth, int depthMax, int &cpt);
    static int valueAlphaBeta(Board board, int player, int AI, int alpha, int beta, int depth, int depthMax, int &cpt);

    static int evaluation0(Board board, int player, int AI, int depth);
    static int evaluation1(Board board, int player, int AI, int depth);

    static int minValueArray(int values[][16]);
    static int maxValueArray(int values[][16]);
    static struct Array2DIndex indexMaxValueArray(int values[][16]);
};

#endif //AWALECUSTOM_AI_H
