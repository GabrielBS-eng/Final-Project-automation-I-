void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("\n\nMaybe x:");
  int PotenX = analogRead(0);
  Serial.print(PotenX);
  delay(500);
  Serial.print("\nMaybe y:");
  int PotenY = analogRead(1);
  Serial.print(PotenY);
  delay(1000);

}
