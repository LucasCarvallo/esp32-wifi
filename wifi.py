# pip install pyserial 

import serial
import csv
from datetime import datetime
import os
import time

# CONFIG
PUERTO = "COM6"
BAUDIOS = 115200
ARCHIVO = "wifi_aps_log.csv"
ARCHIVO_RESUMEN = "wifi_aps_resumen.csv"

# Base de datos en memoria
dispositivos_vistos = {}
dispositivos_info = {}
ultimo_guardado = time.time()

# Crear archivos si no existen
try:
    with open(ARCHIVO, 'x', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["timestamp", "mac", "rssi", "channel", "veces", "ssid"])

    with open(ARCHIVO_RESUMEN, 'x', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["timestamp", "mac", "rssi", "channel", "veces", "ssid", "last_seen"])

except FileExistsError:
    pass

# 🔥 Cargar historial desde CSV
if os.path.exists(ARCHIVO_RESUMEN):
    with open(ARCHIVO_RESUMEN, newline='') as f:
        reader = csv.DictReader(f)

        for row in reader:
            mac = row["mac"]
            rssi = int(row["rssi"])
            channel = int(row["channel"])
            veces = int(row["veces"])
            ssid = row["ssid"]
            last_seen = row.get("last_seen", "")  # robusto

            dispositivos_vistos[mac] = veces

            dispositivos_info[mac] = {
                "rssi": rssi,
                "channel": channel,
                "ssid": ssid,
                "veces": veces,
                "last_seen": last_seen
            }

print(f"📂 Dispositivos cargados desde historial: {len(dispositivos_vistos)}")

# Conectar serial
ser = serial.Serial(PUERTO, BAUDIOS)

print("Escuchando datos...")

try:
    archivo = open(ARCHIVO, 'a', newline='')
    writer_archivo = csv.writer(archivo)

    while True:
        try:
            linea = ser.readline().decode(errors='ignore').strip()

            if not linea.startswith("DATA:"):
                continue

            linea = linea[5:]  # más rápido que replace

            if linea and linea.count(",") >= 3: # Asegurarse de que hay al menos 4 partes (MAC, RSSI, Channel, SSID)
                partes = linea.split(",", 3) # Solo dividir en 4 partes para evitar problemas con SSID que contengan comas

                if len(partes) >= 4:
                    mac = partes[0].strip()
                    try:
                        rssi = int(partes[1].strip())
                        channel = int(partes[2].strip())
                    except:
                        continue

                    ssid = partes[3].strip()
                    ssid = ssid.replace("\n", "").replace("\r", "") # Limpiar caracteres

                    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S") # timestamp_unix = int(datetime.now().timestamp())

                    es_nuevo_historico = mac not in dispositivos_vistos

                    # 🔥 NUEVO HISTÓRICO
                    if es_nuevo_historico:
                        print(f"🚨 NUEVO HISTÓRICO WIFI: {mac}")

                    # Alerta si es nuevo y está cerca
                    if es_nuevo_historico and rssi > -60: # Si no estaba en el histórico y tiene buena señal, es un nuevo dispositivo cercano
                        print(f"⚠️ ALERTA: dispositivo nuevo MUY cercano: {mac}")

                    # Conteo acumulado (FUENTE DE VERDAD)
                    dispositivos_vistos[mac] = dispositivos_vistos.get(mac, 0) + 1
                    veces = dispositivos_vistos[mac]

                    print(f"{timestamp} | {mac} | RSSI: {rssi} | Channel: {channel} | Veces: {veces} | SSID: {ssid}")

                    writer_archivo.writerow([timestamp, mac, rssi, channel, veces, ssid])
                    archivo.flush()

                    # 🔥 RESUMEN EN MEMORIA
                    if mac not in dispositivos_info:
                        dispositivos_info[mac] = {
                            "rssi": rssi,
                            "channel": channel,
                            "ssid": ssid,
                            "veces": veces,
                            "last_seen": timestamp
                        }
                    else:
                        dispositivos_info[mac]["veces"] = veces
                        dispositivos_info[mac]["last_seen"] = timestamp

                        # actualizar mejor señal (y canal asociado)
                        if rssi > dispositivos_info[mac]["rssi"]:
                            dispositivos_info[mac]["rssi"] = rssi
                            dispositivos_info[mac]["channel"] = channel

                        # actualizar SSID si cambia
                        if ssid != "HIDDEN":
                            dispositivos_info[mac]["ssid"] = ssid

            # 🔥 GUARDAR RESUMEN CADA 10 SEGUNDOS
            if time.time() - ultimo_guardado > 10:
                timestamp_guardado = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

                with open(ARCHIVO_RESUMEN, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(["timestamp", "mac", "rssi", "channel", "veces", "ssid", "last_seen"])

                    for mac, data in dispositivos_info.items():
                        writer.writerow([
                            timestamp_guardado,
                            mac,
                            data["rssi"],
                            data["channel"],
                            data["veces"],
                            data["ssid"],
                            data["last_seen"]
                        ])

                print("💾 Resumen actualizado")
                ultimo_guardado = time.time()

        except Exception as e:
            print("Error:", e)

except KeyboardInterrupt:
    print("\n🛑 Escaneo detenido por el usuario")
    ser.close()
    archivo.close()
