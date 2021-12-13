#include <bits/stdc++.h> 
#include "Board.h"
#include <unordered_map>
#include <shared_mutex>
#include <mutex>

using namespace std;

struct HashedBoard {
    long long int zobrist_key;
    int depth;
    int flag;
    int eval;
};


class Hash {
public:
    inline static long long ZobrisHoleTable[2][16][64];
    inline static long long ZobrisAtticTable[2][64];

    inline static unordered_map<long long, struct HashedBoard> TranspositionTable;
    inline static std::shared_mutex mutex_;

    static void init_table()
    {
        TranspositionTable.reserve(100000000);
        int i,k,j;
        for(i = 0;i < 2; i++){
            for(j = 0;j < 16; j++){
                for(k = 0;k < 64; k++)
                {
                    ZobrisHoleTable[i][j][k] = rand();
                }
            }
        }

        for(i = 0; i < 2; i++) {
            for (j = 0; j < 64; j++) {
                ZobrisAtticTable[i][j] = rand();
            }
        }
    }

    static long long computeHash(Board b)
    {
        long long int h=0;
        for(int j=0;j<16;j++) {
            h^=ZobrisHoleTable[0][j][b.redHoles[j]];
            h^=ZobrisHoleTable[1][j][b.blueHoles[j]];
        }
        h^=ZobrisAtticTable[0][b.playersAttic[0]];
        h^=ZobrisAtticTable[1][b.playersAttic[1]];

        return h;
    }

    static void add(long long int hashcode, struct HashedBoard hb) {
        std::unique_lock lock(mutex_);
        TranspositionTable[hashcode] = hb;
    }

    static struct HashedBoard getValue(long long int hashcode) {
        //std::shared_lock lock(mutex_);
        return TranspositionTable[hashcode];
    }

    static bool contains(long long int hashcode) {
        //std::shared_lock lock(mutex_);
        return TranspositionTable.contains(hashcode);
    }
};
