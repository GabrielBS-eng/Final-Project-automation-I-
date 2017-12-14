#include <TimerOne.h>

//################################################
// add set time in value of micro seconds 
volatile uint32_t set_time=1000000;
//################################################
//Motor Parameters 
const float pulse_per_revolution=11; //11 counts per revolution
const float motor_gear_ratio=45;   //1:45 Gear Ratio
//################################################
volatile uint32_t counter = 0 ;  //variable to store count of encoder 
volatile float eclipsed_sec = 0 ;   //time eclipsed 
volatile float Speed = 0.0;     //spped of motor 
bool dir = true;     //for direction detection 
//################################################

#define DEBUG 0

int ENA = 5;
int IN1 = 7;
int IN2 = 8;

int Pwm = 20;

int count=0;

//############################################################
//coder interrupt 
void encoderCounts()
{
  counter++;
}
//############################################################
//timer interrupt 
void timerIsr()
{
  //eclipsed_sec++;
  eclipsed_sec=eclipsed_sec+set_time;     //add the value of set time
  Serial.print("\t eclipsed_sec: ");  
  Speed = (counter/(eclipsed_sec*pulse_per_revolution*motor_gear_ratio)*1000000*60);  //RPM, 1000000 --> for converting micro to seconds 
  //Serial.print(pot_value);
  Serial.print("\t");
  Serial.print("Speed: ");
  Serial.print(Speed);
  //Serial.print(dir);
  counter = 0;
  eclipsed_sec = 0;
}
//############################################################

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
  Serial.print("\n\n");
  delay(1000);

  if(DEBUG == 0)
  {
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);

    if(Pwm > 220) Pwm = 20;
    Pwm = Pwm + 10;
    Serial.print("\nPwm: ");
    Serial.print(Pwm);

    analogWrite(ENA,Pwm);
  }

  Serial.print("\nencoder_count:");
  Serial.print(count);
}
