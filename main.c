#include <reg52.h>
#include "key.h"
#include "key2.h"

void tim0_initial();
void show();
void debug_on();
void debug_off();
void debug_upside();
unsigned char feedback_start0=0, feedback_start1=0;
unsigned int num=0, second=0, feedback0=0, feedback1=0, feedback2=0, feedback3=0, feedback4=0, feedback5=0;
unsigned char LED=0, SW=0;
unsigned char time=0;

sbit OE = P3^4;			//使能端，低电平有效，控制灯和继电器
sbit LE_Led = P3^7;		//锁存端，低电平锁存，控制灯
sbit LE_Relay = P3^6;	//锁存端，低电平锁存，控制继电器

sbit input1=P2^1;

void main()
{
	OE = 0;
	LED = 0;
	show();
	SW = 0;
	show();

	independent_key_initial();
	independent_key_initial_j();
	tim0_initial();

	while(1)
	{
		independent_key_scan();
		independent_key_scan_j();

		/* 作用：判断哪个大按键被按下, 同时，启动大按键对应信号通路的反馈, 定时器每2ms溢出后会作出反馈 */
		/* 现象1：如果按下了，GPO(P0.0~P0.5)对应的按键灯亮，继电器接通 */
		/* 现象2：测试板的小灯会亮 */
		if ((keyname_i&0x80) == 0x80)
		{
			keyname_i&=~0x80;
			switch(keyname_i)
			{
				case 0x01:
					SW ^= (1<<0);
					// SW &=~ (1<<1);
					// SW &=~ (1<<2);
					show();
					LED ^= (1<<0);
					// LED &=~ (1<<1);
					// LED &=~ (1<<2);
					show();
					// feedback_start0 = 1;//启动反馈检测
					break;
				case 0x02:
					// SW &=~ (1<<0);
					SW ^= (1<<1);
					// SW &=~ (1<<2);
					show();
					// LED &=~ (1<<0);
					LED ^= (1<<1);
					// LED &=~ (1<<2);
					show();
					// feedback_start0 = 2;
					break;
				case 0x03:
					if (time == 0)
					{
						time++;
						SW |= (1<<0);
						SW |= (1<<1);
						// SW |= (1<<2);
						show();
						LED |= (1<<0);
						LED |= (1<<1);
						LED |= (1<<2);
						show();
					}
					else 
					{
						time = 0;
						SW &=~ (1<<0);
						SW &=~ (1<<1);
						SW &=~ (1<<2);
						show();
						LED &=~ (1<<0);
						LED &=~ (1<<1);
						LED &=~(1<<2);
						show();
					}
					// feedback_start0 = 3;
					break;
				#if 0
				case 0x04:
					SW |= (1<<3);
					SW &=~ (1<<4);
					SW &=~ (1<<5);
					show();
					LED |= (1<<3);
					LED &=~ (1<<4);
					LED &=~ (1<<5);
					show();
					// feedback_start1 = 1;
					break;
				case 0x05:
					SW &=~ (1<<3);
					SW |= (1<<4);
					SW &=~ (1<<5);
					show();
					LED &=~ (1<<3);
					LED |= (1<<4);
					LED &=~ (1<<5);
					show();
					// feedback_start1 = 2;
					break;
				case 0x06:
					SW &=~ (1<<3);
					SW &=~ (1<<4);
					SW |= (1<<5);
					show();
					LED &=~ (1<<3);
					LED &=~ (1<<4);
					LED |= (1<<5);
					show();
					// feedback_start1 = 3;
					break;
				#endif
				default:
					break;
			}
		}

		#if 0
		if ((keyname_j&0x80) == 0x80)
		{
			keyname_j&=~0x80;
			switch(keyname_j)
			{
				case 0x01:
					SW |= (1<<0);
					SW &=~ (1<<1);
					SW &=~ (1<<2);
					show();
					LED |= (1<<0);
					LED &=~ (1<<1);
					LED &=~ (1<<2);
					show();
					// feedback_start0 = 1;//启动反馈检测
					break;
				case 0x02:
					SW &=~ (1<<0);
					SW |= (1<<1);
					SW &=~ (1<<2);
					show();
					LED &=~ (1<<0);
					LED |= (1<<1);
					LED &=~ (1<<2);
					show();
					// feedback_start0 = 2;
					break;
				case 0x03:
					SW &=~ (1<<0);
					SW &=~ (1<<1);
					SW |= (1<<2);
					show();
					LED &=~ (1<<0);
					LED &=~ (1<<1);
					LED |= (1<<2);
					show();
					// feedback_start0 = 3;
					break;

				case 0x04:
					SW |= (1<<3);
					SW &=~ (1<<4);
					SW &=~ (1<<5);
					show();
					LED |= (1<<3);
					LED &=~ (1<<4);
					LED &=~ (1<<5);
					show();
					// feedback_start1 = 1;
					break;
				case 0x05:
					SW &=~ (1<<3);
					SW |= (1<<4);
					SW &=~ (1<<5);
					show();
					LED &=~ (1<<3);
					LED |= (1<<4);
					LED &=~ (1<<5);
					show();
					// feedback_start1 = 2;
					break;
				case 0x06:
					SW &=~ (1<<3);
					SW &=~ (1<<4);
					SW |= (1<<5);
					show();
					LED &=~ (1<<3);
					LED &=~ (1<<4);
					LED |= (1<<5);
					show();
					// feedback_start1 = 3;
					break;
				default:
					break;
			}
		}
		#endif

		/* 作用：测试板上的小按键测试程序 */
		/* 判断小按键是否被按下,如果按下了，代表线路不通*/
		#if 0
		if ((keyname_j&0x80) == 0x80)
		{
			keyname_j&=~0x80;
			switch(keyname_j)
			{
				case 0x01:
					LED &=~ (1<<0);
					show();
					break;
				case 0x02:
					LED &=~ (1<<1);
					show();
					break;
				case 0x03:
					LED &=~ (1<<2);
					show();
					break;

				case 0x04:
					LED &=~ (1<<3);
					show();
					break;
				case 0x05:
					LED &=~ (1<<4);
					show();
					break;
				case 0x06:
					LED &=~ (1<<5);
					show();
					break;
				default:
					break;
			}
		}
		#endif
	}

}


/* 作用：在12Mhz下的晶振下，每2ms溢出的定时0工作在方式0的初始化程序*/
void tim0_initial()
{
	TMOD = 0x01;//工作在方式1，也就是16位定时器
	TH0 = (65536-2000)/256;//12MHz, 2ms
	TL0 = (65536-2000)%256;
	EA = 1;//开总中断
	ET0 = 1;//开定时器0中断
	TR0 = 1;//启动定时器0
}

/* 作用：定时0工作在方式0的溢出中断程序*/
void tim0() interrupt 1
{
	//因为硬件清除溢出标志，也不用管中断标志位
	TH0 = (65536-2000)/256;//重装初值，因为不会自动重装
	TL0 = (65536-2000)%256;

	independent_tim0_key_scan();
	independent_tim0_key_scan_j();

	#if 0
	/*三路只能启动一路反馈*/
	/* 作用：如果线路断了，就是GPI(P2.0~P2.2)其中一路断路，输入回单片机，单片机检测到哪路断路就灭按键灯*/
	switch(feedback_start0)
	{
		case 1:
			if ((P2&(1<<0)) == (1<<0)){
				LED |= (1<<0);
				show();
			}
			else {
				LED &=~ (1<<0);
				show();
			}
			break;
		case 2:
			if ((P2&(1<<1)) == (1<<1)) {
				LED |= (1<<1);
				show();
			}
			// if(P2&(1<<1) == 0){//==优先级比&更高！这里很坑爹啊
			if((P2&(1<<1)) == 0){
				LED &=~ (1<<1);
				show();
			}
			break;
		case 3:
			if ((P2&(1<<2)) == (1<<2)) {
				LED |= (1<<2);
				show();
			}
			else {
				LED &=~ (1<<2);
				show();
			}
			break;
		default:
			break;
	}

	/*三路只能启动一路反馈*/
	/*作用：如果线路断了，就是GPI(P2.3~P2.5)其中一路断路，输入回单片机，单片机检测到哪路断路就灭按键灯*/
	switch(feedback_start1)
	{
		case 1:
			if ((P2&(1<<3)) == (1<<3)){
				LED |= (1<<3);
				show();
			}
			else {
				LED &=~ (1<<3);
				show();
			}
			break;
		case 2:
			if ((P2&(1<<4)) == (1<<4)) {
				LED |= (1<<4);
				show();
			}
			// if(P2&(1<<4) == 0){//==优先级比&更高！这里很坑爹啊
			if((P2&(1<<4)) == 0){
				LED &=~ (1<<4);
				show();
			}
			break;
		case 3:
			if ((P2&(1<<5)) == (1<<5)) {
				LED |= (1<<5);
				show();
			}
			else {
				LED &=~ (1<<5);
				show();
			}
			break;
		default:
			break;
	}

	second++;
	if (second== 250)
	{
		second= 0;
	}
	#endif
}

/* 作用：显示输出程序 */
/* 分析现象：由于锁存器开的时候就会影响整个P0，因此不能开一次锁存器就设置一个位的电平*/
/* 必须要先开锁存器，然后输出整个P0，最后关锁存器*/
/* 方法：就是每次位操作中间变量LED或者SW，利用LED和SW两个变量来分别记录按键灯和继电器的状态，而无需考虑573，然后再传送到P0 */
void show()
{
	//先输出按键灯
	LE_Led = 1;
	P0 = LED;
	LE_Led = 0;
	P0 = 0x00;
	//先输出按键灯
	LE_Led = 1;
	P0 = LED;
	LE_Led = 0;
	P0 = 0x00;
	//再输出继电器和小灯
	LE_Relay = 1;
	P0 = SW;
	LE_Relay = 0;
	P0 = 0x00;
	//再输出继电器和小灯
	LE_Relay = 1;
	P0 = SW;
	LE_Relay = 0;
	P0 = 0x00;

}

void debug_on()
{
	LED |= (1<<5);
	show();
}
void debug_off()
{
	LED &=~ (1<<5);
	show();
}
void debug_upside()
{
	LED ^= (1<<5);
	show();
}
