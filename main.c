/*
 * NES_Test.c
 *
 * Created: 2/26/2018 12:38:30 PM
 * Author : Kevin
 */
#include <avr/eeprom.h>
#include "Custom_io.c"
#include "CUSTOM_PWM.c"
#include <bit.h>
#include <stdio.h>
#include <stdlib.h>
#include <timer.h>
#include <avr/io.h>
#include <avr/interrupt.h>
// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
#define EEPROM __attribute__((section(".eeprom")))

typedef struct {
	int VarInt1;
	char Array1[2];
}struct_eepvar;

struct_eepvar EEPROM Ep;



typedef struct _task {
    /*Tasks should have members that include: state, period,
        a measurement of elapsed time, and a function pointer.*/
    signed char state; //Task's current state
    unsigned long int period; //Task period
    unsigned long int elapsedTime; //Time elapsed since last task tick
    int (*TickFct)(int); //Task tick function
} task;

unsigned short add = 0;
unsigned char a = 0x00;
char score[2] = {0, 0};
char high[2] = {0, 0};
int intscore;
int highScore;

char temp[2] = {0, 0};
enum STATE {wait, display, over};

int SM2tick(int state){
	unsigned short i = add;
	int array[] =	{4, 4, 4, 4, 4, 4, 3, 4, 4, 1, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 0, 4, 4, 4, 2, 4, 4, 4, 1, 4, 4, 4, 4, 0, 4, 4, 4, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
	int array2[] =	{4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 4, 4, 4, 4, 4, 4, 2, 4, 4, 1, 4, 4, 3, 4, 4, 4, 4, 4, 4, 3, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 2, 3, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
	switch (state){
		case wait:
			if(a == 0x01)
			{
				state = display;
				LCD_Clear();
			}
			else
			{
				state = wait;
			}
			break;
		case display:
			if(add == 51)
			{
				state = over;
			}
			else
			{
				state = display;
			}
			break;
		case over:
			if(a == 0x08)
			{
				state = wait;
			}
			else
			{
				state = over;
			}
			break;
		default:
			state = wait;
			break;
	}
	switch (state){
		case display:
		if(a == 0x10)
		{
			if(score[1] == 9)
			{
				score[1] = 0;
				score[0] ++;
			}
			else
			{
				score[1] ++;
			}
		}
		LCD_Cursor(0,1);

 		for (unsigned short j = 0; j < 17; ++j){
 			LCD_Char(array[i]);
			++i;
 		}
		unsigned short i = add;
		LCD_Cursor(1,1);
		for (unsigned short k = 0; k < 17; ++k){
			LCD_Char(array2[i]);
			++i;
		}
		add = add + 1;
		if (add == 28){
			
		}
		break;
	case wait:
		score[0] = 0;
		score[1] = 0;
		add = 0;
		score[1] = 0;
		LCD_Cursor(0,0);
		LCD_String("PRESS A TO BEGIN");
		LCD_Cursor(1,0);
		LCD_String("HIGH SCORE ");
	
		eeprom_read_block(high, Ep.Array1, 2);
		LCD_Cursor(1,11);
		LCD_Char(high[0] + '0');
		LCD_Cursor(1,12);
		LCD_Char(high[1] + '0');
		break;
	case over:
		intscore = score[0] *10;
		intscore = intscore + score[1];
		highScore = high[0] * 10;
		highScore = highScore + high[1];
		if((intscore >= highScore) || (high[0]<0 || high[0] > 1))
		{
			highScore = intscore;
			for(int i = 0; i<2; i++)
			{
				high[i] = score[i];
				
			}
			eeprom_write_block(high, Ep.Array1, 2);
		}
		LCD_Cursor(0,0);
		LCD_String("GAME OVER");
		LCD_Cursor(1,0);
		LCD_Char(score[0] + '0');
		LCD_Cursor(1,1);
		LCD_Char(score[1] + '0');
	default:
		break;
	}
	return state;
}

int max = 51;
int i = 0;
double platinum [15] = {493.88, 587.33, 659.25, 0, 587.33, 587.33, 587.33, 493.88, 440.00, 493.88, 440.00, 392.00, 392.00, 392.00, 392.00 };
// d, c#, c, b, 0, g, d, 0, g, b, 0, g, d, 0, b, g, f#, g, A, g, f#, g,  / d, b, 0, g, f#, 0, e, f#, 0, g, a, 0, b, c, b, a, d,//,0 c#, d, 0, e, f#, 0, d, c, 0, a,// b, f#, 0, d, b, 0 g, d
double krusty [51] = {1174.66, 1108.73, 1046.50, 987.77, 0, 783.99, 587.33, 0, 783.99, 987.77, 0, 783.99, 1174.66, 0, 987.77, 1567.98, 1479.98, 1567.98, 1760.00, 1567.95, 1479.98, 1567.98, 0,
1174.66, 987.77, 0, 783.99, 739.99, 0, 659.25, 739.99, 0, 783.99, 880.00, 0, 987.77, 1046.50, 987.77, 880.00, 1174.66, 1174.66, 1108.73, 1174.66, 1174.66, 1318.51, 1479.98, 1479.98, 1174.66, 1046.50, 1046.50, 880.00, /*987.77, 987.77, 987.77 ,739.99, 0, 1587.33, 987.77, 0, 783.99, 587.33, 587.33, 587.33,*/};
enum States {Start, Wait, On, OnLong, Off} state;
void ticksm()
{
	switch(state)
	{
		case Start:
		state = Wait;
		break;
		case Wait:
		if(a == 0x01)
		{
			state = On;
		}
		else if(a == 0x02)
		{
			state = OnLong;
		}
		else
		{
			state = Wait;
		}
		break;
		case On:
		if(i < max)
		{
			state = On;
		}
		else
		{
			state = Off;
		}
		break;
		case OnLong:
		if(a == 0x04)
		{
			state = Off;
		}
		else if(i < max)
		{
			state = OnLong;
		}
		else
		{
			i= 3;
			state = OnLong;
		}
		
		break;
		case Off:
		if((~PINA & 0x01) == 0x00)
		{
			state = Wait;
		}
		else
		{
			state = Off;
		}
		break;
	}
	switch(state)
	{
		case Start:
		i = 0;
		break;
		case Wait:
		i = 0;
		set_PWM(0);
		break;
		case On:
		set_PWM(krusty[i]);
		//set_PWM(platinum[i]);
		i++;
		break;
		case OnLong:
		set_PWM(krusty[i]);
		//set_PWM(platinum[i]);
		i++;
		break;
		case Off:
		set_PWM(0);
		break;
		
	}
}




								//A = 0x01, B = 0x02, Select = 0x04, AB = 0x04, Start = 0x08, STARTSELECT = 0x28, UP = 0x10, DOWN = 0x20, LEFT = 0x40, RIGHT = 0x80
int main(void)					//SELECTA = 0x08,  //SELECTB = 0x0C, //STARTA = 0x01, STARTB = 0x18. //UPHARDPRESS = 0x80, //DOWNHARDPRESS = 0x80.
{
	
	
    DDRA = 0x03;
	DDRB = 0xFF;
	PORTA = 0xFC;
	PORTB = 0x00;
	DDRC = 0xFF;
	PORTC = 0x00;
	DDRD = 0xFF;
	PORTD = 0x00;

	
	PWM_on();
	static task task1;
	task *tasks[] = { & task1};
	task1.state = -1;
	task1.period = 1;
	task1.elapsedTime = 1;
	task1.TickFct = &SM2tick;
	LCD_Init();
	LCD_Clear();
	LCD_Custom_Char(0, CharacterLeft);  /* Build Character1 at position 0 */
	LCD_Custom_Char(1, CharacterDown);  /* Build Character2 at position 1 */
	LCD_Custom_Char(2, CharacterUp);  /* Build Character3 at position 2 */
	LCD_Custom_Char(3, CharacterRight);  /* Build Character4 at position 3 */
	LCD_Custom_Char(4, Blank);
	
	unsigned timer_period = 1;
	unsigned long disp = 65;
	//unsigned long controller = 1;
	TimerSet(timer_period);
	TimerOn();
	unsigned short u = 0;
    while (1) 
    {
		if(disp >= 65)
		{
			ticksm();
			for (u = 0; u < 1; ++u){
				if (tasks[u]->elapsedTime == tasks[u]->period){
					tasks[u]->state = tasks[u]->TickFct(tasks[u]->state);
					tasks[u]->elapsedTime = 0;
				}
				tasks[u]->elapsedTime += 1;
			}
			disp = 0;
			
		}
		//if()
		
		
		PORTA = 0xF1;
		unsigned i = 0;
		while (i < 10){
			++i;
		}
		PORTA = 0xF0;
		a = 0;
		for (unsigned i = 0; i < 8; ++i){
			PORTA = 0xF0;
			if ((~PINA & 0x10) == 0x10){
				a = a + 0x01;
				a = (a << i);
			}
			PORTA = 0xF2;
		}
		//PORTB = a;
		
			
			while(!TimerFlag);
			TimerFlag = 0;
			disp += timer_period;
		
    }
}

