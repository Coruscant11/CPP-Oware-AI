#include <iostream>
#include "Engine.h"
#include "Board.h"
#include "hash.h"

using namespace std;

long long int ZobrisTable[2][16][64];

int main() {
    Hash::init_table();
    Engine e;
    e.run();
}
