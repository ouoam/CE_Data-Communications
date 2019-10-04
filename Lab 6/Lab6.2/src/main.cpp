#include <Arduino.h>

#define NUM_DEGREE 16
#define PIN_PWM 5

float zeta[NUM_DEGREE];
float S[NUM_DEGREE];
uint8_t pwmDuty[NUM_DEGREE];

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_PWM, OUTPUT);

  for (int i = 0; i < NUM_DEGREE; i++)
  {
    zeta[i] = 360.0 / NUM_DEGREE * i;
    S[i] = sin(DEG_TO_RAD * zeta[i]);
    pwmDuty[i] = map(S[i] * 1000, -1000, 1000, 0, 255);

    // pwmDuty[i] = map(sin(DEG_TO_RAD * 360.0 / NUM_DEGREE * i) * 500, -500, 500, 0, 255);
    Serial.print(zeta[i]);
    Serial.print(" ");
    Serial.print(S[i]);
    Serial.print(" ");
    Serial.print(pwmDuty[i]);
    Serial.println();
  }
}

void loop()
{
  for (int i = 0; i < NUM_DEGREE; i++)
  {
    analogWrite(PIN_PWM, pwmDuty[i]);
    delayMicroseconds(4000);
  }
}