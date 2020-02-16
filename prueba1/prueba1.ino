void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("Init");
  Serial1.print("Erik");
}

void loop() {
  if(Serial1.available()>0){
    char data = Serial1.read();
    Serial.print(data);
  }
}
