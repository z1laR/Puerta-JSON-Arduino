#include <Ethernet.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "RestClient.h"

#define IP "192.168.0.16"
#define PORT 3000

int estado = 0;
int salida = 0;
int estado_anterior = 0;

int alarma = 0;

String responsePuerta;

int led = 3;
int push = 2;
int puerta = 13;
int buzzer = 8;

RestClient client = RestClient(IP, PORT);

void setup()
{
  pinMode(led, OUTPUT);
    
  Serial.begin(9600);
  
  Serial.println("connect to network");
  client.dhcp();

  Serial.print("IP de Placa Ethernet Shield ");
  Serial.println(Ethernet.localIP());
  
  Serial.println("Listo :D!");
}

void loop()
{
  Boton();
  Puerta();
}

int Boton()
{
  pinMode(push, INPUT);

  estado = digitalRead(push);

  if((estado == HIGH) && (estado_anterior == LOW))
  {
    salida = 1 - salida;
  }

  estado_anterior = estado;
  delay(30);

    if(salida == 1)
    {
      Serial.print("\n");
      Serial.println("Botón presionado");
      Serial.print("\n");
      Serial.println("Alarma ACTIVADA");
      Serial.print("\n");
      
      digitalWrite(led, HIGH);
    }
      else
        {
          Serial.print("\n");
          Serial.println("Botón presionado");
          Serial.print("\n");
          Serial.println("Alarma DESACTIVADA");
          Serial.print("\n");
          
          digitalWrite(led, LOW);
        }
}

int Puerta()
{
  int estadoP = 0;
  int salidaP = 0;
  int estado_anteriorP = 0;

  pinMode(puerta, INPUT);
  pinMode(buzzer, OUTPUT);

  estadoP = digitalRead(puerta);

  if((estadoP == HIGH) && (estado_anteriorP == LOW))
  {
    salidaP = 1 - salidaP;
  }

  estado_anteriorP = estadoP;

    if((estadoP == 1) && (salida == 1))
    {
      Serial.println("Puerta abierta");
      Serial.println("Apertura sospechosa");

      alarma = 1;

      Serial.print("Tono 16.35 Hz");
      Serial.print("\n");
      tone(8, 16.35);
      delay(500);
    }
      if((estadoP == 0) && (salida == 1) && (alarma == 1))
      {
        Serial.println("Puerta abierta");
        Serial.println("Cerrada sospechosamente");
      
        Serial.print("Tono 16.35 Hz");
        Serial.print("\n");
        tone(8, 16.35);
        delay(500);
      }
        if((estadoP == 0 || estadoP == 1) && (salida == 0) && (alarma == 1))
        {
          alarma = 0;
        }
          else
          {
            noTone(buzzer);
          }

  postPuerta(alarma);
}

void postPuerta(int alarma)
{
  responsePuerta = "";

  client.setHeader("Content-Type: application/json");
  
  StaticJsonBuffer<200> jsonBuffer;
  char json[256];  
  JsonObject& root = jsonBuffer.createObject();
  root["alarma"] = alarma;
  root.printTo(json, sizeof(json));
  Serial.println(json);
  
  int statusCode = client.post("http://localhost:3000/api/puerta/", json, &responsePuerta);
  
  Serial.print("Status code from server: ");
  Serial.println(statusCode);
  Serial.print("responsePuerta body from server: ");
  Serial.println(responsePuerta);
  
  delay(3000);
}
