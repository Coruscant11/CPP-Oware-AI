#ifndef AWALECUSTOM_AI_H
#define AWALECUSTOM_AI_H

#include "Board.h"

class AI {
public:
    AI();
    ~AI();
    int decisionMinMax(int player, Board* board, int maxDepth);

private:
    int valueMinMax(Board* board, int player, bool aiPlay, int depth, int depthMax);
    int evaluation(Board* board, int player, bool aiPlayer, int depth);

    int minValueArray(int tab[], int tabSize);
    int maxValueArray(int tab[], int tabSize);
    int indexMaxValueArray(int tab[], int tabSize);
};

#endif //AWALECUSTOM_AI_H
