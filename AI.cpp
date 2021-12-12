#include "AI.h"
#include "Engine.h"
#include <vector>

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
    else if (totalCoupPossible >= 10)
        maxDepth = 9;
    else if (totalCoupPossible >= 7)
        maxDepth = 10;
    else if (totalCoupPossible >= 5)
        maxDepth = 11;
    else if (totalCoupPossible >= 2)
        maxDepth = 12;
    else
        maxDepth = 0;

    cout << "Profondeur max : " << maxDepth << " pour " << totalCoupPossible << " coups" << endl;

    for (threadIndex = 0; threadIndex < indexsPerThreads->size(); threadIndex++) {
        threads[threadIndex] = thread([&tIndex, &indexsPerThreads, &board, &player, &cpt, &cptCut, &values, &maxDepth] (int t) {
            for (int i = 0; i < indexsPerThreads[t].size(); i++) {
                struct Array2DIndex indexs = indexsPerThreads[t][i];
                Board nextBoard = board;
                nextBoard.playMove(player, indexs.holeIndex, indexs.colorIndex == 0 ? 'R' : 'B');
                values[indexs.colorIndex][indexs.holeIndex] = minimaxAlphaBeta(nextBoard, player, Engine::getNextPlayer(player), false, 0, maxDepth, cpt, -10000000, 10000000, cptCut);
                //values[indexs.colorIndex][indexs.holeIndex] = negamaxAlphaBeta(nextBoard, player, 0, maxDepth, cpt, -1000000, 1000000, cptCut);
            }
        }, threadIndex);
    }

    for (int i = 0; i < 4; i++) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }

    cout << cpt->load() << " minmaxs et " << cptCut->load() << " cuts." << endl;
    for (int x = 0; x < 2; x++) { for (int y = 0; y < 16; y++) { cout << values[x][y] << " "; } cout << endl;}
    cout << endl;
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

    /*vector<EvaluatedMove> moves = generateMoves(board, maxPlayer, player, depth);
    sortMoves(moves, isMax);*/
    struct EvaluatedMove moves[32];
    int moveIndex = 0;
    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            char colorC = color == 0 ? 'R' : 'B';
            if (board.isPossibleMove(player, hole, colorC)) {
                Board posNext = board;
                posNext.playMove(player, hole, colorC);
                int eval = evaluation(posNext, maxPlayer, depth);
                struct EvaluatedMove ev;
                ev.eval = eval;
                ev.move = posNext;
                moves[moveIndex] = ev;
            }
            else {
                struct EvaluatedMove ev;
                ev.eval = numeric_limits<int>::min();
                ev.move = board;
                moves[moveIndex] = ev;
            }
            moveIndex++;
        }
    }
    if (isMax) {
        for (int i = 0; i < 32; i++) {
            struct EvaluatedMove xEV = moves[i];

            int j = i;
            while (j>0 && moves[j-1].eval < xEV.eval) {
                moves[j] = moves[j-1];
                j--;
            }

            moves[j] = xEV;
        }
    }
    else {
        for (int i = 0; i < 32; i++) {
            struct EvaluatedMove xEV = moves[i];

            int j = i;
            while (j>0 && moves[j-1].eval > xEV.eval) {
                moves[j] = moves[j-1];
                j--;
            }

            moves[j] = xEV;
        }
    }


    if (isMax) {
        int value = -10000000;
        for (int i = 0; i < 32; i++) {
            if (moves[i].eval > numeric_limits<int>::min()) {
                int eval = minimaxAlphaBeta(moves[i].move, maxPlayer, Engine::getNextPlayer(player), false, depth + 1, maxDepth, cpt, alpha, beta, cptCut);
                value = max(value, eval);
            }
            alpha = max(alpha, value);
            if (beta <= alpha) {
                cptCut->fetch_add(1);
                return value;
            }
        }
        return value;
    }
    else {
        int value = 10000000;
        for (int i = 0; i < 32; i++) {
            if (moves[i].eval > numeric_limits<int>::min()) {
                int eval = minimaxAlphaBeta(moves[i].move, maxPlayer, Engine::getNextPlayer(player), true, depth + 1, maxDepth, cpt, alpha, beta, cptCut);
                value = min(value, eval);
            }
            beta = min(beta, value);
            if (beta <= alpha) {
                cptCut->fetch_add(1);
                return value;
            }
        }

        return value;
    }
}

vector<EvaluatedMove> AI::generateMoves(Board board, int maxPlayer, int player, int depth) {
    vector<EvaluatedMove> moves;

    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            char colorC = color == 0 ? 'R' : 'B';
            if (board.isPossibleMove(player, hole, colorC)) {
                Board posNext = board;
                posNext.playMove(player, hole, colorC);
                int eval = evaluation(posNext, maxPlayer, depth);
                struct EvaluatedMove ev;
                ev.eval = eval;
                ev.move = posNext;
                moves.push_back(ev);
            }
        }
    }

    return moves;
}

void AI::sortMoves(vector<EvaluatedMove> moves, bool isMax) {
    if (isMax) {
        for (int i = 0; i < moves.size(); i++) {
            struct EvaluatedMove xEV = moves[i];

            int j = i;
            while (j>0 && moves[j-1].eval > xEV.eval) {
                moves[j] = moves[j-1];
                j--;
            }

            moves[j] = xEV;
        }
    }
    else {
        for (int i = 0; i < moves.size(); i++) {
            struct EvaluatedMove xEV = moves[i];

            int j = i;
            while (j>0 && moves[j-1].eval < xEV.eval) {
                moves[j] = moves[j-1];
                j--;
            }

            moves[j] = xEV;
        }
    }
}

int AI::evaluation(Board board, int maxPlayer, int depth){
    if (board.isWinning(maxPlayer)){
        return 10000000 - depth;
    }
    if (board.isLoosing(maxPlayer))
        return -10000000;
    if (board.draw())
        return 0;

    int seedDifference = board.getAtticPlayer(maxPlayer) - board.getAtticPlayer((maxPlayer+1)%2);
    int amountSeedOpponent = 0;
    for (int i=0; i<16; i++){
        if (i%2 == 0){
            amountSeedOpponent += board.redHoles[i] + board.blueHoles[i];
        }
    }
    return seedDifference*2 + (64-amountSeedOpponent);
}

/*int AI::evaluation(Board board, int maxPlayer, int depth) {
    if (board.isWinning(maxPlayer))
        return 10000000 - depth;
    if (board.isLoosing(maxPlayer))
        return -10000000 + depth;
    if (board.draw())
        return 0;

    int quality = 0;
    int qualityHoles = 0;
    int qualityEmpty = 0;
    int qualityAttics = 0;
    int qualityDifference = 0;

    int iaAttic = board.getAtticPlayer(maxPlayer);
    int playerAttic = board.getAtticPlayer(Engine::getNextPlayer(maxPlayer));
    qualityAttics = iaAttic - playerAttic;
    
    int totalIaSeeds = 0;
    int totalPlayerSeeds = 0;

    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            int redAmount = board.redHoles[hole];
            int blueAmount = board.blueHoles[hole];
            int sumSeeds = redAmount + blueAmount;
            int actualPlayer = hole % 2;
            if (actualPlayer == maxPlayer)
                totalIaSeeds += sumSeeds;
            else {
                totalPlayerSeeds += sumSeeds;
            }

            int nextHoleRed = hole;
            while (redAmount > 0) {
                nextHoleRed = (nextHoleRed + 1) % 16;
                redAmount--;
                int tmpSum = board.redHoles[nextHoleRed] + board.blueHoles[nextHoleRed];
                if (tmpSum == 1 || tmpSum == 2) {
                    if (actualPlayer == maxPlayer) {
                        qualityHoles++;
                    }
                    else {
                        qualityHoles--;
                    }
                }
            }

            redAmount = board.redHoles[hole];
            blueAmount = board.blueHoles[hole];
            int nextHoleBlue = hole;
            while (blueAmount > 0) {
                while(nextHoleBlue % 2 != Engine::getNextPlayer(actualPlayer)) {
                    nextHoleBlue = (nextHoleBlue + 1) % 16; 
                }
                blueAmount--;

                int tmpSum = board.redHoles[nextHoleRed] + board.blueHoles[nextHoleRed];
                if (tmpSum == 1 || tmpSum == 2) {
                    if (actualPlayer == maxPlayer) {
                        qualityHoles++;
                    }
                    else {
                        qualityHoles--;
                    }
                }
            }

            if (sumSeeds == 0) {
                if (actualPlayer == maxPlayer) {
                    qualityEmpty--;
                }
                else {
                    qualityEmpty++;
                }
            }
        }
    }

    if (totalIaSeeds > 16) {
        qualityDifference++;
    }
    else {
        qualityDifference--;
    }

    quality = (1 * qualityAttics) + (0 * qualityEmpty) + (2 * qualityHoles) + (0*qualityDifference);
    return quality;
}*/

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

/*int AI::negamaxAlphaBeta(Board board, int player, int depth, int maxDepth, atomic<int> *cpt, int alpha, int beta, atomic<int> *cptCut) {
    cpt->fetch_add(1);

    if (depth == maxDepth || board.positionFinale())
        return evaluation(board, player, depth);

    Board moves[16];
    int movesEvals[16];
    int moveIndex = 0;

    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            char colorC = color == 0 ? 'R' : 'B';
            if (board.isPossibleMove(player, hole, colorC)) {
                moves[moveIndex] = board;
                moves[moveIndex].playMove(player, hole, colorC);
                movesEvals[moveIndex] = evaluationNega(moves[moveIndex], player, depth);
            }
            else {
                movesEvals[moveIndex] = numeric_limits<int>::min();
            }
        }
    }
    sortMoves(moves, movesEvals);

    int value = -1000000;
    for (int i = 0; i < 16; i++) {
        if (movesEvals[i] != numeric_limits<int>::min()) {
            value = max(value, -negamaxAlphaBeta(moves[i], player, depth + 1, maxDepth, cpt, -beta, -alpha, cptCut));
            alpha = max(alpha, value);
            if (alpha >= beta) {
                cptCut->fetch_add(1);
                break;
            }
        }
    }
    return value;
}*/

/*void AI::sortMoves(Board moves[16], int movesEvals[16]) {
    for (int i = 0; i < 16; i++) {
        Board xMoves = moves[i];
        int xEvals = movesEvals[i];

        int j = i;
        while (j > 0 && movesEvals[j-1] > xEvals) {
            movesEvals[j] = movesEvals[j-1];
            moves[j] = moves[j-1];
            j = j - 1;
        }

        movesEvals[j] = xEvals;
        moves[j] = xMoves;
    }
}*/


        /*for (int color = 0; color < 2; color++) {
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
        return value;*/




        /*for (int color = 0; color < 2; color++) {
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
        return value;*/