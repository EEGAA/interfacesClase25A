#include <Arduino.h>

#include <FreeRTOS.h>
#include <task.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>  //By lacemra 3.1.0 con modificaciones
#include <AsyncWebSocket.h>
#include <ArduinoJson.h>  //BenoitBlanchon
#include <time.h>
#include <ESP32Servo.h>

#define blinkPin 43

bool heartbeatRecived = false;
unsigned long startTime, lastTime = 0, myTime = 1000;
// Configura tus credenciales de WiFi
/*
char ssid[100]     = "GWN571D04";
char password[100] = "ESP32CUCEI$$";

char ssid[100]     = "POCO";
char password[100] = "PruebaESP32";

*/
char ssid[100]     = "A2501";
char password[100] = "A_2501//";


//Configuracion del servidor y websocket
AsyncServer server(80);
AsyncWebSocket ws("/ws");

void onWebSocketMessage(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
  if(type == WS_EVT_DATA){
    Serial.println("Aqui va todo el proceso de la lectura de los Json");
  }else if(type == WS_EVT_CONNECT)
    Serial.println("Cliente conectado.");
  else if(type == WS_EVT_DISCONNECT)
    Serial.println("Cliente conectado");
  
}
void sendMsgsJson(AsyncWebSocketClient *client){
  if(heartbeatRecived){
    Serial.println("Enviar mensaje Json");
  }
}

void wsMsgsTask(void *param){
  while(true){
    //manten limpios los clientes inactios del ws
    ws.cleanupClients();

    //Verifica si se ha recibido un heartbeat y envia lecturas cada 100 ms
    if(heartbeatRecived && (millis() - lastTime >= myTime)){
      for(AsyncWebSocketClient *client : ws.getClients()){
        if(client->status() == WS_CONNECTED){
          ///FUNCION QUE ENVIA LOS DATOS
          Serial.print("Enviar mensaje json ws");
        }
      }
      lastTime = millis();
    }
  }
}
void accionTask(void *param){
  long int ta = millis();
    long int tb = 0;
    bool estadoLed = false;
    //aqui
    while (1) {
        tb = millis();
        if((tb - ta) > myTime*2){
          digitalWrite(blinkPin, estadoLed);
          estadoLed = !estadoLed;
          ta = millis();
        }
      }
}

void setup(){
  Serial.begin(115200);
  pinMode(blinkPin, OUTPUT);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(2000);
    Serial.println("Conectando a red WiFi...");
  }
  Serial.println("Conetado a la red...   :)");
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());

  //configuracion ws

  server.begin();
  Serial.println("Servidor WebSocket inciado.");
  startTime = millis();

  xTaskCreatePinnedToCore(
    wsMsgsTask,      // funcion de la tarea
    "Task msgs", // nombre de la tarea
    4096,        // tama;o de la pila
    NULL,        // parametro de entrada
    1,           // prioridad
    NULL,        // puntero de la tarea
    0);          // ejecutar en el core 0
    xTaskCreatePinnedToCore(
    accionTask,    // funcion de la tarea
    "Task accion", // nombre de la tarea
    2048,          // tama;o de la pila
    NULL,          // parametro de entrada
    1,             // prioridad
    NULL,          // puntero de la tarea
    1);            // ejecutar en el core 0
}
void loop(){}