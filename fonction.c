#include <stdio.h>
#include "structure.h"


 nbre_occurences *initialiser_liste(){
   return NULL;
 }

nbre_occurences *insert(nbre_occurences *liste, int valeur, int occurrences ) {
    if (liste == NULL) {
        nbre_occurences *elmt = (nbre_occurences *)malloc(sizeof(nbre_occurences));
        elmt->nombre = valeur;
        elmt->occurences = 1;
        elmt->suivant = NULL;
        return elmt;
    }
    
    nbre_occurences *tete = liste;
    while (tete != NULL) {
        if (tete->nombre == valeur) {
            tete->occurences = tete->occurences + occurrences;
            return liste;
        }
        if (tete->suivant == NULL) {
            break;
        }
        tete = tete->suivant;
    }

    // Nouvel élément 
    nbre_occurences *elmt = (nbre_occurences *)malloc(sizeof(nbre_occurences));
    elmt->nombre = valeur;
    elmt->occurences = 1;
    elmt->suivant = NULL;
    tete->suivant = elmt;

    return liste;
}


void liberer_liste(nbre_occurences * liste){
   if(liste == NULL){
      perror("La liste est vide");
      exit(EXIT_FAILURE); 
   }
   nbre_occurences * suivant, *tete = liste;
   while (tete->suivant != NULL) 
   {
      suivant = tete;
      tete = tete->suivant;
      free( suivant);
   }

   return ;

}

