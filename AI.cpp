#include "AI.h"
#include "Engine.h"

AI::AI() {

}

AI::~AI() {

}

struct Array2DIndex AI::decisionMinMax(int player, Board board) {
    int maxDepth;
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();

    atomic<int> *cpt = new atomic<int>(0);
    atomic<int> *cptCut = new atomic<int>(0);

    int values[2][16]; // 0 -> Bleu; 1 -> Rouge

    thread threads[4];
    int threadIndex = 0;
    vector<struct Array2DIndex> indexsPerThreads[4];

    int totalCoupPossible = 0;
    int tIndex = 0;
    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            if (board.isPossibleMove(player, hole, color == 0 ? 'R' : 'B')) {
                struct Array2DIndex indexs;
                indexs.colorIndex = color;
                indexs.holeIndex = hole;
                indexsPerThreads[tIndex].push_back(indexs);
                tIndex = (tIndex + 1) % 4;
                totalCoupPossible++;
            }
            else {
                values[color][hole] = numeric_limits<int>::min();
            }
        }
    }

    if (totalCoupPossible >= 14) 
        maxDepth = 8;
    else if (totalCoupPossible >= 12)
        maxDepth = 9;
    else if (totalCoupPossible >= 10)
        maxDepth = 10;
    else if (totalCoupPossible >= 6)
        maxDepth = 14;
    else if (totalCoupPossible >= 2)
        maxDepth = 15;
    else
        maxDepth = 0;

    cout << "Profondeur max : " << maxDepth << " pour " << totalCoupPossible << " coups" << endl;

    for (threadIndex = 0; threadIndex < indexsPerThreads->size(); threadIndex++) {
        threads[threadIndex] = thread([&tIndex, &indexsPerThreads, &board, &player, &cpt, &cptCut, &values, &maxDepth] (int t) {
            for (int i = 0; i < indexsPerThreads[t].size(); i++) {
                struct Array2DIndex indexs = indexsPerThreads[t][i];
                Board nextBoard = board;
                nextBoard.playMove(player, indexs.holeIndex, indexs.colorIndex == 0 ? 'R' : 'B');
                values[indexs.colorIndex][indexs.holeIndex] = minimaxAlphaBeta(nextBoard, player, Engine::getNextPlayer(player), false, 0, maxDepth, cpt, numeric_limits<int>::min(), numeric_limits<int>::max(), cptCut);
            }
        }, threadIndex);
    }

    for (int i = 0; i < 4; i++) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }

    cout << cpt->load() << " minmaxs et " << cptCut->load() << " cuts." << endl;
    /*for (int x = 0; x < 2; x++) { for (int y = 0; y < 16; y++) { cout << values[x][y] << " "; } cout << endl;}
    cout << endl;*/
    end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;
    cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    delete cpt;
    delete cptCut;

    return indexMaxValueArray(values);
}

int AI::minimaxAlphaBeta(Board board, int maxPlayer, int player, bool isMax, int depth, int maxDepth, atomic<int> *cpt, int alpha, int beta, atomic<int> *cptCut) {
    cpt->fetch_add(1);

    if (depth == maxDepth || board.positionFinale()) {
        int eval = evaluation(board, maxPlayer, depth);
        return eval;
    } 

    if (isMax) {
        int value = numeric_limits<int>::min();
        for (int color = 0; color < 2; color++) {
            for (int hole = 0; hole < 16; hole++) {
                char colorC = color == 0 ? 'R' : 'B';

                if (board.isPossibleMove(player, hole, colorC)) {
                    Board posNext = board;
                    posNext.playMove(player, hole, colorC);
                    int eval = minimaxAlphaBeta(posNext, maxPlayer, Engine::getNextPlayer(player), false, depth + 1, maxDepth, cpt, alpha, beta, cptCut);
                    value = max(value, eval);
                }
                alpha = max(alpha, value);
                if (beta <= alpha) {
                    cptCut->fetch_add(1);
                    return value;
                }
            }
        }
        return value;
    }
    else {
        int value = numeric_limits<int>::max();
        for (int color = 0; color < 2; color++) {
            for (int hole = 0; hole < 16; hole++) {
                char colorC = color == 0 ? 'R' : 'B';

                if (board.isPossibleMove(player, hole, colorC)) {
                    Board posNext = board;
                    posNext.playMove(player, hole, colorC);
                    int eval = minimaxAlphaBeta(posNext, maxPlayer, Engine::getNextPlayer(player), true, depth + 1, maxDepth, cpt, alpha, beta, cptCut);
                    value = min(value, eval);
                }
                beta = min(beta, value);
                if (beta <= alpha) {
                    cptCut->fetch_add(1);
                    return value;
                }
            }
        }
        return value;
    }
}

int AI::evaluation(Board board, int maxPlayer, int depth) {
    if (board.isWinning(maxPlayer))
        return numeric_limits<int>::max() - depth;
    if (board.isLoosing(maxPlayer))
        return numeric_limits<int>::min() + depth;
    if (board.draw())
        return 0;

    int iaAttic = board.getAtticPlayer(maxPlayer);
    int playerAttic = board.getAtticPlayer(Engine::getNextPlayer(maxPlayer));

    return iaAttic - playerAttic;
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



int AI::negamaxAlphaBeta(Board board, int player, int depth, int maxDepth, atomic<int> *cpt, int alpha, int beta, atomic<int> *cptCut) {
    cpt->fetch_add(1);

    if (depth == maxDepth || board.positionFinale())
        return evaluation(board, player, depth);

    int value = numeric_limits<int>::min();
    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            char colorC = color == 0 ? 'R' : 'B';

            if (board.isPossibleMove(player, hole, colorC)) {
                Board posNext = board;
                posNext.playMove(player, hole, colorC);
                int nega = -negamaxAlphaBeta(posNext, Engine::getNextPlayer(player), depth + 1, maxDepth, cpt, -beta, -alpha, cptCut);
                value = max(value, nega);
                alpha = max(alpha, value);
                if (alpha >= beta) {
                    return value;
                    cptCut->fetch_add(1);
                }
            }
        }
    }
    return value;
}