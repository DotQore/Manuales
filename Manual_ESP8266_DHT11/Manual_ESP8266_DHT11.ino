/* Manual - Conecte un NodeMCU ESP8266 a DotQore a través de HTTP
Programa que permite enviar datos de temperatura y humedad a Dotqore
usando un NodeMCU ESP8266 y un Sensor DHT11 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DHTesp.h" 

String nodo_colector = "INGRESA_URL_NODO_COLECTOR";
String id_ref = "INGRESA_IDENTIFICADOR_DISPOSITIVO";
uint32_t temp;
uint32_t hum;
int th = 2; 
int segundos = 60;
const char *ssid = "INGRESA_NOMBRE_DE_WIFI";  
const char *password = "INGRESA_CONTRASEÑA";
String data;
DHTesp dht;

void setup() {
    Serial.begin(115200);
    dht.setup(th, DHTesp::DHT11);    
    WiFi.begin(ssid, password);     
    Serial.println("");
    Serial.print("Conectandose a una red WiFi ");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Conectado a: ");
    Serial.print(ssid);
    Serial.print(" por la dirección IP: ");
    Serial.println(WiFi.localIP()); 
    Serial.print("Nodo Colector de DotQore: ");
    Serial.println(nodo_colector);
}

void loop() {
    hum = dht.getHumidity();
    temp = dht.getTemperature();
    while(isnan(hum) || isnan(temp)){
      delay(dht.getMinimumSamplingPeriod());
      hum = dht.getHumidity();
      temp = dht.getTemperature();
    }    
    data = "{\"deviceId\": \"" + String(id_ref) + "\", \"values\":" + "{\"temp\":" + String(temp)+ ", \"hum\":" + String(hum)+ "}}";
    Serial.println("Datos a enviar: ");
    Serial.println(data);
    if(WiFi.status()== WL_CONNECTED){
        WiFiClient client;
        HTTPClient http;
        String full_url = String(nodo_colector) + "/dotqore/values";
        http.begin(client,full_url);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(data);
        if (String(httpResponseCode) == "200"){
          Serial.println("Datos enviados");
          Serial.println(" ");
        }
        http.end();
    }
    else {
        Serial.println("WiFi Desconectado");
    }
    delay(segundos*1000);
}
