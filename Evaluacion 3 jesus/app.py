from flask import Flask, render_template, request, redirect, url_for
import paho.mqtt.client as mqtt
from supabase import create_client, Client
import threading
import time

app = Flask(__name__)

# =======================================================
# 1. CONFIGURACIÓN DE CREDENCIALES (SUPABASE Y MQTT)
# =======================================================
SUPABASE_URL = "https://iuaehyxpbboslniqrgch.supabase.co" #poner link superbase jesus

# Pega aquí tu llave pública de Supabase
SUPABASE_KEY = "sb_publishable_rYuqXfZi4_sDQzxy3oZouA_UzaRO0GE" # poner key jesus

MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883
MQTT_TOPIC_PUB = "aula/control/chela"  # Canal donde Flask PUBLICA las órdenes para los LEDs
MQTT_TOPIC_SUB = "aula/monitor/chela"  # Canal donde Flask ESCUCHA las temperaturas del ESP32

try:
    supabase: Client = create_client(SUPABASE_URL, SUPABASE_KEY)
    print("¡Cliente de Supabase enlazado con éxito!")
except Exception as e:
    print("Error al inicializar Supabase:", e)

mensajes_pantalla = []

# =======================================================
# 2. LÓGICA DE RECEPCIÓN MQTT (REGISTRO EN LA NUBE)
# =======================================================
def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        print("¡Servidor Flask conectado al Broker HiveMQ!")
        client.subscribe(MQTT_TOPIC_SUB)  # Escuchar temperaturas [cite: 17, 27]
    else:
        print(f"Error de conexión MQTT, código: {rc}")

def on_message(client, userdata, msg):
    """Recibe datos físicos del ESP32 y los guarda en Supabase de forma relacional[cite: 13, 17]."""
    try:
        payload_txt = msg.payload.decode('utf-8')
        print(f"Dato de internet -> {payload_txt}")
        
        mensajes_pantalla.insert(0, payload_txt)
        if len(mensajes_pantalla) > 50:
            mensajes_pantalla.pop()
            
        if "Jesus Gonzalez" in payload_txt:
            partes = payload_txt.split("|")
            temp_str = partes[1].replace("Temperatura:", "").replace("C", "").strip()
            temp_val = float(temp_str)
            
            cont_str = partes[2].replace("Contador:", "").strip()
            cont_val = int(cont_str)
            
            datos_insertar = {
                "Dispositivo": "Jesus Gonzalez",
                "Temperatura": temp_val,
                "Contador": cont_val
            }
            # Guardado automático con año/mes/día/hora/segundo mediante default now() 
            supabase.table("eva 3_poblete").insert(datos_insertar).execute()
            print("¡Evento físico guardado en Supabase!")
    except Exception as e:
        print("Error en procesamiento o Supabase:", e)

try:
    mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
except AttributeError:
    mqtt_client = mqtt.Client()

mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

def bucle_mqtt():
    try:
        mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
        mqtt_client.loop_forever()
    except Exception as e:
        print("Error en loop MQTT:", e)

threading.Thread(target=bucle_mqtt, daemon=True).start()

# =======================================================
# 3. RUTAS DEL BACKEND FLASK (MECANISMO DE ACTIVACIÓN )
# =======================================================
@app.route('/')
def index():
    """Muestra el monitor estilo terminal obligatorio."""
    return render_template('index.html', mensajes=mensajes_pantalla)

@app.route('/enviar_comando', methods=['POST'])
def enviar_comando():
    """Ruta de activación que envía la orden por MQTT al ESP32 (SIN usar JavaScript)."""
    comando = request.form.get('comando')
    if comando:
        # Publicamos el comando ("VERDE_ON", "ROJO_ON", etc.) hacia el ESP32
        mqtt_client.publish(MQTT_TOPIC_PUB, comando)
        print(f"¡Comando enviado al hardware por MQTT -> {comando}!")
    return redirect(url_for('index'))

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=False)