#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>

#define MAX_FILES 100
#define MAX_LENGTH 256
#define MAX_GRAPHES 100
#define INFINITY (INT_MAX / 4) // Valeur représentant l'infini pour éviter les débordements


int estTxt(const char *nom){
    // Vérifie si le nom de fichier se termine par ".txt"
    size_t len = strlen(nom);
    if (len < 4) {
        return 0;
    }
    return (strcmp(nom + len - 4, ".txt") == 0);
}

char* choixTxt() {
    // Liste des fichiers .txt dans le répertoire courant
    DIR *dir;
    struct dirent *entree;
    char *fichiers[MAX_FILES];
    int compteur = 0;

    // Ouverture du répertoire courant
    dir = opendir(".");
    if (dir == NULL){
        printf("Erreur d'ouverture du répertoire.\n");
        return NULL;
    }

    // Parcours des entrées du répertoire
    while ((entree = readdir(dir)) != NULL){
        // Vérification de l'extension .txt
        if (estTxt(entree->d_name)) {
            // Ajout du nom de fichier à la liste
            if (compteur < MAX_FILES) {
                char *copie = malloc(strlen(entree->d_name) + 1);
                strcpy(copie, entree->d_name);
                fichiers[compteur++] = copie;
            }
        }
    }
    closedir(dir);

    // Affichage des fichiers trouvés
    if (compteur == 0) {
        printf("Aucun fichier .txt trouvé dans le répertoire courant.\n");
        return NULL;
    }
    printf("Fichiers .txt trouvés :\n");
    for (int i = 0; i < compteur; i++) {
        printf("%d: %s\n", i + 1, fichiers[i]);
    }

    // Choix de l'utilisateur
    int choix;
    printf("Entrez le numéro du fichier à choisir : ");
    scanf("%d", &choix);
    while (choix < 1 || choix > compteur) {
        printf("Choix invalide. Veuillez réessayer : ");
        scanf("%d", &choix);
    }

    // Retourner le nom du fichier choisi
    char *fichierChoisi = fichiers[choix - 1];

    // Libération de la mémoire des autres fichiers
    for (int i = 0; i < compteur; i++) {
        if (i != choix - 1) {
            free(fichiers[i]);
        }
    }

    return fichierChoisi;
}


typedef struct {
    int nbSommets;
    int **adjMat;
} Graphe;


// Creation d'une matrice n * n initialisée à INFINITY
int **allouerMatrice(int n) {
    int **m = malloc(n * sizeof(int *));
    if (m == NULL) {
        printf("Erreur d'allocation memoire (lignes).\n");
        return NULL;
    }
    for (int i = 0; i < n; i++) {
        m[i] = malloc(n * sizeof(int));
        if (m[i] == NULL) {
            printf("Erreur d'allocation memoire (colonnes).\n");
            // Libérer ce qui a déjà été alloué
            for (int k = 0; k < i; k++) {
                free(m[k]);
            }
            free(m);
            return NULL;
        }
        for (int j = 0; j < n; j++) {
            // Initialiser à l'infini
            m[i][j] = INFINITY;
        }
    }
    return m;
}

// Libération de la mémoire allouée pour la matrice
void libererMatrice(int **m, int n) {
    for (int i = 0; i < n; i++) {
        free(m[i]);
    }
    free(m);
}

void afficherMatrice(int **matAdj, int nbSommets) {
    // 1. Calcul de la largeur maximale nécessaire pour une case
    int larMaxTxt = 1; 
    char tamponTexte[64];

    // On calcule la largeur nécessaire pour afficher le plus grand indice (ex: "10" prend 2 caractères)
    int larIndice = snprintf(NULL, 0, "%d", nbSommets);
    if (larIndice > larMaxTxt) {
        larMaxTxt = larIndice;
    }

    // On parcourt toute la matrice pour trouver le contenu le plus large
    for (int i = 0; i < nbSommets; i++) {
        for (int j = 0; j < nbSommets; j++) {
            if (matAdj[i][j] == INFINITY) {
                // "INF" fait 3 caractères
                if (3 > larMaxTxt) {
                    larMaxTxt = 3;
                }
            } else {
                int larNombre = snprintf(NULL, 0, "%d", matAdj[i][j]);
                if (larNombre > larMaxTxt) {
                    larMaxTxt = larNombre;
                }
            }
        }
    }

    // Définition des dimensions finales pour l'affichage
    int largeurCellule = larMaxTxt + 2; // Contenu + 2 espaces de marge
    int largeurEnteteLigne = larIndice + 1; // Largeur de la colonne de gauche

    // 2. Affichage des en-têtes de colonnes (1, 2, 3...)
    printf("%*s ", largeurEnteteLigne, " "); // Coin vide en haut à gauche
    for (int j = 0; j < nbSommets; j++) {
        // Conversion de l'indice de colonne en texte (j + 1 pour affichage naturel)
        int longueurTexte = snprintf(tamponTexte, sizeof(tamponTexte), "%d", j + 1);
        
        // Calcul des espaces pour centrer le texte
        int espaceGauche = (largeurCellule - longueurTexte) / 2;
        int espaceDroite = largeurCellule - longueurTexte - espaceGauche;
        
        // Affichage : Espaces + Chiffre + Espaces
        printf("%*s%s%*s", espaceGauche, "", tamponTexte, espaceDroite, "");
    }
    printf("\n");

    // 3. Affichage de la ligne de séparation horizontale
    printf("%*s ", largeurEnteteLigne, " ");
    for (int k = 0; k < nbSommets * largeurCellule; k++) {
        printf("-");
    }
    printf("\n");

    // 4. Affichage du contenu de la matrice ligne par ligne
    for (int i = 0; i < nbSommets; i++) {
        // Affichage de l'indice de la ligne à gauche (i + 1)
        printf("%*d|", largeurEnteteLigne, i + 1);

        for (int j = 0; j < nbSommets; j++) {
            int longueurTexte = 0;

            // MODIFICATION ICI : Gestion de l'affichage INF
            if (matAdj[i][j] == INFINITY) {
                strcpy(tamponTexte, "INF");
                longueurTexte = 3;
            } else {
                longueurTexte = snprintf(tamponTexte, sizeof(tamponTexte), "%d", matAdj[i][j]);
            }

            // Calcul du centrage pour la case actuelle
            int espaceGauche = (largeurCellule - longueurTexte) / 2;
            int espaceDroite = largeurCellule - longueurTexte - espaceGauche;

            // Affichage de la valeur centrée
            printf("%*s%s%*s", espaceGauche, "", tamponTexte, espaceDroite, "");
        }
        printf("\n");
    }
}

// Fonction de choix du graphe par l'utilisateur
Graphe *choixGraphe(const char *nomFichier) {
    FILE *f = fopen(nomFichier, "r");
    if (f == NULL) {
        printf("Erreur d ouverture du fichier %s.\n", nomFichier);
        return NULL;
    }

    Graphe *graphes[MAX_GRAPHES];
    int nbGraphes = 0;
    bool erreurLecture = false;

    while (!erreurLecture && nbGraphes < MAX_GRAPHES) {
        int n, m;
        int verif_format;

        // Lecture du nombre de sommets
        verif_format = fscanf(f, "%d", &n);
        if (verif_format == EOF) {
            // Fin de fichier normale : plus de graphes a lire
            break;
        }
        if (verif_format != 1 || n <= 0) { // Ajout vérification n > 0
            printf("Format de fichier incorrect (nb sommets).\n");
            erreurLecture = true;
            break;
        }

        // Lecture du nombre d aretes
        verif_format = fscanf(f, "%d", &m);
        if (verif_format != 1 || m < 0) { // Ajout vérification m >= 0
            printf("Format de fichier incorrect (nb aretes).\n");
            erreurLecture = true;
            break;
        }

        // Creation du graphe
        Graphe *g = malloc(sizeof(Graphe));
        if (g == NULL) {
            printf("Erreur d allocation memoire pour le graphe.\n");
            erreurLecture = true;
            break;
        }
        g->nbSommets = n;
        g->adjMat = allouerMatrice(n); // allouerMatrice initialise à INFINITY
        if (g->adjMat == NULL) {
            printf("Erreur d allocation memoire pour la matrice d adjacence.\n");
            free(g);
            erreurLecture = true;
            break;
        }

        // Initialiser la diagonale à 0
        for (int i = 0; i < n; i++) {
            g->adjMat[i][i] = 0;
        }

        // Lecture des m aretes
        for (int i = 0; i < m; i++) {
            int u, v, w;
            verif_format = fscanf(f, "%d %d %d", &u, &v, &w);
            if (verif_format != 3) {
                printf("Format de fichier incorrect (ligne arete %d).\n", i + 1);
                erreurLecture = true;
                break;
            }
            if (u >= 0 && u < n && v >= 0 && v < n) {
                // On accepte w=0
                g->adjMat[u][v] = w;
            } else {
                printf("Arc invalide (sommet hors limites) : %d -> %d\n", u, v);
            }
        }

        if (erreurLecture) {
            libererMatrice(g->adjMat, g->nbSommets);
            free(g);
            break;
        }

        graphes[nbGraphes++] = g;
    }

    fclose(f);

    if (erreurLecture || nbGraphes == 0) {
        printf("Aucun graphe valide n a ete lu dans le fichier.\n");
        for (int i = 0; i < nbGraphes; i++) {
            libererMatrice(graphes[i]->adjMat, graphes[i]->nbSommets);
            free(graphes[i]);
        }
        return NULL;
    }

    // Affichage des graphes disponibles
    printf("Liste des graphes disponibles dans le fichier %s :\n", nomFichier);
    for (int i = 0; i < nbGraphes; i++) {
        printf("Graphe %d (%d sommets):\n", i + 1, graphes[i]->nbSommets);
        afficherMatrice(graphes[i]->adjMat, graphes[i]->nbSommets);
        printf("\n");
    }

    // Choix du graphe par l utilisateur
    int choix = 0;
    printf("Entrez le numero du graphe a utiliser : ");
    scanf("%d", &choix);
    while (choix < 1 || choix > nbGraphes) {
        printf("Choix invalide. Veuillez reessayer : ");
        scanf("%d", &choix);
    }

    Graphe *gChoisi = graphes[choix - 1];

    // Liberation des autres graphes
    for (int i = 0; i < nbGraphes; i++) {
        if (i != choix - 1) {
            libererMatrice(graphes[i]->adjMat, graphes[i]->nbSommets);
            free(graphes[i]);
        }
    }

    return gChoisi;
}

void afficherChemin(int u, int v, int **P) {
    if (P[u][v] == -1) {
        printf("Pas de chemin de %d a %d\n", u, v);
        return;
    }
    int courant = u;
    printf("%d", courant);
    while (courant != v) {
        courant = P[courant][v];
        if (courant == -1) {
            printf(" -> ? (chemin interrompu)\n");
            return;
        }
        printf(" -> %d", courant);
    }
    printf("\n");
}

// Algorithme de Floyd-Warshall avec affichage des matrices L_k et P_k
// Algorithme de Floyd-Warshall avec affichage des matrices L_k et P_k
void floydWarshall(Graphe *g) {
    int n = g->nbSommets;
    int **L = allouerMatrice(n); // allouerMatrice initialise à INFINITY
    int **P = allouerMatrice(n); // allouerMatrice initialise à INFINITY

    if (L == NULL || P == NULL) {
        printf("Erreur d allocation memoire pour les matrices L ou P.\n");
        if (L) libererMatrice(L, n);
        if (P) libererMatrice(P, n);
        return;
    }

    // CORRECTION : Initialisation des matrices L (L_0) et P (P_0)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // L_0 est une copie de la matrice d'adjacence
            L[i][j] = g->adjMat[i][j];

            // P_0 : s'il y a un chemin direct (pas la diagonale)
            if (i != j && L[i][j] < INFINITY) {
                P[i][j] = j; // Le prochain sommet est j
            } else {
                P[i][j] = -1; // Pas de chemin direct ou diagonale
            }
        }
    }

    // Affichage de l'état initial (L_0 et P_0)
    printf("\nEtat initial (Etape 0):\n");
    printf("Matrice L_0 (Graphe initial) :\n");
    afficherMatrice(L, n);
    printf("Matrice P_0 :\n");
    afficherMatrice(P, n);


    // Algorithme de Floyd-Warshall + affichage intermediaire
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                // Vérification anti-débordement avant l'addition
                if (L[i][k] < INFINITY && L[k][j] < INFINITY) {
                    if (L[i][k] + L[k][j] < L[i][j]) {
                        L[i][j] = L[i][k] + L[k][j];
                        P[i][j] = P[i][k];
                    }
                }
            }
        }

        // Affichage des matrices intermediaires L_k et P_k
        printf("\nEtape numero %d (passage par sommet %d):\n", k + 1, k);
        printf("Matrice L :\n");
        afficherMatrice(L, n);
        printf("Matrice P :\n");
        afficherMatrice(P, n);
    }

    // Affichage des matrices finales L et P
    printf("\n--- Resultats finaux ---\n");
    printf("Matrice des plus courts chemins L :\n");
    afficherMatrice(L, n);
    printf("Matrice des prochains sommets P :\n");
    afficherMatrice(P, n);

    // Detection des circuits absorbants (cycles negatifs)
    bool cycle = false;
    for (int i = 0; i < n; i++) {
        if (L[i][i] < 0) {
            if (!cycle) {
                printf("Au moins un circuit absorbant est present :\n");
            }
            cycle = true;
            printf(" - Un circuit absorbant implique le sommet %d (L[%d][%d] = %d)\n", i, i, i, L[i][i]);
        }
    }
    if (!cycle) {
        printf("Aucun circuit absorbant n est present dans ce graphe.\n");
    }

    // Si pas de circuit absorbant, interface pour afficher les chemins minimaux
    if (!cycle) {
        while (1) {
            char reponse;
            // Vider le buffer d'entrée pour éviter les problèmes avec scanf
            while ((getchar()) != '\n'); 

            printf("\nVoulez-vous afficher un chemin de valeur minimale ? (o/n) ");
            scanf(" %c", &reponse);
            if (reponse != 'o' && reponse != 'O') {
                break; // "Si non, alors arreter"
            }

            int sommet_d = -1, sommet_a = -1;
            printf("Sommet de depart ? ");
            while (scanf("%d", &sommet_d) != 1 || sommet_d < 0 || sommet_d >= n) {
                printf("Entree invalide. Sommet de depart (entre 0 et %d) ? ", n - 1);
                while ((getchar()) != '\n'); // Vider buffer
            }

            printf("Sommet d arrivee ? ");
            while (scanf("%d", &sommet_a) != 1 || sommet_a < 0 || sommet_a >= n) {
                printf("Entree invalide. Sommet d'arrivee (entre 0 et %d) ? ", n - 1);
                while ((getchar()) != '\n'); // Vider buffer
            }


            if (L[sommet_d][sommet_a] == INFINITY) {
                printf("Aucun chemin entre %d et %d.\n", sommet_d, sommet_a);
            } else {
                printf("Distance minimale de %d a %d = %d\n", sommet_d, sommet_a, L[sommet_d][sommet_a]);
                printf("Chemin : ");
                afficherChemin(sommet_d, sommet_a, P);
            }
        }
    }

    // Liberation de la memoire
    libererMatrice(L, n);
    libererMatrice(P, n);
}


// Fonction principale
int main(void) {
    while (1) {
        char *fichier = choixTxt();
        if (fichier == NULL) {
            return 1;
        }

        Graphe *g = choixGraphe(fichier);
        free(fichier);    // on n'a plus besoin du nom
        if (g == NULL) {
            return 1;
        }

        floydWarshall(g);

        libererMatrice(g->adjMat, g->nbSommets);
        free(g);

        char reponse;
        printf("Voulez-vous traiter un autre graphe ? (o/n) ");
        scanf(" %c", &reponse);
        if (reponse != 'o' && reponse != 'O') {
            break;
        }
    }
    return 0;
}
