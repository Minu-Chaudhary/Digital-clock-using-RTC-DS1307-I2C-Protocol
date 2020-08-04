//#include<reg51.h>
#include<stdio.h>
#define lcd P1														//lcd --> port p1 is defined

sbit rs=P1^0;
sbit en=P1^1;

sbit SDA=P2^1;            								// serial _data
sbit SCL=P2^0;														//serial clock

sbit next=P2^2;                   	  	 //increment digit
sbit inc=P2^3;                    	 		 //increment value
sbit set=P2^4;                     	 		//set time

char ack;																//acknowledge step
unsigned char k;
unsigned int hour=0, min=0, sec=0;

// delay subroutine
 void delay(int t)
{
    int i,j;
    for(i=0;i<t;i++)
    for(j=0;j<1200;j++);
}

void data_en()
{
    rs=1;
    en=1;
    delay(1);
    en=0;
}

void lcd_data(unsigned char d)
{
    lcd=d & 0xf0;
    data_en();
    lcd=(d<<4) & 0xf0;
    data_en();
}

void cmd_en(void)
{
    rs=0;
    en=1;
    delay(1);
    en=0;
}

void lcd_cmd(unsigned char c)
{
    lcd=c & 0xf0;
    cmd_en();
    lcd=(c<<4) & 0xf0;
    cmd_en();
}

void lcdprint(char *str)
{
    while(*str)
    {
        lcd_data(*str);
        str++;
    }
}

void lcd_init(void)
{
    lcd_cmd(0x02);
    lcd_cmd(0x28);
    lcd_cmd(0x0c);
    lcd_cmd(0x01);
}

//"start" function for communicate with ds1307 RTC
void I2C_Start()
{
		SDA=1;
		SCL=1;
		SDA=0;
		SCL=0;
}

//"stop" function for communicate wit ds1307 RTC
void I2C_Stop()
{
		SDA=0;
		SCL=1;
		SDA=1;
	  SCL=0;
}

//"send" data to ds1307
unsigned char I2C_Send(unsigned char Data)
{
char i;
char ack_bit;
  for(i=0;i<8;i++)
	{
	if(Data & 0x80)
		SDA=1;
	else
		SDA=0;
		SCL=1;
		Data<<=1;
		SCL=0;
	}
	SDA=1;
	SCL=1;
	SDA=0;//ack_bit=SDA;
	SCL=0;
	return ack_bit;
}

//read data from ds1307
unsigned char I2C_Read(char ack)
{
unsigned char i, Data=0;
	SDA=1;
	for(i=0;i<8;i++)
	{
		Data<<=1;
		do
		{
		SCL=1;
		}
		while(SCL==0);
			if(SDA) Data|=1;
			SCL=0;
	}
	if(ack)
		SDA=0;
	else
		SDA=1;
		SCL=1;
		SCL=0;
		SDA=1;
		return Data;
}

//bcd_2_dec conversion
int BCDToDecimal(char bcdByte)
{
    char a,b,dec;
    a=(((bcdByte & 0xF0) >> 4) * 10);
    b=(bcdByte & 0x0F);
    dec=a+b;
    return dec;
}

char DecimalToBCD (int decimalByte)
{
  char a,b,bcd;
  a=((decimalByte / 10) << 4);
  b= (decimalByte % 10);
  bcd=a|b;
  return bcd;
}

//function to display time on LCD
 void show_time()
	{
  char var[5];
  lcd_cmd(0x80);
  lcdprint("Time:");
  sprintf(var,"%d",hour);
  lcdprint(var);
  sprintf(var,":%d",min);
  lcdprint(var);
  sprintf(var,":%d",sec);
  lcdprint(var);
  lcdprint(" ");
		 lcdprint("   ");
  }

//time set function
 void set_time()
 {
 lcd_cmd(0x0e);
 while(k<7)
 {
  while(k==1)                                      //set hour
  {
   if(inc==0)
   {
		 hour++;
		 while(inc==0);
     if(hour==24)
     {
		 hour=00;
		 }
     show_time();
	 }
   if(next==0)
		 {
			 k=2;
       while(next==0);
		 }
   lcd_cmd(0xc5);
  }

  while(k==2)                                       //set min
  {
   if(inc==0)
   {
		 min++;
		 while(inc==0);
     if(min==60)
     {
		   min=00;
		 }
     show_time();
	 }
   if(next==0)
		 {
		 k=6;
     while(next==0);
		 }
   lcd_cmd(0xc8);
  }
 }
}

void main()
{
    lcd_init();
    lcdprint("Digital Clock");
    lcd_cmd(0xc0);
    lcdprint(" Using I2C & RTC  ");
	  delay(200);
		lcd_cmd(0x01);
    delay(100);
	  show_time();                                           //display time
		delay(300);
   while(1)
 {
		  I2C_Start();
			I2C_Send(0xD0);
			I2C_Send(0x00);
			I2C_Start();
			I2C_Send(0xD1);
			sec=BCDToDecimal(I2C_Read(1));
			min=BCDToDecimal(I2C_Read(1));
			hour=BCDToDecimal(I2C_Read(1));
 			I2C_Stop();
			show_time();                                           //display time
			delay(50);
 }
 }
