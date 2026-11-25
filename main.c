/* Touché-coulé avec des avions (ESME BA1) : 1 joueur contre l'ordinateur */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

/* Dimensions du ciel et nombre d'avions */
#define SIZE 10
#define NB_PLANES 3

/* Dimensions de la forme d'un avion */
#define PLANE_W 5
#define PLANE_H 4

/* État d'un tir sur une case */
#define NOT_SHOT 0
#define MISS 1
#define HIT 2

/* Forme d'un avion, nez vers le haut : 'T' = tête, '#' = reste de l'avion */
const char PLANE[PLANE_H][PLANE_W + 1] = {
    "..T..",
    "#####",
    "..#..",
    ".###."
};

/* true si un avion posé avec son coin haut-gauche en (x, y)
   ne chevauche aucun autre avion */
bool can_place(int sky[SIZE][SIZE], int x, int y)
{
    int i, j;

    for (i = 0; i < PLANE_H; i++) {
        for (j = 0; j < PLANE_W; j++) {
            if (PLANE[i][j] != '.' && sky[y + i][x + j] != 0) {
                return false;
            }
        }
    }
    return true;
}

/* Écrit le numéro de l'avion (1, 2, 3...) dans chacune de ses cases */
void place_plane(int sky[SIZE][SIZE], int x, int y, int number)
{
    int i, j;

    for (i = 0; i < PLANE_H; i++) {
        for (j = 0; j < PLANE_W; j++) {
            if (PLANE[i][j] != '.') {
                sky[y + i][x + j] = number;
            }
        }
    }
}

/* Place les avions au hasard et retient la position de chaque tête */
void place_all_planes(int sky[SIZE][SIZE], int head_x[NB_PLANES], int head_y[NB_PLANES])
{
    int p, x, y;

    for (p = 0; p < NB_PLANES; p++) {
        /* On tire une position jusqu'à trouver une place libre */
        do {
            x = rand() % (SIZE - PLANE_W + 1);
            y = rand() % (SIZE - PLANE_H + 1);
        } while (!can_place(sky, x, y));

        place_plane(sky, x, y, p + 1);
        head_x[p] = x + 2; /* la tête est au milieu de la première ligne */
        head_y[p] = y;
    }
}

/* Affichage du ciel vu par le joueur :
   '.' = pas tiré, 'o' = à l'eau, 'X' = touché, '#' = avion abattu */
void display_sky(int sky[SIZE][SIZE], int shots[SIZE][SIZE], bool downed[NB_PLANES])
{
    int x, y;

    printf("\n   ");
    for (x = 0; x < SIZE; x++) {
        printf(" %c", 'A' + x);
    }
    printf("\n");

    for (y = 0; y < SIZE; y++) {
        printf("%2d ", y + 1);
        for (x = 0; x < SIZE; x++) {
            if (shots[y][x] == HIT) {
                printf(" X");
            } else if (sky[y][x] != 0 && downed[sky[y][x] - 1]) {
                printf(" #");
            } else if (shots[y][x] == MISS) {
                printf(" o");
            } else {
                printf(" .");
            }
        }
        printf("\n");
    }
}

int main(void)
{
    /* Toutes les cases sont vides et non tirées au départ */
    int sky[SIZE][SIZE] = {0};
    int shots[SIZE][SIZE] = {0};
    int head_x[NB_PLANES], head_y[NB_PLANES];
    bool downed[NB_PLANES] = {false};
    int nb_downed = 0;
    int nb_shots = 0;
    char line[32];
    char letter;
    int row, x, y, p;

    srand(time(NULL));
    place_all_planes(sky, head_x, head_y);

    printf("Touché-coulé avec des avions : %d avions sont cachés.\n", NB_PLANES);
    printf("Touchez la tête (cabine) d'un avion pour l'abattre.\n");

    while (nb_downed < NB_PLANES) {
        display_sky(sky, shots, downed);
        printf("Tir (ex. B5) : ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\nPartie interrompue.\n");
            return 0;
        }

        /* Lecture d'une lettre puis d'un numéro de ligne */
        if (sscanf(line, " %c%d", &letter, &row) != 2) {
            printf("Saisie invalide : tapez une lettre puis un nombre (ex. B5).\n");
            continue;
        }
        if (letter >= 'a' && letter <= 'z') {
            letter = letter - 'a' + 'A';
        }
        x = letter - 'A';
        y = row - 1;
        if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
            printf("Case hors du ciel : de A1 à %c%d.\n", 'A' + SIZE - 1, SIZE);
            continue;
        }
        if (shots[y][x] != NOT_SHOT) {
            printf("Case déjà visée.\n");
            continue;
        }

        nb_shots = nb_shots + 1;
        if (sky[y][x] == 0) {
            shots[y][x] = MISS;
            printf("À l'eau.\n");
        } else {
            shots[y][x] = HIT;
            p = sky[y][x] - 1;
            if (downed[p]) {
                printf("Touché (avion déjà abattu).\n");
            } else if (x == head_x[p] && y == head_y[p]) {
                downed[p] = true;
                nb_downed = nb_downed + 1;
                printf("Abattu ! (%d/%d)\n", nb_downed, NB_PLANES);
            } else {
                printf("Touché !\n");
            }
        }
    }

    display_sky(sky, shots, downed);
    printf("Bravo, les %d avions sont abattus en %d tirs.\n", NB_PLANES, nb_shots);
    return 0;
}
