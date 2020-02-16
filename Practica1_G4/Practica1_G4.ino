#include <LedControl.h>
#include "morse.h"

const int DIN_PIN = 7;
const int CS_PIN = 6;
const int CLK_PIN = 5;

LedControl display = LedControl(DIN_PIN, CLK_PIN, CS_PIN);

void setup(){
  display.clearDisplay(0);
  display.shutdown(0, false);
  display.setIntensity(0, 10);
}

void loop(){
  for(int i = 0; i < 26; i++){
    displayImage(MatrixLetters[i]);
    delay(300);
  }
  for(int i = 0; i < 10; i++){
    displayImage(MatrixNumbers[i]);
    delay(300);
  }
}

void displayImage(uint64_t image){
  for(int i = 0; i < 8; i++){
    byte row = (image >> i * 8) & 0xFF;
    for(int j = 0; j < 8; j++){
      display.setLed(0,i,j,bitRead(row,j));
    }
  }
}
