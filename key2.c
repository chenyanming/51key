#include "reg52.h"

#define keyscan_mask 0xff//0：按下为低电平；0xff：按下为高电平

sfr in0=0xA0;//P2
#define in0_mask (1<<0)
sfr in1=0xA0;
#define in1_mask (1<<1)
sfr in2=0xA0;
#define in2_mask (1<<2)

sfr in3=0xA0;
#define in3_mask (1<<3)
sfr in4=0xA0;
#define in4_mask (1<<4)
sfr in5=0xA0;
#define in5_mask (1<<5)

static void independent_key_check(void);
static void independent_key_scan_in(void);
//const+数组的用法，存放在flashrom中
static const unsigned char keytab[] = {0x01,0x02,0x03,0x04,0x05,0x06};//键名

// static unsigned char key20ms,keytime,keystu;
static unsigned char key20ms,keystu;
static unsigned int keytime;
//只是用来存放输入引脚的状态而已，可以存放8个引脚的电平，这里一开始全部是高电平。
static unsigned char  keyscan=0xff;				

unsigned char keyname_j;


/*
**必要参数初始化
*/
void independent_key_initial_j(void)
{
	key20ms=10;
	keystu=0x07;			//赋予0000 0111;不是保留低3位（保留就是原本是什么就是什么）。
	keytime=0;
	keyname_j=0;
}

/*
**键盘定期扫描子程序
*/
void independent_key_scan_j(void)
{	
/*
**keystu的第4位为定时扫描启动位。这里是保留第4位，然后用第4位与1比较，记住两者的掩码是一样的。
*/
	if ((keystu&1<<4)==1<<4)
	{
		/*
		**保存输入引脚状态
		*/
		independent_key_scan_in();
		/*
		** 如果6个输入引脚都是高电平，代表没有按键被按下（没按下是高电平）, 左移一个1，并保留keystu的低3位;
		** 如果有其中一个引脚是高电平，就左移一个0（用<<运算符）,并保留keystu的低3位。保留的同时也对扫描标志位清0
		*/

		if ((keyscan&0xff)==keyscan_mask) 
		{
			keystu=(keystu<<1)|0x01;
			keystu&=0x07;
		}
		else 
		{
			keystu<<=1;
			keystu&=0x07;
		}
		/* 按键状态判断,多选一问题
		** 由此产生多选一公共问题，那就是当遇到多选一的问题的时候，要考虑哪些事情是公共的事情，也就是两个或者多个、
		** 事件都要发生的事情如对keytime清0，这很关键，往往漏了一句就失败了。
		*/
		if (keystu==0x00)					//000
		{
			if (keytime>=500) 
			{
				keytime=0;
				// independent_key_check();
			}
		}
		else if (keystu==0x02)				//010
		{
			keystu&=~0x02;					//变为000
			if (keytime>=500) 
			{
				keytime=0;
				// independent_key_check();
			}
		}
		else if (keystu==0x05) 				//101
		{
			keytime=0;						//不是000,也不是010，就不存在按键闭合时间，对keytime清0
			keystu|=0x02;					//变回111,什么也不做
		}
		else if (keystu==0x04)				//100
		{
			keytime=0;
			independent_key_check();
		}
		
	}

}

/*
**定时器计时部分
**虽然是2ms中断一次，但是，控制键盘扫描的位是20ms才开启一次的。而且，在主程序中没有严格控制2ms内必须完成一个主循环。因此在主程序中工作了20ms还是可以接受的。
*/
void independent_tim0_key_scan_j(void)
{
	key20ms--;
	if (key20ms==0)
	{
		key20ms=10;
		keystu|=(1<<4);
	}
	/*
	**如果keystu是000的话就加时间，每2ms就一次
	*/
	if ((keystu&0x07)==0&&keytime<500)
	{
		keytime++;
	}
}

/*
**按键检测
*/
static void independent_key_check(void)
{

		independent_key_scan_in();//先确认按键是否仍处于按下状态
		if ((keyscan&0xff)!=keyscan_mask) //只要有一个引脚不为1，就继续(不等于和恒等于是完全不同的!这里换成==0就不同结果了)
		{
			if ((keyname_j&(1<<7))==0)//键名最高位为0，就开启键盘扫描了
			{
				// 得出被按下按键的键名
				if ((in0&in0_mask)==0x00)
					keyname_j=keytab[0]|0x80;
				else if ((in1&in1_mask)==0x00)
					keyname_j=keytab[1]|0x80;
				else if ((in2&in2_mask)==0x00)
					keyname_j=keytab[2]|0x80;
				else if ((in3&in3_mask)==0x00)
					keyname_j=keytab[3]|0x80;
				else if ((in4&in4_mask)==0x00)
					keyname_j=keytab[4]|0x80;
				else if ((in5&in5_mask)==0x00)
					keyname_j=keytab[5]|0x80;
			}
		}


}



/*
**保存输入引脚状态到变量keyscan中
*/
static void independent_key_scan_in(void)
{
	if((in0&in0_mask)==in0_mask)
		keyscan|=0x01;
	else
		keyscan&=~0x01;

	if((in1&in1_mask)==in1_mask)
		keyscan|=0x02;
	else
		keyscan&=~0x02;

	if((in2&in2_mask)==in2_mask)
		keyscan|=0x04;
	else
		keyscan&=~0x04;

	if((in3&in3_mask)==in3_mask)
		keyscan|=0x08;
	else
		keyscan&=~0x08;

	if((in4&in4_mask)==in4_mask)
		keyscan|=0x10;
	else
		keyscan&=~0x10;

	if((in5&in5_mask)==in5_mask)
		keyscan|=0x20;
	else
		keyscan&=~0x20;

}


