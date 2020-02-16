String data; //Initialized variable to store recieved data

void setup() {
  //Serial Begin at 9600 Baud 
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("Inicio");
}

void loop() {
  if(Serial1.available() > 0){
    char data = Serial1.read(); //Read the serial data and store it
    Serial.print(data);
  }
}
