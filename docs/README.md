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
| 2   | Breadboard       | 1        | Plaque de prototypage                        | [Lien](#)                                                     |
| 3   | TCS3200          | 1        | Capteur de couleur                           | [Lien](#)                                                     |
| 4   | Bouton-poussoir  | 2        | Validation des couleurs                      | [Lien](#)                                                     |
| 5   | Écran TFT ST7735 | 1        | Affichage des instructions et de la solution | [Lien](#)                                                     |

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

### Communication avec un script Python

Un script Python est utilisé pour faciliter l'interaction entre l'Arduino et l'API de résolution. Il reçoit les couleurs scannées via la communication série, formate ces données selon les exigences de l'API et envoie la requête HTTP pour obtenir la séquence de mouvements de résolution.

#### Fonctionnement du script Python

1. Récupération des couleurs transmises par l'Arduino via le port série.
2. Conversion des couleurs en notation standard du Rubik's Cube (U, D, L, R, F, B).
3. Envoi d'une requête HTTP à l'API Pocket Cube Solver avec les données formatées.
4. Réception de la réponse de l'API contenant la séquence de résolution.
5. Transmission de la solution à l'Arduino pour affichage sur l'écran TFT.

#### Bibliothèques Python utilisées

- `serial` : Pour la communication série avec l'Arduino.
- `requests` : Pour envoyer la requête HTTP à l'API.
- `json` : Pour traiter la réponse JSON de l'API.

### Code principal

(_Insérer ici le code Arduino avec commentaires explicatifs_)

### Exemple de script Python

```python
import serial
import requests
import json

SERIAL_PORT = '/dev/ttyUSB0'  # Adapter selon le système
BAUD_RATE = 115200
API_URL = 'https://pocket-cube-solver.vercel.app/'

# Connexion au port série
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)

def send_to_api(cube_state):
    response = requests.get(API_URL + cube_state)
    if response.status_code == 200:
        return response.text.strip()
    else:
        return "Erreur API"

while True:
    if ser.in_waiting > 0:
        cube_state = ser.readline().decode('utf-8').strip()
        print("Cube détecté:", cube_state)
        solution = send_to_api(cube_state)
        ser.write((solution + '\n').encode())
```

## Roadmap

- Affichage des étapes de résolution.
- Écran d'attente de la réponse de l'API.
- Écran de fin.
- Rafraîchissement de l'écran trop fréquent.
- Optimisation du script Python pour une meilleure gestion des erreurs et des délais de réponse.
- Ajout d'un système de logs pour analyser les performances et les erreurs du dispositif.
