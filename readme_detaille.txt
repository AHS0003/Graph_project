PROJET : Analyse de graphes orientes ponderes avec l’algorithme de Floyd-Warshall
====================================================================================

1. OBJECTIF GENERAL DU PROGRAMME
--------------------------------

Ce programme en langage C permet d’analyser des graphes orientes ponderes a partir de fichiers texte.
Il automatise les etapes suivantes :

1. Recherche de tous les fichiers texte (.txt) presents dans le repertoire courant.
2. Choix interactif d’un fichier par l’utilisateur.
3. Lecture de plusieurs graphes contenus dans ce fichier (un fichier peut en contenir plusieurs a la suite).
4. Stockage de chaque graphe en memoire sous forme de matrice d’adjacence.
5. Affichage de tous les graphes disponibles et choix d’un graphe particulier.
6. Application de l’algorithme de Floyd-Warshall au graphe choisi :
   - calcul des plus courts chemins entre tous les couples de sommets,
   - affichage des matrices intermediaires a chaque etape de l’algorithme (matrices L et P),
   - detection d’eventuels circuits absorbants (cycles de poids total negatif).
7. Si aucun circuit absorbant n’est present :
   - interface interactive permettant de demander un plus court chemin entre deux sommets choisis,
   - affichage de la longueur du chemin minimal,
   - reconstruction du chemin lui-meme (sequence des sommets traverses).
8. Possibilite de traiter plusieurs graphes successivement sans quitter le programme.

Le programme est concu pour une equipe debutante en C : il illustre a la fois
- l’utilisation des bibliotheques standards du langage C,
- la manipulation de fichiers texte,
- les allocations dynamiques de memoire,
- la gestion de tableaux a deux dimensions (matrices),
- l’implementation d’un algorithme classique de theorie des graphes (Floyd-Warshall).


2. BIBLIOTHEQUES UTILISEES ET ROLE DE CHACUNE
---------------------------------------------

Le programme utilise plusieurs bibliotheques standards du C :

2.1. <stdio.h>
--------------
Bibliotheque standard d’entree-sortie. Elle fournit :

- printf : affichage formate vers la sortie standard (le terminal).
- scanf  : lecture formatee depuis l’entree standard (le clavier).
- FILE   : type representant un fichier ouvert.
- fopen  : ouverture d’un fichier.
- fclose : fermeture d’un fichier ouvert.
- fscanf : lecture formatee depuis un fichier.

Toutes ces fonctions sont definies par la norme du langage C et sont indispensables pour lire les fichiers de graphes et dialoguer avec l’utilisateur.

2.2. <stdlib.h>
---------------
Bibliotheque standard utilitaire. Elle fournit notamment :

- malloc : allocation dynamique de memoire (sur le tas / heap).
- free   : liberation de la memoire precedemment allouee par malloc.

Dans ce programme, malloc et free sont utilises pour :
- allouer les matrices d’adjacence des graphes,
- allouer les matrices L et P de l’algorithme de Floyd-Warshall,
- allouer les structures de type Graphe,
- allouer les chaines de caracteres pour les noms de fichiers.

2.3. <stdbool.h> (indirectement)
--------------------------------
Dans ce code, le type bool est utilise (via l’inclusion indirecte eventuelle ou via le compilateur),
mais dans une version propre, il faut inclure explicitement :

    #include <stdbool.h>

Cette bibliotheque introduit :
- bool  : type booleen,
- true  : equivalent a 1,
- false : equivalent a 0.

Dans le programme, ce type est utilise pour les variables logiques, par exemple erreurLecture dans la fonction choixGraphe.

2.4. <string.h>
---------------
Bibliotheque de gestion des chaines de caracteres. Elle fournit :

- strlen : longueur d’une chaine de caracteres (nombre de caracteres avant le caractere nul).
- strcmp : comparaison de deux chaines (retourne 0 si elles sont identiques).
- strcpy : copie d’une chaine dans une autre.

Ces fonctions sont utilisees dans estTxt (pour tester si un nom de fichier se termine par ".txt")
et dans choixTxt (pour copier les noms de fichiers trouves).

2.5. <dirent.h>
---------------
Bibliotheque (souvent specifique aux systemes de type Unix / POSIX) permettant la manipulation des repertoires. Elle fournit :

- DIR           : type representant un repertoire ouvert.
- struct dirent : structure representant une entree de repertoire (un fichier, un sous-dossier, etc.).
- opendir       : ouverture d’un repertoire (ici, le repertoire courant ".").
- readdir       : lecture successive des entrees du repertoire.
- closedir      : fermeture du repertoire.

Cette bibliotheque est utilisee dans choixTxt pour lister tous les fichiers du repertoire courant et filtrer ceux qui se terminent par ".txt".

2.6. <limits.h>
---------------
Bibliotheque qui fournit des constantes sur les limites des types entiers (INT_MAX, INT_MIN, etc.).

Ici, on s’en sert pour definir une valeur de "pseudo-infini" :

    #define INFINITY (INT_MAX / 4)

On prend INT_MAX / 4 au lieu de INT_MAX pour pouvoir encore additionner deux distances sans risque d’overflow (debordement).


3. CONSTANTES, TYPES ET STRUCTURES
----------------------------------

3.1. Macros de configuration
----------------------------

    #define MAX_FILES 100
    #define MAX_LENGTH 256
    #define MAX_GRAPHES 100
    #define INFINITY (INT_MAX / 4)

- MAX_FILES  : nombre maximal de fichiers texte (.txt) que l’on acceptera de lister dans le repertoire courant.
- MAX_LENGTH : taille maximale (en caracteres) d’un buffer eventuel pour lire des lignes (utilise conceptuellement).
- MAX_GRAPHES: nombre maximal de graphes que l’on traite en memoire pour un seul fichier de graphes.
- INFINITY   : valeur entiere tres grande representant "pas de chemin" ou "distance infinie".  
  On evite un overflow en la definissant a INT_MAX / 4.

3.2. Type Graphe
----------------

    typedef struct {
        int nbSommets;
        int **adjMat;
    } Graphe;

- nbSommets : nombre de sommets du graphe, numerotes de 0 a nbSommets - 1.
- adjMat    : matrice d’adjacence de taille nbSommets x nbSommets.

La matrice d’adjacence adjMat est representee comme un tableau de pointeurs (int**) :
- adjMat[i] pointe vers un tableau de int de taille nbSommets,
- adjMat[i][j] contient le poids de l’arete allant de i vers j,
- si aucune arete directe n’existe de i vers j, la case vaut 0 lors de la lecture initiale, puis INFINITY dans les matrices de travail.


4. DESCRIPTION DETAILLEE DES FONCTIONS
--------------------------------------

4.1. int estTxt(const char *nom)
--------------------------------

Rôle :
- Determiner si une chaine de caracteres nom se termine par l’extension ".txt".

Detail :
1. Calcul de la longueur par strlen(nom).
2. Si la longueur est < 4, on retourne 0 (trop court pour contenir ".txt").
3. Sinon, on compare les 4 derniers caracteres a ".txt" avec strcmp(nom + len - 4, ".txt").
4. Si la comparaison retourne 0, la chaine se termine bien par ".txt" → on retourne 1.
5. Sinon, on retourne 0.

Fonctions utilisees :
- strlen, strcmp de <string.h>.

Utilisation :
- Dans choixTxt pour filtrer les noms de fichiers.


4.2. char* choixTxt(void)
-------------------------

Rôle :
- Lister tous les fichiers se terminant par ".txt" dans le repertoire courant,
- Permettre a l’utilisateur de choisir l’un d’eux,
- Retourner une chaine de caracteres allouee dynamiquement contenant le nom du fichier choisi.

Detail :

1. Ouverture du repertoire courant :
   - opendir(".") renvoie un pointeur de type DIR* si reussi, sinon NULL.

2. Parcours des entrees de repertoire :
   - readdir(dir) renvoie successivement des pointeurs vers struct dirent.
   - entree->d_name contient le nom de l’entree.
   - Pour chaque nom :
     - on appelle estTxt(entree->d_name) ;
     - si ca vaut 1, on duplique la chaine:
       - allocation d’un buffer de taille strlen(nom) + 1 avec malloc,
       - copie du nom avec strcpy,
       - stockage dans le tableau fichiers[compteur].

3. Fermeture du repertoire par closedir(dir).

4. Si aucun fichier .txt n’a ete trouve (compteur == 0) :
   - affichage d’un message d’erreur,
   - retour NULL.

5. Affichage de la liste de fichiers .txt :
   - Affichage d’une ligne par fichier avec un numero (i + 1) et le nom correspondant.

6. Lecture du choix utilisateur :
   - scanf("%d", &choix) lit un entier.
   - une boucle de verification s’assure que 1 <= choix <= compteur, sinon on redemande.

7. Preparation du resultat :
   - On retient fichiers[choix - 1] comme fichierChoisi (a retourner).
   - Pour tous les autres i != choix - 1, on libere fichiers[i] avec free.
   - On retourne fichierChoisi (chaine allouee dynamiquement).

Fonctions utilisees :
- opendir, readdir, closedir de <dirent.h>,
- malloc, free de <stdlib.h>,
- strlen, strcpy de <string.h>,
- printf, scanf de <stdio.h>.

Remarque :
- La chaine retournee doit etre liberee par free une fois qu’on n’en a plus besoin.


4.3. int** allouerMatrice(int n) et void libererMatrice(int **m, int n)
-----------------------------------------------------------------------

Rôle :
- allouer et liberer des matrices carrees n x n d’entiers.

allouerMatrice :
1. alloue un tableau de n pointeurs vers int : m = malloc(n * sizeof(int*)).
2. pour chaque i de 0 a n-1 :
   - alloue un tableau de n int : m[i] = malloc(n * sizeof(int));
   - initialise chacune des n cases de cette ligne a 0.
3. retourne m.

libererMatrice :
1. pour chaque ligne i, appelle free(m[i]) ;
2. a la fin, appelle free(m).

Fonctions utilisees :
- malloc et free de <stdlib.h>.

Utilisation :
- pour adjMat dans les graphes,
- pour les matrices L et P dans floydWarshall.


4.4. void afficherMatrice(int **m, int n)
----------------------------------------

Rôle :
- Afficher proprement une matrice d’entiers n x n.

Detail :
1. Affiche une ligne d’en-tete avec les indices de colonnes : 0 1 2 ...
2. Affiche une ligne de separation (des "----").
3. Pour chaque ligne i :
   - affiche l’indice i suivi d’un separateur "|",
   - pour chaque colonne j :
     - si m[i][j] == INFINITY, affiche un point "." pour signifier "pas de chemin" ou "distance infinie" ;
     - sinon, affiche la valeur entiere.

Fonctions utilisees :
- printf de <stdio.h>.

Utilisation :
- pour afficher les matrices d’adjacence,
- pour afficher L et P pendant et apres Floyd-Warshall.


4.5. Graphe* choixGraphe(const char *nomFichier)
------------------------------------------------

Rôle :
- Lire plusieurs graphes dans un fichier, les stocker, les afficher, laisser l’utilisateur choisir celui qu’il veut analyser, et retourner ce graphe.

Detail :

1. Ouvrir le fichier avec fopen(nomFichier, "r").
   - Si fopen renvoie NULL, affichage d’une erreur et retour NULL.

2. Initialiser un tableau de pointeurs Graphe* graphes[MAX_GRAPHES] et un compteur nbGraphes.

3. Utiliser une boucle de lecture tant que :
   - il n’y a pas d’erreur de lecture (erreurLecture == false),
   - et nbGraphes < MAX_GRAPHES.

4. Pour chaque graphe :
   - Lire un entier n (nombre de sommets) avec fscanf(f, "%d", &n).
     - Si fscanf renvoie EOF, cela signifie qu’on a atteint la fin du fichier et qu’il n’y a plus de graphe a lire.
     - Si fscanf renvoie autre chose que 1, le format est invalide -> erreurLecture = true.
   - Lire un entier m (nombre d’aretes).
     - Meme verifications : resultat de fscanf doit etre 1.
   - Allouer un Graphe :
     - g = malloc(sizeof(Graphe));
     - g->nbSommets = n;
     - g->adjMat = allouerMatrice(n);
   - Lire les m aretes :
     - pour i = 0 a m-1 :
       - lire u, v, w par fscanf(f, "%d %d %d", &u, &v, &w);
       - si la lecture echoue (res != 3), format invalide -> erreurLecture.
       - si 0 <= u < n et 0 <= v < n, on met g->adjMat[u][v] = w.
   - Si aucune erreur, ajouter g a graphes[nbGraphes++].

5. Fermer le fichier avec fclose(f).

6. Si erreurLecture == true OU nbGraphes == 0 :
   - afficher un message "Aucun graphe valide n a ete lu dans le fichier." ;
   - liberer tous les graphes eventuellement alloues ;
   - retourner NULL.

7. Afficher tous les graphes disponibles :
   - pour chaque i de 0 a nbGraphes-1 :
     - appeler afficherMatrice(graphes[i]->adjMat, graphes[i]->nbSommets) ;
     - preceder l’affichage par "Graphe i+1 :".

8. Demander a l’utilisateur le numero du graphe a analyser :
   - scanf("%d", &choix) ;
   - verifier que 1 <= choix <= nbGraphes, sinon redemander.

9. Conserver un pointeur vers le graphe choisi :
   - Graphe *gChoisi = graphes[choix - 1];

10. Liberer tous les autres graphes :
    - pour i != choix - 1 :
      - libererMatrice(graphes[i]->adjMat, graphes[i]->nbSommets);
      - free(graphes[i]);

11. Retourner gChoisi.

Fonctions utilisees :
- fopen, fclose, fscanf, printf, scanf de <stdio.h>,
- malloc, free de <stdlib.h>,
- allouerMatrice, libererMatrice, afficherMatrice (fonctions du programme).


4.6. void afficherChemin(int u, int v, int **P)
-----------------------------------------------

Rôle :
- Afficher le chemin minimal de u a v en utilisant la matrice P.

Detail :
1. Si P[u][v] == -1 :
   - Il n’y a pas de chemin enregistre entre u et v -> afficher "Pas de chemin de u a v" et retourner.
2. Sinon :
   - Initialiser courant = u et afficher u.
   - Tant que courant != v :
     - mettre courant = P[courant][v].
     - si courant == -1 (cas anormal), afficher " -> ? (chemin interrompu)" et revenir.
     - sinon, afficher " -> courant".
3. Terminer par un retour a la ligne.

Fonctions utilisees :
- printf de <stdio.h>.


4.7. void floydWarshall(Graphe *g)
----------------------------------

Rôle :
- Appliquer l’algorithme de Floyd-Warshall a un graphe oriente pondere represente par une matrice d’adjacence, puis interagir avec l’utilisateur pour afficher des chemins minimaux.

Detail :

1. Recuperer n = g->nbSommets.

2. Allouer deux matrices n x n : L et P.
   - L : distances minimales entre les sommets.
   - P : prochains sommets a suivre sur les chemins minimaux.

3. Initialisation :
   - pour chaque i, j :
     - si i == j :
       - L[i][j] = 0 ;
       - P[i][j] = -1.
     - sinon, si g->adjMat[i][j] != 0 :
       - il existe une arete directe i -> j de poids w ;
       - L[i][j] = g->adjMat[i][j] ;
       - P[i][j] = j.
     - sinon :
       - aucun arc direct ;
       - L[i][j] = INFINITY ;
       - P[i][j] = -1.

4. Boucle principale de Floyd-Warshall :
   - pour chaque sommet intermediaire k de 0 a n-1 :
     - pour chaque couple (i, j) :
       - si L[i][k] < INFINITY et L[k][j] < INFINITY :
         - calculer un nouveau chemin candidat : L[i][k] + L[k][j] ;
         - si ce candidat est strictement inferieur a L[i][j] :
           - mettre L[i][j] = L[i][k] + L[k][j] ;
           - mettre P[i][j] = P[i][k] (on recopie le premier pas du chemin de i vers k).
     - apres la mise a jour pour ce k, afficher :
       - "Etape numero k+1 :",
       - la matrice L,
       - la matrice P.

5. Matrices finales :
   - apres toutes les iterations, afficher :
     - "Matrice des plus courts chemins L :" puis L,
     - "Matrice des prochains sommets P :" puis P.

6. Detection de circuits absorbants :
   - initialiser un bool cycle = false ;
   - pour chaque i de 0 a n-1 :
     - si L[i][i] < 0 :
       - si cycle est encore false, afficher "Au moins un circuit absorbant est present :" ;
       - mettre cycle = true ;
       - afficher " - cycle absorbant impliquant le sommet i".
   - si cycle reste false apres la boucle :
     - afficher "Aucun circuit absorbant n est present dans ce graphe.".

7. Interface pour l’affichage des chemins :
   - si !cycle (pas de circuit absorbant) :
     - boucle infinie :
       - demander : "Voulez-vous afficher un chemin de valeur minimale ? (o/n) " ;
       - lire un caractere rep avec scanf(" %c", &rep) ;
       - si rep != 'o' et rep != 'O', sortir de la boucle.
       - sinon :
         - demander "Sommet de depart ? " et lire s ;
         - demander "Sommet d arrivee ? " et lire t ;
         - verifier que 0 <= s < n et 0 <= t < n :
           - sinon, message "Sommets invalides." ;
         - si L[s][t] == INFINITY :
           - afficher "Aucun chemin entre s et t." ;
         - sinon :
           - afficher la distance "Distance minimale = L[s][t]" ;
           - appeler afficherChemin(s, t, P) pour afficher la suite des sommets.

8. A la fin, liberer L et P.

Fonctions utilisees :
- allouerMatrice, libererMatrice, afficherMatrice, afficherChemin,
- printf, scanf de <stdio.h>.


4.8. int main(void)
-------------------

Rôle :
- Coordonner l’execution du programme, permettre l’enchainement de plusieurs analyses de graphes sans le relancer.

Detail :

1. Boucle `while (1)` (boucle infinie) :
   - appeler choixTxt() pour obtenir un nom de fichier .txt ;
     - si retour NULL, terminer avec code d’erreur 1.
   - appeler choixGraphe(fichier) pour lire les graphes du fichier et en choisir un ;
     - si retour NULL, liberer fichier et terminer avec code 1.
   - appeler floydWarshall(g) pour analyser le graphe.
   - liberer les ressources :
     - libererMatrice(g->adjMat, g->nbSommets) ;
     - free(g) ;
     - free(fichier).
   - demander : "Voulez-vous traiter un autre graphe ? (o/n) " ;
     - si la reponse est differente de 'o' ou 'O', sortir de la boucle.

2. Retourner 0 pour signaler une fin normale.

Fonctions utilisees :
- toutes les fonctions de plus haut niveau du projet,
- printf, scanf,
- free.


5. FORMAT DES FICHIERS .TXT DE GRAPHES
--------------------------------------

Pour chaque graphe :

    n
    m
    u1 v1 w1
    u2 v2 w2
    ...
    um vm wm

- n : nombre de sommets (les sommets sont numerotes de 0 a n-1).
- m : nombre d’aretes.
- chaque ligne ui vi wi indique :
  - une arete oriente de ui vers vi,
  - de poids wi (entier, qui peut etre negatif).

Plusieurs graphes peuvent s’enchainer dans le meme fichier.

Exemple simple :

    3
    3
    0 1 4
    1 2 5
    0 2 10
    4
    4
    0 1 1
    1 2 2
    2 3 3
    0 3 10

Ici, le fichier contient deux graphes : le premier avec 3 sommets, le second avec 4 sommets.


6. RESUME POUR UNE EQUIPE
-------------------------

- Le programme montre comment :
  - manipuler des fichiers et repertoires en C,
  - structurer des donnees (struct Graphe, matrices),
  - implementer un algorithme non trivial (Floyd-Warshall),
  - gerer des allocations dynamiques et les liberer correctement,
  - interagir avec un utilisateur via la console (menus, questions, etc.).

- Chaque fonction a un role bien delimite :
  - estTxt : filtrer les noms de fichiers,
  - choixTxt : choisir un fichier de graphes,
  - allouerMatrice / libererMatrice : gerer les matrices,
  - afficherMatrice : representer des matrices a l’ecran,
  - choixGraphe : lire, stocker, afficher et choisir un graphe dans un fichier,
  - floydWarshall : coeur algorithmique (plus courts chemins + detection de cycles),
  - afficherChemin : reconstruire un chemin minimal,
  - main : coordonner l’ensemble.
