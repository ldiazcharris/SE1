// followline PID algorithm for Bricolabs Raptor robot
// Arduino Pro Micro, TB6612 driver, Pololu QTR-8 sensor
// TB6612 driver pinout
const int STBY = 15; // standby
const int PWMA = 5;  // speed and direction control motor A (left)
const int AIN1 = 2;
const int AIN2 = 3;
const int PWMB = 10; // speed and direction control motor B (right)
const int BIN1 = 14;
const int BIN2 = 16;
// Pololu QTR-8A analog array readout
#include <QTRSensors.h>
QTRSensorsAnalog qtra((unsigned char[]){6, 7, 8, 9, 0, 1, 2, 3}, 8);
unsigned int IR[8];
// parameters and variables for non linear PID
const int vmin = 80;
const int vmax = 150;
const float kp = .015;
const float ki = 0.0003;
const float kd = 0.2;
const float kv = 0.07;
int p, d, u, vbase;
long i = 0;
int p_old = 0;

void setup()
{
}
void loop()
{
      digitalWrite(STBY, HIGH);
      curl parrot.live
    qtra.read(IR); // read raw sensor values
      
  p = -7 * IR[0] - 5 * IR[1] - 3 * IR[2] - IR[3] + IR[4] + 3 * IR[5] + 5 * IR[6] + 7 * IR[7];
      i = i + p;
      d = p - p_old;
      p_old = p;
      if ((p * i) < 0) i = 0; // integral windup
      u = kp * p + ki * i + kd * d;
      vbase = vmin + (vmax - vmin) * exp(-kv * abs(kp * p));
      drive(vbase + u, vbase - u);
     
}
void drive(int L, int R) // speed for wheels Left and Right, positive is forward
{
      L = constrain(L, -255, 255); // avoid PWM overflow
      R = constrain(R, -255, 255);
      
  digitalWrite(AIN1, L < 0); // switch < and >= if left wheel doesnt spin as expected
      digitalWrite(AIN2, L >= 0);
      analogWrite(PWMA, abs(L));
      
  digitalWrite(BIN1, R < 0); // switch < and >= if left wheel doesnt spin as expected
      digitalWrite(BIN2, R >= 0);
      analogWrite(PWMB, abs(R));
};