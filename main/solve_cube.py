import serial
import requests
import json
import re
import time

PORT = "COM5"  # Remplace par ton port correct
BAUDRATE = 115200
TIMEOUT = 5

# Fonction pour bien séparer les mouvements
def split_moves(solution_str):
    return re.findall(r"[FRUBLD][2']?", solution_str)  # Retourne une liste avec les mouvements séparés

try:
    ser = serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT)
    print("Connexion au port série réussie.")
except serial.SerialException as e:
    print(f"Erreur d'accès au port série : {e}")
    exit(1)

cube_data = ""
while not cube_data:
    print("En attente des données du cube depuis l'Arduino...")
    ligne = ser.readline().decode('utf-8', errors='ignore').strip()
    if ligne == "SEND_START":
        print("Début de réception des données du cube...")
        cube_data = ser.readline().decode('utf-8', errors='ignore').strip()
        print(f"Cube détecté : {cube_data}")
    elif ligne == "SEND_END":
        print("Fin de réception des données.")

if not cube_data:
    print("Erreur : aucune donnée de cube reçue.")
    ser.close()
    exit(1)

# Envoi de la requête API
url = f"https://pocket-cube-solver.vercel.app/{cube_data}"
print(f"Envoi de la requête API : {url}")

try:
    response = requests.get(url, timeout=10)
    if response.status_code == 200:
        solution_data = json.loads(response.text)
        raw_moves = solution_data.get("sol", "")

        # Création d'une liste de mouvements bien formatée
        moves_list = split_moves(raw_moves)

        print(f"Mouvements envoyés à l'Arduino : {' '.join(moves_list)}")

        # Envoi des mouvements un par un
        ser.write("MOVES_START\n".encode())
        time.sleep(0.2)

        for move in moves_list:
            ser.write(f"{move}\n".encode())
            print(f"Envoyé à l'Arduino : {move}")
            time.sleep(0.2)  # Petite pause pour éviter les conflits série

        ser.write("MOVES_END\n".encode())
        print("Tous les mouvements ont été envoyés.")

        # Attente d'une confirmation de l'Arduino
        while True:
            ligne = ser.readline().decode('utf-8', errors='ignore').strip()
            if ligne == "OK":
                print("Arduino a bien reçu la solution.")
                break
    else:
        print(f"Erreur API : {response.status_code} - {response.text}")
except requests.RequestException as e:
    print(f"Erreur réseau : {e}")

ser.close()
print("Port série fermé.")
