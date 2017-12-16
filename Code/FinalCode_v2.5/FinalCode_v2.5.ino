//NILE UNIVERSITY - FALL - 2017
//MASTER IN MECHATRONICS AND MICROMECHATRONICS SYSTEMS
//EU4M GROUP - FINAL PROJECT - AUTOMATION 1
//PROF.: AHMAD TAHER

//BERIT ADINA HÄNDEL
//GABRIEL DE BRITO SILVA
//HEDAYA ALI 
//SAIFUL AKMAL RAZALI


//TODO: USE ONE ENCODER OF EACH MOTOR IN PARALEL

#include <TimerOne.h> //For the encoder
#include <PID_v1.h>

//################# H-BRIDGE VARIABLES #######################
int ENA = 44; //right motor
int IN1 = 51;
int IN2 = 52;

int ENB = 46; //left motor
int IN3 = 49;
int IN4 = 48;
//############################################################

//#####################  _ \ | / _ Joystick - 2 Potenciometer - from 0 to 1023 - middle point (zero point): 511. ###########################
int PotenX;
int PotenY;
int PotPinX = A0; // A0 --> gray --> blue --> green --> X
int PotPinY = A1; // A1 --> white --> purple --> yellow --> Y

int x_compensator = 0;  //Maybe the central point of the x potenciometer of the joystick could not be 512 or 511... so we need to compensate it.
int y_compensator = 0;  //Maybe the central point of the y potenciometer of the joystick could not be 512 or 511... so we need to compensate it.
//positive values if the error is below 511 and negative values if the error is over 511 

int x_dead_zone_limit = 10; //It is the security range around the setPoint value in x (middle of the joystick) that the motors will not move, because our system is not perfect.
int y_dead_zone_limit = 10; //It is the security range around the setPoint value in x (middle of the joystick) that the motors will not move, because our system is not perfect.
//##########################################################################################################################################

//###############################PID Parameters########################################
int kpL = 0, kiL = 0, kdL = 0;
int kpR = 0, kiR = 0, kdR = 0;
int P_LinearMov = 0, I_LinearMov = 0, D_LinearMov = 0;
int P_AngularMov = 0, I_AngularMov = 0, D_AngularMov = 0;
//-----------------
double verrL = 0;
double verrLold = 0;
double verrLsum = 0;
double pwmL = 0;

//#####################################################################################

//############################################# ENCODER PARAMETERS AND FUNCTIONS#########################################################
int Encod = 0; //Either pin2 or pin3 for Arduino Uno and Nano; pin2, pin3, pin18, pin19, pin20 or pin21 for Arduino Mega.
//VALUE 0 MEANS pin2, 1 MEANS pin3, 2 MEANS pin 18, AND SO ON.

// add set time in value of micro seconds 
volatile uint32_t set_time=1000000;

//Motor Parameters 
const float pulse_per_revolution=11; //11 counts per revolution
const float motor_gear_ratio=45;   //1:45 Gear Ratio

volatile uint32_t counter_motor_right = 0 ;//pin2  //variable to store count of encoder
volatile uint32_t counter_motor_left = 0;//pin3 
volatile float eclipsed_sec = 0 ;   //time eclipsed 
volatile float vRcurr = 0.0;     //spped of motor 
volatile float vLcurr = 0.0;

int count=0;

//=======================INTERRUPT FUNCTIONS (count encoder steps, calc motor speed)===============================
//coder interrupt 
void encoderCounts1(){counter_motor_right++;}
void encoderCounts2(){counter_motor_left++;}

//timer interrupt 
void timerIsr()
{
  //eclipsed_sec++;
  eclipsed_sec=eclipsed_sec+set_time;     //add the value of set time
  //Serial.print("\t eclipsed_sec: ");  
  vRcurr = (counter_motor_right/(eclipsed_sec*pulse_per_revolution*motor_gear_ratio)*1000000*60);  //RPM, 1000000 --> for converting micro to seconds
  vLcurr = (counter_motor_left/(eclipsed_sec*pulse_per_revolution*motor_gear_ratio)*1000000*60);  //RPM, 1000000 --> for converting micro to seconds 
  //Serial.print("\t");
  //Serial.print("Speed: ");
  Serial.print(vRcurr);
  Serial.print("\t");
  Serial.print(vLcurr);
  Serial.print("\n");
  counter_motor_right = 0;
  counter_motor_left = 0;
  eclipsed_sec = 0;
}
//========================================================

void setvmotorL(float vLsp) {
  verrL = vLsp-vLcurr; //calculate error
  pwmL = verrL * kpL + verrLsum * kiL + (verrL - verrLold) * kpL; //apply controller
  verrLold = verrL; //store current error for next run
  verrLsum += verrL; //add error to integral
  
  if (verrLsum >4000) verrLsum = 4000;
  if (verrLsum <-4000) verrLsum = -4000;

  analogWrite(ENB,pwmL);
  }


//  Serial.print("\n\nMaybe x:");
//  PotenX = analogRead(PotPinX);
//  Serial.print(PotenX);
//  Serial.print("\nMaybe y:");
//  PotenY = analogRead(PotPinY);
//  Serial.print(PotenY);
//  delay(1000);
//
//  xAxis = PotenX + x_compensator;
//  yAxis = PotenY + y_compensator;
//
//
//
//  //################################# Y POTENCIOMETER FOR FORWARD AND BACKWARD #############################
//  if(PotenY < (511 - y_dead_zone_limit))//backward
//  {
//    digitalWrite(IN3, HIGH);
//    digitalWrite(IN4, LOW);
//    y_motor_factor = map(yAxis, (511 - y_dead_zone_limit), 0 , 0, 255);
//  }
//  if(PotenY > (511 + y_dead_zone_limit))//forward
//  {
//    digitalWrite(IN3, LOW);
//    digitalWrite(IN4, HIGH);
//    y_motor_factor = map(yAxis, (511 + y_dead_zone_limit), 1023, 0, 255);
//  }
//  else
//  {
//    y_motor_factor = 0;
//    digitalWrite(IN3, LOW);
//    digitalWrite(IN4, LOW);
//  }
//  //########################################################################################################
//
//  //################################# X POTENCIOMETER FOR TURN LEFT AND RIGHT ##############################
//  if(xAxis < (511 - x_dead_zone_limit)) //left
//  {
//    digitalWrite(IN1, HIGH);
//    digitalWrite(IN2, LOW);
//    x_motor_factor = map(xAxis, (511 - x_dead_zone_limit), 0, 0, 255);
//  }
//  else if(PotenX > (511 + x_dead_zone_limit)) //right
//  {
//    digitalWrite(IN1, LOW);
//    digitalWrite(IN2, HIGH);
//    x_motor_factor = map(xAxis, (511 + x_dead_zone_limit), 1023, 0, 255);
//  }
//  else 
//  {
//    x_motor_factor = 0; 
//    digitalWrite(IN3, LOW);
//    digitalWrite(IN4, LOW);
//  }
//  //########################################################################################################
//  
//  motorAspeed = y_motor_factor + x_motor_factor;
//  motorBspeed = y_motor_factor - x_motor_factor;
//  
//  analogWrite(ENA, motorAspeed);
//  analogWrite(ENB, motorBspeed);


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
  
  attachInterrupt(0 , encoderCounts1 , RISING); //connect the encoder pin to 0 interput pin of Arduino like pin 2 in arduino mega 
  attachInterrupt(1 , encoderCounts2 , RISING); //connect the encoder pin to 1 interput pin of Arduino like pin 3 in arduino mega
  
  Timer1.initialize(set_time);   //getting values of speed every 100 mili seconds set time
  Timer1.attachInterrupt(timerIsr);
  counter_motor_right = 0;
  counter_motor_left = 0;
  eclipsed_sec=0;

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
}

void loop()
{

analogWrite(ENA,255);
}
