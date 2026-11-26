/* Conway Battle (ESME BA1) : le jeu de la vie à deux joueurs */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* ===================== Paramètres du jeu ===================== */

/* Dimensions du monde : colonnes 0 à 14 = zone du joueur 1,
   colonnes 15 à 29 = zone du joueur 2 */
#define WIDTH 30
#define HEIGHT 20
#define HALF (WIDTH / 2)   /* 15 : première colonne du joueur 2 */

#define ACTIONS_PER_TURN 2
#define WIN_POINTS 15
#define MAX_ROUNDS 30

/* Contenu d'une case (unité) et résultats possibles d'une partie */
#define EMPTY 0
#define PLAYER1 1
#define PLAYER2 2
#define NO_WINNER 0
#define DRAW 3
#define ALL_ZONES 0

/* Actions possibles pendant un tour */
#define ACTION_ADD 1
#define ACTION_KILL 2
#define ACTION_STRUCTURE 3
#define ACTION_PASS 4

/* Structures prédéfinies 3 x 3, dessinées pour le joueur 1.
   Pour le joueur 2 elles sont retournées gauche-droite :
   son planeur part donc lui aussi vers l'adversaire. */
#define NB_STRUCTURES 3
#define STRUCT_SIZE 3
#define BLOCK 0
#define BLINKER 1
#define GLIDER 2

/* Chaque structure = 3 chaînes (ses 3 lignes), '#' = cellule vivante.
   STRUCT_SIZE + 1 : 3 caractères + le '\0' de fin de chaîne. */
const char STRUCTURES[NB_STRUCTURES][STRUCT_SIZE][STRUCT_SIZE + 1] = {
    { "##.", "##.", "..." },   /* bloc (stable) */
    { ".#.", ".#.", ".#." },   /* clignotant (oscillateur) */
    { ".#.", "..#", "###" }    /* planeur (vaisseau) */
};

/* ===================== Logique du jeu ===================== */
/* Aucune fonction de cette partie n'affiche ni ne lit au clavier.
   Toutes les boucles sont des while pour suivre les logigrammes flow-c. */

/* Chaque case stocke 2 états, comme dans l'atelier 5 :
   unité = propriétaire actuel, dizaine = propriétaire futur
   (0 = vide, 1 = joueur 1, 2 = joueur 2).
   Ex. 21 : cellule du joueur 1 qui sera au joueur 2 à la génération suivante. */

/* true si la cellule (x, y) est vivante actuellement (unité non nulle) */
bool cell_cur_state(int world[HEIGHT][WIDTH], int x, int y)
{
    return world[y][x] % 10 != 0;   /* % 10 garde l'unité : 21 % 10 = 1 */
}

/* true si la cellule (x, y) sera vivante à la génération suivante (dizaine non nulle) */
bool cell_next_state(int world[HEIGHT][WIDTH], int x, int y)
{
    return world[y][x] / 10 != 0;   /* division entière, garde la dizaine : 21 / 10 = 2 */
}

/* Propriétaire actuel de la case (x, y) : l'unité */
int cell_cur_owner(int world[HEIGHT][WIDTH], int x, int y)
{
    return world[y][x] % 10;
}

/* Propriétaire futur de la case (x, y) : la dizaine */
int cell_next_owner(int world[HEIGHT][WIDTH], int x, int y)
{
    return world[y][x] / 10;
}

/* Joueur à qui appartient la colonne x */
int zone_owner(int x)
{
    int owner;

    if (x < HALF) {
        owner = PLAYER1;
    } else {
        owner = PLAYER2;
    }
    return owner;
}

/* Calcule le propriétaire futur de la case (x, y) avec les règles de Conway,
   à partir de l'état ACTUEL des 8 voisines, et l'écrit dans la dizaine.
   Monde torique : les bords se rejoignent grâce au modulo. */
void cell_compute_next_state(int world[HEIGHT][WIDTH], int x, int y)
{
    int dx, dy, nx, ny, owner;
    int n1 = 0;
    int n2 = 0;
    bool alive = cell_cur_state(world, x, y);
    int next = EMPTY;

    /* dx et dy vont de -1 à 1 : on parcourt le carré 3 x 3 autour de (x, y) */
    dy = -1;
    while (dy <= 1) {
        dx = -1;
        while (dx <= 1) {
            if (dx != 0 || dy != 0) {   /* on ne compte pas la case elle-même */
                /* Monde torique : + WIDTH évite un modulo négatif.
                   Ex. x = 0, dx = -1 : (0 - 1 + 30) % 30 = 29, la colonne de l'autre bord. */
                nx = (x + dx + WIDTH) % WIDTH;
                ny = (y + dy + HEIGHT) % HEIGHT;
                /* On lit l'unité (état actuel), jamais la dizaine déjà calculée */
                owner = cell_cur_owner(world, nx, ny);
                /* n1 et n2 : voisines de chaque joueur (utile pour la couleur majoritaire) */
                if (owner == PLAYER1) {
                    n1 = n1 + 1;
                } else if (owner == PLAYER2) {
                    n2 = n2 + 1;
                }
            }
            dx = dx + 1;
        }
        dy = dy + 1;
    }

    /* Survie avec 2 ou 3 voisines : la cellule garde son propriétaire.
       Naissance avec exactement 3 voisines : elle prend la couleur majoritaire.
       Sinon : vide. */
    /* Les règles comptent n1 + n2 : toutes les voisines, quelle que soit leur couleur */
    if (alive && (n1 + n2 == 2 || n1 + n2 == 3)) {
        next = cell_cur_owner(world, x, y);
    } else if (!alive && n1 + n2 == 3) {
        /* 3 voisines : jamais d'égalité possible (3-0, 2-1, 1-2 ou 0-3) */
        if (n1 > n2) {
            next = PLAYER1;
        } else {
            next = PLAYER2;
        }
    }
    /* On écrit le futur dans la dizaine sans toucher l'unité.
       Ex. cellule du joueur 1 (1) qui deviendra au joueur 2 : 1 + 2 * 10 = 21 */
    world[y][x] = world[y][x] + next * 10;
}

/* Passage à la génération suivante en 2 phases.
   Pourquoi 2 phases ? Si on mettait à jour les cases une par une,
   une case déjà modifiée fausserait le calcul de ses voisines. */
void next_generation(int world[HEIGHT][WIDTH])
{
    int x, y;

    /* Phase 1 : calcul de l'état futur de toutes les cases */
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
            if (cell_next_state(world, x, y)) {
                world[y][x] = cell_next_owner(world, x, y);   /* ex. 21 devient 2 */
            } else {
                world[y][x] = EMPTY;
            }
            x = x + 1;
        }
        y = y + 1;
    }
}

/* Action « ajouter » : une cellule sur une case vide de sa propre zone */
bool add_cell(int world[HEIGHT][WIDTH], int player, int x, int y)
{
    bool ok = false;   /* renvoie false si l'action est refusée : le joueur recommence */

    if (zone_owner(x) == player && world[y][x] == EMPTY) {
        world[y][x] = player;
        ok = true;
    }
    return ok;
}

/* Action « tuer » : supprime une cellule adverse, n'importe où */
bool kill_cell(int world[HEIGHT][WIDTH], int player, int x, int y)
{
    bool ok = false;

    /* Pas une case vide et pas une de ses propres cellules */
    if (world[y][x] != EMPTY && world[y][x] != player) {
        world[y][x] = EMPTY;
        ok = true;
    }
    return ok;
}

/* true si la case (i, j) de la structure est vivante, vue par le joueur */
bool structure_cell(int type, int player, int i, int j)
{
    bool alive;

    if (player == PLAYER1) {
        alive = STRUCTURES[type][i][j] == '#';
    } else {
        /* Miroir : on lit la ligne de droite à gauche (j = 0 lit la colonne 2) */
        alive = STRUCTURES[type][i][STRUCT_SIZE - 1 - j] == '#';
    }
    return alive;
}

/* Action « structure » : pose une structure, coin haut-gauche en (x, y).
   Elle doit tenir entièrement dans la zone du joueur, sans recouvrir
   de cellule adverse. */
bool place_structure(int world[HEIGHT][WIDTH], int player, int type, int x, int y)
{
    int i, j;
    bool ok = true;

    /* Refus si la structure dépasse de la grille, ou si sa première
       ou sa dernière colonne n'est pas dans la zone du joueur */
    if (x + STRUCT_SIZE > WIDTH || y + STRUCT_SIZE > HEIGHT
        || zone_owner(x) != player || zone_owner(x + STRUCT_SIZE - 1) != player) {
        ok = false;
    }

    /* Vérification : aucune cellule adverse sous la structure.
       « && ok » dans les while : on s'arrête dès qu'un problème est trouvé.
       On vérifie tout AVANT de poser, pour ne jamais poser une structure à moitié. */
    i = 0;
    while (i < STRUCT_SIZE && ok) {
        j = 0;
        while (j < STRUCT_SIZE && ok) {
            if (structure_cell(type, player, i, j)
                && world[y + i][x + j] != EMPTY && world[y + i][x + j] != player) {
                ok = false;
            }
            j = j + 1;
        }
        i = i + 1;
    }

    /* Pose */
    if (ok) {
        i = 0;
        while (i < STRUCT_SIZE) {
            j = 0;
            while (j < STRUCT_SIZE) {
                if (structure_cell(type, player, i, j)) {
                    world[y + i][x + j] = player;
                }
                j = j + 1;
            }
            i = i + 1;
        }
    }
    return ok;
}

/* Nombre de cellules du joueur dans une zone (PLAYER1, PLAYER2 ou ALL_ZONES) */
int count_cells(int world[HEIGHT][WIDTH], int player, int zone)
{
    int x, y;
    int count = 0;

    y = 0;
    while (y < HEIGHT) {
        x = 0;
        while (x < WIDTH) {
            /* La cellule est au joueur ET (on compte partout OU elle est dans la zone voulue) */
            if (world[y][x] == player && (zone == ALL_ZONES || zone_owner(x) == zone)) {
                count = count + 1;
            }
            x = x + 1;
        }
        y = y + 1;
    }
    return count;
}

/* Joueur qui mène : PLAYER1 si value1 > value2, PLAYER2 si value2 > value1, sinon DRAW */
int leader(int value1, int value2)
{
    int result = DRAW;

    if (value1 > value2) {
        result = PLAYER1;
    }
    if (value2 > value1) {
        result = PLAYER2;
    }
    return result;
}

/* Résultat après un tour complet : NO_WINNER, PLAYER1, PLAYER2 ou DRAW.
   Les tests vont du moins prioritaire au plus prioritaire :
   un test plus bas remplace le résultat d'un test plus haut. */
int check_winner(int score1, int score2, int cells1, int cells2, int round)
{
    int result = NO_WINNER;   /* NO_WINNER : la partie continue */

    if (round >= MAX_ROUNDS) {
        result = leader(cells1, cells2);
    }
    if (score1 >= WIN_POINTS || score2 >= WIN_POINTS) {
        result = leader(score1, score2);
    }
    if (cells2 == 0) {
        result = PLAYER1;
    }
    if (cells1 == 0) {
        result = PLAYER2;
    }
    if (cells1 == 0 && cells2 == 0) {
        result = DRAW;
    }
    return result;
}

/* Flotte de départ, symétrique : un bloc, un clignotant et un planeur par joueur */
void init_world(int world[HEIGHT][WIDTH])
{
    place_structure(world, PLAYER1, BLOCK, 3, 3);
    place_structure(world, PLAYER1, BLINKER, 8, 8);
    place_structure(world, PLAYER1, GLIDER, 3, 13);
    /* Joueur 2 : mêmes positions vues dans un miroir (WIDTH - x - 3) */
    place_structure(world, PLAYER2, BLOCK, WIDTH - 6, 3);
    place_structure(world, PLAYER2, BLINKER, WIDTH - 11, 8);
    place_structure(world, PLAYER2, GLIDER, WIDTH - 6, 13);
}

/* ===================== Affichage ===================== */

/* Affichage du monde : 'O' = joueur 1, 'X' = joueur 2, '.' = vide,
   '|' = frontière entre les deux zones */
void display_world(int world[HEIGHT][WIDTH])
{
    int x, y;

    /* Numéros de colonnes sur 2 lignes : dizaines puis unités */
    printf("   ");
    x = 0;
    while (x < WIDTH) {
        if (x == HALF) {
            printf(" ");
        }
        printf("%d", x / 10);
        x = x + 1;
    }
    printf("\n   ");
    x = 0;
    while (x < WIDTH) {
        if (x == HALF) {
            printf(" ");
        }
        printf("%d", x % 10);
        x = x + 1;
    }
    printf("\n");

    y = 0;
    while (y < HEIGHT) {
        printf("%2d ", y);
        x = 0;
        while (x < WIDTH) {
            if (x == HALF) {
                printf("|");
            }
            if (world[y][x] == PLAYER1) {
                printf("O");
            } else if (world[y][x] == PLAYER2) {
                printf("X");
            } else {
                printf(".");
            }
            x = x + 1;
        }
        printf("\n");
        y = y + 1;
    }
}

/* Tour, scores et nombre de cellules de chaque joueur */
void display_scores(int round, int score1, int score2, int cells1, int cells2)
{
    printf("Tour %d/%d - Joueur 1 (O) : %d pts, %d cellules"
           " | Joueur 2 (X) : %d pts, %d cellules (victoire à %d pts)\n",
           round, MAX_ROUNDS, score1, cells1, score2, cells2, WIN_POINTS);
}

/* Annonce du résultat final */
void display_result(int result, int score1, int score2)
{
    if (result == DRAW) {
        printf("Égalité ! (%d - %d)\n", score1, score2);
    } else {
        printf("Le joueur %d gagne ! (%d - %d)\n", result, score1, score2);
    }
}

/* ===================== Saisie ===================== */

/* Lit un entier entre min et max, redemande tant que la saisie est invalide */
int read_int(const char prompt[], int min, int max)
{
    int value = min - 1;   /* valeur invalide pour entrer dans la boucle */
    int c;

    while (value < min || value > max) {
        printf("%s (%d-%d) : ", prompt, min, max);
        /* scanf renvoie 1 s'il a lu un nombre ; sinon (ex. une lettre) la saisie est invalide */
        if (scanf("%d", &value) != 1) {
            if (feof(stdin)) {   /* fin de l'entrée (Ctrl+D) : on quitte proprement */
                printf("\nPartie interrompue.\n");
                exit(0);
            }
            value = min - 1;
        }
        /* On vide le reste de la ligne, sinon une lettre resterait bloquée
           et scanf échouerait en boucle */
        c = getchar();
        while (c != '\n' && c != EOF) {
            c = getchar();
        }
    }
    return value;
}

/* ===================== Déroulement d'un tour ===================== */

/* Le joueur effectue ses actions ; une action refusée ne compte pas */
void play_turn(int world[HEIGHT][WIDTH], int player, int round, int score1, int score2)
{
    int action = 1;
    int choice, type, x, y;
    bool ok;

    printf("\033[2J\033[H"); /* efface l'écran (code ANSI) */
    while (action <= ACTIONS_PER_TURN) {
        printf("\n");
        display_scores(round, score1, score2,
                       count_cells(world, PLAYER1, ALL_ZONES), count_cells(world, PLAYER2, ALL_ZONES));
        display_world(world);
        printf("Joueur %d, action %d/%d\n", player, action, ACTIONS_PER_TURN);
        printf("1 : ajouter une cellule  2 : tuer une cellule adverse"
               "  3 : poser une structure  4 : passer\n");
        choice = read_int("Choix", ACTION_ADD, ACTION_PASS);

        ok = true;   /* passer est toujours accepté */
        if (choice != ACTION_PASS) {
            type = BLOCK;   /* valeur par défaut, utilisée seulement pour l'action 3 */
            if (choice == ACTION_STRUCTURE) {
                type = read_int("Structure 0 = bloc, 1 = clignotant, 2 = planeur", 0, NB_STRUCTURES - 1);
            }
            x = read_int("Colonne", 0, WIDTH - 1);
            y = read_int("Ligne", 0, HEIGHT - 1);
            if (choice == ACTION_ADD) {
                ok = add_cell(world, player, x, y);
            } else if (choice == ACTION_KILL) {
                ok = kill_cell(world, player, x, y);
            } else {
                ok = place_structure(world, player, type, x, y);
            }
        }

        if (ok) {
            action = action + 1;   /* on ne passe à l'action suivante que si elle a réussi */
        } else {
            printf("Action impossible ici, recommencez.\n");
        }
    }
}

int main(void)
{
    /* Toutes les cases sont vides au départ */
    int world[HEIGHT][WIDTH] = {0};
    int score1 = 0;
    int score2 = 0;
    int cells1, cells2;
    int round = 0;
    int result = NO_WINNER;

    init_world(world);

    /* Boucle de partie : un passage = un tour complet des deux joueurs */
    while (result == NO_WINNER) {
        round = round + 1;
        play_turn(world, PLAYER1, round, score1, score2);   /* 2 actions du joueur 1 */
        next_generation(world);                             /* Conway fait évoluer la grille */
        play_turn(world, PLAYER2, round, score1, score2);   /* 2 actions du joueur 2 */
        next_generation(world);

        /* 1 point par cellule à soi dans la zone adverse */
        score1 = score1 + count_cells(world, PLAYER1, PLAYER2);
        score2 = score2 + count_cells(world, PLAYER2, PLAYER1);
        cells1 = count_cells(world, PLAYER1, ALL_ZONES);
        cells2 = count_cells(world, PLAYER2, ALL_ZONES);
        result = check_winner(score1, score2, cells1, cells2, round);   /* la boucle s'arrête si quelqu'un gagne */
    }

    printf("\n");
    display_world(world);
    display_result(result, score1, score2);
    return 0;
}
