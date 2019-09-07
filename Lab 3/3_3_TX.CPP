#define COM3BASE 0x3E8
#define COM4BASE 0x2E8
#define COM3BASE 0x3E8
#define COM4BASE 0x2E8
#define TXDATA COM3BASE
#define LCR (COM3BASE+3)
#define LSR (COM3BASE+5)
#include<stdio.h>
#include<dos.h>
#include<conio.h>
void setup_serial(void);
void send_character(int ch);
//int get_character(void);
int main(void)
{
 int intchar,outchar;
 setup_serial();
 do
 {
  puts("Enter char to be transmitted (Ctrl-D to end)");
  outchar = getch();
  printf("Character transmitted is %c \n",outchar);
  send_character(outchar);
 // intchar = get_character();
 // printf("Character received was %c \n",intchar);
 }while(outchar != 4);
 return(0);
}
void setup_serial(void)
{
 outportb(LCR,0x80);
 outportb(TXDATA,0x0C);
 outportb(TXDATA+1,0x00);
 outportb(LCR,0x0A);
}
void send_character(int ch)
{
 char status;
 do
 {
  status = inportb(LSR) & 0x40;
 }while(status != 0x40);
 outportb(TXDATA,(char)ch);
}
/*int get_character(void)
{
 int status;
 do
 {
  status = inportb(LSR) & 0x01;
 }while(status != 0x01);
 return((int)inport(TXDATA));
} */