#include <Arduino.h>

//#include <FreeRTOS.h>
//#include <task.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>  //By lacemra 3.1.0 con modificaciones
#include <AsyncWebSocket.h>
#include <ArduinoJson.h>  //BenoitBlanchon
#include <time.h>
#include <ESP32Servo.h>

#include "ultrasonico.hpp"
senUltra miUltrasonico = {7, 6};
#include "miPuenteH.hpp"
puenteH myMotors = {12, 11, 10, 9};
bool stateMotors = false;
unsigned long timeMotorsStart = 0, timeMotorsOn = 0;
int comandoAux;
int timeON[4] = {500, 500, 250, 250};//Timpo de uso en las 4 direcciones, milisegundos
//{adelante, atras, derecha, izquierda}
#include "dataDistacia.hpp"
dtaDIS myDta;
bool iniciarBusqueda = false;
int objBuscado = -1;
AsyncWebSocketClient* clientBusqueda = nullptr;

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


Servo miServo;
#define pinServo 19
//Configuracion del servidor y websocket
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
//Funcion para actualizar el tiempo de la ESP32
void updateESP32Time(unsigned long timestamp){
  timeval tv;
  tv.tv_sec = timestamp;
  tv.tv_usec = 0;
  settimeofday(&tv, nullptr); //actualizar el tiempo de la esp32
  Serial.println("Tiempor de la ESP32 actualizado.");  
}
void onWebSocketMessage(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
  if(type == WS_EVT_DATA){
    //Serial.println("Aqui va todo el proceso de la lectura de los Json");
    //Si hay mensaje
    Serial.print("Menssage length: ");
    Serial.println(len);//longitud del mensaje JSON
    if(len > 0){//El mensaje existe
      //convertir los datos en una cadena
      String message = String((char*)data);
      Serial.println("Mensaje recibido");

      //crear un objeto JSON para almacenar el mensajel
      DynamicJsonDocument doc(1024);
      //JsonDocument doc(1024);
      //Intentar deserializar el mensaje JSON
      DeserializationError error = deserializeJson(doc, message);
      //Si el mensaje es un JSON valido
      if(!error){
        Serial.print("El mensaje JSON recibido es valido. ");
        //comprobar si es un heartbeat
        String tipo = doc["type"].as<String>();
        if(tipo == "heartbeat"){
          //Obtener el timestamp del heartbeat
          unsigned long timestamp = doc["timestamp"];
          Serial.print("Heartbeat recibido con timestamp: ");
          Serial.println(timestamp);
          //actualizar el tiempo de la ESP32
          updateESP32Time(timestamp);
          //bandera de recibido
          heartbeatRecived = true;
          
        }//Aqui hay una seria de else if pues type puede ser cualquier cosa
        //Con estos else if existen las respuestas que el usuario busca desde Qt
        else if(tipo == "motor"){
          //En caso de que se usen los motores pasa:
          String comando = doc["comando"].as<String>();
          if(comando == "adelante"){
            Serial.println("Carrito adelante...");
            myMotors.adelante();
            comandoAux = 1;
          }else if(comando == "atras"){
            Serial.println("Carrito atras...");
            myMotors.atras();
            comandoAux = 2;
          }else if(comando == "derecha"){
            Serial.println("Carrito derecha...");
            myMotors.derecha();
            comandoAux = 3;
          }else if(comando == "izquierda"){
            Serial.println("Carrito izquierda...");
            myMotors.izquierda();
            comandoAux = 4;
          }else if(comando == "stop"){
            Serial.println("Carrito parado");
            myMotors.mtrStop();
            //comando == 0;
          }
          timeMotorsStart = millis();
          stateMotors = true;
        }else if(tipo == "servo"){
          int angulo = doc["angulo"];
          Serial.print("Moviendo servo al angulo: ");
          Serial.println(angulo);
          miServo.write(angulo);
        }else if(tipo == "ultrasonico"){
          float distancia = miUltrasonico.getDistancia();
          Serial.print("Distancia medida: ");
          Serial.print(distancia);
          Serial.println("cm");
          //Crear objeto JSON para enviar las mediciones
          DynamicJsonDocument doc(256);
          doc["type"] = "medicionDistancia";
          doc["distancia"] = distancia;
          //Serializar el JSON y enviarlo
          String jsonString;
          serializeJson(doc, jsonString);
          if(client->status() == WS_CONNECTED && client->canSend()){
            client->text(jsonString);
          }else{
            Serial.println("Cliente no disponible.");
          }
          
          //Imprimir el JSON enviado
          Serial.print("Enviando datos Ultrasonico ");
          Serial.println(jsonString);
        }else if(tipo == "busqueda"){
          if(doc.containsKey("objeto")){
            objBuscado = doc["objeto"];
            clientBusqueda = client;
            iniciarBusqueda = true;
            Serial.println("Solicitud de busqueda recibida");
          }
        }else if(tipo = "setTimeMotors"){
          int adelanteTime = doc["adelante"].as<int>();
          int atrasTime = doc["atras"].as<int>();
          int lateralesTime = doc["laterales"].as<int>();
          if(adelanteTime > 0 && atrasTime > 0 && lateralesTime > 0){
            timeON[0] = adelanteTime;
            timeON[1] = atrasTime;
            timeON[2] = lateralesTime;
            timeON[3] = timeON[2];
          }else{
            timeON[0] = 500;
            timeON[1] = 500;
            timeON[2] = 250;
            timeON[3] = timeON[2];
          }
        }
      }else{
        //Si el mensaje no es un JSON valido
        Serial.println("El mensaje JSON recibido es invalido ;/");
        
      }
    }else{
      Serial.println("No hay datos recibos");
    }
  
  }else if(type == WS_EVT_CONNECT)
    Serial.println("Cliente conectado.");
  else if(type == WS_EVT_DISCONNECT)
    Serial.println("Cliente desconectado");
  
}
void sendMsgsJson(AsyncWebSocketClient *client){
  if(heartbeatRecived){
    //Originalmente aqui se media la temperatura.
    Serial.println("Enviar mensaje Json");
    //Crear objeto JSON
    DynamicJsonDocument doc(1024);
    doc["type"] = "generica";
    doc["error"] = "niniguno";
    
    //Serializar el JSON y enviarlo
    String jsonString;
    serializeJson(doc, jsonString);
    if(client->status() == WS_DISCONNECTED && client->canSend()){
      client->text(jsonString);
    }else{
      Serial.println("Cliente no disponible.");
    }
    //Imprimir los datos enviados
    Serial.print("Enviando datos: ");
    Serial.println(jsonString);
    Serial.println(" ");
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
          sendMsgsJson(client);
        }
      }
      lastTime = millis();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);//Retardo para no saturar el procesador
  }
}
void accionTask(void *param){
    while(true){
      if(stateMotors){
        timeMotorsOn = millis();
        if((timeMotorsOn - timeMotorsStart) > timeON[comandoAux-1]){
          myMotors.mtrStop();
          Serial.println("Carrito detenido.");
          stateMotors = false;
        }
      }
      vTaskDelay(10 / portTICK_PERIOD_MS);//Retardo para no saturar el procesador
    }
}
void busquedaTask(void *param){
  while(true){
    if(iniciarBusqueda && clientBusqueda != nullptr){
      for(int i=0; i <= 180; i++){
        miServo.write(i);
        float d = miUltrasonico.getDistancia();
        myDta.setDTA(i, d);
        delay(20);
      }

      int aquiEsta = myDta.busqueda(objBuscado);
      float busDis = myDta.getDTA(aquiEsta);
      miServo.write(aquiEsta);

      DynamicJsonDocument response(256);
      response["type"] = "medicionDistancia";
      response["distancia"] = busDis;
      String jsonString;
      serializeJson(response, jsonString);

      if(clientBusqueda->status() == WS_CONNECTED && clientBusqueda->canSend()){
        clientBusqueda->text(jsonString);
      }

      Serial.println("Búsqueda completada y respuesta enviada.");
      iniciarBusqueda = false;
      clientBusqueda = nullptr;
    }
    vTaskDelay(20 / portTICK_PERIOD_MS); // Esperar entre chequeos
  }
}
void setup(){
  Serial.begin(115200);
  Serial.println("\n\nINICIANDO ESP32");
  WiFi.begin(ssid, password);
  Serial.print("\nConetado a la red: ");
  Serial.println(ssid);
  int cont = 0;
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print("..");
    if(cont==10)
      ESP.restart();
    cont++;
  }
  Serial.print("\nCONECTADO!!!\nIP asignada: ");
  Serial.println(WiFi.localIP());

  //configuracion ws
    ws.onEvent(onWebSocketMessage);
    server.addHandler(&ws);

  server.begin();
  Serial.println("Servidor WebSocket inciado.");
  startTime = millis();

  //configuracion del servo
  miServo.setPeriodHertz(50);
  miServo.attach(pinServo, 500, 2400);
  miServo.write(90);

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
    4096,          // tama;o de la pila
    NULL,          // parametro de entrada
    1,             // prioridad
    NULL,          // puntero de la tarea
    1);            // ejecutar en el core 0
    xTaskCreatePinnedToCore(
    busquedaTask,
    "tarea de busqueda",
    4096,
    NULL,
    1,
    NULL,
    1);
}
void loop(){}