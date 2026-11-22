/* Atelier 5 - Jeu de la vie (ESME BA1) */
#include <stdio.h>
#include <stdbool.h>

/* Dimensions du monde */
#define WIDTH 20
#define HEIGHT 12

/* Nombre de générations affichées après la génération 0 */
#define GENERATIONS 20

/* Chaque cellule stocke 2 états :
   unité = état actuel (0/1), dizaine = état futur (0/1)
   0 : morte qui le reste, 1 : vivante qui va mourir,
   10 : va naître, 11 : vivante qui le reste */

/* true si la cellule (x, y) est vivante actuellement (unité) */
bool cell_cur_state(int world[HEIGHT][WIDTH], int x, int y)
{
    return world[y][x] % 10 == 1;
}

/* true si la cellule (x, y) sera vivante à la génération suivante (dizaine) */
bool cell_next_state(int world[HEIGHT][WIDTH], int x, int y)
{
    return world[y][x] / 10 == 1;
}

/* Calcule l'état futur de la cellule (x, y) à partir de l'état ACTUEL
   de ses 8 voisines, et l'écrit dans la dizaine sans toucher l'unité.
   Monde torique : les bords se rejoignent grâce au modulo, ainsi le
   planeur qui sort d'un côté réapparaît de l'autre au lieu de disparaître. */
void cell_compute_next_state(int world[HEIGHT][WIDTH], int x, int y)
{
    int dx, dy, nx, ny;
    int neighbors = 0;

    for (dy = -1; dy <= 1; dy++) {
        for (dx = -1; dx <= 1; dx++) {
            if (dx != 0 || dy != 0) {
                nx = (x + dx + WIDTH) % WIDTH;
                ny = (y + dy + HEIGHT) % HEIGHT;
                if (cell_cur_state(world, nx, ny)) {
                    neighbors = neighbors + 1;
                }
            }
        }
    }

    /* Règles de Conway : une vivante survit avec 2 ou 3 voisines,
       une morte naît avec exactement 3 voisines, sinon morte */
    if (neighbors == 3 || (cell_cur_state(world, x, y) && neighbors == 2)) {
        world[y][x] = world[y][x] + 10;
    }
}

/* Passage à la génération suivante en 2 phases
   (boucles while pour suivre exactement le logigramme flow-c) */
void next_generation(int world[HEIGHT][WIDTH])
{
    int x, y;

    /* Phase 1 : calcul de l'état futur de toutes les cellules */
    y = 0;
    while (y < HEIGHT) {
        x = 0;
        while (x < WIDTH) {
            cell_compute_next_state(world, x, y);
            x = x + 1;
        }
        y = y + 1;
    }

    /* Phase 2 : le futur devient l'actuel (la dizaine passe à l'unité) */
    y = 0;
    while (y < HEIGHT) {
        x = 0;
        while (x < WIDTH) {
            world[y][x] = cell_next_state(world, x, y);
            x = x + 1;
        }
        y = y + 1;
    }
}

/* Placement d'un bloc 2x2 (structure stable), coin haut-gauche en (x, y) */
void add_block(int world[HEIGHT][WIDTH], int x, int y)
{
    world[y][x] = 1;
    world[y][x + 1] = 1;
    world[y + 1][x] = 1;
    world[y + 1][x + 1] = 1;
}

/* Placement d'un clignotant vertical de 3 cellules (oscillateur de période 2) */
void add_blinker(int world[HEIGHT][WIDTH], int x, int y)
{
    world[y][x] = 1;
    world[y + 1][x] = 1;
    world[y + 2][x] = 1;
}

/* Placement d'un planeur (vaisseau) qui se déplace vers le bas à droite */
void add_glider(int world[HEIGHT][WIDTH], int x, int y)
{
    world[y][x + 1] = 1;
    world[y + 1][x + 2] = 1;
    world[y + 2][x] = 1;
    world[y + 2][x + 1] = 1;
    world[y + 2][x + 2] = 1;
}

/* Affichage du monde : '#' = cellule vivante, '.' = cellule morte
   (boucles while pour suivre exactement le logigramme flow-c) */
void display_world(int world[HEIGHT][WIDTH])
{
    int x, y;

    y = 0;
    while (y < HEIGHT) {
        x = 0;
        while (x < WIDTH) {
            if (world[y][x] == 1) {
                printf("#");
            } else {
                printf(".");
            }
            x = x + 1;
        }
        printf("\n");
        y = y + 1;
    }
}

int main(void)
{
    /* Toutes les cellules sont mortes au départ */
    int world[HEIGHT][WIDTH] = {0};
    int generation;

    /* Commenter une ligne pour désactiver la structure correspondante */
    add_block(world, 1, 1);
    add_blinker(world, 7, 1);
    add_glider(world, 12, 1);

    for (generation = 0; generation <= GENERATIONS; generation++) {
        printf("\033[2J\033[H"); /* efface l'écran (code ANSI) */
        printf("Génération %d\n", generation);
        display_world(world);
        if (generation < GENERATIONS) {
            printf("Entrée : génération suivante\n");
            getchar();
            next_generation(world);
        }
    }
    return 0;
}
