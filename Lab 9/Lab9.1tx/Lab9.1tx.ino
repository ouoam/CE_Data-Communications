#include <Arduino.h>
#include <Wire.h>

#define NUM_CYCLE 5
#define NUM_SAMPLE 4

#define FREQ0 2500

int delay0;
float S[NUM_SAMPLE];
uint16_t S_DAC[NUM_SAMPLE];
char inData[40];
float A[4] = {0.25, 0.50, 0.75, 1.00};

void setVoltage(uint16_t vol)
{
  Wire.beginTransmission(0x64);
  Wire.write((vol >> 8) & 0b00001111);
  Wire.write(vol & 0b11111111);
  Wire.endTransmission();
}

void setup()
{
  Serial.begin(115200);
  Serial.flush();
  Wire.begin();
  Wire.setClock(400000UL);

  delay0 = ((1000000 / FREQ0) / NUM_SAMPLE) - 100;

  for (int i = 0; i < NUM_SAMPLE; i++)
  {
    S[i] = sin(DEG_TO_RAD * 360.0 / NUM_SAMPLE * i);
    S_DAC[i] = S[i] * 2047.5 + 2047.5;
    Serial.print(S[i]);
    Serial.print(" ");
    Serial.print(S_DAC[i]);
    Serial.println();
  }
}

void loop()
{
  if (Serial.available() > 0)
  {
    int i = 0;
    delay(5);
    while (Serial.available())
    {
      inData[i] = Serial.read();
      i++;
    }
    for (int j = 0; j < i; j++)
    {
      for (int k = 7; k >= 0; k -= 2)
      {
        uint8_t temp = inData[j] & B0011;
        for (int cycle = 0; cycle < NUM_CYCLE; cycle++)
        {
          for (int sample = 0; sample < NUM_SAMPLE; sample++)
          {
            setVoltage((((int16_t)S_DAC[sample] - 2047) * A[temp]) + 2047);
            delayMicroseconds(delay0);
          }
        }
        inData[j] = inData[j] >> 2;
      }
    }
    setVoltage(0);
  }
}
