#include <sstream>
#include <iostream>
#include <stdlib.h>
#include "Board.h"
#include "Engine.h"


#define cRESET   "\033[0m"
#define cRED     "\033[31m"
#define cBLUE    "\033[34m"
#define cYELLOW  "\033[33m"
#define cGREEN   "\033[32m"

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

Board::Board(const Board &b) {
    for (int i=0; i<16; i++){
        this->blueHoles[i] = b.blueHoles[i];
        this->redHoles[i] = b.redHoles[i];
    }

    // Player attic
    this->playersAttic[0] = b.blueHoles[0];
    this->playersAttic[1] = b.redHoles[1];
}

Board::~Board() {}

int Board::getAtticPlayer(int player) {
    return playersAttic[(player % 2)];
}

void Board::printBoard() {
    // Trou 1 en haut a gauche jusqu'au 8
    // Trou 9 en bas a droite (en dessous de 8)
    // puis vers la gauche jusqu'a 16 (en desous du 1)
    stringstream ss;
    ss << endl;

    for (int i = 1; i <= 8; i++) {
        ss << "   ";
        if (i%2 == 0) ss << cYELLOW << i << "    " << cRESET;
        else ss << cGREEN << i << "    " << cRESET;
    }
    ss << endl;
    for (int i=0; i<8; i++){
        ss << "[" << cRED << redHoles[i] << "R-" << cBLUE << blueHoles[i] <<"B" << cRESET << "] ";
    }
    ss << endl << endl;;
    for (int i= 15; i>7; i--){
        ss << "[" << cRED << redHoles[i] << "R-" << cBLUE << blueHoles[i] <<"B" << cRESET << "] ";
    }
    ss << endl;
    for (int i = 16; i >= 9; i--) {
        ss << "  ";
        if (i%2 == 0) ss << cYELLOW << i << "    " << cRESET;
        else ss << cGREEN << i << "    " << cRESET;
    }
    ss << endl;
    ss << cGREEN << "J1" << " Attic: " << playersAttic[1] << cRESET << endl;
    ss << cYELLOW << "J2" << " Attic: " << playersAttic[0] << cRESET << endl;

    cout << ss.str() << endl;
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
        currentHole = (currentHole - 1) % 16;
        nbBlueSeed = blueHoles[currentHole];
        nbRedSeed = redHoles[currentHole];
    }
    // Ajouter les seed dans le grenier
    playersAttic[(chosenHole + 1)  % 2] = nbTotalSeed;
    return nbTotalSeed;
}

Board Board::playMove(Board board, int player, int hole, char color) {
    Board nextBoard = board;
    int lastHole;
    if (color == 'B'){
        lastHole = nextBoard.distributeBlueSeed(hole);
    }
    else if (color == 'R'){
        lastHole  = nextBoard.distributeRedSeed(hole);
    }
    nextBoard.pickSeed(lastHole,hole);
    return nextBoard;
}


bool* Board::getPossibleRedMove(int player) {
    bool possibleRedMoves[16] ={false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};
    for (int i=0; i<16; i++){
        if ((i + 1) % 2 == player){
            if (redHoles[i] == 0){
                possibleRedMoves[i] = false;
            }
            else{
                possibleRedMoves[i] = true;
            }
        }
    }
    cout << possibleRedMoves <<endl;
    return possibleRedMoves;
}

bool* Board::getPossibleBlueMove(int player) {
    bool possibleBlueMoves[16] ={false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};
    for (int i=0; i<16; i++){
        if ((i + 1) % 2 == player){
            if (blueHoles[i] == 0){
                possibleBlueMoves[i] = false;
            }
            else{
                possibleBlueMoves[i] = true;
            }
        }
    }
    cout << possibleBlueMoves <<endl;
    return possibleBlueMoves;
}


bool Board::isPossibleMove(int player, int move, char color){
    if (move >= 16){
        return false;
    }
    if (move % 2 == player) {
        if (color == 'B') {
            if (blueHoles[move] != 0) {
                return true;
            } else {
                return false;
            }
        } else if (color == 'R') {
            if (redHoles[move] != 0) {
                return true;
            } else {
                return false;
            }
        }
    }
}

int Board::checkWin() {
    for (int player = 0; player < 2; player++) {
        if (checkFamine(Engine::getNextPlayer(player))) return player;
        if (checkHasMoreThanHalfSeeds(player)) return player;
    }

    if (checkLessHeightSeed()) {
        if (playersAttic[0] == playersAttic[1]) return 2;
        else return playersAttic[0] > playersAttic[1] ? 0 : 1;
    }

    return -1;
}

// Regarde si un joueur est en famine
bool Board::checkFamine(int player) {
    for (int i=player; i<16; i+=2){
        if (blueHoles[i]+redHoles[i] != 0){
            return false;
        }
    }

    void giveAllSeedsToPlayer(int player);
    return true;
}

// Regarde si un joueur a plus de la moitié des graines
bool Board::checkHasMoreThanHalfSeeds(int player) {
    return playersAttic[player] >= 33;
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
    else {
        return false;
    }
}

void Board::giveAllSeedsToPlayer(int player) {
    for (int i = 0; i < 16; i++) {
        if (redHoles[i] > 0) {
            playersAttic[player] += redHoles[i];
            redHoles[i] = 0;
        }
        if (blueHoles[i] > 0) {
            playersAttic[player] += blueHoles[i];
            blueHoles[i] = 0;
        }
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




