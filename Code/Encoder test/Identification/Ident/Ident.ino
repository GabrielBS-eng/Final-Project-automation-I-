#include <TimerOne.h>
//============================================
// add set time in value of micro seconds 
volatile uint32_t set_time=100000;
//============================================
//Motor Parameters 
const float pulse_per_revolution=11; //11 counts per revolution
const float motor_gear_ratio=45;   //1:45 Gear Ratio 
//============================================
//variables for DC Motor control Pins & PWM Pin 
const int motor_cont_1=2;
const int motor_cont_2=3; 
const int motor_pwm=5;
//variables for set values of speed change using potentiometer 
//const int pot=A0;
//int pot_value; 
//============================================================
volatile uint32_t counter = 0 ;  //variable to store count of encoder 
volatile uint32_t eclipsed_sec = 0 ;   //time eclipsed 
volatile float Speed = 0.0;     //spped of motor 
bool dir = true;     //for direction detection 
//============================================================

int i = 20;

void setup() 
{
  // put your setup code here, to run once:
  analogReference(DEFAULT);
  pinMode(motor_cont_1,OUTPUT);
  pinMode(motor_cont_2,OUTPUT);
  //pinMode(pot,INPUT);
  attachInterrupt( 0 , encoderCounts , RISING); //connect the encoder pin to 0 interput pin of Arduino like pin 2 in arduino uno 
  Timer1.initialize(set_time);   //getting values of speed every 100 milli seconds set time
  Timer1.attachInterrupt(timerIsr);
  counter = 0;
  eclipsed_sec=0;
  Serial.begin(9600);
}
//=============================================================
void loop() 
{
  // put your main code here, to run repeatedly:
  digitalWrite(motor_cont_1,HIGH);  
  digitalWrite(motor_cont_2,LOW);
  //pot_value=analogRead(pot);
  //pot_value=map(pot_value,0,1024,0,255); 
  if(i>220) i=20;
  analogWrite(motor_pwm,i);
  i+10;
}
//========================================================
//timer interrupt 
void timerIsr()
{
  //eclipsed_sec++;
  eclipsed_sec=eclipsed_sec+set_time;     //add the value of set time  
  Speed = (counter/(eclipsed_sec*pulse_per_revolution*motor_gear_ratio)*1000000*60);  //RPM, 1000000 --> for converting micro to seconds 
  //Serial.print(pot_value);
  Serial.print("\t");
  Serial.println(Speed);
  //Serial.print(dir);
  counter = 0;
  eclipsed_sec = 0;  
}
//===========================================================
//coder interrupt 
void encoderCounts()
{
  //delayMicroseconds(50);
  if (digitalRead(11)==HIGH) dir = true;
  else dir = false;
  counter++;
}
//============================================================
