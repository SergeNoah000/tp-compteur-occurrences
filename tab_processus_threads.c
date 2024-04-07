


/*

        Assignment: interprocess communication
Using the command line...

1. Write a C program that takes as input an integer n and creates n processes.
2. Each process has a number (from 1 to n)
3. All the process should display their identity (PID) and their parent’s identity (PPID)
4. In addition to the previous requirement, even processes should initialise a large
size array and create p threads, each counting the number of occurrences of
each value of the array.
5. The generated array should be written in a file.
6. The work of each thread should be saved in a distinct file specifying the portion
of the file, the thread was working on.

---prendre en entrée trois paramètres : nombre de threads, nombre de processus et taille du tableau, 
   gerer et mettre les types appropriés
   ajouter le makefile
   chaque thread cherche le nombre d'occurences de chaque mot qui est contenu dans sa portion du tableau

*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#include "structure.h"
/*
La logique est la suivante:

    - Apres avoir rempli le tableau, une boucle cree le nombre de processus entrés.
    - Une fois créé, le processus va a son tour créer le nombre de threada définit par l'utilisateur
    - chaque thread créera une liste pour compter le nombre d'occurrences de chaque element dans la portion 
    du tableau qui lui est attribuée. Il le fait en parcourant le éléments de sa portion du tableau puis en appelant
    la fonction insert(int value) de struture.h pour qu'elle ajoute l'element dans la liste.
    - Une fois tous les éléments parcourus et inserés dans la liste, il ne reste plus qu'à écrire le resultat 
    dans le fichier du processus.
    - On verifie qu'une portion du tableau n'est pas restée non prise en charge par un thread. Si oui alors crée un 
    thread qui va s'en occuper. si non passer.

    - Dès que tous les thread ont finit chacun son travail, le thread principal du processus va ouvrir le fichier
    et compter et additionner le nombre d'occurrence de chaque élément du tableau.

    - Écrire enfin le résultat obtenu à la fin du fichier
*/


// Function to count occurrences of process number in a portion of the array
void *conter_occurences(void *arg) {

    // Récuperation des arguments
    ThreadData *data = (ThreadData *)arg;
    int *array = data->array;
    int start_index = data->start_index;
    int end_index = data->end_index;
    int process_num = data->thread_id; // Process number
    int thread_id = data->thread;

    //initialisation de la liste
    nbre_occurences *liste = initialiser_liste();


    // insertion des éléments dans la liste
    int count = 0;
    for (int i = start_index; i < end_index; i++) {
        liste = insert( liste, array[i], 1 );
    }

    //Écriture des resultat
    char filename[50];
    sprintf(filename, "process_%d_result.txt", process_num);
    FILE *file = fopen(filename, "a"); // Open file in append mode


    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    // fprintf(file, "Portion: [%d, %d] gérée par le thread: %d\n", start_index, end_index, thread_id);
    nbre_occurences *tete = liste;
    
    while (tete != NULL){
        fprintf(file, "%d   |   %d   |   %d   |   [%d, %d] \n", tete->nombre, tete->occurences, thread_id, start_index, end_index );
        tete = tete->suivant;
    }

    fclose(file);
    liberer_liste(liste);
    pthread_exit(NULL);
}






int main() {
    int  nombre_processus, nombre_threads, taille_tableau ;
    printf("Entrer le nombre de processus: ");
    scanf("%d", &nombre_processus);

    printf("\n\nEntrer le nombre de threads par processus: ");
    scanf("%d", &nombre_threads);

    printf("\n\nEntrer la taille du tableau: ");
    scanf("%d", &taille_tableau);

    if (nombre_threads > taille_tableau)
    {
        perror("Le nombre de threads ne peut être supérieur à la taille du tableau");
        exit(EXIT_FAILURE);
    }
    

    // Create processes
    for (pid_t numero_processus = 1; numero_processus <= nombre_processus; numero_processus++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            printf("Processus fils %d - PID: %d, PPID: %d\n", numero_processus, getpid(), getppid());
           
                // Generate and write array to file
                int *tableau_entiers = (int *)malloc(sizeof(int) * taille_tableau);
                for (int j = 0; j < taille_tableau; j++) {
                    tableau_entiers[j] = rand() % taille_tableau; // Fill array with random values
                }
                char fichier_processus[40];
                sprintf(fichier_processus, "tableau_processus_%d.txt", numero_processus );
                FILE *fichier = fopen(fichier_processus, "w");
                if (fichier == NULL) {
                    perror("Error opening file");
                    exit(EXIT_FAILURE);
                }
                for (int j = 0; j < taille_tableau; j++) {
                    fprintf(fichier, "%d ", tableau_entiers[j]);
                }
                fclose(fichier);


            //Création du fichier de resultat du processus
            char filename[50];
            sprintf(filename, "process_%d_result.txt", numero_processus);
            FILE *file = fopen(filename, "a"); // Open file in append mode
            if (file == NULL) {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
            fprintf(file, "Nombre    | Occurrences   |  Thread_id  |  Portion: [index_debut, index_fin]\n");
            fclose(file);

            

            // Division du tableau en petites portions
            pthread_t *tableau_threads = (pthread_t *)malloc(sizeof(pthread_t) * nombre_threads + 1); // ajout d'un thread pour la compatibilité
            ThreadData *tableau_threads_donnees = (ThreadData *)malloc(sizeof(ThreadData) * nombre_threads + 1);
            int chunk_size = taille_tableau / nombre_threads;

            // Création des threads
            for (int k = 0; k < nombre_threads; k++) {

                tableau_threads_donnees[k].thread_id = numero_processus ;
                tableau_threads_donnees[k].array = tableau_entiers;
                tableau_threads_donnees[k].start_index = k * chunk_size;
                tableau_threads_donnees[k].end_index = (k + 1) * chunk_size ;
                tableau_threads_donnees[k].thread = k + 1;
                pthread_create(&tableau_threads[k], NULL, conter_occurences, (void *)&tableau_threads_donnees[k]);
            }
            int k = (nombre_threads-1)*chunk_size;
            // Ajouter un dernier thraed qui s'occupe du dernier segment du tablea
            if (k < taille_tableau)
            {
                
                tableau_threads_donnees[nombre_threads].thread_id = numero_processus ;
                tableau_threads_donnees[nombre_threads].array = tableau_entiers;
                tableau_threads_donnees[nombre_threads].start_index = (nombre_threads)* chunk_size  ;   
                tableau_threads_donnees[nombre_threads].end_index = taille_tableau;
                tableau_threads_donnees[nombre_threads].thread = nombre_threads;
                pthread_create(&tableau_threads[nombre_threads], NULL, conter_occurences, (void *)&tableau_threads_donnees[nombre_threads]);
                pthread_join(tableau_threads[nombre_threads], NULL);

            }
            
            // Attendre que les threads finissent leur travail
            for (int j = 0; j < nombre_threads; j++) {
                pthread_join(tableau_threads[j], NULL);
            }

            // Liste pour stocker les occurrences totales
            nbre_occurences *occurrences_globales = initialiser_liste();

            sprintf(filename, "process_%d_result.txt", numero_processus + 0);
            file = fopen(filename, "r");
            if (file == NULL) {
                    perror("Error opening file");
                    exit(EXIT_FAILURE);
                }
            // char test[80];
            // fseek(file, 78 * sizeof(char), SEEK_SET);
        // Lire et stocker la première ligne sans la traiter
            char firstLine[100];
            fgets(firstLine, 100, file);

            // printf("test");
            for (int j = 0; j < nombre_threads; j++) {
                int nombre, occurences, thread_id, start_index, end_index;
                while (fscanf(file, "%d   |   %d   |   %d   |   [%d, %d]", &nombre, &occurences, &thread_id, &start_index, &end_index) == 5 ) {
                    occurrences_globales = insert(occurrences_globales, nombre, occurences);
                }

               
            }
            fclose(file);

            // Parcourir la liste pour afficher les occurrences totales
            nbre_occurences *tete_globale = occurrences_globales;
            int total = 0;
            FILE *ajout = fopen(filename, "a");

            while (tete_globale != NULL) {
                //printf("Nombre %d : Occurrences totales %d\n", tete_globale->nombre, tete_globale->occurences);
                fprintf(ajout, "Nombre %d : Occurrences totales %d\n", tete_globale->nombre, tete_globale->occurences);
                
                total = total+tete_globale->occurences;
                tete_globale = tete_globale->suivant;
            }
                
            fprintf(ajout, "\n\nNombre total d'élement %d \n", total);

            // Libérer la mémoire allouée pour la liste
            liberer_liste(occurrences_globales);
            fclose(ajout);
            free(tableau_entiers);
            exit(EXIT_SUCCESS);
        } else { // Parent process
            wait(NULL);
        }
    }


    return 0;
}
