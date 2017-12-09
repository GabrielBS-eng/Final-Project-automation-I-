int sensR= 8;
int sensF= 7;
int sensL= 9;
void setup ()
{
pinMode(sensL, INPUT);
  pinMode(sensR, INPUT);
  pinMode(sensF, INPUT);
}

sL= digitalRead(sensL);
 sF= digitalRead(sensF);
 sR= digitalRead(sensR);
void loop(){
  sL= digitalRead(sensL);
  sF= digitalRead(sensF);
  sR= digitalRead(sensR);
  
  if (sF== HIGH) {
    backward(5);
  }
  if (sF== LOW) {
    if (sL==HIGH && sR== LOW){
      right(2.5);
    }
    if (sL== LOW && sR== HIGH){
      
left(2.5);
    }
    if (sL== HIGH && sR == HIGH){
     
left(2.5);
    }
    if (sL== LOW && sR== LOW){
      
forward();
    }  
  }

