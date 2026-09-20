bool run = true;

void setup() {
    pinMode(21, OUTPUT);
    Serial.begin(9600);

}

void loop() {
    if (Serial.available() > 0) {
        while (Serial.available() > 0) {
            Serial.read();
            delay(2);
        }
        Serial.println("Igniting");
        digitalWrite(21, HIGH);
        delay(5000);
        digitalWrite(21, LOW);
        run = false;
    }
  
}
