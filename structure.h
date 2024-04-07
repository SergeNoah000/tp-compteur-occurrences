#include <stdlib.h>
// Structure qui va aider à la recherche
typedef struct {
    int nombre;
    int occurences;
    struct nbre_occurences *suivant;
} nbre_occurences;


// Structure qui va aider à la recherche
typedef struct {
    int thread_id;
    int *array;
    int start_index;
    int end_index;
    int thread ;
} ThreadData;


//initialiser la liste
 nbre_occurences *initialiser_liste();

// fonction pour inserer un element du tableau dans la liste
 nbre_occurences *insert(nbre_occurences* liste, int valeur, int occurence);

 // fonction pour vider la liste une qu'elle n'est plus utile
 void liberer_liste(nbre_occurences *);



