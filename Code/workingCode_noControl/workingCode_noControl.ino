#include <TimerOne.h>
#include <PID_v1.h>

//MOTOR LEFT
#define ENA 44
#define IN1 51
#define IN2 52

//MOTOR RIGHT
#define ENB 46
#define IN3 49
#define IN4 48

#define IR_BACK 4
#define IR_LEFT 5
#define IR_RIGHT 6

#define x_offset -9
#define y_offset -3

#define x_dead_zone 0
#define y_dead_zone 0

int PotenX, PotenY, IRback, IRleft, IRright;
int x_factor_motors, ir_factor_motors; 
int speedMotorRight, speedMotorLeft;

//############################# PID Parameters ########################################
double kpL = 0.28, kiL = 3.5, kdL = 0;
double kpR = 0.28, kiR = 3.5, kdR = 0;
//-----------------
double LSetpoint = 0, RSetpoint = 0;
double LInput = 0, RInput = 0;
double LOutput = 0, ROutput = 0;
PID LPID(&LInput, &LOutput, &LSetpoint, kpL, kiL, kdL, DIRECT);
PID RPID(&RInput, &ROutput, &RSetpoint, kpR, kiR, kdR, DIRECT);

double pwmL = 0, pwmR = 0;
int pwmLres, pwmRres = 0;
//#####################################################################################

//############################################# ENCODER PARAMETERS AND FUNCTIONS#########################################################
//VALUE 0 MEANS pin2, 1 MEANS pin3, 2 MEANS pin 18, AND SO ON.

// add set time in value of micro seconds 
volatile uint32_t set_time=50000;

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
  eclipsed_sec=eclipsed_sec+set_time;     //add the value of set time
  Speed_right = (counter_motor_right/(eclipsed_sec*pulse_per_revolution*motor_gear_ratio)*1000000*60);  //RPM, 1000000 --> for converting micro to seconds
  Speed_left = (counter_motor_left/(eclipsed_sec*pulse_per_revolution*motor_gear_ratio)*1000000*60);  //RPM, 1000000 --> for converting micro to seconds 
  //Serial.println(Speed_right);
  //Serial.println(Speed_left);
  //Serial.println("\n");
  counter_motor_right = 0;
  counter_motor_left = 0;
  eclipsed_sec = 0;
}
//========================================================
//#######################################################################################################################################

//=========================== SPEED CONTROL FUNCTION ================================
void setvmotorL(float vLsp)
{
//  if (vLsp > 0)
//  { //set rotation direction
//    digitalWrite(IN1, LOW);
//    digitalWrite(IN2, HIGH);
//  }
//  else if (vLsp < 0)
//  {
//    digitalWrite(IN1, HIGH);
//    digitalWrite(IN2, LOW);
//  }
//  else
//  {
//    digitalWrite(IN1, LOW);
//    digitalWrite(IN2, LOW);
//  }

  LSetpoint = vLsp;
  LInput = Speed_left;
  LPID.Compute();
  pwmL = LOutput;
  
  pwmLres = min(max(pwmL, 0), 255); //restrict PWM from 54 to 255
  analogWrite(ENA,pwmLres); //send PWM to motor

//  #if DEBUG == 1
//    Serial.print(millis()); //time
//    Serial.print("\t");
//    Serial.print(vLsp); //setpoint
//    Serial.print("\t");
//    Serial.print(vLcurr); //current speed
//    Serial.print("\t");
//    Serial.print(pwmL); // PWM
//    Serial.print("\t");
//    Serial.print(pwmLres); // PWM restricted
//    Serial.print("\n");
//  #endif
}

void setvmotorR(float vRsp)
{
//  if (vRsp > 0)
//  { //set rotation direction
//    digitalWrite(IN1, LOW);
//    digitalWrite(IN2, HIGH);
//  }
//  else if (vRsp < 0)
//  {
//    digitalWrite(IN1, HIGH);
//    digitalWrite(IN2, LOW);
//  }
//  else
//  {
//    digitalWrite(IN1, LOW);
//    digitalWrite(IN2, LOW);
//  }

  RSetpoint = vRsp;
  RInput = Speed_right;
  RPID.Compute();
  pwmR = ROutput;
  
  pwmRres = min(max(pwmR, 0), 255); //restrict PWM from 54 to 255
  analogWrite(ENB,pwmRres); //send PWM to motor

//  #if DEBUG == 1
//    Serial.print(millis()); //time
//    Serial.print("\t");
//    Serial.print(vLsp); //setpoint
//    Serial.print("\t");
//    Serial.print(vLcurr); //current speed
//    Serial.print("\t");
//    Serial.print(pwmL); // PWM
//    Serial.print("\t");
//    Serial.print(pwmLres); // PWM restricted
//    Serial.print("\n");
//  #endif
}
//======================================================================================

void setup() {
  LPID.SetMode(AUTOMATIC);
  
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
     digitalWrite(IN1, LOW);
     digitalWrite(IN2, HIGH);
     speedMotorLeft = map(PotenY, 511+y_dead_zone, 1023, 0, 255);
     
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, HIGH);
     speedMotorRight = speedMotorLeft;
  }
  else if(PotenY < (511 - y_dead_zone))
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    speedMotorLeft = map(PotenY, 511-y_dead_zone, 0, 0, 255);
    
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    speedMotorRight = speedMotorLeft;
  }
  else
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    speedMotorLeft = 0;
    speedMotorRight = 0;
  }

  IRback = analogRead(IR_BACK);
  if(IRback > 350)
  { 
    ir_factor_motors = map(IRback, 350, 750, 0, 255);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
     
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
     
    speedMotorLeft = speedMotorLeft + ir_factor_motors;
    speedMotorRight = speedMotorLeft;
  }
  
  if(PotenX > (511 + x_dead_zone))
  {
    x_factor_motors = map(PotenX, 511+x_dead_zone, 1023, 0, 255);
    speedMotorLeft = speedMotorLeft + x_factor_motors;
    speedMotorRight = speedMotorRight - x_factor_motors;
  }
  else if(PotenX < (511 - x_dead_zone))
  {
    x_factor_motors = map(PotenX, 511-x_dead_zone, 0, 0, 255);
    speedMotorLeft = speedMotorLeft - x_factor_motors;
    speedMotorRight = speedMotorRight + x_factor_motors;
  }

  IRleft = analogRead(IR_LEFT);
  if(IRleft > 350)
  {
    ir_factor_motors = map(IRleft, 350, 750, 0, 255);
    speedMotorLeft = speedMotorLeft + ir_factor_motors;
    //speedMotorRight = speedMotorRight - ir_factor_motors;
  }

  IRright = analogRead(IR_RIGHT);
  if(IRright > 350)
  {
    ir_factor_motors = map(IRright, 350, 750, 0, 255);
    //speedMotorLeft = speedMotorLeft - ir_factor_motors;
    speedMotorRight = speedMotorRight + ir_factor_motors;
  }
  
  if(speedMotorLeft < 50) speedMotorLeft = 0;
  if(speedMotorLeft > 255) speedMotorLeft = 255;
  if(speedMotorRight < 50) speedMotorRight = 0;
  if(speedMotorRight > 255) speedMotorRight = 255;

  analogWrite(ENA, speedMotorLeft);
  analogWrite(ENB, speedMotorRight);

//  if(!(speedMotorLeft==0 && speedMotorRight ==0))
//  {
//    Serial.print("\nMotor Left (PWM): ");
//    Serial.print(speedMotorLeft);
//    Serial.print("\tEncoder Left (RPM): ");
//    Serial.print(Speed_left);  
//    Serial.print("\tMotor Right (PWM): ");
//    Serial.print(speedMotorRight);
//    Serial.print("\tEncoder Right (RPM): ");
//    Serial.print(Speed_right);
//  }

  setvmotorL(Speed_left);
  setvmotorR(Speed_right);
}
