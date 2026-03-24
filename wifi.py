# pip install pyserial

import serial
import csv
from datetime import datetime

# CONFIG
PUERTO = "COM6"
BAUDIOS = 115200
ARCHIVO = "log_dispositivos.csv"

# Base de datos en memoria
dispositivos_vistos = {}

# Crear archivo si no existe
try:
    with open(ARCHIVO, 'x', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["timestamp", "mac", "rssi", "channel", "veces", "ssid"])
except FileExistsError:
    pass

# Conectar serial
ser = serial.Serial(PUERTO, BAUDIOS)

print("Escuchando datos...")

try:
    while True:
        try:
            linea = ser.readline().decode(errors='ignore').strip()

            # if linea and "," in linea:
            if linea and linea.count(",") >= 3:
                partes = linea.split(",")

                if len(partes) >= 4:
                    mac = partes[0].strip()
                    rssi = int(partes[1].strip())
                    channel = int(partes[2].strip())
                    ssid = partes[3].strip()

                    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

                    if mac in dispositivos_vistos:
                        dispositivos_vistos[mac] += 1
                    else:
                        dispositivos_vistos[mac] = 1
                        print(f"🚨 NUEVO DISPOSITIVO WIFI: {mac}")

                    if dispositivos_vistos[mac] == 1 and rssi > -60:
                        print(f"⚠️ ALERTA: dispositivo nuevo MUY cercano: {mac}")

                    veces = dispositivos_vistos[mac]

                    # print(f"{timestamp} | {mac} | RSSI: {rssi} | Veces: {veces}")
                    print(f"{timestamp} | {mac} | RSSI: {rssi} | Channel: {channel} | Veces: {veces} | SSID: {ssid}")

                    with open(ARCHIVO, 'a', newline='') as f:
                        writer = csv.writer(f)
                        writer.writerow([timestamp, mac, rssi, channel, veces, ssid])

        except Exception as e:
            print("Error:", e)

except KeyboardInterrupt:
    print("\n🛑 Escaneo detenido por el usuario")
    ser.close()
