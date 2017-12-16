#include <TimerOne.h>

//MOTOR LEFT
#define ENA 44
#define IN1 51
#define IN2 52

//MOTOR RIGHT
#define ENB 46
#define IN3 49
#define IN4 48

#define x_offset -9
#define y_offset -3

#define x_dead_zone 0
#define y_dead_zone 0

int PotenX, PotenY;
int x_factor_motors, y_factor_motors; 
int speedMotorRight, speedMotorLeft;

//############################################# ENCODER PARAMETERS AND FUNCTIONS#########################################################
int Encod = 0; //Either pin2 or pin3 for Arduino Uno and Nano; pin2, pin3, pin18, pin19, pin20 or pin21 for Arduino Mega.
//VALUE 0 MEANS pin2, 1 MEANS pin3, 2 MEANS pin 18, AND SO ON.

// add set time in value of micro seconds 
volatile uint32_t set_time=500000;

//Motor Parameters 
const float pulse_per_revolution=11; //11 counts per revolution
const float motor_gear_ratio=45;   //1:45 Gear Ratio

volatile uint32_t counter_motor_right = 0 ;//pin2  //variable to store count of encoder
volatile uint32_t counter_motor_left = 0;//pin3 
volatile float eclipsed_sec = 0 ;   //time eclipsed 
volatile float Speed_right = 0.0;     //spped of motor 
volatile float Speed_left = 0.0;

//======================================================
//coder interrupt 
void encoderCounts1(){counter_motor_right++;}
void encoderCounts2(){counter_motor_left++;}

//timer interrupt 
void timerIsr()
{
  //eclipsed_sec++;
  eclipsed_sec=eclipsed_sec+set_time;     //add the value of set time
  //Serial.print("\t eclipsed_sec: ");  
  Speed_right = (counter_motor_right/(eclipsed_sec*pulse_per_revolution*motor_gear_ratio)*1000000*60);  //RPM, 1000000 --> for converting micro to seconds
  Speed_left = (counter_motor_left/(eclipsed_sec*pulse_per_revolution*motor_gear_ratio)*1000000*60);  //RPM, 1000000 --> for converting micro to seconds 
  Serial.println(Speed_right);
  Serial.println(Speed_left);
  Serial.println("\n");
  counter_motor_right = 0;
  counter_motor_left = 0;
  eclipsed_sec = 0;
}
//========================================================
//#######################################################################################################################################

void setup() {
  // put your setup code here, to run once: 
  Serial.begin(9600);
  analogReference(DEFAULT);
  attachInterrupt(0 , encoderCounts1 , RISING); //motorRight
  attachInterrupt(1 , encoderCounts2 , RISING); //motorLeft
  Timer1.initialize(set_time);   //getting values of speed every 100 mili seconds set time
  Timer1.attachInterrupt(timerIsr);
  counter_motor_right = 0;
  counter_motor_left = 0;
  eclipsed_sec=0;
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.print("\n\nx compensated:");
  PotenX = analogRead(2) + x_offset;
  if(PotenX < 0) PotenX = 0;
  if(PotenX > 1023 + 2*x_offset) PotenX = 1023;
  //Serial.print(PotenX);
  //Serial.print("\ny compensated:");
  PotenY = analogRead(3) + y_offset;
  if(PotenY < 0) PotenY = 0;
  if(PotenY > 1023 + 2*y_offset) PotenY = 1023;
  //Serial.print(PotenY);

  if(PotenY > (511 + y_dead_zone))
  {
     y_factor_motors = map(PotenY, 511+y_dead_zone, 1023, 0, 255);
     digitalWrite(IN1, LOW);
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, HIGH);
  }
  else if(PotenY < (511 - y_dead_zone))
  {
    y_factor_motors = map(PotenY, 511-y_dead_zone, 0, 0, 255);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  else
  {
    y_factor_motors = 0;
  }

  if(PotenX > (511 + x_dead_zone))
  {
    x_factor_motors = map(PotenX, 511+x_dead_zone, 1023, 0, 255);
    speedMotorLeft = y_factor_motors + x_factor_motors;
    speedMotorRight = y_factor_motors - x_factor_motors;

    if(speedMotorLeft > 255) speedMotorLeft = 255;
    if(speedMotorRight < 0) speedMotorRight = 0;
  }
  else if(PotenX < (511 - x_dead_zone))
  {
    x_factor_motors = map(PotenX, 511-x_dead_zone, 0, 0, 255);
    speedMotorLeft = y_factor_motors - x_factor_motors;
    speedMotorRight = y_factor_motors + x_factor_motors;

    if(speedMotorLeft < 0) speedMotorLeft = 0;
    if(speedMotorRight > 255) speedMotorRight = 255;
  }

  analogWrite(ENA, speedMotorLeft);
  analogWrite(ENB, speedMotorRight);
}
