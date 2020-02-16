#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <SoftwareSerial.h>

#ifndef STASSID
//#define STASSID "TURBONETT_9140EC"
//#define STAPSK  "DDECDD8434"
#define STASSID "iPhone"
#define STAPSK  "5678wifi"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const char* host = "arqui1g15bp2.appspot.com";
const int httpsPort = 443;

const char fingerprint[] PROGMEM = "56 C7 E7 1F EC 7F 3F 11 09 6F 68 52 78 98 63 AF B6 1F 32 71";

byte estado = 0;
String cadena = "";
int indice = 0;

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  /*if(Serial.available()>0){
    char data = Serial.read();
    if(data == '@'){
      estado = 0;
      Estado1();
    }
    else if(data == '&'){
      GetCadena();
    }
  }*/
  Estado1();
  //GetEstado();
  //RecibirCadena();
  //delay(1000);

}

void Estado1(){
  while(estado == 0){
    ReadAsm();
  }
  char first = cadena.charAt(0);
  if(first == '<'){
    enviarEcuacion(cadena);
    estado = 0;
    cadena = ""; 
  }
  else if(first == 'a'){
    Recibir3D();
  }
}

void ReadAsm(){
  if(Serial.available()>0){
      char data = Serial.read();
      if(data == '$'){
        estado = 1;
      }
      else{
        cadena = cadena + data;
      }
  }
}

void GetCadena(){
  WiFiClientSecure client;
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  String url = "/data";
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP8266\r\n");
  client.print("Connection: close\r\n\r\n");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    } 
  }
  String line = client.readStringUntil('\n');
  Serial.println(line);
}

void enviarEcuacion(String entrada){
  WiFiClientSecure client;
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  String url = "/procesar";
  
  StaticJsonDocument<700> jdoc;
  jdoc["text"] = entrada;
  
  client.print(String("POST ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP8266\r\n");
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  size_t len = measureJson(jdoc);
  client.println(len);
  client.print("Connection: close\r\n\r\n");
  serializeJson(jdoc, (Client&)client);
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    } 
  }
  String line = client.readStringUntil('\n');
  Serial.println(line);
}

void Recibir3D(){
  WiFiClientSecure client;
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  String url = "/status";
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP8266\r\n");
  client.print("Connection: close\r\n\r\n");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    } 
  }
  String line = client.readStringUntil('\n');
  Serial.println(line);
}

void RecibirCadena(){
    WiFiClientSecure client;
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  String url = "/getdata";
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP8266\r\n");
  client.print("Connection: close\r\n\r\n");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    } 
  }
  String line = client.readStringUntil('\n');
  Serial.println(line);
}
