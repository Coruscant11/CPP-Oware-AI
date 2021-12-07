 struct Position {
       int cases_joueur[6]; // chaque case contient un nombre de
                                       // pions
       int cases_ordi[6];
       bool ordi_joue; // boolean vrai si c'est à l'ordi de jouer et faux sinon
       int pions_pris_joueur; // pions pris par le joueur
       int pions_pris_ordi; // pions pris par l'ordi
 };

 int valeurMinMax(Position* pos_courante, ordi_joue, prof, profMax){
       // ordi_joue est un booleen qui est vrai si l'ordi joue
       int tab_valeurs[6];
       Position pos_next; // En C on crée dans la pile = TRES rapide

       if (positionFinale(pos_courante, ordi_joue,prof)){
               // code à écrire
               // on retourne VALMAX (=48) si l'ordi gagne et -48 si l'ordi perd  et 0 si nul
       }
       if (prof == profMax) {
               return evaluation(pos_courante, ordi_joue,prof);
               // dans un premier temps l'évaluation sera la
               // différence du nb de pions pris
       }
       for(int i=0;i<6;i++){
               // on joue le coup i
               // ecrire la fn coupValide(pos_courante,ordi_joue,i)
               // elle teste si on peut prendre les pions dans la
               // case i et les jouer (si pas de pion alors elle retourne invalide). La position de départ
               // est pos_courante
               if (coupValide(pos_courante,ordi_joue,i)){
                       // ecrire la fn :
			 jouerCoup(&pos_next,pos_courante, ordi_joue,i)
                       // on joue le coup i a partir de la position
                       // pos_courante et on met le résultat
                       // dans pos_next
                       jouerCoup(&pos_next,pos_courante,ordi_joue,i);
 			// pos_next devient la position courante, et on change le joueur
                       tab_valeurs[i]=valeurMinMax(&pos_next, !ordi_joue,prof+1,profMax);
               } else {
			if (joueur==0) tab_valeurs[i]=-100.
			else tab_valeurs[i]=+100;
               }
       }
       int res;
       if (ordi_joue){
               // ecrire le code: res contient le MAX de
               // tab_valeurs
       } else {
               // ecrire le code : res contient le MIN de
               // tab_valeurs
       }
       return res;
}