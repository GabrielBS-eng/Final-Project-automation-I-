//NILE UNIVERSITY - FALL - 2017
//MASTER IN MECHATRONICS AND MICROMECHATRONICS SYSTEMS
//EU4M GROUP - FINAL PROJECT - AUTOMATION 1
//PROF.: AHMAD TAHER

//BERIT ADINA HÄNDEL
//GABRIEL DE BRITO SILVA
//HEDAYA ALI 
//SAIFUL AKMAL RAZALI

#include <TimerOne.h> //For the encoder

//################# H-BRIDGE VARIABLES #######################
int ENA = 44;
int IN1 = 51;
int IN2 = 52;

int ENB = 46;
int IN3 = 49;
int IN4 = 48;
//############################################################

//#####################  _ \ | / _ Joystick - 2 Potenciometer - from 0 to 1023 - middle point (zero point): 511. ###########################
int PotenX;
int PotenY;

int x_compensator = 0;  //Maybe the central point of the x potenciometer of the joystick could not be 512 or 511... so we need to compensate it.
int y_compensator = 0;  //Maybe the central point of the y potenciometer of the joystick could not be 512 or 511... so we need to compensate it.
//positive values if the error is below 511 and negative values if the error is over 511 

int x_dead_zone_limit = 10; //It is the security range around the setPoint value in x (middle of the joystick) that the motors will not move, because our system is not perfect.
int y_dead_zone_limit = 10; //It is the security range around the setPoint value in x (middle of the joystick) that the motors will not move, because our system is not perfect.
//##########################################################################################################################################

//############################################# ENCODER PARAMETERS AND FUNCTIONS#########################################################
int Encod = 0; //Either pin2 or pin3 for Arduino Uno and Nano; pin2, pin3, pin18, pin19, pin20 or pin21 for Arduino Mega.
//VALUE 0 MEANS pin2, 1 MEANS pin3, 2 MEANS pin 18, AND SO ON.

// add set time in value of micro seconds 
volatile uint32_t set_time=1000000;

//Motor Parameters 
const float pulse_per_revolution=11; //11 counts per revolution
const float motor_gear_ratio=45;   //1:45 Gear Ratio

volatile uint32_t counter = 0 ;  //variable to store count of encoder 
volatile float eclipsed_sec = 0 ;   //time eclipsed 
volatile float Speed = 0.0;     //spped of motor 

int count=0;

//======================================================
//coder interrupt 
void encoderCounts()
{
  counter++;
}

//timer interrupt 
void timerIsr()
{
  //eclipsed_sec++;
  eclipsed_sec=eclipsed_sec+set_time;     //add the value of set time
  Serial.print("\t eclipsed_sec: ");  
  Speed = (counter/(eclipsed_sec*pulse_per_revolution*motor_gear_ratio)*1000000*60);  //RPM, 1000000 --> for converting micro to seconds 
  Serial.print("\t");
  Serial.print("Speed: ");
  Serial.print(Speed);
  counter = 0;
  eclipsed_sec = 0;
}
//========================================================
//#######################################################################################################################################

int x_motor_factor;
int y_motor_factor;
  
int motorAspeed;
int motorBspeed;

int xAxis;
int yAxis;

void setup()
{
  Serial.begin(9600);
  analogReference(DEFAULT);
  attachInterrupt(0 , encoderCounts , RISING); //connect the encoder pin to 0 interput pin of Arduino like pin 2 in arduino uno 
  Timer1.initialize(set_time);   //getting values of speed every 100 mili seconds set time
  Timer1.attachInterrupt(timerIsr);
  counter = 0;
  eclipsed_sec=0;
}

void loop()
{
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
  //########################################################################################################
  
  motorAspeed = y_motor_factor + x_motor_factor;
  motorBspeed = y_motor_factor - x_motor_factor;
  
  analogWrite(ENA, motorAspeed);
  analogWrite(ENB, motorBspeed);

}
