#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>

#define NUM_DEGREE 16

Adafruit_MCP4725 dac;
int delay0;

float S[NUM_DEGREE];
uint16_t S_DAC[NUM_DEGREE];

#define defaultFreq 10000
#define freq0 200

void setup()
{
  Serial.begin(115200);
  dac.begin(0x64);
  delay0 = ((1000000 / freq0) / NUM_DEGREE) - 1000000 / defaultFreq;
  Serial.print("delay0 is ");
  Serial.println(delay0);

  for (int i = 0; i < NUM_DEGREE; i++)
  {
    S[i] = sin(DEG_TO_RAD * 360.0 / NUM_DEGREE * i);
    S_DAC[i] = map(S[i] * 4000, -4000, 4000, 0, 4095);

    // pwmDuty[i] = map(sin(DEG_TO_RAD * 360.0 / NUM_DEGREE * i) * 500, -500, 500, 0, 255);
    Serial.print(S[i]);
    Serial.print(" ");
    Serial.print(S_DAC[i]);
    Serial.println();
  }
  TWBR = ((F_CPU / 400000L) - 16) / 2; // Set I2C frequency to 400kHz
}

void loop()
{

  for (int i = 0; i < NUM_DEGREE; i++)
  {
    Wire.beginTransmission(0x64);
    // dac.setVoltage(S_DAC[i], false);
    Wire.write((S_DAC[i] >> 8) & 0b00001111);
    Wire.write(S_DAC[i] & 0b11111111);
    delayMicroseconds(delay0);
    Wire.endTransmission(false);
  }
}