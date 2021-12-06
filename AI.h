#ifndef AWALECUSTOM_AI_H
#define AWALECUSTOM_AI_H

#include "Board.h"

class AI {
public:
    AI();
    ~AI();
    static int decisionMinMax(int player, Board board, int maxDepth);

private:
    static int valueMinMax(Board board, int player, bool aiPlay, int depth, int depthMax);
    static int evaluation(Board board, int player, bool aiPlayer, int depth);

    static int minValueArray(int tab[], int tabSize);
    static int maxValueArray(int tab[], int tabSize);
    static int indexMaxValueArray(int tab[], int tabSize);
};

#endif //AWALECUSTOM_AI_H
