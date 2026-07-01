# 🌐 Distributed Edge-HUD IoT Network — Evaluación Sumativa 3
*[span_5](start_span)Asignatura:* Desarrollo de Software para Hardware (DCSH01)[span_5](end_span)  
*[span_6](start_span)Integrantes:* Ignacio Henseleit & Jesus Gonzalez[span_6](end_span)

---

## 📝 1. Explicación General del Sistema Distributed IoT
[span_7](start_span)Este proyecto implementa un ecosistema de Internet de las Cosas (IoT) con arquitectura distribuida y procesamiento perimetral (Edge) para el monitoreo térmico industrial y la mitigación de riesgos en tiempo real[span_7](end_span). [span_8](start_span)A diferencia de los sistemas centralizados convencionales, este desarrollo desacopla las tareas de adquisición y actuación utilizando múltiples microcontroladores autónomos comunicados mediante el protocolo industrial MQTT[span_8](end_span).

### Flujo de Trabajo del Ecosistema:
* *[span_9](start_span)[span_10](start_span)Capa de Adquisición (Edge-Sensor):* El primer nodo lee variables físicas críticas (Temperatura) de forma cíclica y las transmite codificadas en tramas de texto hacia un Broker en la nube (broker.hivemq.com)[span_9](end_span)[span_10](end_span).
* *[span_11](start_span)[span_12](start_span)Capa de Coordinación y Persistencia (Backend Flask + Supabase):* Un servidor centralizado en Python/Flask actúa como el motor lógico del sistema[span_11](end_span)[span_12](end_span). [span_13](start_span)[span_14](start_span)Escucha los canales de telemetría, parsea los datos e interactúa de forma directa con una base de datos relacional en la nube (Supabase)[span_13](end_span)[span_14](end_span). [span_15](start_span)Supabase almacena cada registro inyectando de manera automática marcas temporales precisas con resolución de segundos (TIMESTAMPTZ)[span_15](end_span).
* *[span_16](start_span)[span_17](start_span)Capa de Interfaz de Usuario (Cyber-HUD):* Una consola web con diseño vanguardista y animaciones CSS nativas (sin una sola línea de JavaScript y estructurada puramente con selectores de ID) despliega los registros históricos en un monitor estilo terminal[span_16](end_span)[span_17](end_span). [span_18](start_span)Desde un panel táctil de conmutación de comandos, el operador puede inyectar directivas críticas de control hacia la red[span_18](end_span).
* *[span_19](start_span)Capa de Actuación Externa (Edge-Actuator):* El segundo nodo en la red capta asíncronamente las órdenes de control emitidas desde la web de Flask, procesando los comandos a través de una función callback interna para alterar instantáneamente el estado de una matriz de 3 indicadores lumínicos (LEDs)[span_19](end_span).

---

## 🎛️ 2. Justificación del Rol de las Tarjetas (Arquitectura Multnodo)
[span_20](start_span)[span_21](start_span)Para optimizar el uso de los recursos de hardware disponibles y cumplir estrictamente con los requisitos de la rúbrica institucional respecto al trabajo grupal, las responsabilidades de procesamiento se han distribuido simétricamente[span_20](end_span)[span_21](end_span):

* *Nodo Alfa (ESP32 — Responsable: Ignacio Henseleit):* Operando como una estación de telemetría pura. [span_22](start_span)Su firmware está optimizado para la inicialización del stack de red Wi-Fi, el empaquetado seguro de datos dinámicos que incluyen la firma de identidad del operador y la publicación síncrona cada 5000ms de los datos físicos en el tópico de monitoreo, protegiendo al sistema contra bloqueos de concurrencia[span_22](end_span).
* *Nodo Beta (ESP32 — Responsable: Jesus Gonzalez):* Operando como una estación terminal de actuación remota. [span_23](start_span)Este dispositivo se mantiene permanentemente suscrito al canal de control emitido por el backend[span_23](end_span). [span_24](start_span)Cuenta con un motor de interrupciones por software (callback) que decodifica las cadenas de comando recibidas en tiempo real para conmutar las líneas físicas digitales de los pines GPIO asociados a la matriz obligatoria de 3 LEDs (Verde: Estable, Amarillo: Advertencia, Rojo: Alerta Crítica)[span_24](end_span).

---

## 📊 3. Diagrama General del Sistema Distribuido
[span_25](start_span)El flujo de datos bidireccional y asíncrono se distribuye a través del Broker en la nube bajo el siguiente esquema de arquitectura[span_25](end_span):
[ NODO ALFA: ESP32 SENSOR ]            [ NODO BETA: ESP32 ACTUADOR ]
(Ignacio Henseleit)                       (Jesus Gonzalez)
|                                         ^
(Publica Telemetría)                     (Escucha Comandos)
[aula/monitor/proceso]                   [aula/control/proceso]
|                                         |
v                                         v
+-----------------------------------------------------------------+
|                    BROKER MQTT (HiveMQ Cloud)                   |
+-----------------------------------------------------------------+
|                                         ^
(Suscripción Datos)                       (Publicación Control)
v                                         |
+-----------------------------------------------------------------+
|                     BACKEND CENTRAL (Flask)                     |
|      - Servidor de Telemetría en Python                         |
|      - Panel de Control Remoto Estilo Cyber-HUD (Solo IDs)      |
+-----------------------------------------------------------------+
|
(Query SQL Nativa)
v
+-----------------------------------------------------------------+
|                 BASE DE DATOS NUBE (Supabase)                   |
|      - Registro Relacional de Variables Físicas                 |
|      - Estampado de Tiempo Automático (Año/Mes/Día/Hora/Seg)    |
+-----------------------------------------------------------------+

## 🚀 4. Instrucciones para la Ejecución y Despliegue del Sistema

### [span_26](start_span)Configuración del Entorno Local (Windows)[span_26](end_span):
[span_27](start_span)Asegúrese de inicializar su terminal de comandos (cmd) con permisos de administración e instalar las dependencias de control requeridas ejecutando[span_27](end_span):
```cmd
pip install flask paho-mqtt supabase

Despliegue de Hardware: * Cargue el firmware de telemetría en el Nodo Alfa de Ignacio, configurando el SSID y clave de la red Wi-Fi correspondiente.
​Cargue el firmware de escucha y callback en el Nodo Beta de Jesús, verificando el correcto cableado de las resistencias de protección y la matriz de 3 LEDs en los pines digitales.
Enlace Cloud de Base de Datos: Ingrese a su consola de Supabase, deshabilite las políticas restrictivas RLS e inyecte los parámetros URL y API Key dentro de las constantes de configuración en el archivo base app.py.
Lanzamiento del Backend: Desde la ruta raíz del proyecto en la consola de Windows, ejecute el servidor Python:
python app.py
Acceso Operador: Abra cualquier navegador web local e ingrese al puerto de escucha de Flask: http://127.0.0.1:5000/.
