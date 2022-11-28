/* Manual - Conecte un TTGO-T-Beam a DotQore a través de WiFi
Programa que permite enviar los siguientes datos a Dotqore
Latitude  : 2.45172
Longitude : -76.5908
Satellites: 4
Altitude  : 1852.72M

2022-11-28
*/

// Librerias  
#include <WiFi.h>
#include <HTTPClient.h>
#include <TinyGPS++.h>
#include <axp20x.h>

// Config gps
TinyGPSPlus gps;
HardwareSerial GPS(1);
AXP20X_Class axp;

// Variables de DotQore
String nodo_colector = "REEMPLAZA_CON_URL_NODO_COLECTOR";
String id_ref = "REEMPLAZA_CON_ID_DE_REFERENCIA";

float lat; // Variable a medir
float lon; // Variable a medir
uint32_t sat; // Variable a medir
float alt; // Variable a medir
String dot; // Mensaje a enviar

// Credenciales de WiFi
const char* ssid = "REEMPLAZA_CON_TU_SSID";
const char* password = "REEMPLAZA_CON_TU_CONTRASEÑA";

// Tiempo de muestreo
int segundos = 60;
unsigned long lastTime = 0;
unsigned long timerDelay = segundos*1000;

void setup()
{
  Serial.begin(115200);
  Serial.println("--- Bienvenido a DotQore ---");
  //---GPS---
  Wire.begin(21, 22);
  if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS)) {
    Serial.println("AXP192 Funcionando");
  } else {
    Serial.println("AXP192 Fallido");
  }
  axp.setPowerOutPut(AXP192_LDO2, AXP202_ON);
  axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
  axp.setPowerOutPut(AXP192_DCDC2, AXP202_ON);
  axp.setPowerOutPut(AXP192_EXTEN, AXP202_ON);
  axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON);
  GPS.begin(9600, SERIAL_8N1, 34, 12);   //17-TX 18-RX
  //--- ---
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
  Serial.println("Ubicando satélites");
}

void loop(){
  Serial.println("**********************");
  Serial.print("Latitud  : ");
  Serial.println(gps.location.lat(), 5);
  Serial.print("Longitud : ");
  Serial.println(gps.location.lng(), 4);
  Serial.print("Altitud  : ");
  Serial.print(gps.altitude.feet() / 3.2808);
  Serial.println("M");
  Serial.print("Satelites: ");
  Serial.println(gps.satellites.value());
  Serial.print("Tiempo      : ");
  Serial.print(gps.time.hour());
  Serial.print(":");
  Serial.print(gps.time.minute());
  Serial.print(":");
  Serial.println(gps.time.second());
  Serial.println("**********************");

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No se reciben datos GPS: compruebe el cableado"));
    
  if ((millis() - lastTime) > timerDelay) {
      if(WiFi.status()== WL_CONNECTED){
        WiFiClient client;
        HTTPClient http;
        String serverName = nodo_colector + "/dotqore/values";
        http.begin(client, serverName);           
        http.addHeader("Content-Type", "application/json");
        getDataGPS();
        if(lat != NULL && lon != NULL && alt !=  NULL) {
          Serial.println("Verificando GPS");
          dot = "{\"deviceId\": \"" + String(id_ref) + "\",\"values\": {\"lat\":" + String(lat)+ ",\"lon\":" + String(lon)+ ",\"alt\":" + String(alt)+ " }}";
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
      Serial.println("WiFi Desconectado");
    }
    lastTime = millis();
    }
}

void getDataGPS(){
  lat = gps.location.lat(), 5;
  lon = gps.location.lng(), 5;
  alt = gps.altitude.feet() / 3.2808;
}

 static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (GPS.available())
      gps.encode(GPS.read());
  } while (millis() - start < ms);
}
