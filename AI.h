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

private:
    static int valueMinMax(Board board, int player, bool aiPlay, int depth, int depthMax);
    static int evaluation(Board board, int player, bool aiPlayer, int depth);

    static int minValueArray(int values[][16]);
    static int maxValueArray(int values[][16]);
    static struct Array2DIndex indexMaxValueArray(int values[][16]);
};

#endif //AWALECUSTOM_AI_H
