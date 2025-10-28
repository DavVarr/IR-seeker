#include <Servo.h>
int u = A0;
int r = A1;
int d = A2;
int l = A3;
int xPos = 90;
int yPos = 90;
Servo xServo;
Servo yServo;
const double deadZone = 0.4;
const double expBase = 100;
const double cornerThreshold = 0.25;
const double noiseThreshold = 40;

double lf = 0, rf = 0, uf = 0, df = 0;


void setup() {
  xServo.attach(9);
  yServo.attach(10);
  xServo.write(90);
  yServo.write(90);
}

void loop() {
  
  lf = 1024 - analogRead(l);
  rf = 1024 - analogRead(r);
  uf = 1024 - analogRead(u);
  df = 1024 - analogRead(d);
  double eh = 0;
  double ev = 0;
  if (rf+lf > noiseThreshold) eh = (rf-lf)/(rf+lf);
  if (uf+df > noiseThreshold) ev = (uf-df)/(uf+df);
  if (fabs(eh) > cornerThreshold && fabs(ev) > cornerThreshold){
      if(fabs(eh) - fabs(ev) > -cornerThreshold && fabs(eh)-fabs(ev) < cornerThreshold) eh *= 1.3, ev *= 1.3; 
  }
  if (fabs(eh) < deadZone) eh = 0;
  if (fabs(ev) < deadZone) ev = 0;
 
  double xInc;
  double yInc;
  
  if (eh > 0) xInc = pow(expBase,eh-deadZone);
  else if (eh < 0) xInc = -pow(1.0/expBase,eh+deadZone);
  else xInc = 0;
  if (ev > 0) yInc = pow(expBase,ev-deadZone);
  else if (ev < 0) yInc = -pow(1.0/expBase,ev+deadZone);
  else yInc = 0;
  xPos += xInc;
  yPos += yInc;
  if (xPos > 180) xPos = 180;
  if (xPos < 0 ) xPos = 0;
  if (yPos > 180) yPos = 180;
  if (yPos < 0 ) yPos = 0;
  xServo.write(xPos);
  yServo.write(yPos);
  
  delay(20);
}
 