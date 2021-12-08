#include "AI.h"
#include "Engine.h"
#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <chrono>
#include <ctime>

AI::AI() {

}

AI::~AI() {

}

struct Array2DIndex AI::decisionMinMax(int player, Board board, int maxDepth) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    std::atomic<int> *cpt = new std::atomic<int>(0);

    int values[2][16]; // 0 -> Bleu; 1 -> Rouge
    std::thread threads[4];
    int threadIndex = 0;

    int totalCoupPossible = 0;
    std::vector<struct Array2DIndex> indexsPerThreads[4];

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
                values[color][hole] = -10000;
            }
        }
    }

    switch(totalCoupPossible) {
        case 16:
        case 15:
        case 14:
        case 13:
        case 12:
        case 11:
        case 10:
            maxDepth = 5;
            break;
        case 9:
        case 8:
        case 7:
            maxDepth = 6;
            break;
        case 6:
        case 5:
        case 4:
            maxDepth = 7;
            break;
        case 3:
        case 2:
            maxDepth = 8;
            break;
        case 1:
            maxDepth = 1;
            break;
    }

    std::cout << "Profondeur max : " << maxDepth << " pour " << totalCoupPossible << "coups" << std::endl;

    for (threadIndex = 0; threadIndex < indexsPerThreads->size(); threadIndex++) {
        threads[threadIndex] = std::thread([&tIndex, &indexsPerThreads, &board, &player, &cpt, &values, &maxDepth] (int t) {
            for (int i = 0; i < indexsPerThreads[t].size(); i++) {
                struct Array2DIndex indexs = indexsPerThreads[t][i];
                Board nextBoard = board;
                nextBoard.playMove(player, indexs.holeIndex, indexs.colorIndex == 0 ? 'R' : 'B');
                values[indexs.colorIndex][indexs.holeIndex] = valueMinMax(nextBoard, Engine::getNextPlayer(player), 0, maxDepth, cpt);
            }
        }, threadIndex);
    }

    /*
    for (int color = 0; color < 2; color++) {
        for(int hole = 0; hole < 16; hole++) {
            if (board.isPossibleMove(player, hole, color == 0 ? 'R' : 'B')) {
                Board nextBoard = board;
                nextBoard.playMove(player, hole, color == 0 ? 'R' : 'B');
                futures[color][hole] = std::async(std::launch::async, valueMinMax, nextBoard, player, 0, 6, cpt);
            }
            else {
                futures[color][hole] = std::async(std::launch::async, []{ return -10000; });
            }
        }
    }*/

    /*
    for (int color = 0; color < 2; color++) {
        for (int move=0; move<16; move++) {
            threads[color][move] = std::thread([&board, &player, &move, &color, &cpt, &values] () {
                if (board.isPossibleMove(player, move, color == 0 ? 'R' : 'B')) {
                    Board nextBoard = board;
                    nextBoard.playMove(player, move, color == 0 ? 'R' : 'B');
                    values[color][move] = valueMinMax(nextBoard, player, 0, , cpt);
                }
                else {
                    values[color][move] = -10000;
                }
            });*/
/*
    for (int color = 0; color < 2; color++) {
        for(int hole = 0; hole < 16; hole++) {
            if (board.isPossibleMove(player, hole, color == 0 ? 'R' : 'B')) {
                Board nextBoard = board;
                nextBoard.playMove(player, hole, color == 0 ? 'R' : 'B');
                //threads[color][move] = std::thread(valueMinMax, nextBoard, player, 0, 10, cpt);
                //std::thread t(valueMinMax, nextBoard, player, 0, maxDepth, cpt[color][move])
                values[color][hole] = valueMinMax(nextBoard, player, 0, maxDepth, cpt);
            }
            else {
                values[color][hole] = -10000;
            }
        }
    }*/

    /*int acc = 0;
    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            futures[color][hole].wait();
            values[color][hole] = futures[color][hole].get();
        }
    }*/

    for (int i = 0; i < 4; i++) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }

    std::cout << cpt->load() << " appels de minmax" << std::endl;
    for (int x = 0; x < 2; x++) { for (int y = 0; y < 16; y++) { std::cout << values[x][y] << " "; } std::cout << endl;}
    std::cout << std::endl;
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    delete cpt;

    return indexMaxValueArray(values);
}

int AI::valueMinMax(Board board, int player, int depth, int depthMax, std::atomic<int> *cpt) {
    cpt->fetch_add(1);
    int tab_values[2][16];
    Board nextBoard = board;

    //Positions finales
    if (nextBoard.isWinning(player)) {
        return player == 1 ? 64 - depth : -64 + depth;
    }
    if (nextBoard.isLoosing(player))
        return player == 1 ? -64 - depth : 64 - depth;
    if (nextBoard.draw())
        return 0;

    // Profondeur maximale
    if (depth == depthMax){
        return evaluation(nextBoard, player, depth);
    }

    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            char colorC = color == 0 ? 'R' : 'B';

            if (nextBoard.isPossibleMove(player, hole, colorC)) {
                Board pos_next = nextBoard;
                pos_next.playMove(player, hole, colorC);
                tab_values[color][hole] = valueMinMax(pos_next, Engine::getNextPlayer(player), depth+1, depthMax, cpt);
            }

            else {
                if (player == 1)
                    tab_values[color][hole] = -1000;
                else
                    tab_values[color][hole] = 1000;
            }
        }
    }
    if (player == 1)
        return maxValueArray(tab_values);
    else
        return minValueArray(tab_values);  
}

int AI::evaluation(Board board, int player, int depth) {
    int quality = (board.getAtticPlayer(1) - board.getAtticPlayer(0));

    /*
    for (int i = 0; i < 8; i++) {
        int idx = i * 2 + player;
        if (board.blueHoles[idx] + board.redHoles[idx] == 2 && board.blueHoles[idx] + board.redHoles[idx] == 3) quality - 1;
        if (board.blueHoles[idx] > 3) quality += 2;
        if (board.redHoles[idx] > 3) quality += 2;
    }

    for (int i = 0; i < 8; i++) {
        int idx = i * 2 + Engine::getNextPlayer(player);
        if (board.blueHoles[idx] + board.redHoles[idx] == 2 && board.blueHoles[idx] + board.redHoles[idx] == 3) quality += 2;
        if (board.blueHoles[idx] > 3) quality -= 1;
        if (board.redHoles[idx] > 3) quality -= 1;
    }*/

    return quality;
}

int AI::minValueArray(int values[][16]) {
    struct Array2DIndex indexs;
    indexs.colorIndex = 0;
    indexs.holeIndex = 0;

    for (int color = 0; color < 2; color++) {
        for (int hole = 0; hole < 16; hole++) {
            if (values[color][hole] < values[indexs.colorIndex][indexs.holeIndex]) {
                indexs.colorIndex = color;
                indexs.holeIndex = hole;
            }
        }
    }

    return values[indexs.colorIndex][indexs.holeIndex];
}

int AI::maxValueArray(int values[][16]) {
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

    return values[indexs.colorIndex][indexs.holeIndex];
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