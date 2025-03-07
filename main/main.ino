#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

void traiterSolution(String solution);
void afficherMouvement();



// Définition des couleurs pour le Rubik's Cube
#define WHITE   ST77XX_WHITE
#define RED     ST77XX_RED
#define BLUE    ST77XX_BLUE
#define GREEN   ST77XX_GREEN
#define YELLOW  ST77XX_YELLOW
#define ORANGE  0xFB20  // Orange plus profond et moins jaune (RGB565)
#define GRAY    0x8410  // Gris pour indiquer les cases non scannées

// Définition des broches de l'écran TFT
#define TFT_CS     A2
#define TFT_RST    A3
#define TFT_DC     8

// Définition des broches du capteur de couleur
#define S0 3
#define S1 4
#define S2 5
#define S3 6
#define sensorOut 7

//  Boutons pour avancer/reculer dans les mouvements
#define BUTTON_NEXT  1
#define BUTTON_PREV  2

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// État de la capture des couleurs
int faceIndex = 0;
int caseIndex = 0;
bool scanningComplete = false;

// Tableau pour stocker les couleurs du cube (6 faces, 4 cases par face)
uint16_t cubeColors[6][4];
int colorReference[6][3];  // Stocke les valeurs R, G, B des 6 couleurs
const char* colorNames[6] = {"Vert", "Jaune", "Blanc", "Rouge", "Orange", "Bleu"};
uint16_t colorMapping[6] = {GREEN, YELLOW, WHITE, RED, ORANGE, BLUE};  // Association couleurs - hex

uint16_t lastDisplayedFace[4];  // Permet de stocker la dernière face affichée pour éviter les refresh inutiles

#define MAX_MOVES 20
// Liste des mouvements de résolution
String moves[20];  
 int moveCount = 0;
 int moveIndex = 0;
 bool showSolution = false;

bool attenteValidation = false; 

String rotations[5] = {
  "Tourne à gauche",   
  "Tourne en BAS",     
  "Tourne en BAS",   
  "Tourne en BAS",    
  "Tourne à droite"  
};

bool attenteRotation = false;

// Déclarations des fonctions pour éviter les erreurs de compilation
void afficherFace();
void scannerFace();
void afficherValidation();
void afficherInstructionRotation();
void envoyerDonneesAPI();
bool facesIdentiques(uint16_t face1[4], uint16_t face2[4]);
uint16_t detecterCouleur();
int readColor(bool s2, bool s3);
bool apiEnvoyee = false; // Nouvelle variable pour suivre si l'API a déjà été envoyée

void calibrerCouleurs() {
    Serial.println("Calibration des couleurs : place chaque couleur sous le capteur et appuie sur le bouton NEXT.");

    for (int i = 0; i < 6; i++) {
        tft.fillScreen(ST77XX_BLACK);
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(10, 40);
        tft.print("Place le ");
        tft.print(colorNames[i]);
        tft.println(" sous le capteur");
        tft.println("Appuie sur NEXT");

        while (digitalRead(BUTTON_NEXT) == HIGH); // Attente d'appui
        delay(200);
        while (digitalRead(BUTTON_NEXT) == LOW); // Attente du relâchement

        // Scanner la couleur et stocker les valeurs RGB
        colorReference[i][0] = readColor(LOW, LOW);   // Rouge
        colorReference[i][1] = readColor(HIGH, HIGH); // Vert
        colorReference[i][2] = readColor(LOW, HIGH);  // Bleu

        Serial.print(colorNames[i]);
        Serial.print(" -> R:");
        Serial.print(colorReference[i][0]);
        Serial.print(" G:");
        Serial.print(colorReference[i][1]);
        Serial.print(" B:");
        Serial.println(colorReference[i][2]);
    }

    Serial.println("Calibration terminée !");
}



void setup() {
  Serial.begin(115200);
  tft.initR(INITR_144GREENTAB);
  tft.setRotation(0);
  
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_PREV, INPUT_PULLUP);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  calibrerCouleurs();


  // Initialiser les couleurs à gris (cases non scannées)
  for (int i = 0; i < 6; i++)
    for (int j = 0; j < 4; j++)
      cubeColors[i][j] = GRAY;

  Serial.println("Démarrage du scan des couleurs...");
  tft.fillScreen(ST77XX_BLACK);
  afficherFace();
}

void loop() {
    if (Serial.available()) {
        String ligne = Serial.readStringUntil('\n');
        ligne.trim();

        if (ligne == "MOVES_START") {
            moveCount = 0;  
            showSolution = false;
        } else if (ligne == "MOVES_END") {
            showSolution = (moveCount > 0);
            moveIndex = 0;
            afficherMouvement();
        } else if (moveCount < 20) { 
            moves[moveCount++] = ligne;
            Serial.println("OK");
        }
    }

    if (attenteValidation) {
        afficherValidation();
    } else if (attenteRotation) {
        afficherInstructionRotation();
    } else if (!scanningComplete) {
        scannerFace();
    } else if (!showSolution && !apiEnvoyee) {
        envoyerDonneesAPI();
        apiEnvoyee = true;
    } else {
        gererBoutons(); // Ajout de la gestion des boutons ici
        afficherMouvement();
    }
}




void scannerFace() {
    if (!facesIdentiques(cubeColors[faceIndex], lastDisplayedFace)) {
        afficherFace();
    }

    // Bouton NEXT → Scanner une couleur
    if (!attenteValidation && digitalRead(BUTTON_NEXT) == LOW) {
        delay(200);
        while (digitalRead(BUTTON_NEXT) == LOW); // Attendre le relâchement du bouton

        uint16_t couleurDetectee = detecterCouleur();
        cubeColors[faceIndex][caseIndex] = couleurDetectee;

        Serial.print("Case ");
        Serial.print(caseIndex + 1);
        Serial.print(" de la face ");
        Serial.print(faceIndex + 1);
        Serial.print(" détectée : ");
        Serial.println(couleurDetectee, HEX);

        caseIndex++;

        if (caseIndex >= 4) {  
            caseIndex = 0;
            attenteValidation = true; // On demande la validation manuelle
            Serial.println("Face complète. Appuie sur le bouton NEXT pour valider.");
        }
        delay(300);
    }

    // Bouton PREV → Annuler la dernière entrée
    if (!attenteValidation && digitalRead(BUTTON_PREV) == LOW) {
        delay(200);
        while (digitalRead(BUTTON_PREV) == LOW); // Attendre le relâchement du bouton

        if (caseIndex > 0) {
            caseIndex--;  
        } else if (faceIndex > 0) {
            faceIndex--;
            caseIndex = 3;
        }

        cubeColors[faceIndex][caseIndex] = GRAY;
        afficherFace();
        Serial.println(" Annulation : dernière case supprimée.");
        delay(300);
    }

    //  Validation manuelle → Passer à la rotation ou finaliser le scan
    if (attenteValidation) {
        afficherValidation(); // Attendre ici l'action de l'utilisateur
    }
}



uint16_t detecterCouleur() {
    int red = readColor(LOW, LOW);
    int green = readColor(HIGH, HIGH);
    int blue = readColor(LOW, HIGH);

    Serial.print("Rouge: "); Serial.print(red);
    Serial.print(" Vert: "); Serial.print(green);
    Serial.print(" Bleu: "); Serial.println(blue);

    int distances[6];
    for (int i = 0; i < 6; i++) {
        distances[i] = abs(red - colorReference[i][0]) + 
                       abs(green - colorReference[i][1]) + 
                       abs(blue - colorReference[i][2]);
    }

    int minIndex = 0;
    for (int i = 1; i < 6; i++) {
        if (distances[i] < distances[minIndex]) {
            minIndex = i;
        }
    }

    return colorMapping[minIndex];
}



//  Vérifie si une face a changé pour éviter les refresh inutiles
bool facesIdentiques(uint16_t face1[4], uint16_t face2[4]) {
  for (int i = 0; i < 4; i++) {
    if (face1[i] != face2[i]) return false;
  }
  return true;
}

//  Afficher une face sur l'écran (seulement si changement)
void afficherFace() {
  memcpy(lastDisplayedFace, cubeColors[faceIndex], 4 * sizeof(uint16_t)); // Sauvegarde l'état actuel

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 10);
  tft.print("Face ");
  tft.print(faceIndex + 1);

  int x = 30, y = 60, size = 30, spacing = 2;

  for (int i = 0; i < 4; i++) {
    uint16_t color = (i == caseIndex) ? GRAY : cubeColors[faceIndex][i];
    tft.fillRect(x + (i % 2) * (size + spacing), y + (i / 2) * (size + spacing), size, size, color);
    tft.drawRect(x + (i % 2) * (size + spacing), y + (i / 2) * (size + spacing), size, size, WHITE);
  }
}




// Fonction pour lire une couleur avec pulseIn()
int readColor(bool s2, bool s3) {
  digitalWrite(S2, s2);
  digitalWrite(S3, s3);
  delay(10);  
  return pulseIn(sensorOut, LOW, 30000);
}

void afficherInstructionRotation() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(YELLOW);
    tft.setTextSize(2);
    tft.setCursor(10, 40);

    int instructionIndex = min(faceIndex - 1, 4);
    tft.print(rotations[instructionIndex]);

    Serial.println(rotations[instructionIndex]);
    Serial.println("Appuie sur le bouton pour continuer.");

    delay(1000); // Donne le temps de lire

    // Attendre que le bouton soit relâché avant de continuer
    while (digitalRead(BUTTON_NEXT) == LOW);

    // Attente d'un nouvel appui pour continuer
    while (digitalRead(BUTTON_NEXT) == HIGH);
    delay(200); // Anti-rebond

    attenteRotation = false;
    
    // Maintenant seulement, afficher la nouvelle face
    afficherFace();
}




void afficherValidation() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(YELLOW);
    tft.setTextSize(2);
    tft.setCursor(10, 40);
    tft.print("Valider ? (BTN NEXT)");

    Serial.println("Face complète. Appuie sur le bouton pour valider.");

    // Attendre que le bouton soit relâché avant validation
    while (digitalRead(BUTTON_NEXT) == LOW); 

    // Attente explicite d'un nouvel appui (évite la validation instantanée)
    while (digitalRead(BUTTON_NEXT) == HIGH);
    delay(200); // Anti-rebond

    faceIndex++;
    attenteValidation = false;

    if (faceIndex < 6) {
        attenteRotation = true;  // Passe à la rotation
        afficherInstructionRotation(); // Affiche l’instruction avant d'afficher la nouvelle face
    } else {
        scanningComplete = true;
        Serial.println("Scan du cube terminé !");
    }
}

String convertirCubeEnString() {
    char couleurs[6] = {'W', 'R', 'B', 'G', 'Y', 'O'}; // Lettres associées aux couleurs
    uint16_t valeursHexa[6] = {WHITE, RED, BLUE, GREEN, YELLOW, ORANGE}; // Valeurs en RGB565

    String cubeString = "";

    // **Mapping API demandé**
    int scanToAPI[24] = {
        0, 1, 2, 3, 5, 7, 9, 11, 13, 15, 17, 19,  // 1,2,3,4,6,8,10,12,14,16,18,20
        4, 6, 8, 10, 12, 14, 16, 18,             // 5,7,9,11,13,15,17,19
        22, 20, 23, 21                          // 23,21,24,22
    };

    for (int i = 0; i < 24; i++) {
        int face = scanToAPI[i] / 4;  // Trouver la face correcte
        int index = scanToAPI[i] % 4; // Trouver la case correcte

        uint16_t couleur = cubeColors[face][index];

        // Trouver la lettre associée
        char lettre = '?';
        for (int j = 0; j < 6; j++) {
            if (couleur == valeursHexa[j]) {
                lettre = couleurs[j];
                break;
            }
        }

        cubeString += lettre;
    }

    Serial.print("Cube formaté pour API : ");
    Serial.println(cubeString);

    return cubeString;
}


void envoyerDonneesAPI() {
    String cubeData = convertirCubeEnString();
    
    Serial.println("\nDonnées prêtes pour l'API : " + cubeData);
    Serial.println("SEND_START"); 
    Serial.println(cubeData);
    Serial.println("SEND_END");

    Serial.println("Copie-colle cette URL dans ton navigateur si besoin :");
    Serial.print("https://pocket-cube-solver.vercel.app/");
    Serial.println(cubeData);
}


void afficherMouvement() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(20, 40);

    if (moveCount > 0) {
        tft.print("Etape ");
        tft.print(moveIndex + 1);
        tft.print("/");
        tft.print(moveCount);

        tft.setCursor(20, 80);
        tft.setTextSize(4);
        tft.print(moves[moveIndex]);
    } else {
        tft.setCursor(10, 40);
        tft.print("Aucune solution");
    }
}

void gererBoutons() {
    if (showSolution) {
        if (digitalRead(BUTTON_NEXT) == LOW) {
            delay(200);
            while (digitalRead(BUTTON_NEXT) == LOW); // Attente du relâchement du bouton
            if (moveIndex < moveCount - 1) moveIndex++;
            afficherMouvement();
        }

        if (digitalRead(BUTTON_PREV) == LOW) {
            delay(200);
            while (digitalRead(BUTTON_PREV) == LOW); // Attente du relâchement du bouton
            if (moveIndex > 0) moveIndex--;
            afficherMouvement();
        }
    }
}