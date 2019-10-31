#include <Arduino.h>
#include <Wire.h>

#define NUM_SAMPLE 4
#define NUM_FREQ 4
#define FREQ_DIFF 250

int delay0;
float S[NUM_SAMPLE];
uint16_t S_DAC[NUM_SAMPLE];

uint16_t freq[NUM_FREQ];
uint16_t freqDelay[NUM_FREQ];

void setVoltage(uint16_t vol)
{
  Wire.beginTransmission(0x62);
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

  for (int i = 0; i < NUM_FREQ; i++)
  {
    freq[i] = (i+2) * FREQ_DIFF;
    freqDelay[i] = ((1000000 / freq[i]) / NUM_SAMPLE) - 110;
    Serial.print(freq[i]);
    Serial.print(" ");
    Serial.print(freqDelay[i]);
    Serial.print(" ");
    Serial.print(freq[i] / FREQ_DIFF);
    Serial.println();
  }

  for (int i = 0; i < NUM_SAMPLE; i++)
  {
    S[i] = sin(DEG_TO_RAD * 360.0 / NUM_SAMPLE * i);
    S_DAC[i] = S[i] * 2047.5 + 2047.5;
    Serial.print(S[i]);
    Serial.print(" ");
    Serial.print(S_DAC[i]);
    Serial.println();
  }
  setVoltage(0);
}

void loop()
{
  if (Serial.available() > 0)
  {
    delay(5);
    int in = Serial.parseInt();
    int input[4];
    input[0] = (in >> 0) & B0011;
    input[1] = (in >> 2) & B0011;
    input[2] = (in >> 4) & B0011;
    input[3] = (in >> 6) & B0011;
    
    for (int k = 3; k >= 0; k--)
    {
      for (int cycle = freq[input[k]] / FREQ_DIFF; cycle > 0; cycle--)
      {
        for (int sample = 0; sample < NUM_SAMPLE; sample++)
        {
          setVoltage(S_DAC[sample]);
          delayMicroseconds(freqDelay[input[k]]);
        }
      }
    }
    setVoltage(0);
  }
}
