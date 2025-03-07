# Documentation

## Recherche et conception

Ce projet vise à rendre la résolution d'un Rubik's Cube plus accessible grâce à un dispositif de scan basé sur un capteur de couleur et un affichage TFT. L'utilisateur place chaque face du cube sous le capteur et valide la couleur scannée avec un bouton-poussoir. Les données sont ensuite envoyées à une API de résolution qui fournit une séquence de mouvements permettant de résoudre le cube.

## Circuit électronique

Le circuit utilise un **capteur de couleur TCS3200**, un **écran TFT ST7735**, des **boutons-poussoirs** pour la validation et un **microcontrôleur Arduino Uno**.

### Schéma

![Schéma du circuit](/docs/assets/CubeResolver_bb.png)

### BOM (Bill of Materials)

| Réf | Composant        | Quantité | Description                                  | Fournisseur / Lien                                            |
| --- | ---------------- | -------- | -------------------------------------------- | ------------------------------------------------------------- |
| 1   | Arduino Uno R3   | 1        | Microcontrôleur ATmega328P                   | [Arduino](https://store.arduino.cc/products/arduino-uno-rev3) |
| 2   | Breadboard       | 1        | Plaque de prototypage                        | (#)                                                           |
| 3   | TCS3200          | 1        | Capteur de couleur                           | (#)                                                           |
| 4   | Bouton-poussoir  | 2        | Validation des couleurs                      | (#)                                                           |
| 5   | Écran TFT ST7735 | 1        | Affichage des instructions et de la solution | (#)                                                           |

## Programme

Le programme gère la **capture des couleurs**, l'**envoi des données** à l'API de résolution et l'**affichage de la solution**. Il est structuré en plusieurs étapes :

1. **Calibration des couleurs** : L'utilisateur place chaque couleur sous le capteur et valide avec un bouton.
2. **Scan des faces** : Les couleurs sont enregistrées face par face.
3. **Envoi à l'API Pocket Cube Solver** ([https://pocket-cube-solver.vercel.app/](https://pocket-cube-solver.vercel.app/)).
4. **Affichage de la solution** : La séquence de mouvements est affichée sur l'écran TFT, et l'utilisateur peut naviguer entre les étapes avec les boutons.

### Bibliothèques utilisées

- `Adafruit_GFX.h`
- `Adafruit_ST7735.h`
- `SPI.h`

## Roadmap

- Affichage des étapes de résolution.
- Écran d'attente de la réponse de l'API.
- Écran de fin.
- Rafraîchissement de l'écran trop fréquent.
