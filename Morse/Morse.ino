#include <LedControl.h>
#include <SoftwareSerial.h>
#include "letras.h"
#include "extras.h"

const int DIN = 10;
const int CS =  9;
const int CLK = 8;

const int btnCambio = 2;
const int btnMorse = 4;
const int btnEnvio = 6;

//const int buzzer = 38;
LedControl lc = LedControl(DIN, CLK, CS, 0);

String cadena;
String messageSend;
boolean estado = false;

//CONTROLES DEL BOTON
int buttonState = 0;     // current state of the button
int lastButtonState = 0; // previous state of the button
int startPressed = 0;    // the time button was pressed
int endPressed = 0;      // the time button was released
int timeHold = 0;        // the time button was hold
int timeReleased = 0;    // the time button was released

//CONTROLES DE CAMBIO
int buttonStateChange = 0;     // current state of the button
int lastButtonStateChange = 0; // previous state of the button
int startPressedChange = 0;    // the time button was pressed
int endPressedChange = 0;      // the time button was released
int timeHoldChange = 0;        // the time button was hold
int timeReleasedChange = 0;    // the time button was released

//Morse controller
short messageMorse[5] = {0,0,0,0,0};
int contador = 0;
boolean termino = false;

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 10);
  lc.clearDisplay(0);
  Serial.begin(9600);
  //Serial1.begin(115200);
  pinMode(buzzer, OUTPUT);
  pinMode(btnMorse, INPUT);
  pinMode(btnCambio, INPUT);
  pinMode(btnEnvio, INPUT);
  Serial.println("Init");
  //cancion();
}

void loop() {
  cadena = "HOLA GRUPO 4";
  menu();
}

void menu(){
  leerBoton();
  if(!estado){
    //cadena = "HOLA GRUPO 4";
    //mensajeInicial(); 
    Serial.println("Jugando");
    cadena = "ABCDEFG";
    jugar(cadena);
    Serial.println("Fin");
  }
  /*else if(Serial1.available()){
    String data = Serial1.readString();
    Serial.println(data);
    if(data.charAt(0) == '1'){
      estado = false;
      cadena = data.substring(1);
      imprimirCadena();
    }
    else{
      jugar(data);
    }
  }*/
  else{
    leerMorse();
  }
}

boolean leerBoton(){
  if(digitalRead(btnCambio) == HIGH){
    delay(150);
    if(digitalRead(btnCambio) == HIGH){
      if(!estado){
        lc.clearDisplay(0);
        messageSend = "";
      }
      estado = !estado;
      Serial.println("Press");
      return true;
    }
  }
  if(Serial1.available()){
    estado = true;
    return true; 
  }
  return false;
}

void leerMorse(){
  buttonState = digitalRead(btnMorse); // Leer el boton
  if (buttonState != lastButtonState) { // Verificar si el estado del boton cambio
    updateState();
  }
  lastButtonState = buttonState;        // Guardar el estado actual del boton

  buttonStateChange = digitalRead(btnEnvio);
  if(buttonStateChange != lastButtonStateChange){
    updateStateChange();
  }
  lastButtonStateChange = buttonStateChange;
}

void leerMorseJugando(){
  buttonState = digitalRead(btnMorse); // Leer el boton
  if (buttonState != lastButtonState) { // Verificar si el estado del boton cambio
    updateState();
  }
  lastButtonState = buttonState;        // Guardar el estado actual del boton
  buttonStateChange = digitalRead(btnEnvio);
  if(buttonStateChange != lastButtonStateChange){
    updateStateChangeMorse();
  }
  lastButtonStateChange = buttonStateChange;
}

void updateState(){
  if(buttonState == HIGH){
    startPressed = millis();
    timeReleased = startPressed - endPressed;
  }
  else{
    endPressed = millis();
    timeHold = endPressed - startPressed;
    if(timeHold < 250){
      Serial.println("Punto");
      messageMorse[contador] = 1;
      buzz(buzzer, 1200, 700 / 16);
    }
    else{
      Serial.println("Barra");
      messageMorse[contador] = 2;
      buzz(buzzer, 1200, 700 / 2);
    }
    startPressed = millis();
    contador = contador == 4 ? 0 : contador + 1;
  }
}

void updateStateChange(){
  if(buttonStateChange == HIGH){
    startPressedChange = millis();
    timeReleasedChange = startPressedChange - endPressedChange;
  }
  else{
    endPressedChange = millis();
    timeHoldChange = endPressedChange - startPressedChange;
    if(timeHoldChange < 400){
      convertMorseToAscii();
      Serial.println(messageSend);
    }
    else{
      convertMorseToAscii();
      Serial.println(messageSend);
      Serial1.println(messageSend);
      messageSend = "";
      estado = false;
    }
    cleanMorse();
    contador = 0;
  }
}

void updateStateChangeMorse(){
  if(buttonStateChange == HIGH){
    startPressedChange = millis();
    timeReleasedChange = startPressedChange - endPressedChange;
  }
  else{
    endPressedChange = millis();
    timeHoldChange = endPressedChange - startPressedChange;
    contador = 0;
    termino = true;
  }
}

void jugar(String message){
  int puntaje = 0;
  for(int i = 0; i < message.length(); i++){
    int ascii = cadena.charAt(i);
    int a = -1;
    boolean isletter = true;
    if (ascii == 32) {
      a = 26;
    } else if (65 <= ascii && ascii <= 90) {
      a = ascii - 65;
    } else if (48 <= ascii && ascii <= 57) {
      a = ascii - 48;
      isletter = false;
    } else{
      continue;
    }

    if(isletter){
      displayArray(letrasNormales[a]);
      jugando(letrasMorse[a],&puntaje);
    }
    else{
      displayArray(numerosNormales[a]);
      jugando(numeroMorse[a],&puntaje);
    }
    displayArray(letrasNormales[26]);
  }
  //Enviar puntaje a Server
  Serial.println(puntaje);
}

void jugando(short correcto[5], int *puntaje){
  startPressed = millis();
  endPressed = millis();
  while (!termino && endPressed - startPressed < 8000)
  {
    leerMorseJugando();
    endPressed = millis();
  }
  if(compare(correcto,messageMorse)){
    *puntaje = *puntaje + 1;
  }
  Serial.println(*puntaje);
  termino = false;
  contador = 0;
  cleanMorse();
}

boolean imprimirMorse(byte arreglo[][8], int a, short morse[][5]){
  for(int j = 0; j < 17; j++){
    if(j < 8){
      displayBin();
      desplazar(arreglo[a][j]);
    }
    else{
      displayBin();
      desplazar(B00000000);
    }
    if(j == 8){
      delay(150);
    }
    if(leerBoton())
      return true;
    delay(25);

    for(int m = 0; m < 5; m++){
      if(morse[a][m] == 1){ //Punto
        for(int j = 0; j < 17; j++){
          if(j < 8){
            displayBin();
            desplazar(punto[j]);
          }
          else
          {
            displayBin();
            desplazar(B00000000);            
          }
          if( j == 8){
            buzz(buzzer,1200,700/4);
          }
          if(leerBoton()){
            return true;
          }
          delay(25);
        }
      } 
      else if(morse[a][m] == 2){ //Barra
        for(int j = 0; j < 17; j++){
          if(j < 8){
            displayBin();
            desplazar(guiyon[j]);
          }
          else
          {
            displayBin();
            desplazar(B00000000);
          }
          if(j == 8){
            buzz(buzzer,1200,600);
          }
          if(leerBoton()){
            return true;
          }
          delay(25);
        }
      }
    }
  }
  return false;
}

void imprimirCadena() {
  for (int i = 0; i < cadena.length(); i++) {
    int ascii = (int) cadena.charAt(i);
    int a = -1;
    boolean isletter = true;
    if (ascii == 32) {
      a = 26;
    } else if (65 <= ascii && ascii <= 90) {
      a = ascii - 65;
    } else if (48 <= ascii && ascii <= 57) {
      a = ascii - 48;
      isletter = false;
    } else{
      continue;
    }
    
    if(isletter){
      if(imprimirMorse(letrasNormales,a,letrasMorse))
        return;
    }
    else{
      if(imprimirMorse(numerosNormales,a,numeroMorse))
        return;
    }
    for (int i = 0; i < 8; i++) {
      desplazamientoHor[i] = B00000000;
    }
  }
}

boolean imprimirInicial(byte arreglo[][8], int a, short morse[][5]){
  for(int j = 0; j < 17; j++){
    if(j < 8){
      displayBin();
      desplazar(arreglo[a][j]);
    }
    else{
      displayBin();
      desplazar(B00000000);
    }
    if(j == 8){
      for(int m = 0; m < 5; m++){
        if(morse[a][m] == 1){
           buzz(buzzer,1200,700/4);
           delay(250);
        }
        else if(morse[a][m] == 2){
          buzz(buzzer,1200,600);
          delay(250);
        }
        if(leerBoton()){
            return true;
        }
      }
    }
    if(leerBoton()){
      return true;
    }
    delay(25);
  }
  return false;
}

void mensajeInicial(){
  for (int i = 0; i < cadena.length(); i++) {
    int ascii = (int) cadena.charAt(i);
    int a = -1;
    boolean isletter = true;
    if (ascii == 32) {
      a = 26;
    } else if (65 <= ascii && ascii <= 90) {
      a = ascii - 65;
    } else if (97 <= ascii && ascii <= 122) {
      a = ascii - 70;
    } else if (48 <= ascii && ascii <= 57) {
      a = ascii - 48;
      isletter = false;
    } else{
      continue;
    }

    if(isletter){
      if(imprimirInicial(letrasNormales,a,letrasMorse))
        return;
    }
    else{
      if(imprimirInicial(numerosNormales,a,numeroMorse))
        return;
    }
    for (int i = 0; i < 8; i++) {
      desplazamientoHor[i] = B00000000;
    }
  }
}

void buzz(int targetPin, long frequency, long length) {
  digitalWrite(buzzer, HIGH);
  long delayValue = 1000000 / frequency / 2;
  long numCycles = frequency * length / 1000;
  int x = millis();
  for (long i = 0; i < numCycles; i++) {
    digitalWrite(targetPin, HIGH);
    delayMicroseconds(delayValue);
    digitalWrite(targetPin, LOW);
    delayMicroseconds(delayValue);
  }
  digitalWrite(buzzer, LOW);
}

void displayBin(){
  for (int x = 8; x >= 0; x--) {
    lc.setRow(0, x, desplazamientoHor[x]);
  }
}

void displayArray(byte ascii[8]){
  for(size_t x = 0; x < 8; x++){
    lc.setRow(0,x,desplazamientoHor[x]);
  }
}

void desplazar(byte meter) {
  for (int i = 0; i < 8; i++) {
    if (i == 7) {
      desplazamientoHor[7] = meter;
    } else {
      desplazamientoHor[i] = desplazamientoHor[i + 1];
    }
  }
}

void cleanMorse(){
  for(size_t i = 0; i < 5; i++){
    messageMorse[i] = 0;
  }
}

void convertMorseToAscii(){
  for(int i = 0; i < 26; i++){
    if(compare(messageMorse,letrasMorse[i])){
      messageSend += (char)(i + 65);
      return;
    }
  }
  for(int i = 0; i < 10; i++){
    if(compare(messageMorse,numeroMorse[i])){
      messageSend += (char)(i + 48);
      return;
    }
  }
}

boolean compare(short cadena1[5], short cadena2[5]){
  for(size_t i = 0; i < 5; i++){
    if(cadena1[i] != cadena2[i])
      return false;
  }
  return true;
}
