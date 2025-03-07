# Documentation

## Recherche et conception

Ce projet vise à rendre la résolution d'un Rubik's Cube plus accessible grâce à un dispositif de scan basé sur un capteur de couleur et un affichage TFT. L'utilisateur place chaque face du cube sous le capteur et valide la couleur scannée avec un bouton-poussoir. Les données sont ensuite envoyées à une API de résolution qui fournit une séquence de mouvements permettant de résoudre le cube.

![Croquis de recherche](/docs/assets/croquis-de-recherche.png)

## Circuit électronique

Le circuit utilise un **capteur de couleur TCS3200**, un **écran TFT ST7735**, des **boutons-poussoirs** pour la validation et un **microcontrôleur Arduino Uno**.

### Schéma

![Schéma du circuit](/docs/assets/CubeResolver_bb.png)

### BOM (Bill of Materials)

| Réf | Composant        | Quantité | Description                                  | Fournisseur / Lien                                            |
| --- | ---------------- | -------- | -------------------------------------------- | ------------------------------------------------------------- |
| 1   | Arduino Uno R3   | 1        | Microcontrôleur ATmega328P                   | [Arduino](https://store.arduino.cc/products/arduino-uno-rev3) |
| 2   | Breadboard       | 1        | Plaque de prototypage                        | [Lien](#)                                                     |
| 3   | TCS3200          | 1        | Capteur de couleur                           | [Lien](#)                                                     |
| 4   | Bouton-poussoir  | 2        | Validation des couleurs                      | [Lien](#)                                                     |
| 5   | Écran TFT ST7735 | 1        | Affichage des instructions et de la solution | [Lien](#)                                                     |

## Programme

Le programme gère la **capture des couleurs**, l'**envoi des données** à l'API de résolution et l'**affichage de la solution**. Il est structuré en plusieurs étapes :

1. **Calibration des couleurs** : L'utilisateur place chaque couleur sous le capteur et valide avec un bouton.
2. **Scan des faces** : Les couleurs sont enregistrées face par face dans un ordre spécifique.
3. **Envoi à l'API Pocket Cube Solver** ([https://pocket-cube-solver.vercel.app/](https://pocket-cube-solver.vercel.app/)).
4. **Affichage de la solution** : La séquence de mouvements est affichée sur l'écran TFT, et l'utilisateur peut naviguer entre les étapes avec les boutons.

### Bibliothèques utilisées

- `Adafruit_GFX.h`
- `Adafruit_ST7735.h`
- `SPI.h`

### Ordre de scan des couleurs

![Schéma du cube](/docs/assets/cube-positions.png)

### Ordre de scan des couleurs

L'ordre des faces et des positions a été choisi pour correspondre aux notations standard du Rubik's Cube. Chaque face doit être scannée en s'alignant sur le schéma conventionnel du cube, en respectant l'ordre suivant :

Voici l'ordre dans lequel nous souhaitons scanner les cases du cube :
1, 2, 3, 4, 6, 8, 10, 12, 14, 16, 18, 20, 5, 7, 9, 11, 13, 15, 17, 19, 23, 21, 24, 22.

Cet ordre permet de garantir une correspondance correcte avec le solveur utilisé et assure une reconnaissance fiable des couleurs.

L'ordre des faces et des positions a été choisi pour correspondre aux notations standard du Rubik's Cube. Chaque face doit être scannée en s'alignant sur le schéma conventionnel du cube, en respectant l'ordre suivant :

L'utilisateur doit scanner les faces du cube dans l'ordre suivant, conformément au schéma d'un cube standard :

1. **U (Up - Haut)**
2. **L (Left - Gauche)**
3. **F (Front - Devant)**
4. **R (Right - Droite)**
5. **B (Back - Arrière)**
6. **D (Down - Bas)**

Pour chaque face, les couleurs doivent être scannées dans cet ordre précis pour respecter la notation standard utilisée par les solveurs de Rubik’s Cube :

- **En haut à droite**
- **En haut à gauche**
- **En bas à droite**
- **En bas à gauche**

Cette méthode permet de garantir une correspondance correcte avec le schéma de représentation du cube utilisé par l’API de résolution.

### Communication avec un script Python

Un script Python est utilisé pour faciliter l'interaction entre l'Arduino et l'API de résolution. Il reçoit les couleurs scannées via la communication série, formate ces données selon les exigences de l'API et envoie la requête HTTP pour obtenir la séquence de mouvements de résolution.

#### Fonctionnement du script Python

1. Récupération des couleurs transmises par l'Arduino via le port série.
2. Conversion des couleurs en notation standard du Rubik's Cube (U, D, L, R, F, B).
3. Envoi d'une requête HTTP à l'API Pocket Cube Solver avec les données formatées.
4. Réception de la réponse de l'API contenant la séquence de résolution.
5. Transmission de la solution à l'Arduino pour affichage sur l'écran TFT.

### Utilisation

#### Démarrage du programme

1. **Branchez l'Arduino** à l'ordinateur via un câble USB.
2. **Téléversez le programme Arduino** via l'IDE Arduino.
3. **Lancez le script Python** en exécutant la commande suivante :
   ```bash
   python3 script.py
   ```
   Assurez-vous d'avoir installé les bibliothèques nécessaires avec :
   ```bash
   pip install pyserial requests
   ```

#### Scanner un cube

1. **Allumez le système** et placez une face du cube sous le capteur de couleur.
2. **Appuyez sur le bouton** pour valider chaque couleur lue.
3. **Respectez l'ordre de scan des faces et des couleurs** en suivant la séquence U, L, F, R, B, D.
4. **Positionnez chaque case selon la méthode définie** : en haut à droite, en haut à gauche, en bas à droite, puis en bas à gauche.
5. **Suivez les instructions à l'écran** pour tourner le cube et scanner toutes les faces.
6. **Une fois toutes les couleurs enregistrées**, l'API calculera la solution.
7. **La séquence de mouvements s'affichera sur l'écran TFT** et pourra être suivie pour résoudre le cube.

## Roadmap

- Affichage des étapes de résolution.
- Écran d'attente de la réponse de l'API.
- Écran de fin.
- Rafraîchissement de l'écran trop fréquent.
- Optimisation du script Python pour une meilleure gestion des erreurs et des délais de réponse.
- Ajout d'un système de logs pour analyser les performances et les erreurs du dispositif.
