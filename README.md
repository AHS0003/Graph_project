# Projet : Analyse de graphes orientés pondérés avec l'algorithme de Floyd-Warshall

## 1. Objectif général du programme

Ce programme en langage C permet d'analyser des graphes orientés pondérés à partir de fichiers texte.

Il automatise les étapes suivantes :

1. Recherche de tous les fichiers texte (`.txt`) présents dans le répertoire courant.
2. Choix interactif d'un fichier par l'utilisateur.
3. Lecture de plusieurs graphes contenus dans ce fichier (un fichier peut en contenir plusieurs à la suite).
4. Stockage de chaque graphe en mémoire sous forme de matrice d'adjacence.
5. Affichage de tous les graphes disponibles et choix d'un graphe particulier.
6. Application de l'algorithme de Floyd-Warshall au graphe choisi :
   - calcul des plus courts chemins entre tous les couples de sommets,
   - affichage des matrices intermédiaires à chaque étape de l'algorithme (matrices L et P),
   - détection d'éventuels circuits absorbants (cycles de poids total négatif).
7. Si aucun circuit absorbant n'est présent :
   - interface interactive permettant de demander un plus court chemin entre deux sommets choisis,
   - affichage de la longueur du chemin minimal,
   - reconstruction du chemin lui-même (séquence des sommets traversés).
8. Possibilité de traiter plusieurs graphes successivement sans quitter le programme.

Le programme est conçu pour une équipe débutante en C : il illustre à la fois :
- l'utilisation des bibliothèques standards du langage C,
- la manipulation de fichiers texte,
- les allocations dynamiques de mémoire,
- la gestion de tableaux à deux dimensions (matrices),
- l'implémentation d'un algorithme classique de théorie des graphes (Floyd-Warshall).

---

## 2. Bibliothèques utilisées et rôle de chacune

Le programme utilise plusieurs bibliothèques standards du C :

### 2.1. `<stdio.h>`

Bibliothèque standard d'entrée-sortie. Elle fournit :

| Fonction | Description |
|----------|-------------|
| `printf` | Affichage formaté vers la sortie standard (le terminal) |
| `scanf` | Lecture formatée depuis l'entrée standard (le clavier) |
| `FILE` | Type représentant un fichier ouvert |
| `fopen` | Ouverture d'un fichier |
| `fclose` | Fermeture d'un fichier ouvert |
| `fscanf` | Lecture formatée depuis un fichier |

### 2.2. `<stdlib.h>`

Bibliothèque standard utilitaire. Elle fournit notamment :

| Fonction | Description |
|----------|-------------|
| `malloc` | Allocation dynamique de mémoire (sur le tas / heap) |
| `free` | Libération de la mémoire précédemment allouée par malloc |

Dans ce programme, `malloc` et `free` sont utilisés pour :
- allouer les matrices d'adjacence des graphes,
- allouer les matrices L et P de l'algorithme de Floyd-Warshall,
- allouer les structures de type `Graphe`,
- allouer les chaînes de caractères pour les noms de fichiers.

### 2.3. `<stdbool.h>`

Cette bibliothèque introduit :

| Élément | Description |
|---------|-------------|
| `bool` | Type booléen |
| `true` | Équivalent à 1 |
| `false` | Équivalent à 0 |

Dans le programme, ce type est utilisé pour les variables logiques, par exemple `erreurLecture` dans la fonction `choixGraphe`.

### 2.4. `<string.h>`

Bibliothèque de gestion des chaînes de caractères. Elle fournit :

| Fonction | Description |
|----------|-------------|
| `strlen` | Longueur d'une chaîne de caractères |
| `strcmp` | Comparaison de deux chaînes (retourne 0 si identiques) |
| `strcpy` | Copie d'une chaîne dans une autre |

Ces fonctions sont utilisées dans `estTxt` et `choixTxt`.

### 2.5. `<dirent.h>`

Bibliothèque permettant la manipulation des répertoires. Elle fournit :

| Élément | Description |
|---------|-------------|
| `DIR` | Type représentant un répertoire ouvert |
| `struct dirent` | Structure représentant une entrée de répertoire |
| `opendir` | Ouverture d'un répertoire |
| `readdir` | Lecture successive des entrées du répertoire |
| `closedir` | Fermeture du répertoire |

### 2.6. `<limits.h>`

Bibliothèque qui fournit des constantes sur les limites des types entiers (`INT_MAX`, `INT_MIN`, etc.).

Utilisée pour définir une valeur de "pseudo-infini" :

```c
#define INFINITY (INT_MAX / 4)
```

> **Note :** On prend `INT_MAX / 4` au lieu de `INT_MAX` pour pouvoir encore additionner deux distances sans risque d'overflow.

---

## 3. Constantes, types et structures

### 3.1. Macros de configuration

```c
#define MAX_FILES 100
#define MAX_LENGTH 256
#define MAX_GRAPHES 100
#define INFINITY (INT_MAX / 4)
```

| Macro | Description |
|-------|-------------|
| `MAX_FILES` | Nombre maximal de fichiers `.txt` à lister |
| `MAX_LENGTH` | Taille maximale d'un buffer pour lire des lignes |
| `MAX_GRAPHES` | Nombre maximal de graphes traités en mémoire |
| `INFINITY` | Valeur représentant "pas de chemin" ou "distance infinie" |

### 3.2. Type `Graphe`

```c
typedef struct {
    int nbSommets;
    int **adjMat;
} Graphe;
```

| Champ | Description |
|-------|-------------|
| `nbSommets` | Nombre de sommets du graphe (numérotés de 0 à n-1) |
| `adjMat` | Matrice d'adjacence de taille n × n |

La matrice d'adjacence `adjMat` est représentée comme un tableau de pointeurs (`int**`) :
- `adjMat[i][j]` contient le poids de l'arête allant de `i` vers `j`
- Si aucune arête directe n'existe, la case vaut 0 puis `INFINITY` dans les matrices de travail

---

## 4. Description détaillée des fonctions

### 4.1. `int estTxt(const char *nom)`

**Rôle :** Déterminer si une chaîne de caractères `nom` se termine par l'extension `.txt`.

**Algorithme :**
1. Calcul de la longueur par `strlen(nom)`
2. Si longueur < 4, retourne 0
3. Compare les 4 derniers caractères à `.txt` avec `strcmp`
4. Retourne 1 si correspondance, 0 sinon

---

### 4.2. `char* choixTxt(void)`

**Rôle :** 
- Lister tous les fichiers `.txt` dans le répertoire courant
- Permettre à l'utilisateur de choisir l'un d'eux
- Retourner le nom du fichier choisi (alloué dynamiquement)

**Algorithme :**
1. Ouverture du répertoire courant avec `opendir(".")`
2. Parcours des entrées avec `readdir(dir)`
3. Filtrage avec `estTxt()`
4. Affichage de la liste et choix utilisateur
5. Retour du fichier choisi (à libérer avec `free`)

---

### 4.3. `int** allouerMatrice(int n)` et `void libererMatrice(int **m, int n)`

**Rôle :** Allouer et libérer des matrices carrées n × n d'entiers.

**allouerMatrice :**
```c
int** allouerMatrice(int n) {
    int** m = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        m[i] = malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            m[i][j] = 0;
        }
    }
    return m;
}
```

**libererMatrice :**
```c
void libererMatrice(int **m, int n) {
    for (int i = 0; i < n; i++) {
        free(m[i]);
    }
    free(m);
}
```

---

### 4.4. `void afficherMatrice(int **m, int n)`

**Rôle :** Afficher proprement une matrice d'entiers n × n.

**Format d'affichage :**
- Ligne d'en-tête avec les indices de colonnes
- Pour chaque case : valeur entière ou `.` si `INFINITY`

---

### 4.5. `Graphe* choixGraphe(const char *nomFichier)`

**Rôle :** 
- Lire plusieurs graphes dans un fichier
- Les stocker et les afficher
- Laisser l'utilisateur choisir celui à analyser
- Retourner ce graphe

**Algorithme détaillé :**
1. Ouvrir le fichier avec `fopen(nomFichier, "r")`
2. Pour chaque graphe :
   - Lire `n` (nombre de sommets) et `m` (nombre d'arêtes)
   - Allouer le graphe et sa matrice d'adjacence
   - Lire les `m` arêtes (triplets `u v w`)
3. Afficher tous les graphes disponibles
4. Demander le choix utilisateur
5. Libérer les graphes non sélectionnés
6. Retourner le graphe choisi

---

### 4.6. `void afficherChemin(int u, int v, int **P)`

**Rôle :** Afficher le chemin minimal de `u` à `v` en utilisant la matrice `P`.

**Algorithme :**
1. Si `P[u][v] == -1` : pas de chemin
2. Sinon : suivre les successeurs jusqu'à `v`

---

### 4.7. `void floydWarshall(Graphe *g)`

**Rôle :** Appliquer l'algorithme de Floyd-Warshall au graphe, puis interagir avec l'utilisateur pour afficher des chemins minimaux.

**Algorithme :**

1. **Initialisation des matrices L et P :**
   - `L[i][j] = 0` si `i == j`
   - `L[i][j] = poids` si arête directe existe
   - `L[i][j] = INFINITY` sinon
   - `P[i][j] = j` si arête directe, `-1` sinon

2. **Boucle principale :**
   ```
   Pour chaque sommet intermédiaire k de 0 à n-1 :
       Pour chaque couple (i, j) :
           Si L[i][k] + L[k][j] < L[i][j] :
               L[i][j] = L[i][k] + L[k][j]
               P[i][j] = P[i][k]
   ```

3. **Détection de circuits absorbants :**
   - Si `L[i][i] < 0` pour un sommet `i`, il y a un circuit absorbant

4. **Interface utilisateur :**
   - Si pas de circuit absorbant, proposer d'afficher des chemins minimaux

---

### 4.8. `int main(void)`

**Rôle :** Coordonner l'exécution du programme.

**Boucle principale :**
1. Appeler `choixTxt()` pour obtenir un fichier
2. Appeler `choixGraphe(fichier)` pour choisir un graphe
3. Appeler `floydWarshall(g)` pour l'analyser
4. Libérer les ressources
5. Proposer de traiter un autre graphe

---

## 5. Format des fichiers `.txt` de graphes

Pour chaque graphe :

```
n
m
u1 v1 w1
u2 v2 w2
...
um vm wm
```

| Élément | Description |
|---------|-------------|
| `n` | Nombre de sommets (numérotés de 0 à n-1) |
| `m` | Nombre d'arêtes |
| `ui vi wi` | Arête orientée de `ui` vers `vi` de poids `wi` |

**Exemple :**

```
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
```

> Ce fichier contient deux graphes : le premier avec 3 sommets, le second avec 4 sommets.

---

## 6. Résumé

Le programme montre comment :
- ✅ Manipuler des fichiers et répertoires en C
- ✅ Structurer des données (`struct Graphe`, matrices)
- ✅ Implémenter un algorithme non trivial (Floyd-Warshall)
- ✅ Gérer des allocations dynamiques et les libérer correctement
- ✅ Interagir avec un utilisateur via la console

### Récapitulatif des fonctions

| Fonction | Rôle |
|----------|------|
| `estTxt` | Filtrer les noms de fichiers |
| `choixTxt` | Choisir un fichier de graphes |
| `allouerMatrice` / `libererMatrice` | Gérer les matrices |
| `afficherMatrice` | Représenter des matrices à l'écran |
| `choixGraphe` | Lire, stocker, afficher et choisir un graphe |
| `floydWarshall` | Cœur algorithmique (plus courts chemins + détection de cycles) |
| `afficherChemin` | Reconstruire un chemin minimal |
| `main` | Coordonner l'ensemble |
