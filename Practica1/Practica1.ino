#include <LedControl.h>
#include <FrequencyTimer2.h> //Libreria que pinta en la matriz sin driver
#include "letras.h"

LedControl lc = LedControl(18, 19, 20); //Controlador de la matriz con Driver

#define numCaracteres (17) //Numero de caracteres del mensaje inicial
#define btn0 A0 // Boton Inicio / Fin / Pausa
#define btn1 A1 // Boton Izquierda
#define btn2 A2 // Boton Derecha
#define btn3 A3 // Boton Rotar
#define pot A8  // Entrada Analogica del potenciometro

//VARIABLES Y CONSTANTES PARA EL MENSAJE INICIAL
byte col = 0;

byte leds[8][16];

//Total de pines usados en la matriz
int pins[16] = {5, 4, 3, 2, 14, 15, 16, 17, 13, 12, 11, 10, 9, 8, 7, 6};

//Para prueba en Protues
//int cols[8] = {13, 12, 11, 10, 9, 8, 7, 6};
//int rows[8] = {5, 4, 3, 2, 14, 15, 16, 17};

//Pines finales
int cols[8] = {2, 11, 10, 16, 8, 15, 4, 5};
int rows[8] = {17, 3, 6, 14, 13, 7, 12, 9};

//Mensaje Inicial
byte Caracter[numCaracteres][8][8] = {
  SPACE, A, R, Q, U, I, UNO, LINE, G, R, U, P, O, LINE, UNO, CINCO, B
};

//Array de numeros para el puntaje
byte Numeros[10][8][8] = {
  CERO, UNO, DOS, TRES, CUATRO, CINCO, SEIS, SIETE, OCHO, NUEVE
};

int contCaracter = 0;
int contRow = 0;

//CONTROLES DEL BOTON
int buttonState = 0;     // current state of the button
int lastButtonState = 0; // previous state of the button
int startPressed = 0;    // the time button was pressed
int endPressed = 0;      // the time button was released
int timeHold = 0;        // the time button was hold
int timeReleased = 0;    // the time button was released

//Estados del juego Estado 0: Sin jugar, Estado 1: Jugando, isPaussed 0: Jugando isPaussed 1: En pausa
int estado = 0;
int isPaussed = 0;
bool gameover = false;

//TETRIS
int field[18][11] = {};
int stage[18][11] = {};

//Auxiliares
int pause1 = 0;
byte col1 = 0;
int y = 0;
int x = 4;
int puntaje = 0;
long valor; //Guarda el valor ingresado del potenciometro de 0 - 1024

int wait = 300; //Variable que guarda el tiempo de espera moderado por el potenciometro

unsigned long startTime;
unsigned long currentTime;

void setup() {
  Serial.begin(9600); //Puerto serial
  randomSeed(analogRead(0)); //Generador de numeros aleatorios

  //Driver matriz 1
  lc.shutdown(0, false);
  lc.setIntensity(0, 5);
  lc.clearDisplay(0);

  //Definir los pines de salida Matriz 2
  for (int i = 0; i < 16; i++) {
    pinMode(pins[i], OUTPUT);
  }

  //Apagar la matriz 2
  for (int i = 0; i < 8; i++) {
    digitalWrite(cols[i], LOW);
    digitalWrite(rows[i], LOW);
  }

  clearLeds();

  //Configurar el FrequencyTimer2 que pinta en la matriz 2
  FrequencyTimer2::disable();
  FrequencyTimer2::setPeriod(2000);
  FrequencyTimer2::setOnOverflow(display); //Aqui se define el metodo que estara llamando constantemente para pintar la matriz 2

  setFirst(contCaracter);

  //Definir los pines de los botones como entradas
  pinMode(btn0, INPUT);
  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT);
  pinMode(btn3, INPUT);

  startTime = millis();
}

void loop() {
  btn3segundos(); //Verifica el estado del boton inicio/fin/pausa
  menu(wait); //funciones del juego
  velocidad(); //lee el potenciometro
}

void menu(int del) {
  if (!estado) {
    FrequencyTimer2::setOnOverflow(display);
    changeChar();
    desplazar(contCaracter, del);
  }
  else {
    jugando();
  }
}

void jugando() {
  if (!isPaussed) {
    currentTime = millis();
          userInput();
    if(currentTime - startTime > wait){
     setFirst(0);
    FrequencyTimer2::setOnOverflow(displayP);
    if (!gameover) {
      gameLoop();
      if (puntaje >= 10) {
        gano();
      }
    }
    else {
      gameover = false;
      estado = 0;
      puntaje = 0;
      gameOver();
    }
    velocidad();
    startTime = currentTime; 
    }
  }
  else {
    if (pause1)
      pausado();
  }
}

//Pinta la matriz 2 cuando se termina el juego
void FF() {
  digitalWrite(cols[col1], LOW);
  col1++;
  if (col1 == 8) {
    col1 = 0;
  }
  for (int row = 0; row < 8; row++) {
    digitalWrite(rows[row], HIGH);
  }
  digitalWrite(cols[col1], HIGH);
}

void pausado() {
  FrequencyTimer2::setOnOverflow(0);
  pause1 = 0;
  for (int j = 0; j < 8; j++) {
    digitalWrite(cols[j], LOW);
    digitalWrite(rows[j], HIGH);
  }
  setPuntaje();
  displayDriver();
}

//Si gano pinta una G y regresa al mensaje inicial
void gano() {
  gameover = false;
  estado = 0;
  FrequencyTimer2::setOnOverflow(0);
  pause1 = 0;
  for (int j = 0; j < 8; j++) {
    digitalWrite(cols[j], LOW);
    digitalWrite(rows[j], HIGH);
  }
  for (int i = 0; i < 8; i++) {
    for (int j = 8; j < 16; j++) {
      leds[i][j] = Caracter[8][i][j - 8];
    }
  }
  displayDriver();
  puntaje = 0;
  delay(3000);
}

//Cambia de letra del mensaje incial
void changeChar() {
  if (++contRow == 8) {
    contCaracter = ++contCaracter % numCaracteres;
    contRow = 0;
  }
}

//
void btn3segundos() {
  buttonState = digitalRead(btn0); // Leer el boton
  if (buttonState != lastButtonState) { // Verificar si el estado del boton cambio
    updateState();
  }
  lastButtonState = buttonState;        // Guardar el estado actual del boton
}

void updateState() {
  if (buttonState == HIGH) {
    startPressed = millis();
    timeReleased = startPressed - endPressed;
  }
  else {
    endPressed = millis();
    timeHold = endPressed - startPressed;
    if (timeHold < 2000) {
      if (estado) {
        isPaussed = !isPaussed;
        pause1 = 1;
      }
    }
    else {
      digitalWrite(21, LOW);
      estado = !estado;
      isPaussed = 0;
      if (estado) {
        puntaje = 0;
        clear();
        FrequencyTimer2::setOnOverflow(0);
        display();
        contCaracter = 0;
        setFirst(contCaracter);
        FrequencyTimer2::setOnOverflow(displayP);
        initGame();
      }
    }
  }
}

//limpia la matriz
void clearLeds() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 16; j++) {
      leds[i][j] = 0;
    }
  }
}

//Agrega el primer caracter
void setFirst(int contCaracter) {
  for (int i = 0; i < 8; i++) {
    for (int j = 8; j < 16; j++) {
      leds[i][j] = Caracter[contCaracter][i][j - 8];
    }
  }
}

//Agrega el puntaje si se presiono pausar
void setPuntaje() {
  for (int i = 0; i < 8; i++) {
    for (int j = 8; j < 16; j++) {
      leds[i][j] = Numeros[puntaje][i][j - 8];
    }
  }
}

//Desplaza la matriz verticalmente
void desplazar(int contCaracter, int del) {
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 8; j++) {
      leds[j][i] = leds[j][i + 1];
    }
  }
  for (int j = 0; j < 8; j++) {
    leds[j][15] = Caracter[contCaracter][j][contRow];
  }
  displayDriver();
  delay(del);
}

//Metodo que pinta en la matriz del driver el mensaje inicial
void displayDriver() {
  for (int col = 0; col < 8; col++) {
    for (int row = 0; row < 8; row++) {
      if (leds[col][15 - row] == 1) {
        lc.setLed(0, 7 - col, row, true);
      }
      else {
        lc.setLed(0, 7 - col, row, false);
      }
    }
  }
}

//Metodo que pinta en la matriz 2 el mensaje inicial
void display() {
  digitalWrite(rows[7 - col], HIGH);
  col++;
  if (col == 8) {
    col = 0;
  }
  for (int row = 0; row < 8; row++) {
    if (leds[col][7 - row] == 1) {
      digitalWrite(cols[row], HIGH);
    }
    else {
      digitalWrite(cols[row], LOW);
    }
  }
  digitalWrite(rows[7 - col], LOW);
}

//Limpia la matriz del driver
void clear() {
  clearLeds();
  for (int col = 0; col < 8; col++) {
    for (int row = 0; row < 8; row++) {
      lc.setLed(0, row, col, false);
    }
  }
}

//Lee la entrada del potenciometro y varia la velocidad segun el caso
void velocidad() {
  valor = analogRead(pot);
  if (valor > 750) {
    wait = 50;
  }
  else if (valor > 500) {
    wait = 100;
  }
  else if (valor > 250) {
    wait = 200;
  }
  else {
    wait = 350;
  }
}

//Mensaje de game over
int gameOver()
{
  col = 8;
  FrequencyTimer2::setOnOverflow(FF);
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, B11111111);
      delay(wait);
    }
    lc.clearDisplay(0);
  }
  return 0;
}

//Ciclo del juego
void gameLoop()
{
  spawnBlock();
  //seElimina();
  displayM();
}

//Metodo que pinta en la matriz del driver el juego
void displayM()
{
  for (size_t i = 0; i < 8; i++)
  {
    for (size_t j = 0; j < 8; j++)
    {
      //TODO: pintar matriz
      if (field[i][j + 1] == 0) {
        lc.setLed(0, j, i, false);
      }
      else {
        lc.setLed(0, j, i, true);
      }
    }
  }
}

//Metodo que pinta en la matriz 2 el juego
void displayP() {
  digitalWrite(cols[col1], LOW);
  col1++;
  if (col1 == 8) {
    col1 = 0;
  }
  for (int row = 0; row < 8; row++) {
    if (field[col1 + 8][row + 1] == 1) {
      digitalWrite(rows[row], LOW);
    }
    else {
      digitalWrite(rows[row], HIGH);
    }
  }
  digitalWrite(cols[col1], HIGH);
}

//Inicia un nuevo juego
void initGame()
{
  for (size_t i = 0; i <= 16; i++)
  {
    for (size_t j = 0; j <= 9; j++)
    {
      if ((j == 0) || (j == 9) || (i == 16))
      {
        field[i][j] = stage[i][j] = 9;
      }
      else
      {
        field[i][j] = stage[i][j] = 0;
      }
    }
  }
  crearBlocks();
  displayM();
}

//Crea un nuevo bloque y lo agrega
bool crearBlocks()
{
  x = 4;
  y = 0;
  int tipo = random(7);
  for (size_t i = 0; i < 4; i++)
  {
    for (size_t j = 0; j < 4; j++)
    {
      block[i][j] = 0;
      block[i][j] = bloques[tipo][i][j];
    }
  }
  for (size_t i = 0; i < 4; i++)
  {
    for (size_t j = 0; j < 4; j++)
    {
      field[i][j + 4] = stage[i][j + 4] + block[i][j];
      if (field[i][j + 4] > 1)
      {
        gameover = true;
        return true;
      }
    }
  }
  return false;
}

//Mover bloque hacia abajo
void moveBlock(int x2, int y2)
{
  //Remover bloque
  for (size_t i = 0; i < 4; i++)
  {
    for (size_t j = 0; j < 4; j++)
    {
      field[y + i][x + j] -= block[i][j];
    }
  }
  //actualizar coordenadas
  x = x2;
  y = y2;
  for (size_t i = 0; i < 4; i++)
  {
    for (size_t j = 0; j < 4; j++)
    {
      field[y + i][x + j] += block[i][j];
    }
  }
  displayM();
}

//Unir el bloque a la matriz principal del juego
void choque()
{
  for (size_t i = 0; i < 17; i++)
  {
    for (size_t j = 0; j < 10; j++)
    {
      stage[i][j] = field[i][j];
    }
  }
}

//Verifica si chocaron los bloques
bool esChoque(int x2, int y2)
{
  for (size_t i = 0; i < 4; i++)
  {
    for (size_t j = 0; j < 4; j++)
    {
      if (block[i][j] && stage[y2 + i][x2 + j] != 0)
      {
        return true;
      }
    }
  }
  return false;
}

//Botones del juego
void userInput()
{
  if (digitalRead(btn1) == HIGH)
  {
    delay(50);
    if (digitalRead(btn1) == HIGH)
    if (!esChoque(x + 1, y))
      moveBlock(x + 1, y);
  }
  if (digitalRead(btn2) == HIGH)
  {
        delay(50);
    if (digitalRead(btn2) == HIGH)
    if (!esChoque(x - 1, y)) {
      moveBlock(x - 1, y);
    }
  }
  if (digitalRead(btn3) == HIGH)
  {
        delay(50);
    if (digitalRead(btn3) == HIGH)
    rotarBloque();
  }
}

//Rotacion de un bloque
bool rotarBloque()
{
  int tmp[4][4] = {};
  for (size_t i = 0; i < 4; i++)
  {
    for (size_t j = 0; j < 4; j++)
    {
      tmp[i][j] = block[i][j];
    }
  }
  for (size_t i = 0; i < 4; i++)
  {
    for (size_t j = 0; j < 4; j++)
    {
      block[i][j] = tmp[3 - j][i];
    }
  }
  if (esChoque(x, y))
  {
    for (size_t i = 0; i < 4; i++)
    {
      for (size_t j = 0; j < 4; j++)
      {
        block[i][j] = tmp[i][j];
      }
    }
    return true;
  }
  for (size_t i = 0; i < 4; i++)
  {
    for (size_t j = 0; j < 4; j++)
    {
      field[y + i][x + j] -= tmp[i][j];
      field[y + i][x + j] += block[i][j];
    }
  }
  displayM();
  return false;
}

void spawnBlock()
{
  if (!esChoque(x, y + 1))
  {
    moveBlock(x, y + 1);
  }
  else
  {
    seElimina();
    choque();
    crearBlocks();
    displayM();
  }
}

//Verifica si se completo alguna linea y suma puntos
void seElimina()
{
  for (size_t i = 0; i < 17; i++)
  {
    size_t j = 1;
    if (lineaLlena(i, j))
    {
      eliminarFila(i);
      puntaje++;
    }
  }
}

//Elimina la fila y desplaza todo hacia abajo
void eliminarFila(size_t x)
{
  for (size_t i = x; i > 1; i--)
  {
    for (size_t j = 8; j > 0; j--)
    {
      field[i][j] = field[i - 1][j];
    }
  }
}

//Retorna true si se completo toda una fila
bool lineaLlena(size_t x, size_t y)
{
  if (y == 9)
  {
    return true;
  }
  if (field[x][y] == 1)
  {
    return lineaLlena(x, y + 1);
  }
  else
  {
    return false;
  }
}
