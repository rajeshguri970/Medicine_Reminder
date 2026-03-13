// Requirements:jumper wires,20X4 LCD,KEYPAD,TWO SWITCHES,LPC2148 uC.
#include <lpc214x.h>
#include "lcd.h"
#include "keypad.h"
#include "lcd_defines.h"
#include "delay.h"

#define sw1 22 //EDIT RTC AND MEDICINE TIMINGS SWITCH.
#define sw2 23 //ALARM STOP SWITCH.

#define BUZZER_PIN 0
#define BUZZER_ON IOSET0=(1<<BUZZER_PIN)
#define BUZZER_OFF IOCLR0=(1<<BUZZER_PIN)

// System clock and peripheral clock Macros
#define FOSC	12000000 
#define CCLK  (5*FOSC)   
#define PCLK  (CCLK/4)

// RTC Macros
#define PREINT_VAL  (int)((PCLK / 32768) - 1)
#define PREFRAC_VAL (PCLK-((PREINT_VAL + 1) * 32768))

#define RTC_ENABLE	0x01  
#define RTC_RESET   0x02  


void RTC_Init(void);

void RTC_SetTimeInfo(unsigned int hour, unsigned int min, unsigned int sec);
void RTC_SetDateInfo(unsigned int date, unsigned int month, unsigned int year);

void RTC_SetHour(unsigned int);
void RTC_SetMin(unsigned int);
void RTC_SetSec(unsigned int);
void RTC_SetDate(unsigned int);
void RTC_SetMonth(unsigned int);
void RTC_SetYear(unsigned int);
void RTC_SetDay(unsigned int);

unsigned int RTC_GetHour(void);
unsigned int RTC_GetMin(void);
unsigned int RTC_GetSec(void);
unsigned int RTC_GetDate(void);
unsigned int RTC_GetMonth(void);
unsigned int RTC_GetYear(void);
unsigned int RTC_GetDay(void);

signed char week[][4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};
u32 KeyC;
u32 hour,min,sec,date,month,year;
int h3,m3,s3,day1,d3,mon3,y5;
int mh3,mm3,ms3,mh23,mm23,ms23;

// Main function
int main()
{
	IODIR0|=(1<<BUZZER_PIN);
	BUZZER_OFF;

	h3=m3=s3=day1=d3=mon3=y5=-1;
	 mh3=mm3=ms3=mh23=mm23=ms23=00;
	// Initialize RTC 
	RTC_Init();
	// Initialize the LCD
	InitLCD();
	Keypad_Init();
	CmdLCD(0x0C);
	// Set the initial time (hours, minutes, seconds)
	RTC_SetTimeInfo(19, 54,00);
	RTC_SetDateInfo(5,2,2025);
	DOW = 4;
	hour = HOUR;
	min = MIN;
	sec = SEC;

	date = DOM;
	month = MONTH;
	year = YEAR;
	
	while (1) 
	{
		// Get and display the current time on LCD
		//HH:MM:SS
		CmdLCD(0X0C);
		CmdLCD(0x80);
		if(h3>-1)
		{
			hour=HOUR=h3;
			h3=-1;
		}
		hour=HOUR;
		if(m3>-1)
		{
			min=MIN=m3;
			m3=-1;
		}
		min=MIN;
		if(s3>-1)
		{
			sec=SEC=s3;
			s3=-1;
		}

		if(day1>-1)
		{
			DOW=day1;
			day1=-1;
		}
		sec=SEC;
		CharLCD((hour/10)+48);
		CharLCD((hour%10)+48);
		CharLCD(':');
		CharLCD((min/10)+48);
		CharLCD((min%10)+48);
		CharLCD(':');
		CharLCD((sec/10)+48);
		CharLCD((sec%10)+48);
		CmdLCD(0x8A);
		StrLCD(week[DOW]);

		CmdLCD(0xC0);
		if(d3>-1)
		{
			date=DOM=d3;
		}

		if(mon3>-1)
		{
			month=MONTH=mon3;
		}

		if(y5>-1)
		{
			year=YEAR=y5;
		}
		date = DOM;
		month = MONTH;
		year = YEAR;
		CharLCD((date/10)+48);
		CharLCD((date%10)+48);
		CharLCD('/');
		CharLCD((month/10)+48);
		CharLCD((month%10)+48);
		CharLCD('/');
		U32LCD(year);
		CmdLCD(0X94);
		StrLCD("ALARM 1--> ");
		U32LCD(mh3);
		CharLCD(':');
		U32LCD(mm3);
		CharLCD(':');
		U32LCD(ms3);
		CmdLCD(0Xd4);
		StrLCD("ALARM 2--> ");
		U32LCD(mh23);
		CharLCD(':');
		U32LCD(mm23);
		CharLCD(':');
		U32LCD(ms23);
		



		if(((IOPIN0>>sw1)&1)==0)//EDIT SWITCH IS PRESSED.
		{
			int r=1;
			while(((IOPIN0>>sw1)&1)==0);
			//EDIT SWITCH IS RELEASED.
			while(r)   //EDIT MENU
			{
				CmdLCD(0x01);
				CmdLCD(0x80);
				StrLCD("1.RTC EDIT.");
				CmdLCD(0x94);
				StrLCD("2.UPDATE MEDICINE");
				CmdLCD(0xD4);
				StrLCD("  TIMINGS.");
				KeyC = KeyDetect();
				if(KeyC=='1' ||KeyC=='2')
				{
					r=0;
				}
			}
			if((KeyC=='1') || (KeyC=='2'))
			{

				if(KeyC=='1')//FROM HERE RTC EDIT MENU STARTED.
				{
					int i=1;
					while(i)
					{
						CmdLCD(0x01);
						CmdLCD(0X80);
						StrLCD("1.HOURS   2.MINUTES");
						CmdLCD(0XC0);
						StrLCD("3.SECONDS 4.DAY");	
						CmdLCD(0X94);
						StrLCD("5.DATE    6.MONTH");
						CmdLCD(0XD4);
						StrLCD("7.YEAR    8.EXIT");
						KeyC = KeyDetect();


						if(KeyC==49)
						{ 	int h1,h2;
							h3=25;
							while(h3>=24)
							{int hou=1;
								while(hou)
								{			
									CmdLCD(0x01);
									CmdLCD(0X80);
									StrLCD("HOURS:");
									CmdLCD(0x0f);
									h1=(KeyDetect());
									CharLCD(h1);
									h2=(KeyDetect());
									CharLCD(h2);
									CmdLCD(0x0c);
									if((h1>='0' && h1<='9') && (h2>='0' && h2<='9'))
									{ hou=0;
									}
								}
								h3=(h1-'0')*10+(h2-'0');
								delay_ms(500);
								if(h3>=24)
								{ CmdLCD(0x98);
									StrLCD("Invalid hours");
									delay_ms(900);
									StrLCD("             ");
								}
							}
						}
						else if(KeyC==50)
						{
							int m1,m2;
							m3=60;
							while(m3>=60)
							{ int M=1;
								while(M)
								{
									CmdLCD(0x01);
									CmdLCD(0X80);
									StrLCD("MINUTES:");
									CmdLCD(0x0f);
									m1=(KeyDetect());
									CharLCD(m1);
									m2=(KeyDetect());
									CharLCD(m2);
									CmdLCD(0x0c);
									if((m1>='0' && m1<='9') && (m2>='0' && m2<='9'))
									{ M=0;
									}
								}
								m3=(m1-'0')*10+(m2-'0');
								delay_ms(500);
								if(m3>59)
								{ CmdLCD(0x98);
									StrLCD("Invalid Mins");
									delay_ms(900);
									StrLCD("            ");
								}
							}
						}
						else if(KeyC==51)
						{
							int s1,s2,S;
							s3=60;
							while(s3>=60)
							{ S=1;
								while(S)
								{
									CmdLCD(0x01);
									CmdLCD(0X80);
									StrLCD("SECONDS:");
									CmdLCD(0x0f);
									s1=(KeyDetect());
									CharLCD(s1);
									s2=(KeyDetect());
									CharLCD(s2);
									CmdLCD(0x0c);
									if((s1>='0' && s1<='9') && (s2>='0' && s2<='9'))
									{ S=0;
									}
								}
								s3=(s1-'0')*10+(s2-'0');
								delay_ms(500);
								if(s3>=60)
								{ CmdLCD(0x98);
									StrLCD("Invalid Sec");
									delay_ms(900);
									StrLCD("           ");
								}
							}
						}
						else if(KeyC==52)
						{
							day1=10;
							while(day1>=7)
							{ int DAY=1;
								while(DAY)
								{
									CmdLCD(0x01);
									CmdLCD(0X80);
									StrLCD("0:SUN 1:MON 2:TUE");
									delay_ms(100);
									CmdLCD(0Xc0);
									StrLCD("3:WED 4:THU 5:FRI");
									delay_ms(100);
									CmdLCD(0X94);
									StrLCD("6:SAT");
									delay_ms(500);
									CmdLCD(0XD4);
									StrLCD("Which Day? ");
									CmdLCD(0x0f);
									day1=(KeyDetect()-'0');

									U32LCD(day1);
									if(day1>=0 && day1<=6)
									{	CmdLCD(0X0C);
										StrLCD("-->");
										StrLCD(week[day1]);
										DAY=0;
									}
									if(day1>=7)
									{	CmdLCD(0x01);
										CmdLCD(0x98);
										StrLCD("Invalid day");
										delay_ms(900);
										StrLCD("           ");
									}
								}
								delay_ms(500);
							}
						}
						else if(KeyC==53)
						{
							int d1,d2,D;
							d3=32;
							while(d3>=32)
							{
								D=1;
								while(D)
								{
									CmdLCD(0x01);
									CmdLCD(0X80);
									StrLCD("DATE:");
									CmdLCD(0x0f);
									d1=(KeyDetect());
									CharLCD(d1);
									d2=(KeyDetect());
									CharLCD(d2);
									CmdLCD(0x0c);
									delay_ms(100);
									if((d1>='0' && d1<='9') && (d2>='0' && d2<='9'))
									{ D=0;
									}
								}
								d3=(d1-'0')*10+(d2-'0');
								if(d3>=31)
								{ CmdLCD(0x98);
									StrLCD("Invalid Date");
									delay_ms(900);
									StrLCD("            ");
								}
							}
						}
						else if(KeyC==54)
						{
							int mon1,mon2,M;
							mon3=13;

							while(mon3>=13)
							{
								M=1;
								while(M)
								{
									CmdLCD(0x01);
									CmdLCD(0X80);
									StrLCD("MONTH:");
									CmdLCD(0x0f);
									mon1=(KeyDetect());
									CharLCD(mon1);
									mon2=(KeyDetect());
									CharLCD(mon2);
									CmdLCD(0x0c);
									delay_ms(100);
									if((mon1>='0' && mon1<='9') && (mon2>='0' && mon2<='9'))
									{ 
										mon3=(mon1-'0')*10+(mon2-'0');
										M=0;
									}
								}
								if(mon3>=13)
								{ CmdLCD(0x98);
									StrLCD("Invalid Month");
									delay_ms(900);
									StrLCD("             ");
								}
							}
						}
						else if(KeyC==55)
						{
							int y1,y2,y3,y4;
							int Y=1;
							while(Y)
							{
								CmdLCD(0x01);
								CmdLCD(0X80);
								StrLCD("YEAR:");
								CmdLCD(0x0f);
								y1=(KeyDetect());
								CharLCD(y1);
								y2=(KeyDetect());
								CharLCD(y2);
								y3=(KeyDetect());
								CharLCD(y3);
								y4=(KeyDetect());
								CharLCD(y4);
								CmdLCD(0x0c);
								if((y1>='0' && y1<='9') && (y2>='0' && y2<='9')&& (y3>='0' && y3<='9') &&(y4>='0' && y4<='9'))
								{ Y=0;
								}

							}
							y5=(((((y1-'0')*10)+(y2-'0'))*10+(y3-'0'))*10+(y4-'0'));
							delay_ms(500);
						}
						else if(KeyC==56)
						{
							i=0;

						}							
						if(KeyC>='1' && KeyC<='8')
						{
						}
						else
						{
							CmdLCD(0x01);
							CmdLCD(0XC0);
							StrLCD("  PRESS 1 TO 8 ONLY");
							delay_ms(900);
						}
						CmdLCD(0x01);

					}//WHILE LOOP FOR RTC EDIT MENU
				}//UPTO RTC EDIT SECTION
				
/***************************************************************************************************************/
				
				if(KeyC=='2')//FROM HERE MEDICINE TIME UPDATE SECTION.
				{ 
					CmdLCD(0x01);
				CmdLCD(0x80);
				StrLCD("1.Alarm 1");
				CmdLCD(0xc0);
						CharLCD(' ');
						CharLCD(' ');
						U32LCD(mh3);
						CharLCD(' ');
						CharLCD(' ');
						CharLCD(':');
						U32LCD(mm3);
						CharLCD(' ');
						CharLCD(' ');
						CharLCD(':');
						U32LCD(ms3);
				CmdLCD(0x94);
					StrLCD("2.Alarm 2");
				CmdLCD(0XD4);
						CharLCD(' ');
						CharLCD(' ');
						U32LCD(mh23);
						CharLCD(' ');
						CharLCD(' ');
						CharLCD(':');
						U32LCD(mm23);
						CharLCD(' ');
						CharLCD(' ');
						CharLCD(':');
						U32LCD(ms23);
						
					KeyC = KeyDetect();
				if(KeyC=='1')//ALARAM 1 SECTION.
				{ 
					int j=1;
					while(j)
					{ 
						CmdLCD(0X01);
						CmdLCD(0X80);
						StrLCD("1.HOUR 2.MIN 3.SEC");
						CmdLCD(0XC0);
						CharLCD(' ');
						CharLCD(' ');
						U32LCD(mh3);
						CharLCD(' ');
						CharLCD(' ');
						CharLCD(':');
						U32LCD(mm3);
						CharLCD(' ');
						CharLCD(' ');
						CharLCD(':');
						U32LCD(ms3);
						CmdLCD(0XDF);
						StrLCD("4.EXIT");
						KeyC = KeyDetect();
						if((KeyC>='1') && (KeyC<='4'))
						{


							if(KeyC=='1')
							{ 					
								int mh1,mh2;
								mh3=25;
								while(mh3>=24)
								{ int h=1;
									while(h)
									{
										CmdLCD(0x01);
										CmdLCD(0X80);
										StrLCD("HOURS:");
										CmdLCD(0x0f);
										mh1=(KeyDetect());
										CharLCD(mh1);
										mh2=(KeyDetect());
										CharLCD(mh2);
										CmdLCD(0x0c);
										if((mh1>='0' && mh1<='9') && (mh1>='0' && mh1<='9'))
										{ h=0;
										}
										delay_ms(500);
									}
									mh3=(mh1-'0')*10+(mh2-'0');
									if(mh3>=24)
									{
										CmdLCD(0X98);
										StrLCD("Invalid hours");
										delay_ms(900);
										StrLCD("             ");
									}	
								}
							}

							else if(KeyC=='2')
							{
								int mm1,mm2;
								mm3=60;
								while(mm3>=60)
								{
									int m=1;
									while(m)
									{
										CmdLCD(0x01);
										CmdLCD(0X80);
										StrLCD("MINUTES:");
										CmdLCD(0x0f);
										mm1=(KeyDetect());
										CharLCD(mm1);
										mm2=(KeyDetect());
										CharLCD(mm2);
										CmdLCD(0x0c);
										if((mm1>='0' && mm1<='9') && (mm1>='0' && mm1<='9'))
										{ m=0;
										}
										delay_ms(500);
									}
									mm3=(mm1-'0')*10+(mm2-'0');
									if(mm3>59)
									{ 
										CmdLCD(0X98);
										StrLCD("Invalid Mins");
										delay_ms(900);
										StrLCD("            ");
									}
								}
							}
							else if(KeyC=='3')
							{
								int ms1,ms2;
								ms3=60;
								while(ms3>=60)
								{ int s=1;
									while(s)
									{
										CmdLCD(0x01);
										CmdLCD(0X80);
										StrLCD("SECONDS:");
										CmdLCD(0x0f);
										ms1=(KeyDetect());
										CharLCD(ms1);
										ms2=(KeyDetect());
										CharLCD(ms2);
										CmdLCD(0x0c);
										if((ms1>='0' && ms1<='9') && (ms1>='0' && ms1<='9'))
										{ s=0;
										}
										delay_ms(500);
									}
									ms3=(ms1-'0')*10+(ms2-'0');
									if(ms3>59)
									{
										CmdLCD(0X98);
										StrLCD("Invalid Sec");
										delay_ms(900);
										StrLCD("           ");
									}
								}
							}
							else if(KeyC=='4')
							{
								j=0;

							}				
						}
						if(KeyC=='1' ||KeyC=='2' ||KeyC=='3' ||KeyC=='4')
						{

							CmdLCD(0x01);
						}
						else
						{
							CmdLCD(0x01);
							CmdLCD(0XC0);
							StrLCD("press only 1,2,3,4");
							delay_ms(900);
						}
					}//WHILE LOOP OF ALARAM 1 SECTION.
				}//UPTO THIS ALARM 1 SECTION.
				
				if(KeyC=='2')//ALARAM 2 SECTION.
				{ 
					int j=1;
					while(j)
					{ 
						CmdLCD(0X01);
						CmdLCD(0X80);
						StrLCD("1.HOUR 2.MIN 3.SEC");
						CmdLCD(0XC0);
						CharLCD(' ');
						CharLCD(' ');
						U32LCD(mh23);
						CharLCD(' ');
						CharLCD(':');
						U32LCD(mm23);
						CharLCD(' ');
						CharLCD(':');
						U32LCD(ms23);
						CmdLCD(0XDF);
						StrLCD("4.EXIT");
						KeyC = KeyDetect();
						if((KeyC>='1') && (KeyC<='4'))
						{
							if(KeyC=='1')
							{ 					
								int mh21,mh22;
								mh23=25;
								while(mh23>=24)
								{ int h=1;
									while(h)
									{
										CmdLCD(0x01);
										CmdLCD(0X80);
										StrLCD("HOURS:");
										CmdLCD(0x0f);
										mh21=(KeyDetect());
										CharLCD(mh21);
										mh22=(KeyDetect());
										CharLCD(mh22);
										CmdLCD(0x0c);
										if((mh21>='0' && mh21<='9') && (mh21>='0' && mh21<='9'))
										{ h=0;
										}
										delay_ms(500);
									}
									mh23=(mh21-'0')*10+(mh22-'0');
									if(mh23>=24)
									{
										CmdLCD(0X98);
										StrLCD("Invalid hours");
										delay_ms(900);
										StrLCD("             ");
									}	
								}
							}

							else if(KeyC=='2')
							{
								int mm21,mm22;
								mm23=60;
								while(mm23>=60)
								{
									int m=1;
									while(m)
									{
										CmdLCD(0x01);
										CmdLCD(0X80);
										StrLCD("MINUTES:");
										CmdLCD(0x0f);
										mm21=(KeyDetect());
										CharLCD(mm21);
										mm22=(KeyDetect());
										CharLCD(mm22);
										CmdLCD(0x0c);
										if((mm21>='0' && mm21<='9') && (mm21>='0' && mm21<='9'))
										{ m=0;
										}
										delay_ms(500);
									}
									mm23=(mm21-'0')*10+(mm22-'0');
									if(mm23>=60)
									{ 
										
										CmdLCD(0X98);
										StrLCD("Invalid Mins");
										delay_ms(900);
										StrLCD("            ");
									}
								}
							}
							else if(KeyC=='3')
							{
								int ms21,ms22;
								ms23=70;
								while(ms23>=60)
								{ int s=1;
									while(s)
									{
										CmdLCD(0x01);
										CmdLCD(0X80);
										StrLCD("SECONDS:");
										CmdLCD(0x0f);
										ms21=(KeyDetect());
										CharLCD(ms21);
										ms22=(KeyDetect());
										CharLCD(ms22);
										CmdLCD(0x0c);
										if((ms21>='0' && ms21<='9') && (ms21>='0' && ms21<='9'))
										{ s=0;
										}
										delay_ms(500);
									}
									ms23=(ms21-'0')*10+(ms22-'0');
									if(ms23>=60)
									{
										CmdLCD(0X98);
										StrLCD("Invalid Sec");
										delay_ms(900);
										StrLCD("           ");
									}
								}
							}
							else if(KeyC=='4')
							{
								j=0;

							}				
						}

						if(KeyC=='1' ||KeyC=='2' ||KeyC=='3' ||KeyC=='4')
						{

							CmdLCD(0x01);
						}
						else
						{
							CmdLCD(0x01);
							CmdLCD(0XC0);
							StrLCD("press only 1,2,3,4");
							delay_ms(900);
						}

					}//while loop ALARM 2 SECTION
				}//UPTO THIS ALARM 2 SECTION
			}//UPTO THIS MEDICINE TIME UPDATE 
		}//KEYC CHECKER FOR BOTH RTC EDIT SECTION AND MEDICINE TIME UPDATE SECTION.
	}//UPTO THIS EDIT SWITCH SECTION.
		
		if(hour==mh3 && min==mm3 && sec==ms3)//CHECKING TIME WITH ALARM 1.
		{
			CmdLCD(0X01);
			CmdLCD(0xC0);
			StrLCD("First Medicine Time!");
			BUZZER_ON;
			while(((IOPIN0>>sw2)&1)==1);
			delay_ms(100);
			while(((IOPIN0>>sw2)&1)==0);
			delay_ms(100);
			BUZZER_OFF;
			CmdLCD(0X01);
		}
		if(hour==mh23 && min==mm23 && sec==ms23)//CHECKING TIME WITH ALARM 2.
		{
			CmdLCD(0X01);
			CmdLCD(0xC0);
			StrLCD("  Second Medicine ");
			CmdLCD(0x94);
			StrLCD("       Time!      ");
			BUZZER_ON;
			while(((IOPIN0>>sw2)&1)==1);
			delay_ms(100);
			while(((IOPIN0>>sw2)&1)==0);
			delay_ms(100);
			BUZZER_OFF;
			CmdLCD(0X01);
		}
		CmdLCD(0X0C);
	}//MAIN WHILE LOOP
}//MAIN

// RTC Initialization: Configures and enables the RTC
void RTC_Init(void) 
{
	// Reset the RTC
	CCR = RTC_RESET;	
	// Set prescaler integer part
	PREINT = PREINT_VAL;
	// Set prescaler fractional part
	PREFRAC = PREFRAC_VAL;
	// Enable the RTC
	CCR = RTC_ENABLE;
}
// RTC_SetTime: Sets the time and date on the RTC
void RTC_SetTimeInfo(unsigned int hour, unsigned int min, unsigned int sec) 
{ 
	
	// Reset the RTC
	CCR = RTC_RESET;
	//CCR = 0x02;
	// Set hours
	HOUR = hour;
	// Set minutes
	MIN = min;	
	// Set seconds
	SEC = sec;
	// Enable the RTC
	CCR = RTC_ENABLE;
	//CCR = 0x01;
}

void RTC_SetDateInfo(unsigned int date, unsigned int month, unsigned int year)
{
	//set DOM
	DOM = date;
	//set MONTH
	MONTH = month;
	//set year
	YEAR = year;	
}


