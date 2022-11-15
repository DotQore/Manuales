/* Manual - Conecte un NodeMCU ESP32 a DotQore a través de HTTP
Programa que permite enviar datos de distancia a Dotqore
usando un NodeMCU ESP32 y un Sensor HC-SR04 */

// Librerias
#include "NewPing.h"      
#include <WiFi.h>
#include <HTTPClient.h>

// Variables de DotQore
String nodo_colector = "REEMPLAZA_CON_URL_NODO_COLECTOR";
String id_ref = "REEMPLAZA_CON_ID_DE_REFERENCIA";

// Sensor de ultrasonido
// Pines ESP32 
int trigPin = 19;      // trigger pin
int echoPin = 18;      // echo pin
NewPing sonar(trigPin, echoPin); 
uint32_t distance; // Variable a medir
String dot; // Mensaje a enviar

// Credenciales de WiFi
const char* ssid = "REEMPLAZA_CON_TU_SSID";
const char* password = "REEMPLAZA_CON_TU_CONTRASEÑA";

// Tiempo de muestreo
int segundos = 30;
unsigned long lastTime = 0;
unsigned long timerDelay = segundos*1000;

void setup(){
  Serial.begin(115200);
  Serial.println("--- Bienvenido a DotQore ---");
  WiFi.begin(ssid, password);
  Serial.println("Estableciendo conexión WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado a la red WiFi con dirección IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("Temporizador establecido en "+String(segundos)+" segundos.");
 
}

void loop(){
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      String serverName = nodo_colector + "/dotqore/values";
      http.begin(client, serverName);           
      http.addHeader("Content-Type", "application/json");
      distance = sonar.ping_cm();      
      if(distance>400 || distance<2) {
        Serial.println("Distancia fuera del rango (2cm..400cm)");
        Serial.println("Tomando nueva medición");
        distance = sonar.ping_cm();
      }
      else
      {
        dot = "{\"deviceId\": \"" + String(id_ref) + "\", \"values\":" + "{\"distance\":" + String(distance)+ "}}";
        Serial.println("Enviando el siguiente mensaje hacia DotQore: ");
        Serial.println(dot);
        int httpResponseCode = http.POST(dot);
           if (httpResponseCode == 200){
              Serial.println("Mensaje enviado correctamente");
           }
        http.end();
      }
    }
    else {
      Serial.println("WiFi Disconectado");
    }
    lastTime = millis();
  } 
}
