//NILE UNIVERSITY - FALL - 2017
//MASTER IN MECHATRONICS AND MICROMECHATRONICS SYSTEMS
//EU4M GROUP - FINAL PROJECT - AUTOMATION 1
//PROF.: AHMAD TAHER

//BERIT ADINA HÄNDEL
//GABRIEL DE BRITO SILVA
//HEDAYA ALI 
//SAIFUL AKMAL RAZALI

//################# H-BRIDGE VARIABLES #######################
int ENA = 44;
int IN1 = 51;
int IN2 = 52;

int ENB = 46;
int IN3 = 49;
int IN4 = 48;
//############################################################

//###############  _ \ | / _ Joystick - 2 Potenciometer - from 0 to 1023 - middle point (zero point): 511. #####################
int PotenX;
int PotenY;

int x_compensator;  //Maybe the central point of the x potenciometer of the joystick could not be 512 or 511... so we need to compensate it.
int y_compensator;  //Maybe the central point of the y potenciometer of the joystick could not be 512 or 511... so we need to compensate it.
//positive values if the error is below 511 and negative values if the error is over 511 


int x_dead_zone_limit; //It is the security range around the setPoint value in x (middle of the joystick) that the motors will not move, because our system is not perfect.
int y_dead_zone_limit; //It is the security range around the setPoint value in x (middle of the joystick) that the motors will not move, because our system is not perfect.
//###############################################################################################################################

int x_motor_factor;
int y_motor_factor;
  
int motorAspeed;
int motorBspeed;

int xAxis;
int yAxis;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("\n\nMaybe x:");
  PotenX = analogRead(0);
  Serial.print(PotenX);
  delay(500);
  Serial.print("\nMaybe y:");
  PotenY = analogRead(1);
  Serial.print(PotenY);
  delay(1000);

  xAxis = PotenX + x_compensator;
  yAxis = PotenY + y_compensator;

  //################################# Y POTENCIOMETER FOR FORWARD AND BACKWARD #############################
  
  if(PotenY < (511 - y_dead_zone_limit))
  {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    y_motor_factor = map(yAxis, (511 - y_dead_zone_limit), 0 , 0, 255);
  }
  if(PotenY > (511 + y_dead_zone_limit))
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    y_motor_factor = map(yAxis, (511 + y_dead_zone_limit), 1023, 0, 255);
  }
  else
  {
    y_motor_factor = 0;
  }
  //########################################################################################################

  //################################# X POTENCIOMETER FOR TURN LEFT AND RIGHT ##############################
  if(xAxis < (511 - x_dead_zone_limit))
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    x_motor_factor = map(xAxis, (511 - x_dead_zone_limit), 0, 0, 255);
  }
  else if(PotenX > (511 + x_dead_zone_limit))
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    x_motor_factor = map(xAxis, (511 + x_dead_zone_limit), 1023, 0, 255);
  }
  else 
  {
    x_motor_factor = 0;
  }

  motorAspeed = y_motor_factor + x_motor_factor;
  motorBspeed = y_motor_factor - x_motor_factor;
  
  analogWrite(ENA, motorAspeed);
  analogWrite(ENB, motorBspeed);

}
