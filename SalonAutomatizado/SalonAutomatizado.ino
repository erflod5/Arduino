const int trigPin = 13;
const int echoPin = 12;
const int led = 11;
int contador = 0;

     void setup()
        {   Serial.begin (9600);
            pinMode(trigPin, OUTPUT);
            pinMode(echoPin, INPUT); 
            pinMode(led,OUTPUT);
        }

     void loop()
        {   
            long duracion, distancia ;
            digitalWrite(trigPin, LOW);        // Nos aseguramos de que el trigger está desactivado
            delayMicroseconds(2);              // Para asegurarnos de que el trigger esta LOW
            digitalWrite(trigPin, HIGH);       // Activamos el pulso de salida
            delayMicroseconds(10);             // Esperamos 10µs. El pulso sigue active este tiempo
            digitalWrite(trigPin, LOW);        // Cortamos el pulso y a esperar el echo
            duracion = pulseIn(echoPin, HIGH) ;
            distancia = duracion / 2 / 29.1  ;
            Serial.println(String(distancia) + " cm.") ;
            if(distancia < 10){
              digitalWrite(led,HIGH);
              contador = contador + 1;
              delay (500);
            }
            else{
              digitalWrite(led,LOW);
            }
            Serial.println(contador);
        }

/*const int row1 = 10;
const int row2 = 11;
const int row3 = 12;
const int row4 = 13;
*/

/*  pinMode(row1,OUTPUT);
  pinMode(row2,OUTPUT);
  pinMode(row3,OUTPUT);
  pinMode(row4,OUTPUT);*/

/*void upload(int ledOn, int ledOff1, int ledOff2, int ledOff3){
  digitalWrite(ledOn, HIGH);
  digitalWrite(ledOff1, LOW);
  digitalWrite(ledOff2, LOW);
  digitalWrite(ledOff3, LOW);
  delay(1000);
}
  upload(row1,row2,row3,row4);
  upload(row2,row1,row3,row4);
  upload(row3,row1,row2,row4);
  upload(row4,row1,row2,row3);
*/
