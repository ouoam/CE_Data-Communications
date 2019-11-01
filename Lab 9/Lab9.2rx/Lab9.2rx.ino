// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define r_slope 100
void setup() {
  // put your setup code here, to run once:
  sbi(ADCSRA,ADPS2);
  cbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);

  Serial.begin(115200);
  Serial.flush();
}

int prev = 0;
int check = false;
int check2 = false;
int count = 0;

uint32_t startFreq = 0;

char data = 0;
uint8_t bitC = 0;

void loop() {
  int tmp = analogRead(A2);
  if(tmp > r_slope && prev < r_slope && check == false) {
    check = true;
    if (check2 == false) {
      startFreq = micros();
    }
  }
  if(tmp < r_slope && check2 == true) {
    if (micros() - startFreq > 3800) {
      data <<= 2;
      data |= (count - 2) & B0011;
      bitC++;
      if (bitC == 4) {
        Serial.print(data);
        data = 0;
        bitC = 0;
      }
      check2 = false;
      count = 0;
    }
  }
  if(tmp > 255 - r_slope && check == true) {
    count++;
    check2 = true;
    check = false;
  }
  prev = tmp;
}
