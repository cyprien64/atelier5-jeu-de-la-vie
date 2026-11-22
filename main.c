/* Atelier 5 - Jeu de la vie (ESME BA1) */
#include <stdio.h>
#include <stdbool.h>

/* Dimensions du monde */
#define WIDTH 20
#define HEIGHT 12

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

    /* Commenter une ligne pour désactiver la structure correspondante */
    add_block(world, 1, 1);
    add_blinker(world, 7, 1);
    add_glider(world, 12, 1);

    display_world(world);
    return 0;
}
