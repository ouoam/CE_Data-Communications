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
int get_character(void);
char massage[500];
int main(void)
{
 int intchar = 0,outchar = 0, stage, isTX, i;
 setup_serial();
 clrscr();
 printf("Send or Receive : ");
 stage = getch();
 printf("%c\n",stage);
 if(stage == 'S') isTX = 0;
 else isTX = 1;
 do
 {
  i = 0;
  if(isTX % 2 == 0)
  {
   if(stage == 'S') printf("Pc1 : ");
   else printf("Pc2 : ");
   outchar = getch();
   if(outchar == 17)
   {
   send_character(17);
   break;
   }
   printf("%c",outchar);
   massage[0] = outchar;
   gets(massage+1);
   while(massage[i] != 0) send_character(massage[i++]);
   send_character(0);
  }
  else
  {
   if(stage == 'S') printf("Pc2 : ");
   else printf("Pc1 : ");
   intchar = get_character();
   if(intchar == 17)break;
   massage[0] = intchar;
   i++;
   while ((intchar = get_character()) != 0) massage[i++] = intchar;
   massage[i] = 0;
   puts(massage);
  }
  isTX++;
 }while(1);

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
int get_character(void)
{
 int status;
 do
 {
  status = inportb(LSR) & 0x01;
 }while(status != 0x01);
 return((int)inport(TXDATA));
}