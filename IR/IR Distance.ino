int sensorPin = 0; 
void setup()
{  Serial.begin(9600); } 

void loop()
{ 
   int val = analogRead(sensorPin);  Serial.println(val); // read the distance 
    // Add the same Code Or IR object detiction 
    delay(100);
}

