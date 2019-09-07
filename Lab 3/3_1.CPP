#include<stdio.h>
#include<dos.h>
void main(void)
{
 unsigned int far *ptraddr;
 unsigned int address;
 int a;
 ptraddr = (unsigned int far *)0x00000400;
 for(a = 0; a < 4; a++)
 {
  address = *ptraddr;
  if(address == 0)
  {
   printf("No port found for COM%d \n",a+1);
  }
  else
  {
   printf("Addres assigned to COM%d is %Xh\n",a+1,address);
  }
  *ptraddr++;
 }

}