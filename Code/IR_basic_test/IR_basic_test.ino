void setup()
{
  Serial.begin(9600);
}

int motor;
int ir_back;
int ir_left;
int ir_right;

void loop()
{
  motor = 0;
  ir_back = analogRead(5);
  Serial.print(ir_back);
  delay(125);

 if(ir_back >= 350)
 {
  motor = map(ir_back, 350, 750, 0, 255);
 }
  Serial.print("\t");
  Serial.println(motor);
}
