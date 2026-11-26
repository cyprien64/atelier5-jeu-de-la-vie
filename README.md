# Conway Battle

Projet de groupe (ESME, Bachelor 1) en C : le jeu de la vie de Conway transformé en jeu de stratégie à deux joueurs.

Groupe : Cyprien, Vianney, Swann, Thiruvanen.

## Compiler et lancer

```bash
make
./conway_battle
```

## Règles du jeu

### Le terrain
- Grille de 26 x 20 cases, **torique** : les bords se rejoignent.
- Les colonnes sont repérées par une lettre, les lignes par un numéro (ex. `K` puis `5`).
- Colonnes A à M : zone du joueur 1 (`O`). Colonnes N à Z : zone du joueur 2 (`X`).
- Au départ, chaque joueur a la même flotte, placée en miroir : un bloc, un clignotant et un planeur.

### Un tour
1. Le joueur 1 fait **2 actions**, puis la grille évolue d'une génération.
2. Le joueur 2 fait **2 actions**, puis la grille évolue d'une génération.
3. On compte les points.

Actions possibles (une action refusée ne compte pas) :

| Action | Effet |
|---|---|
| 1 : ajouter | une cellule sur une case vide de **sa** zone |
| 2 : tuer | une cellule **adverse**, n'importe où |
| 3 : structure | un bloc, un clignotant ou un planeur, entièrement dans **sa** zone |
| 4 : passer | ne rien faire |

### L'évolution : les règles de Conway
- Une cellule avec 2 ou 3 voisines vivantes survit et garde son propriétaire.
- Une case vide avec exactement 3 voisines vivantes voit naître une cellule, qui prend la **couleur majoritaire** de ses 3 voisines.
- Dans tous les autres cas, la case devient vide.

Les voisines des deux joueurs comptent : ce sont exactement les règles de Conway, la couleur sert seulement à savoir à qui appartient chaque cellule.

### Gagner
- À la fin de chaque tour, chaque joueur marque **1 point par cellule à lui dans la zone adverse**.
- Le premier à **15 points** gagne.
- Un joueur qui n'a plus aucune cellule perd.
- Après **30 tours**, le joueur qui a le plus de cellules gagne.
- Dans les autres cas d'égalité, la partie est nulle.

## Choix de conception

- **Deux états par case, comme dans l'atelier 5** : l'unité contient le propriétaire actuel (0 = vide, 1 ou 2) et la dizaine le propriétaire futur.
  - `cell_cur_state` (`% 10`) et `cell_next_state` (`/ 10`) renvoient `true`/`false` : la cellule est (ou sera) vivante ou morte, comme demandé dans l'atelier 5.
  - `cell_cur_owner` et `cell_next_owner` renvoient à quel joueur elle appartient (0, 1 ou 2).
- **Génération en 2 phases** (`next_generation`) : on calcule d'abord l'état futur de toutes les cases à partir de l'état actuel, puis le futur devient l'actuel. Ainsi, le calcul d'une case n'est jamais faussé par une case déjà mise à jour.
- **Monde torique** : les voisines sont calculées avec un modulo sur `WIDTH` et `HEIGHT`. Les structures ne disparaissent pas au bord, et chaque zone touche la zone adverse au milieu et par le bord extérieur.
- **Couleur majoritaire** pour les naissances (variante « Immigration » du jeu de la vie) : les règles de naissance, de survie et de mort ne changent pas.
- **Seulement des tableaux statiques**, des constantes `#define` et des fonctions : pas de `struct`, pas de pointeurs, pas de bibliothèque externe.
- **Séparation des rôles** dans `main.c` :
  - logique du jeu : aucune de ces fonctions n'affiche ni ne lit au clavier ;
  - affichage : `display_world`, `display_scores`, `display_result` ;
  - saisie : `read_int`, qui redemande tant que la valeur est invalide ;
  - déroulement : `play_turn` et `main`.
- **Boucles `while`** dans les fonctions de jeu, pour correspondre exactement aux logigrammes flow-c (l'outil n'a pas de bloc « for »).

## Logigrammes

Un logigramme par fonction de jeu, plus celui de l'affichage (`display_world`). Les fichiers `docs/logigrammes/*.json` s'ouvrent dans [flow-c](https://labasse.github.io/flowc/) avec le bouton **Open**. Le bouton **Save** donne ensuite le PNG.
