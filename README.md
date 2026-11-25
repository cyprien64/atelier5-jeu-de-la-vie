# Touché-coulé avec des avions

Projet de groupe (ESME, Bachelor 1) en C : un touché-coulé où l'on abat des avions au lieu de couler des bateaux.

Groupe : Cyprien, Vianney, Swann, Thiruvanen.

## Compiler et lancer

```bash
make
./avions
```

## Règles

- L'ordinateur cache 3 avions dans un ciel de 10 x 10 cases (colonnes A à J, lignes 1 à 10).
- Chaque avion a cette forme (nez vers le haut), les avions ne se chevauchent pas :

```
..T..   T = tête (cabine)
#####
..#..
.###.
```

- Le joueur tire en tapant une case, par exemple `B5` :
  - « À l'eau » : aucune case d'avion ;
  - « Touché » : une case d'avion, mais pas la tête ;
  - « Abattu » : la tête de l'avion est touchée, l'avion entier est révélé.
- La partie est gagnée quand les 3 avions sont abattus.

Affichage : `.` pas tiré, `o` à l'eau, `X` touché, `#` avion abattu.
