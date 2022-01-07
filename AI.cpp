#include "AI.h"
#include "Engine.h"
#include "hash.h"

#define EXACT 0
#define LOWERBOUND 1
#define UPPERBOUND 2

#define THREAD_AMOUNT 4

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
    atomic<int> *cptHf = new atomic<int>(0);

    int values[2][16]; // 0 -> Bleu; 1 -> Rouge

    thread threads[THREAD_AMOUNT];
    int threadIndex = 0;
    vector<struct Array2DIndex> indexsPerThreads[THREAD_AMOUNT];

    int totalCoupPossible = 0;
    int tIndex = 0;
    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            if (board.isPossibleMove(player, hole, color == 0 ? 'R' : 'B')) {
                struct Array2DIndex indexs;
                indexs.colorIndex = color;
                indexs.holeIndex = hole;
                indexsPerThreads[tIndex].push_back(indexs);
                tIndex = (tIndex + 1) % THREAD_AMOUNT;
                totalCoupPossible++;
            }
            else {
                values[color][hole] = numeric_limits<int>::min();
            }
        }
    }

    switch(totalCoupPossible) {
        case 16:
            maxDepth = 9;
            break;
        case 15:
            maxDepth = 9;
            break;
        case 14:
            maxDepth = 9;
            break;
        case 13:
            maxDepth = 9;
            break;
        case 12:
            maxDepth = 10;
            break;
        case 11:
            maxDepth = 10;
            break;
        case 10:
            maxDepth = 10 ;
            break;
        case 9:
            maxDepth = 10;
            break;
        case 8:
            maxDepth = 10;
            break;
        case 7:
            maxDepth = 10;
            break;
        case 6:
            maxDepth = 10;
            break;
        case 5:
            maxDepth = 10;
            break;
        case 4:
            maxDepth = 10;
            break;
        case 3:
            maxDepth = 10;
            break;
        case 2:
            maxDepth = 10;
            break;
        case 1:
            maxDepth = 0;
            break;
        default:
            maxDepth = 10;
            break;
    }

    cout << "Starting turn..." << endl;

    end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;
    while(elapsed_seconds.count() <= 0.5) {
        for (threadIndex = 0; threadIndex < THREAD_AMOUNT; threadIndex++) {
            threads[threadIndex] = thread([&tIndex, &indexsPerThreads, &board, &player, &cpt, &cptCut, &cptHf, &values, &maxDepth] (int t) {
                for (int i = 0; i < indexsPerThreads[t].size(); i++) {
                    struct Array2DIndex indexs = indexsPerThreads[t][i];
                    Board nextBoard = board;
                    nextBoard.playMove(player, indexs.holeIndex, indexs.colorIndex == 0 ? 'R' : 'B');
                    int local_cpt = 0;
                    int local_cptCut = 0;
                    values[indexs.colorIndex][indexs.holeIndex] = minimaxAlphaBeta(nextBoard, player, Engine::getNextPlayer(player), false, 0, maxDepth, local_cpt, -10000000, 10000000, local_cptCut);
                    cpt->fetch_add(local_cpt);
                    cptCut->fetch_add(local_cptCut);                
                }
            }, threadIndex);
        }

        for (int i = 0; i < THREAD_AMOUNT; i++) {
            if (threads[i].joinable()) {
                threads[i].join();
            }
        }
        maxDepth++;
        end = chrono::system_clock::now();
        elapsed_seconds = end - start;
        if (elapsed_seconds.count() < 0.275) {
            maxDepth++;
        }
    }
    cout << "Depth : " << maxDepth << ", for " << totalCoupPossible << " move(s) available." << endl;
    cout << cpt->load() << " minimaxs calls, with " << cptCut->load() << " alphabeta cuts, " << endl;
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    delete cpt;
    delete cptCut;

    return indexMaxValueArray(values);
}

int AI::minimaxAlphaBeta(Board board, int maxPlayer, int player, bool isMax, int depth, int maxDepth, int &cpt, int alpha, int beta, int &cptCut) {
    cpt += 1;

    int alphaOrig = alpha;

    if (depth == maxDepth || board.positionFinale()) {
        int eval = evaluation(board, maxPlayer, depth);
        return eval;
    }

    /*auto currentHash = Hash::computeHash(board);
    if (Hash::contains(currentHash)) {
        struct HashedBoard hbEntry = Hash::getValue(currentHash);
        if (hbEntry.depth >= depth) {
            if (hbEntry.flag = EXACT) {
                cptHf->fetch_add(1);
                return hbEntry.eval;
            }
            else if (hbEntry.flag == LOWERBOUND) {
                alpha = max(alpha, hbEntry.eval);
            }
            else if (hbEntry.flag == UPPERBOUND) {
                beta = min(beta, hbEntry.eval);
            }
            if (beta <= alpha) {
                cptHf->fetch_add(1);
                return hbEntry.eval;
            }
        }
    }*/

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


    int value;
    if (isMax) {
        value = -10000000;
        for (int i = 0; i < 32; i++) {
            if (moves[i].eval > numeric_limits<int>::min()) {
                int eval = minimaxAlphaBeta(moves[i].move, maxPlayer, Engine::getNextPlayer(player), false, depth + 1, maxDepth, cpt, alpha, beta, cptCut);
                value = max(value, eval);
                if (value >= beta) {
                    cptCut += 1;
                    break;
                }
                alpha = max(alpha, value);
            }
        }
    }
    else {
        value = 10000000;
        for (int i = 0; i < 32; i++) {
            if (moves[i].eval > numeric_limits<int>::min()) {
                int eval = minimaxAlphaBeta(moves[i].move, maxPlayer, Engine::getNextPlayer(player), true, depth + 1, maxDepth, cpt, alpha, beta, cptCut);
                value = min(value, eval);
                if (alpha >= value) {
                    cptCut += 1;
                    break;
                }
                beta = min(beta, value);
            }
        }
    }

    /*struct HashedBoard newHbEntry;
    newHbEntry.zobrist_key = currentHash;
    newHbEntry.eval = value;
    newHbEntry.depth = depth;
    if (value <= alphaOrig) {
        newHbEntry.flag = UPPERBOUND;
    }
    else if (value >= beta) {
        newHbEntry.flag = LOWERBOUND;
    }
    else {
        newHbEntry.flag = EXACT;
    }
    Hash::add(currentHash, newHbEntry);*/

    return value;
}

int AI::evaluation(Board board, int maxPlayer, int depth) {
    if (board.isWinning(maxPlayer))
        return 10000000 - depth;
    if (board.isLoosing(maxPlayer))
        return -10000000 + depth;
    if (board.draw())
        return 0;

    int opponent = (maxPlayer+1)%2;
    int diff_seed_attic = board.getAtticPlayer(maxPlayer) - board.getAtticPlayer(opponent);

    int nbRedSeedPair = board.redHoles[0]+
                            board.redHoles[2]+
                            board.redHoles[4]+
                            board.redHoles[6]+
                            board.redHoles[8]+
                            board.redHoles[10]+
                            board.redHoles[12]+
                            board.redHoles[14];
    int nbBlueSeedPair = board.blueHoles[0]+
                            board.blueHoles[2]+
                            board.blueHoles[4]+
                            board.blueHoles[6]+
                            board.blueHoles[8]+
                            board.blueHoles[10]+
                            board.blueHoles[12]+
                            board.blueHoles[14];
    int nbSeedPair = nbRedSeedPair + nbBlueSeedPair;

    int nbRedSeedImpair = board.redHoles[1]+
                            board.redHoles[3]+
                            board.redHoles[5]+
                            board.redHoles[7]+
                            board.redHoles[9]+
                            board.redHoles[11]+
                            board.redHoles[13]+
                            board.redHoles[15];
    int nbBlueSeedImpair = board.blueHoles[1]+
                             board.blueHoles[3]+
                             board.blueHoles[5]+
                             board.blueHoles[7]+
                             board.blueHoles[9]+
                             board.blueHoles[11]+
                             board.blueHoles[13]+
                             board.blueHoles[15];
    int nbSeedImpair = nbRedSeedImpair + nbBlueSeedImpair;

    if (maxPlayer == 0){
        return diff_seed_attic*3 + (64-nbSeedImpair) + (0*nbSeedPair) + (nbBlueSeedPair);
    }
    else{
        return diff_seed_attic*3 + (64-nbSeedPair) + (0*nbSeedImpair) + (nbBlueSeedImpair);
    }
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
