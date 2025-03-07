# Résolveur de Rubik's Cube avec Arduino

## Description et motivations

Ce projet vise à concevoir un dispositif électronique permettant de scanner les faces d'un Rubik's Cube et d'afficher la solution en temps réel. L'objectif principal est de rendre la résolution du Rubik's Cube accessible à tous, en simplifiant l'analyse des couleurs et en fournissant une solution claire et compréhensible. Contrairement à d'autres projets automatisés, ici, le cube est déplacé manuellement par l'utilisateur devant un capteur de couleur, supprimant ainsi la complexité mécanique et réduisant les coûts.

L'utilisateur doit valider manuellement chaque couleur scannée en appuyant sur un bouton, ce qui permet d'éviter les erreurs et d'assurer une bonne reconnaissance des couleurs avant de passer à l'étape suivante.

## Mini "état de l'art"

Voici trois projets similaires ou inspirants :

1. **Rubik's Cube Solver with OpenCV & Python** - Un projet utilisant la vision par ordinateur pour identifier les couleurs et résoudre le cube.
2. **Arduino Rubik's Cube Solver** - Un dispositif mécanique entièrement automatisé pour résoudre le cube après analyse.
3. **MindCub3r** - Un robot basé sur LEGO Mindstorms capable de résoudre le Rubik's Cube de manière autonome.

## Photographies et vidéo du prototype final

## Schéma du circuit électronique et BOM

- **Capteur de couleur** : TCS3200
- **Microcontrôleur** : Arduino Uno R3
- **Affichage** : Écran LCD Display LCI
- **Bouton poussoir** : Validation de chaque couleur scannée
- **Autres composants** : câblage

## Recette pour reproduire le dispositif

1. Assembler le capteur de couleur sur une base stable.
2. Connecter les composants électroniques selon le schéma.
3. Programmer l'Arduino pour gérer la lecture des couleurs et la transmission des données à un ordinateur ou un écran.
4. Ajouter un bouton permettant à l'utilisateur de valider manuellement chaque couleur scannée.
5. Développer un algorithme permettant d’interpréter les couleurs et d’envoyer les données à une API de résolution de Rubik’s Cube.
6. Tester et ajuster en fonction des résultats obtenus.

## API et algorithmes utilisés

Pour résoudre le Rubik's Cube, j'ai utilisé l'API **Pocket Cube Solver** ([https://pocket-cube-solver.vercel.app/](https://pocket-cube-solver.vercel.app/)), qui fournit une solution rapide basée sur un algorithme optimisé pour le cube 2x2x2. Cette API prend en entrée la configuration des couleurs scannées et retourne une séquence de mouvements permettant de résoudre le cube efficacement.

### Fonctionnement général du code :

1. Capture des couleurs des six faces du cube avec le capteur.
2. L'utilisateur valide chaque couleur en appuyant sur un bouton.
3. Conversion des valeurs du capteur en notation standard du Rubik’s Cube (U, D, L, R, F, B).
4. Envoi des données à l'API Pocket Cube Solver via une communication série ou une requête HTTP.
5. Récupération et affichage de la solution.

## Difficultés et évolutions possibles

### Difficultés rencontrées

- Calibration du capteur de couleur pour éviter les erreurs de lecture.
- Conversion fiable des couleurs captées en notation Rubik's Cube standard.
- Passage des données à l'API selon l'ordre demandé, et pas l'ordre scanné.
- Gestion de la communication entre l'Arduino et l'ordinateur/API pour obtenir la solution.

### Évolutions possibles

- **Ajout de plusieurs capteurs** : Permettrait de capturer plusieurs couleurs simultanément et d'accélérer le processus de lecture.
- **Déplacement automatisé des capteurs** : Utiliser des servomoteurs pour déplacer les capteurs autour du cube plutôt que de le manipuler manuellement.
- **Utilisation d’une caméra** : Pour une reconnaissance plus rapide et précise des couleurs.
- **Développement d'une application mobile** : Pour visualiser les étapes de la solution de manière plus interactive.
- **Intégration d’une interface graphique** : Sur un écran embarqué pour afficher directement la solution.
