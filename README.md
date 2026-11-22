# Atelier 5 - Jeu de la vie

Amorce du projet de groupe (ESME, Bachelor 1) : version classique du jeu de la vie de Conway en C.

Groupe : Cyprien, Vianney, Swann, Thiruvanen.

## Compiler et lancer

```bash
make
./life
```

## Contenu

- `main.c` : le monde (`world[HEIGHT][WIDTH]`), les 3 structures (bloc stable, clignotant, planeur), l'affichage, les états des cellules (`cell_cur_state`, `cell_next_state`) et la génération en 2 phases (`cell_compute_next_state`, `next_generation`). Monde torique.
- Appuyer sur Entrée pour passer à la génération suivante (20 générations).
- `docs/logigrammes/*.json` : logigrammes à ouvrir dans [flow-c](https://labasse.github.io/flowc/) (bouton **Open**), puis **Save** pour obtenir le PNG.
