int ENA = 2;
int IN1 = 2;
int IN2 = 3;

int C1 = 1;
int C2 = 0;

int enc1;
int enc2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  enc1 = analogRead(C1);
  enc2 = analogRead(C2);

  Serial.print("\n\nencoder 1: ");
  Serial.print(enc1);
  Serial.print("\nencoder 2: ");
  Serial.print(enc2);
  delay(250);
}
