#include <sstream>
#include "Board.h"

using namespace std;

Board::Board() {
    // J'ai trouvé cette technique pour initialiser les trous
    // si tu trouve mieux je t'en pris
    // Base of the board
    for (int i=0; i<16; i++){
        this->blueHoles[i] = 2;
        this->redHoles[i] = 2;
    }

    // Player attic
    this->playersAttic[0] = 0;
    this->playersAttic[1] = 0;
}

string Board::printBoard() {
    //TODO
    // Trou 1 en haut a gauche jusqu'au 8
    // Trou 9 en bas a droite (en dessous de 8)
    // puis vers la gauche jusqu'a 16 (en desous du 1)
    stringstream ss;
    ss << "Print the board" << endl;
}

// Distribute only on opponent hole
int Board::distributeBlueSeed(int chosenHole) {
    int nbSeed = blueHoles[chosenHole];
    blueHoles[chosenHole] = 0;
    int currentHole = (chosenHole + 1) % 16; // Si le trou choisi est a moi le suivant et forcément celui de l'adversaire
    while (nbSeed != 0){
        blueHoles[currentHole] += 1;
        nbSeed --;
        currentHole = (currentHole + 2) % 16; // on a un trou sur deux donc on incrémente de 2 au lieu de 1
    }
    return currentHole;
}

// Distribute in each hole
int Board::distributeRedSeed(int chosenHole) {
    int nbSeed = redHoles[chosenHole];
    redHoles[chosenHole] = 0;
    int currentHole = (chosenHole + 1) % 16;
    while (nbSeed != 0){
        if (currentHole != chosenHole){
            redHoles[currentHole] += 1;
            nbSeed --;
            currentHole = (currentHole + 1) % 16;
        }
    }
    return currentHole;
}

int Board::pickSeed(int lastHole, int chosenHole) {
    int currentHole = lastHole;
    int nbBlueSeed = blueHoles[currentHole];
    int nbRedSeed = redHoles[currentHole];
    int nbTotalSeed = 0;
    while ((nbRedSeed+ nbBlueSeed) == 2 || (nbRedSeed + nbBlueSeed) == 3){
        // Convention si ça te va
        // Le joueur 0 possède le grenier 0 et les trou pair
        // Le joueur 1 possède le grenier 1 et les trous impair

        // Update board
        redHoles[currentHole] = 0;
        blueHoles[currentHole] = 0;

        // Update total seed
        nbTotalSeed = nbTotalSeed + nbRedSeed + nbBlueSeed;

        // Update the hole
        currentHole --;
        if (currentHole == -1) currentHole = 12;

        nbBlueSeed = blueHoles[currentHole];
        nbRedSeed = redHoles[currentHole];
    }
    // Ajouter les seed dans le grenier
    playersAttic[chosenHole % 2] = nbTotalSeed;
    return nbTotalSeed;
}

// Convention si ça te va
// Le joueur 0 possède le grenier 0 et les trou pair
// Le joueur 1 possède le grenier 1 et les trous impair
bool* Board::getPossibleMove(int player) {
    bool* possibleMoves;
    for (int i=0; i<16; i++){
        if (i % 2 == player){
            if (blueHoles[i]+redHoles[i] == 0){
                possibleMoves[i] = false;
            }
            else{
                possibleMoves[i] = true;
            }
        }
        else{
            possibleMoves[i] = false;
        }
    }
    return possibleMoves;
}

bool Board::checkFamine(int player) {
    for (int i=player; i<16; i+=2){
        if (blueHoles[i]+redHoles[i] != 0){
            return false;
        }
    }
    return true;
}

bool Board::checkHasHalfSeeds(int player) {
    if (playersAttic[player] >= 33){
        return true;
    }
    else{
        return false;
    }
}

bool Board::checkLessHeightSeed(){
    int nbTotalSeed = 0;
    for (int i=0; i<16; i++){
        nbTotalSeed = nbTotalSeed + redHoles[i] + blueHoles[i];
    }
    if (nbTotalSeed < 8){
        return true;
    }
    else{
        return false;
    }
}


