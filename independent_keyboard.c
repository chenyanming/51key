#include "stm8s105c6.h"
void independent_key_check(void);
void independent_key_scan_in(void);
//const+数组的用法，存放在flashrom中
static const unsigned char keytab[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10};//键名

static unsigned char key20ms,keytime,keystu;
static unsigned char  keyscan=0xff;				//只是用来存放输入引脚的状态而已，可以存放8个引脚的电平，这里一开始全部是高电平。

unsigned char keyname_i;


/*
**必要参数初始化
*/
void independent_key_initial(void)
{

#define in0(x) 	PA_##x
#define in0_mask		(1<<3)//PA3
#define in1(x) 	PC_##x
#define in1_mask		(1<<3)//PC3
#define in2(x) 	PC_##x
#define in2_mask		(1<<6)//PC6
#define in3(x) 	PC_##x
#define in3_mask		(1<<7)//PC7

/*
**上拉不带中断输入
*/

	in0(DDR)&=~in0_mask;
	in0(CR1)|=in0_mask;
	in0(CR2)&=~in0_mask;
	

	in1(DDR)&=~in1_mask;
	in1(CR1)|=in1_mask;
	in1(CR2)&=~in1_mask;
#if 0
	in2(DDR)&=~in2_mask;
	in2(CR1)|=in2_mask;
	in2(CR2)&=~in2_mask;

	in3(DDR)&=~in3_mask;
	in3(CR1)|=in3_mask;
	in3(CR2)&=~in3_mask;
#endif

/*
**变量初始化
*/

	key20ms=10;
	keystu=0x07;												//赋予0000 0111;不是保留低3位（保留就是原本是什么就是什么）。
	keytime=0;
	keyname_i=0;

}

/*
**键盘定期扫描子程序
*/
void independent_key_scan(void)
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
		**如果4个输入引脚都是高电平，左移一个1，并保留keystu的低3位;如果有其中一个引脚是低电平，就左移一个0（用<<运算符）,并保留keystu的低3位。保留的同时也对扫描标志位清0
		*/
		if ((keyscan&0xff)==0xff) 
		{
			keystu=(keystu<<1)|0x01;
			keystu&=0x07;
		}
		else 
		{
			keystu<<=1;
			keystu&=0x07;
		}
		/*按键状态判断,多选一问题
		**由此产生多选一公共问题，那就是当遇到多选一的问题的时候，要考虑哪些事情是公共的事情，也就是两个或者多个、
		**事件都要发生的事情如对keytime清0，这很关键，往往漏了一句就失败了。
		*/
		if (keystu==0x00) 														//000
		{
			if (keytime>=125) 
			{
				keytime=0;
				independent_key_check();
			}
		}
		else if (keystu==0x02)												//010
		{
			keystu&=~0x02;													//变为000
			if (keytime>=125) 
			{
				keytime=0;
				independent_key_check();
			}
		}
		else if (keystu==0x05) 												//101
		{
			keytime=0;															//不是000,也不是010，就不存在按键闭合时间，对keytime清0
			keystu|=0x02;														//变回111,什么也不做
		}
		else if (keystu==0x04)												//100
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
void independent_tim2_key_scan(void)
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
	if ((keystu&0x07)==0&&keytime<125)
	{
		keytime++;
	}
}


/*
**按键检测
*/
void independent_key_check(void)
{

		independent_key_scan_in();
		if ((keyscan&0xff)!=0xff) //只要有一个引脚不为1，就继续(不等于和恒等于是完全不同的!这里换成==0就不同结果了)
		{
			if ((keyname_i&1<<7)==0)//键名最高位为0，就开启键盘扫描了
			{
				//得出被按下按键的键名
				if ((in0(IDR)&in0_mask)==0x00) 
				{
					keyname_i=keytab[0]|0x80;
				}
				else if ((in1(IDR)&in1_mask)==0x00) 
				{
					keyname_i=keytab[1]|0x80;
				}
#if 0				
				else if ((in2(IDR)&in2_mask)==0x00) 
				{
					keyname_i=keytab[2]|0x80;
				}
				else if ((in3(IDR)&in3_mask)==0x00) 
				{
					keyname_i=keytab[3]|0x80;
				}
#endif

				
			}
		}


}



/*
**保存输入引脚状态到变量keyscan中
*/
void independent_key_scan_in(void)
{
		if((in0(IDR)&in0_mask)==in0_mask)
			keyscan|=0x10;
		else
			keyscan&=~0x10;

		if((in1(IDR)&in1_mask)==in1_mask)
			keyscan|=0x20;
		else
			keyscan&=~0x20;
#if 0			
		if((in2(IDR)&in2_mask)==in2_mask)
			keyscan|=0x40;
		else
			keyscan&=~0x40;
		if((in3(IDR)&in3_mask)==in3_mask)
			keyscan|=0x80;
		else
			keyscan&=~0x80;
#endif

}


