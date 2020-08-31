#include<lpc214x.h>
#include<string.h>

#define sw  1<<3
#define buz 1<<4

void delay(unsigned int d)
{
	unsigned int a,b;
	for(a=0;a<d;a++)
	for(b=0;b<1000;b++);
}

void lcd_cmd(unsigned char ch)			//loop for lcd command
{
	IO1PIN=ch<<18;
	IO1CLR=1<<16;
	IO1SET=1<<17;
	delay(10);
	IO1CLR=1<<17;
}

void lcd_data(unsigned char dat)		//loop for lcd data
{
	IO1PIN=dat<<18;
	IO1SET=1<<16;
	IO1SET=1<<17;
	delay(10);
	IO1CLR=1<<17;
}
	
void lcd_init()											//lcd inisiligation commands
{
	IO1DIR=0xffffffff;
	lcd_cmd(0x38);
	lcd_cmd(0x06);
	lcd_cmd(0x0c);
	lcd_cmd(0x01);
}
	
void lcd_str(unsigned char *str)
{
	while(*str)
		lcd_data(*str++);
}

void ser_init1()
{
	PINSEL0|=0X05<<16;
	U1LCR=0x83;
	U1DLL=0X61;
	U1DLM=0X00;
	U1LCR=0x03;
}
	
unsigned char rx1()
{
	while(!(U1LSR&0x01));
	return U1RBR;
}

void tx1(unsigned char t)
{
	while(!(U1LSR & 0x20));
	U1THR=t;
}
	
void ser_tx1(unsigned char *txt)
{
	while(*txt)
	tx1(*txt++);
}

void ser_init0()
{
	PINSEL0|=0X5<<0;
	U0LCR=0x83;
	U0DLL=0X61;
	U0DLM=0X00;
	U0LCR=0x03;
}
	
unsigned char rx0()
{
	while(!(U0LSR&0x01));
	return U0RBR;
}

void tx0(unsigned char t)
{
	while(!(U0LSR & 0x20));
	U0THR=t;
}
	
void ser_tx0(unsigned char *txt)
{
	while(*txt)
	{
	while(!(U0LSR & 0X20));
	U0THR=*txt++;
}
}

void gsm_msg()
{
	ser_tx1("AT+CMGS=\"");
	delay(50);
	ser_tx1("9493033886\"\r\n");
	delay(5000);
	ser_tx1("Person is waiting. Please check authourigation");
	delay(100);
	tx1(0x1a);
	delay(10000);
}

void gsm_init()
{
	ser_tx1("AT\r\n");
	delay(4000);
	ser_tx1("AT+CMGF=1\r\n");
	delay(4000);
	ser_tx1("AT+CNMI=2,2,0,0,0\r\n");
	delay(4000);
	ser_tx1("AT+CMGR=1\r\n");
	delay(4000);
}



int main()
{
	unsigned char i;
	unsigned char tmp[20];
	
	IO1DIR|=0xffffffff;
//	IO0DIR|=(1<<10);
//	IO0DIR&=!(1<<3);
//	IO0CLR|=(1<<10);
	IO0DIR=0xfffffff0;
	//ExtInt_Init1();
	lcd_init();
	ser_init0();
	ser_init1();
	lcd_cmd(0x80);
	lcd_str("   IOT  BASED   ");
	lcd_cmd(0xc0);
	lcd_str("   DOOR  BELL   ");
	
	gsm_init();
	lcd_cmd(0x01);
	while(1)
	{
		lcd_cmd(0x01);
		lcd_cmd(0x80);
		lcd_str("  PLEASE PRESS  ");
//		if(!(IO0PIN&(1<<11)))
//		{
//			lcd_cmd(0x01);
//			IO0SET|=(1<<4);
//			lcd_str("  PERSON DETECTED");
			
//			ser_tx0("PERSON\r\n");
//			gsm_msg();
//			delay(20000);
//			while(!(IO0PIN&(1<<3)))
//				IO0SET|=(1<<4);
//			IO0CLR|=(1<<4);
//		if(U1LSR&0x01)
//			i=rx1();
//		}
//		else
//		{
//			lcd_cmd(0x80);
//			lcd_str(" PRESS DOOR BELL");
//		}
		
		if(U1LSR&0x01)
		{
			do{
				i=rx1();
 		}while(i!='*');
//			lcd_cmd(0x01);
//			lcd_cmd(0x80);
//			lcd_str("SEND MESSAGE");
			for(i=0;i<4;i++)
				tmp[i]=rx1();
			tmp[4]='\0';
//			lcd_cmd(0xc0);
//			lcd_str("MESSAGE RECEIVED");
			if(!(strcmp(tmp,"OPEN")))
			{
				lcd_cmd(0x01);
				lcd_cmd(0x80);
				lcd_str("  OPENING DOOR  ");
				IO0SET|=(1<<5);
				IO0CLR|=(1<<6);
				delay(50000);
				IO0CLR|=(1<<5);
				IO0CLR|=(1<<6);
				lcd_cmd(0x01);
				lcd_cmd(0x80);
				lcd_str("  DOOR CLOSING  ");
			}
			else
			{
				lcd_cmd(0x01);
				lcd_cmd(0x80);
				lcd_str("GARBAGE");
				delay(20000);
			}
		}
	}
}
