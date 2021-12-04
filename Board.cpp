#include <sstream>
#include "Board.h"

using namespace std;

// Convention: (pour l'instant)
// J1 a les case impair donc pair dans le tableau et l'attic 1
// J2 a les cases pair donc impair dans le tableau et l'attic 0

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

Board::~Board() {}

string Board::printBoard() {
    //TODO
    // Trou 1 en haut a gauche jusqu'au 8
    // Trou 9 en bas a droite (en dessous de 8)
    // puis vers la gauche jusqu'a 16 (en desous du 1)
    stringstream ss;
    ss << endl;

    for (int i=0; i<8; i++){
        ss << "[" << redHoles[i] << "R-" << blueHoles[i] <<"B] ";
    }
    ss << endl;
    for (int i= 15; i>7; i--){
        ss << "[" << redHoles[i] << "R-" << blueHoles[i] <<"B] ";
    }
    ss << endl;
    ss << "J1 Attic: " << playersAttic[1] << endl;
    ss << "J2 Attic: " << playersAttic[0] << endl;

    return ss.str();
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
    playersAttic[(chosenHole + 1)  % 2] = nbTotalSeed;
    return nbTotalSeed;
}

bool* Board::getPossibleMove(int player) {
    bool possibleMoves[16];
    for (int i=0; i<16; i++){
        if ((i + 1) % 2 == player){
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

// Regarde si un joueur est en famine
bool Board::checkFamine(int player) {
    for (int i=player; i<16; i+=2){
        if (blueHoles[i]+redHoles[i] != 0){
            return false;
        }
    }
    return true;
}

// Regarde si un joueur a plus de la moitié des graines
bool Board::checkHasMoreThanHalfSeeds(int player) {
    if (playersAttic[player] >= 33){
        return true;
    }
    else{
        return false;
    }
}

// Regarde si le jeu contient 8 ou moins graines
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

bool Board::isLoosing(int player) {
    if (checkFamine(player)                     // Famine du joueur
    || checkHasMoreThanHalfSeeds((player + 1) % 2)      // Joueur adverse a plus de 33 graines
    || (checkLessHeightSeed() && playersAttic[player] < playersAttic[(player + 1) % 2]) // Moins de 8 graines dans le jeu et moins de graine que l'adversaire
    ){
        return true;
    }
    else{
        return false;
    }
}

bool Board::isWinning(int player) {
    if (checkFamine((player + 1) %2)        // Joueur adverse en famine
    || checkHasMoreThanHalfSeeds(player)    // joueur a plus de 33 graines
    || (checkLessHeightSeed() && playersAttic[player] > playersAttic[(player + 1) % 2]) // Moins de 8 graines dans le jeu et plus de graines que l'adversaire
    ){
        return true;
    }
    else{
        return false;
    }
}

bool Board::draw() {
    if (checkLessHeightSeed() && playersAttic[0] == 32 == playersAttic[1]){
        return true;
    }
    else{
        return false;
    }
}




