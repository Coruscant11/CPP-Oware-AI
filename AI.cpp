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

    if (totalCoupPossible >= 11) 
        maxDepth = 7;
    else if (totalCoupPossible >= 8)
        maxDepth = 9;
    else if (totalCoupPossible >= 5)
        maxDepth = 11;
    else if (totalCoupPossible >= 2)
        maxDepth = 12;
    else
        maxDepth = 3;

    cout << "Profondeur max : " << maxDepth + 1 << " pour " << totalCoupPossible << " coups" << endl;

    for (threadIndex = 0; threadIndex < THREAD_AMOUNT; threadIndex++) {
        //cout << "ti : " << threadIndex << endl;
        threads[threadIndex] = thread([&tIndex, &indexsPerThreads, &board, &player, &cpt, &cptCut, &cptHf, &values, &maxDepth] (int t) {
            //cout << "start thread " << t << endl;
            //chrono::time_point<chrono::system_clock> startT, endT;
            //startT = chrono::system_clock::now();
            for (int i = 0; i < indexsPerThreads[t].size(); i++) {
                struct Array2DIndex indexs = indexsPerThreads[t][i];
                Board nextBoard = board;
                nextBoard.playMove(player, indexs.holeIndex, indexs.colorIndex == 0 ? 'R' : 'B');
                values[indexs.colorIndex][indexs.holeIndex] = minimaxAlphaBeta(nextBoard, player, Engine::getNextPlayer(player), false, 0, maxDepth, cpt, -10000000, 10000000, cptCut, cptHf);
                //values[indexs.colorIndex][indexs.holeIndex] = negamaxAlphaBeta(nextBoard, player, 0, maxDepth, cpt, -1000000, 1000000, cptCut);
            }
            //endT = chrono::system_clock::now();
            //chrono::duration<double> es = endT - startT;
            //cout << "end thread " << t << " : " << es.count() << "s" << endl;
        }, threadIndex);
    }

    for (int i = 0; i < THREAD_AMOUNT; i++) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }

    cout << cpt->load() << " minmaxs, " << cptCut->load() << " cuts, " << cptHf->load() << " nodes reutilisees via la TP." << endl;
    /*for (int x = 0; x < 2; x++) { for (int y = 0; y < 16; y++) { cout << values[x][y] << " "; } cout << endl;}
    cout << endl;*/
    end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;
    cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    delete cpt;
    delete cptCut;

    return indexMaxValueArray(values);
}

int AI::minimaxAlphaBeta(Board board, int maxPlayer, int player, bool isMax, int depth, int maxDepth, atomic<int> *cpt, int alpha, int beta, atomic<int> *cptCut, atomic<int> *cptHf) {
    cpt->fetch_add(1);

    int alphaOrig = alpha;

    /*auto currentHash = Hash::computeHash(board);
    if (Hash::contains(currentHash)) {
        struct HashedBoard hbEntry = Hash::getValue(currentHash);
        if (hbEntry.depth >= depth) {
            cptHf->fetch_add(1);
            if (hbEntry.flag = EXACT) {
                return hbEntry.eval;
            }
            else if (hbEntry.flag == LOWERBOUND) {
                alpha = max(alpha, hbEntry.eval);
            }
            else if (hbEntry.flag == UPPERBOUND) {
                beta = min(beta, hbEntry.eval);
            }
            if (beta <= alpha) {
                return hbEntry.eval;
            }
        }
    }*/


    if (depth == maxDepth || board.positionFinale()) {
        int eval;
        // 1 AI
        /*
        if (player == maxPlayer) {
            eval = evaluation_ju_first(board, maxPlayer, depth);
        }
        */
        // 2 AI
        if (maxPlayer == 0){
            eval = evaluation_ju_fourth(board, maxPlayer, depth);
        } else{
            eval = evaluation_ju_first(board, maxPlayer, depth);
        }
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
                int eval;
                // 1 AI
                /*
                if (player == maxPlayer) {
                    eval = evaluation_ju_first(posNext, maxPlayer, depth);
                }
                 */
                // 2 AI
                if (maxPlayer == 0){
                    eval = evaluation_ju_fourth(posNext, maxPlayer, depth);
                } else{
                    eval = evaluation_ju_first(posNext, maxPlayer, depth);
                }
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
                int eval = minimaxAlphaBeta(moves[i].move, maxPlayer, Engine::getNextPlayer(player), false, depth + 1, maxDepth, cpt, alpha, beta, cptCut, cptHf);
                value = max(value, eval);
            }
            alpha = max(alpha, value);
            if (beta <= alpha) {
                cptCut->fetch_add(1);
                break;
            }
        }
    }
    else {
        value = 10000000;
        for (int i = 0; i < 32; i++) {
            if (moves[i].eval > numeric_limits<int>::min()) {
                int eval = minimaxAlphaBeta(moves[i].move, maxPlayer, Engine::getNextPlayer(player), true, depth + 1, maxDepth, cpt, alpha, beta, cptCut, cptHf);
                value = min(value, eval);
            }
            beta = min(beta, value);
            if (beta <= alpha) {
                cptCut->fetch_add(1);
                break;
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


int AI::evaluation (Board board, int maxPlayer, int depth){
    if (board.isWinning(maxPlayer))
        return 10000000 - depth;
    if (board.isLoosing(maxPlayer))
        return -10000000 + depth;
    if (board.draw())
        return 0;

    int quality = 0;
    int qualityAttic = board.getAtticPlayer(maxPlayer) - board.getAtticPlayer(Engine::getNextPlayer(maxPlayer));
    int qualityEmpty = 0;
    int qualityCombo = 0;
    int qualitySum = 0;
    int qualityWeak = 0;
    int qualityOffense = 0;

    int iaAcc = 0;
    int playerAcc = 0;
    for (int hole = 0; hole < 16; hole++) {
        int sum = board.blueHoles[hole] + board.redHoles[hole];
        int player = hole%2;

        if (sum > 3) {
            if (hole%2 == maxPlayer) {
                qualitySum++;
            }
            else {
                qualitySum--;
            }
        }
        if (sum == 0) {
            if (hole%2 == maxPlayer) {
                qualityEmpty--;
            }
            else {
                qualityEmpty++;
            }
        }
        if (sum == 1 || sum == 2) {
            if (hole % 2 == maxPlayer) {
                qualityWeak--;
            }
            else {
                qualityWeak++;
            }
        }
        if (sum > 0) {
            for (int red = board.redHoles[hole], currentHole = (hole + 1) % 16; red > 0; red--) {
                int sumCH = board.blueHoles[currentHole] + board.redHoles[currentHole];
                if (sumCH == 1 || sumCH == 2) {
                    if (maxPlayer == player) {
                        qualityOffense++;
                    }
                    else {
                        qualityOffense--;
                    }
                }
                else {
                    break;
                }
                currentHole = (currentHole + 1) % 16;
            }
            for (int blue = board.blueHoles[hole], currentHole = (hole + 1) % 16; blue > 0; blue--) {
                int sumCH = board.blueHoles[currentHole] + board.redHoles[currentHole];
                if (sumCH == 1 || sumCH == 2) {
                    if (maxPlayer == player) {
                        qualityOffense++;
                    }
                    else {
                        qualityOffense--;
                    }
                }
                else {
                    break;
                }

                currentHole = (currentHole + 2) % 16;
            }
        }
    }
    if (maxPlayer == 1)
        quality = (1 * qualityAttic) + (1 * qualityWeak) + (1 * qualityEmpty) + (1 * qualitySum) + (1 * qualityOffense);
    return quality;
}


int AI::evaluation_ju_second(Board board, int maxPlayer, int depth){
    if (board.isWinning(maxPlayer))
        return 10000000 - depth;
    if (board.isLoosing(maxPlayer))
        return -10000000 + depth;
    if (board.draw())
        return 0;

    int opponent = (maxPlayer+1)%2;
    int diff_seed_attic = board.getAtticPlayer(maxPlayer) - board.getAtticPlayer(opponent);

    int nbRedSeedPair = board.redHoles[0] + board.redHoles[2] + board.redHoles[4] + board.redHoles[6]+
                        board.redHoles[8] + board.redHoles[10] + board.redHoles[12] + board.redHoles[14];
    int nbBlueSeedPair = board.blueHoles[0] + board.blueHoles[2] + board.blueHoles[4] + board.blueHoles[6]+
                         board.blueHoles[8] + board.blueHoles[10] + board.blueHoles[12] + board.blueHoles[14];
    int nbSeedPair = nbRedSeedPair + nbBlueSeedPair;

    int nbRedSeedImpair = board.redHoles[1] + board.redHoles[3] + board.redHoles[5] + board.redHoles[7]+
                          board.redHoles[9] + board.redHoles[11] + board.redHoles[13] + board.redHoles[15];
    int nbBlueSeedImpair = board.blueHoles[1] + board.blueHoles[3] + board.blueHoles[5] + board.blueHoles[7]+
                           board.blueHoles[9] + board.blueHoles[11] + board.blueHoles[13] + board.blueHoles[15];
    int nbSeedImpair = nbRedSeedImpair + nbBlueSeedImpair;

    int nb1_2Pair= 0;
    int nb1_2Impair = 0;
    for (int i=0; i<16; i++){
        if (board.redHoles[i]+board.blueHoles[i]){
            if (i==0 || i==2 || i==4 || i==6 || i==8 || i==10 || i==12 || i==14){
                nb1_2Pair++;
            } else{
                nb1_2Impair++;
            }
        }
    }

    int total = 0;
    if (maxPlayer == 0){
        total =  diff_seed_attic * 3;
        total += (64-nbSeedImpair);
        total += nbSeedPair;
        total += nbBlueSeedPair;
        total += 64 - nbRedSeedPair;
    }
    else{
        total =  diff_seed_attic * 3;
        total += 64 - nbSeedPair;
        total += nbSeedImpair;
        total += nbBlueSeedImpair;
        total += 64 - nbRedSeedImpair;
    }
    return total;
}

int AI::evaluation_ju_first(Board board, int maxPlayer, int depth) {
    if (board.isWinning(maxPlayer))
        return 10000000 - depth;
    if (board.isLoosing(maxPlayer))
        return -10000000 + depth;
    if (board.draw())
        return 0;

    int opponent = (maxPlayer+1)%2;
    int diff_seed_attic = board.getAtticPlayer(maxPlayer) - board.getAtticPlayer(opponent);

    int nbRedSeedPair = board.redHoles[0] + board.redHoles[2] + board.redHoles[4] + board.redHoles[6]+
                        board.redHoles[8] + board.redHoles[10] + board.redHoles[12] + board.redHoles[14];
    int nbBlueSeedPair = board.blueHoles[0] + board.blueHoles[2] + board.blueHoles[4] + board.blueHoles[6]+
                         board.blueHoles[8] + board.blueHoles[10] + board.blueHoles[12] + board.blueHoles[14];
    int nbSeedPair = nbRedSeedPair + nbBlueSeedPair;

    int nbRedSeedImpair = board.redHoles[1] + board.redHoles[3] + board.redHoles[5] + board.redHoles[7]+
                          board.redHoles[9] + board.redHoles[11] + board.redHoles[13] + board.redHoles[15];
    int nbBlueSeedImpair = board.blueHoles[1] + board.blueHoles[3] + board.blueHoles[5] + board.blueHoles[7]+
                           board.blueHoles[9] + board.blueHoles[11] + board.blueHoles[13] + board.blueHoles[15];
    int nbSeedImpair = nbRedSeedImpair + nbBlueSeedImpair;

    int nb1_2Pair= 0;
    int nb1_2Impair = 0;
    for (int i=0; i<16; i++){
        if (board.redHoles[i]+board.blueHoles[i]){
            if (i==0 || i==2 || i==4 || i==6 || i==8 || i==10 || i==12 || i==14){
                nb1_2Pair++;
            } else{
                nb1_2Impair++;
            }
        }
    }
    // changer par rapport au début / milieu / fin de partie
    int total = 0;
    if (maxPlayer == 0){
        total =  diff_seed_attic * 3;
        total += (64-nbSeedImpair);
        total += nbBlueSeedPair;
        total += 64 - nbRedSeedPair;
    }
    else{
        total =  diff_seed_attic * 3;
        total += 64 - nbSeedPair;
        total += nbBlueSeedImpair;
        total += 64 - nbRedSeedImpair;
    }
    return total;
}

int AI::evaluation_ju_third (Board board, int maxPlayer, int depth) {
    if (board.isWinning(maxPlayer))
        return 10000000 - depth;
    if (board.isLoosing(maxPlayer))
        return -10000000 + depth;
    if (board.draw())
        return 0;

    int opponent = (maxPlayer+1)%2;
    int diff_seed_attic = board.getAtticPlayer(maxPlayer) - board.getAtticPlayer(opponent);

    int nbRedSeedPair = board.redHoles[0] + board.redHoles[2] + board.redHoles[4] + board.redHoles[6]+
                        board.redHoles[8] + board.redHoles[10] + board.redHoles[12] + board.redHoles[14];
    int nbBlueSeedPair = board.blueHoles[0] + board.blueHoles[2] + board.blueHoles[4] + board.blueHoles[6]+
                         board.blueHoles[8] + board.blueHoles[10] + board.blueHoles[12] + board.blueHoles[14];
    int nbSeedPair = nbRedSeedPair + nbBlueSeedPair;

    int nbRedSeedImpair = board.redHoles[1] + board.redHoles[3] + board.redHoles[5] + board.redHoles[7]+
                          board.redHoles[9] + board.redHoles[11] + board.redHoles[13] + board.redHoles[15];
    int nbBlueSeedImpair = board.blueHoles[1] + board.blueHoles[3] + board.blueHoles[5] + board.blueHoles[7]+
                           board.blueHoles[9] + board.blueHoles[11] + board.blueHoles[13] + board.blueHoles[15];
    int nbSeedImpair = nbRedSeedImpair + nbBlueSeedImpair;

    int nb1_2Pair= 0;
    int nb1_2Impair = 0;
    for (int i=0; i<16; i++){
        if (board.redHoles[i]+board.blueHoles[i]){
            if (i==0 || i==2 || i==4 || i==6 || i==8 || i==10 || i==12 || i==14){
                nb1_2Pair++;
            } else{
                nb1_2Impair++;
            }
        }
    }
    // changer par rapport au début / milieu / fin de partie
    int total = 0;
    if (maxPlayer == 0){
        total =  diff_seed_attic * 3;
        total += (64-nbSeedImpair);
        total += nbBlueSeedPair;
        //total += 64 - nbRedSeedPair;
    }
    else{
        total =  diff_seed_attic * 3;
        total += 64 - nbSeedPair;
        total += nbBlueSeedImpair;
        //total += 64 - nbRedSeedImpair;
    }
    return total;
}

int AI::evaluation_ju_fourth (Board board, int maxPlayer, int depth) {
    if (board.isWinning(maxPlayer))
        return 10000000 - depth;
    if (board.isLoosing(maxPlayer))
        return -10000000 + depth;
    if (board.draw())
        return 0;

    int opponent = (maxPlayer+1)%2;
    int diff_seed_attic = board.getAtticPlayer(maxPlayer) - board.getAtticPlayer(opponent);

    int nbRedSeedPair = board.redHoles[0] + board.redHoles[2] + board.redHoles[4] + board.redHoles[6]+
                        board.redHoles[8] + board.redHoles[10] + board.redHoles[12] + board.redHoles[14];
    int nbBlueSeedPair = board.blueHoles[0] + board.blueHoles[2] + board.blueHoles[4] + board.blueHoles[6]+
                         board.blueHoles[8] + board.blueHoles[10] + board.blueHoles[12] + board.blueHoles[14];
    int nbSeedPair = nbRedSeedPair + nbBlueSeedPair;

    int nbRedSeedImpair = board.redHoles[1] + board.redHoles[3] + board.redHoles[5] + board.redHoles[7]+
                          board.redHoles[9] + board.redHoles[11] + board.redHoles[13] + board.redHoles[15];
    int nbBlueSeedImpair = board.blueHoles[1] + board.blueHoles[3] + board.blueHoles[5] + board.blueHoles[7]+
                           board.blueHoles[9] + board.blueHoles[11] + board.blueHoles[13] + board.blueHoles[15];
    int nbSeedImpair = nbRedSeedImpair + nbBlueSeedImpair;

    int nb1_2Pair= 0;
    int nb1_2Impair = 0;
    for (int i=0; i<16; i++){
        if (board.redHoles[i]+board.blueHoles[i]){
            if (i==0 || i==2 || i==4 || i==6 || i==8 || i==10 || i==12 || i==14){
                nb1_2Pair++;
            } else{
                nb1_2Impair++;
            }
        }
    }
    // changer par rapport au début / milieu / fin de partie
    int total = 0;

    if (maxPlayer == 0){
        total += diff_seed_attic * 3;
        total += (64 - nbSeedImpair);
        if (nbSeedPair + nbSeedImpair < 15){    // Debut de partie
            if (nb1_2Pair == 0) total += 5;
            if (nbRedSeedImpair < nbRedSeedPair) total += 2;
        }
        if (nbSeedPair + nbSeedImpair > 40){    // Fin de partie
            total += nbSeedPair;
            total += nbBlueSeedImpair;
            total += 64 - nbRedSeedImpair;
        }
        else{   // Milieu de partie
            //if (nbRedSeedImpair < nbRedSeedPair) total += 5;
            if (nbBlueSeedPair == 0) total += 10;
            total += nbBlueSeedImpair;
            total += 64 - nbRedSeedImpair;
        }
    }
    else{
        total += diff_seed_attic * 3;
        total += (64 - nbSeedPair);
        if (nbSeedImpair + nbSeedPair < 15){    // Debut de partie
            if (nb1_2Impair == 0) total += 5;
            if (nbRedSeedPair < nbRedSeedImpair) total += 2;

        }
        if (nbSeedImpair + nbSeedPair > 40){    // Fin de partie
            total += nbSeedImpair;
            total += nbBlueSeedPair;
            total += 64 - nbRedSeedPair;
        }
        else{   // Milieu de partie
           // if (nbRedSeedPair < nbRedSeedImpair) total += 5;
            if (nbBlueSeedImpair == 0) total += 10;
            total += nbBlueSeedPair;
            total += 64 - nbRedSeedPair;
        }
    }
    return total;
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