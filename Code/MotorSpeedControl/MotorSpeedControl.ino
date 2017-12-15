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
//#include <PID_v1.h>

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



//############################### PID Parameters ########################################
double kpL = 0.6, kiL = 0.009, kdL = 12.4;
//int kpR = 0, kiR = 0, kdR = 0;
//-----------------
double verrL = 0;
double verrLold = 0;
double verrLsum = 0;
double pwmL = 0;
//#####################################################################################


//########################### ENCODER PARAMETERS AND FUNCTIONS ###########################
int Encod = 0; //Either pin2 or pin3 for Arduino Uno and Nano; pin2, pin3, pin18, pin19, pin20 or pin21 for Arduino Mega.
//VALUE 0 MEANS pin2, 1 MEANS pin3, 2 MEANS pin 18, AND SO ON.

// add set time in value of micro seconds 
volatile uint32_t set_time=100000;

//Motor Parameters 
const float pulse_per_revolution=11; //11 counts per revolution
const float motor_gear_ratio=45;   //1:45 Gear Ratio

volatile uint32_t counter_motor_right = 0 ;//pin2  //variable to store count of encoder
volatile uint32_t counter_motor_left = 0;//pin3 
volatile float eclipsed_sec = 0 ;   //time eclipsed 
volatile float vRcurr = 0.0;     //spped of motor 
volatile float vLcurr = 0.0;

int count=0;
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
  //Serial.print("\t");
  //Serial.print("Speed: ");
  //Serial.print(vRcurr);
  //Serial.print(vLcurr);
  counter_motor_right = 0;
  counter_motor_left = 0;
  eclipsed_sec = 0;
}
//===========================================================================================


//=========================== SPEED CONTROL FUNCTION ================================
void setvmotorL(float vLsp) {
  if (vLsp > 0) { //set rotation direction
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
  else if (vLsp < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
  
  verrL = vLsp - vLcurr; //calculate error
  pwmL = verrL * kpL + verrLsum * kiL + (verrL - verrLold) * kpL; //apply controller
  verrLold = verrL; //store current error for next run
  verrLsum += verrL; //add error to integral
  
  if (verrLsum >4000) verrLsum = 4000;
  if (verrLsum <-4000) verrLsum = -4000;

  min(max(pwmL, 0), 255); //restrict PWM from 0 to 255
  analogWrite(ENA,pwmL); //send PWM to motor

  #if DEBUG == 1
    Serial.print(millis()); //time
    Serial.print("\t");
    Serial.print(vLsp); //setpoint
    Serial.print("\t");
    Serial.print(vLcurr); //current speed
    Serial.print("\t");
    Serial.print(pwmL); // PWM
    Serial.print("\n");
  #endif
  }
//======================================================================================




//###################################### SETUP ############################################
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
}


//###################################### LOOP ############################################
void loop()
{
  
setvmotorL(100);
delay(5);
}
