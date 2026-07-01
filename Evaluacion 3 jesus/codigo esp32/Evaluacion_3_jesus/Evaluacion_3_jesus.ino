#include <WiFi.h>
#include <PubSubClient.h>

// =======================================================
// CONFIGURACIÓN DE TU RED WI-FI
// =======================================================
const char* ssid = "Jesusnote20ultra";       
const char* password = "jesus000";   

// =======================================================
// CONFIGURACIÓN MQTT (Pauta: Protocolo visto en clase)
// =======================================================
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic_pub = "aula/monitor/chela";  // Canal donde el ESP32 publica
const char* mqtt_topic_sub = "aula/control/chela";  // Canal donde el ESP32 escucha comandos

// Definición de pines para los 3 LEDs obligatorios
const int LED_VERDE = 23;
const int LED_AMARILLO = 25;
const int LED_ROJO = 33;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
int contador = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Wi-Fi conectado!");
}

// -------------------------------------------------------
// FUNCIÓN CALLBACK: Se ejecuta cuando llega una orden desde Flask
// -------------------------------------------------------
void callback(char* topic, byte* payload, unsigned length) {
  Serial.print("Comando recibido en el canal [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String mensaje = "";
  for (int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }
  Serial.println(mensaje);

  // Lógica de actuación real basada en los comandos de la interfaz web
  if (mensaje == "VERDE_ON") {
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_AMARILLO, LOW);
    digitalWrite(LED_ROJO, LOW);
    Serial.println("Actuación: Estado NORMAL (LED Verde Encendido)");
  } 
  else if (mensaje == "AMARILLO_ON") {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARILLO, HIGH);
    digitalWrite(LED_ROJO, LOW);
    Serial.println("Actuación: Estado ADVERTENCIA (LED Amarillo Encendido)");
  } 
  else if (mensaje == "ROJO_ON") {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARILLO, LOW);
    digitalWrite(LED_ROJO, HIGH);
    Serial.println("Actuación: Estado ALERTA (LED Rojo Encendido)");
  }
  else if (mensaje == "APAGAR_TODO") {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARILLO, LOW);
    digitalWrite(LED_ROJO, LOW);
    Serial.println("Actuación: Todos los LEDs apagados");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    String clientId = "ESP32Client-" + String(random(0, 10000));
    
    if (client.connect(clientId.c_str())) {
      Serial.println(" ¡CONECTADO!");
      // OBLIGATORIO: Nos suscribimos al canal de control para recibir órdenes
      client.subscribe(mqtt_topic_sub);
      Serial.println("Suscrito al canal de control.");
    } else {
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Configurar pines de los LEDs como salidas
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  
  // Apagar todos los LEDs al iniciar
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_ROJO, LOW);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); // Asignamos la función de escucha
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publicar mediciones simuladas de temperatura cada 5 segundos
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    contador++;
    
    String mensaje = "Jesus Gonzalez | Temperatura: " + String(random(20, 35)) + "C | Contador: " + String(contador);
    Serial.print("Publicando: ");
    Serial.println(mensaje);
    
    client.publish(mqtt_topic_pub, mensaje.c_str());
  }
}
