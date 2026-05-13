# Project: Analysis of Weighted Directed Graphs Using the Floyd-Warshall Algorithm

## 1. General Objective of the Program

This C-language program is designed to analyze weighted directed graphs from text files.

It automates the following tasks:

1. Searching for all text files (`.txt`) present in the current directory.
2. Allowing the user to interactively choose a file.
3. Reading multiple graphs contained in the selected file (a single file may contain several graphs consecutively).
4. Storing each graph in memory using an adjacency matrix representation.
5. Displaying all available graphs and allowing the user to select a specific graph.
6. Applying the Floyd-Warshall algorithm to the chosen graph:
   - computing the shortest paths between every pair of vertices,
   - displaying intermediate matrices at each step of the algorithm (L and P matrices),
   - detecting possible negative cycles (absorbing circuits).
7. If no negative cycle exists:
   - interactive interface allowing the user to request the shortest path between two chosen vertices,
   - displaying the minimal path length,
   - reconstructing the actual path (sequence of visited vertices).
8. Allowing the processing of multiple graphs successively without exiting the program.

This program is intended for beginner C programmers and demonstrates:
- the use of standard C libraries,
- text file manipulation,
- dynamic memory allocation,
- handling two-dimensional arrays (matrices),
- implementation of a classic graph theory algorithm (Floyd-Warshall).

---

## 2. Libraries Used and Their Roles

The program uses several standard C libraries.

### 2.1. `<stdio.h>`

Standard input/output library. It provides:

| Function | Description |
|----------|-------------|
| `printf` | Formatted output to the standard output (terminal) |
| `scanf` | Formatted input from the standard input (keyboard) |
| `FILE` | Type representing an opened file |
| `fopen` | Opens a file |
| `fclose` | Closes an opened file |
| `fscanf` | Formatted reading from a file |

### 2.2. `<stdlib.h>`

Standard utility library. It provides:

| Function | Description |
|----------|-------------|
| `malloc` | Dynamic memory allocation (heap allocation) |
| `free` | Frees memory previously allocated with malloc |

In this program, `malloc` and `free` are used to:
- allocate graph adjacency matrices,
- allocate the L and P matrices of the Floyd-Warshall algorithm,
- allocate `Graph` structures,
- allocate character strings for file names.

### 2.3. `<stdbool.h>`

This library introduces:

| Element | Description |
|---------|-------------|
| `bool` | Boolean type |
| `true` | Equivalent to 1 |
| `false` | Equivalent to 0 |

In the program, this type is used for logical variables such as `readError` in the `chooseGraph` function.

### 2.4. `<string.h>`

String handling library. It provides:

| Function | Description |
|----------|-------------|
| `strlen` | Length of a string |
| `strcmp` | Compares two strings (returns 0 if identical) |
| `strcpy` | Copies a string into another |

These functions are used in `isTxt` and `chooseTxt`.

### 2.5. `<dirent.h>`

Directory manipulation library. It provides:

| Element | Description |
|---------|-------------|
| `DIR` | Type representing an opened directory |
| `struct dirent` | Structure representing a directory entry |
| `opendir` | Opens a directory |
| `readdir` | Reads directory entries successively |
| `closedir` | Closes a directory |

### 2.6. `<limits.h>`

Library providing constants related to integer limits (`INT_MAX`, `INT_MIN`, etc.).

Used to define a "pseudo-infinity" value:

```c
#define INFINITY (INT_MAX / 4)
```

> **Note:** `INT_MAX / 4` is used instead of `INT_MAX` to avoid overflow when adding distances.

---

## 3. Constants, Types, and Structures

### 3.1. Configuration Macros

```c
#define MAX_FILES 100
#define MAX_LENGTH 256
#define MAX_GRAPHS 100
#define INFINITY (INT_MAX / 4)
```

| Macro | Description |
|-------|-------------|
| `MAX_FILES` | Maximum number of `.txt` files to list |
| `MAX_LENGTH` | Maximum buffer size for reading lines |
| `MAX_GRAPHS` | Maximum number of graphs stored in memory |
| `INFINITY` | Value representing "no path" or "infinite distance" |

### 3.2. `Graph` Type

```c
typedef struct {
    int nbVertices;
    int **adjMat;
} Graph;
```

| Field | Description |
|-------|-------------|
| `nbVertices` | Number of graph vertices (numbered from 0 to n-1) |
| `adjMat` | n × n adjacency matrix |

The adjacency matrix `adjMat` is represented as an array of pointers (`int**`):
- `adjMat[i][j]` contains the weight of the edge from `i` to `j`
- If no direct edge exists, the value is initially `0`, then replaced by `INFINITY` in working matrices

---

## 4. Detailed Description of Functions

### 4.1. `int isTxt(const char *name)`

**Purpose:** Determine whether a string `name` ends with the `.txt` extension.

**Algorithm:**
1. Compute the length using `strlen(name)`
2. If length < 4, return 0
3. Compare the last 4 characters to `.txt` using `strcmp`
4. Return 1 if matching, otherwise 0

---

### 4.2. `char* chooseTxt(void)`

**Purpose:**
- List all `.txt` files in the current directory
- Allow the user to choose one
- Return the selected file name (dynamically allocated)

**Algorithm:**
1. Open the current directory with `opendir(".")`
2. Iterate through entries using `readdir(dir)`
3. Filter entries with `isTxt()`
4. Display the list and ask for user selection
5. Return the chosen file name (must later be freed with `free`)

---

### 4.3. `int** allocateMatrix(int n)` and `void freeMatrix(int **m, int n)`

**Purpose:** Allocate and free square integer matrices of size n × n.

**allocateMatrix:**
```c
int** allocateMatrix(int n) {
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

**freeMatrix:**
```c
void freeMatrix(int **m, int n) {
    for (int i = 0; i < n; i++) {
        free(m[i]);
    }
    free(m);
}
```

---

### 4.4. `void displayMatrix(int **m, int n)`

**Purpose:** Display an integer matrix neatly.

**Display format:**
- Header row with column indices
- Each cell displays either the integer value or `.` if equal to `INFINITY`

---

### 4.5. `Graph* chooseGraph(const char *fileName)`

**Purpose:**
- Read multiple graphs from a file
- Store and display them
- Let the user choose one graph
- Return the selected graph

**Detailed Algorithm:**
1. Open the file with `fopen(fileName, "r")`
2. For each graph:
   - Read `n` (number of vertices) and `m` (number of edges)
   - Allocate the graph and its adjacency matrix
   - Read the `m` edges (`u v w`)
3. Display all available graphs
4. Ask for user selection
5. Free all unselected graphs
6. Return the chosen graph

---

### 4.6. `void displayPath(int u, int v, int **P)`

**Purpose:** Display the shortest path from `u` to `v` using matrix `P`.

**Algorithm:**
1. If `P[u][v] == -1`, no path exists
2. Otherwise, follow successors until reaching `v`

---

### 4.7. `void floydWarshall(Graph *g)`

**Purpose:** Apply the Floyd-Warshall algorithm to the graph and interactively display shortest paths.

**Algorithm:**

1. **Initialize matrices L and P:**
   - `L[i][j] = 0` if `i == j`
   - `L[i][j] = weight` if a direct edge exists
   - `L[i][j] = INFINITY` otherwise
   - `P[i][j] = j` if a direct edge exists, otherwise `-1`

2. **Main loop:**
   ```text
   For each intermediate vertex k from 0 to n-1:
       For each pair (i, j):
           If L[i][k] + L[k][j] < L[i][j]:
               L[i][j] = L[i][k] + L[k][j]
               P[i][j] = P[i][k]
   ```

3. **Negative cycle detection:**
   - If `L[i][i] < 0` for some vertex `i`, a negative cycle exists

4. **User interface:**
   - If no negative cycle exists, allow shortest path queries

---

### 4.8. `int main(void)`

**Purpose:** Coordinate the execution of the program.

**Main loop:**
1. Call `chooseTxt()` to select a file
2. Call `chooseGraph(file)` to select a graph
3. Call `floydWarshall(g)` to analyze it
4. Free allocated resources
5. Ask whether to process another graph

---

## 5. Graph `.txt` File Format

For each graph:

```text
n
m
u1 v1 w1
u2 v2 w2
...
um vm wm
```

| Element | Description |
|---------|-------------|
| `n` | Number of vertices (numbered from 0 to n-1) |
| `m` | Number of edges |
| `ui vi wi` | Directed edge from `ui` to `vi` with weight `wi` |

**Example:**

```text
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

> This file contains two graphs: the first with 3 vertices and the second with 4 vertices.

---

## 6. Summary

This program demonstrates how to:
- ✅ Manipulate files and directories in C
- ✅ Structure data using `struct Graph` and matrices
- ✅ Implement a non-trivial algorithm (Floyd-Warshall)
- ✅ Manage dynamic memory allocation and deallocation correctly
- ✅ Interact with the user through the console

### Function Summary

| Function | Purpose |
|----------|---------|
| `isTxt` | Filter file names |
| `chooseTxt` | Select a graph file |
| `allocateMatrix` / `freeMatrix` | Manage matrices |
| `displayMatrix` | Display matrices |
| `chooseGraph` | Read, store, display, and choose a graph |
| `floydWarshall` | Core algorithm (shortest paths + cycle detection) |
| `displayPath` | Reconstruct shortest paths |
| `main` | Coordinate the entire program |
