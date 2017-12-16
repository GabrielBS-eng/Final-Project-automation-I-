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

#define DEBUG 1
  // DEBUG 0: normal code (no debugging additions)
  // DEBUG 1: print time, setpoint, current speed to serial

//################# H-BRIDGE VARIABLES #######################
int ENA = 44; //left motor
int IN1 = 51;
int IN2 = 52;

int ENB = 46; //right motor
int IN3 = 49;
int IN4 = 48;
//############################################################

  int i=0; //pwm for ramp response

//########################### ENCODER PARAMETERS AND FUNCTIONS ###########################
int Encod = 0; //Either pin2 or pin3 for Arduino Uno and Nano; pin2, pin3, pin18, pin19, pin20 or pin21 for Arduino Mega.
//VALUE 0 MEANS pin2, 1 MEANS pin3, 2 MEANS pin 18, AND SO ON.

// add set time in value of micro seconds 
volatile uint32_t set_time=30000; //0.005s

//Motor Parameters 
const float pulse_per_revolution=11; //11 counts per revolution
const float motor_gear_ratio=45;   //1:45 Gear Ratio

volatile uint32_t counter_motor_right = 0 ;//pin2  //variable to store count of encoder
volatile uint32_t counter_motor_left = 0;//pin3 
volatile float eclipsed_sec = 0 ;   //time eclipsed 
volatile float vRcurr = 0.0;     //spped of motor 
volatile float vLcurr = 0.0;

int count=0;
long timecurr=0;
//#####################################################################################


//==================INTERRUPT FUNCTIONS (count encoder steps, calc motor speed)=============
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
  
  analogWrite(ENB,255);
  //timecurr = micros();
  
  //Serial.print(timecurr);
  Serial.print("\t");
  Serial.print(eclipsed_sec);
  Serial.print("\t");
  Serial.print(counter_motor_right);
  Serial.print("\t");
  Serial.print(counter_motor_left);
  Serial.print("\t");
  Serial.print(vRcurr);
  Serial.print("\n");
  
  counter_motor_right = 0;
  counter_motor_left = 0;
  eclipsed_sec = 0;
}
//===========================================================================================


//###################################### SETUP ############################################
void setup()
{
  Serial.begin(9600);
  analogReference(DEFAULT);

  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  //analogWrite(ENA,255);
  
  attachInterrupt(0 , encoderCounts1 , RISING); //connect the encoder pin to 0 interput pin of Arduino like pin 2 in arduino mega 
  attachInterrupt(1 , encoderCounts2 , RISING); //connect the encoder pin to 1 interput pin of Arduino like pin 3 in arduino mega
  
  Timer1.initialize(set_time);   //getting values of speed
  Timer1.attachInterrupt(timerIsr);
  counter_motor_right = 0;
  counter_motor_left = 0;
  eclipsed_sec=0;


  delay(1000);
}


//###################################### LOOP ############################################
void loop()
{
//  Serial.print("START motor on: ");
//  Serial.print(millis());
//  Serial.print("\n");
//
//
//  analogWrite(ENA,255);
//  i=255;
////  while (i<255){
////    analogWrite(ENA,i);
////    i=i+1;
////    delay(500);
////  }
//  delay(4000);
//  analogWrite(ENA,0);
//  i=0;
//  
//  Serial.print("END motor off: ");
//  Serial.print(millis());
//  Serial.print("\n");
//
//  
  delay(4000);
}
