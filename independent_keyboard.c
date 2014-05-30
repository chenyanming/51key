#include "stm8s105c6.h"
void independent_key_check(void);
void independent_key_scan_in(void);
//const+������÷��������flashrom��
static const unsigned char keytab[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10};//����

static unsigned char key20ms,keytime,keystu;
static unsigned char  keyscan=0xff;				//ֻ����������������ŵ�״̬���ѣ����Դ��8�����ŵĵ�ƽ������һ��ʼȫ���Ǹߵ�ƽ��

unsigned char keyname_i;


/*
**��Ҫ������ʼ��
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
**���������ж�����
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
**������ʼ��
*/

	key20ms=10;
	keystu=0x07;												//����0000 0111;���Ǳ�����3λ����������ԭ����ʲô����ʲô����
	keytime=0;
	keyname_i=0;

}

/*
**���̶���ɨ���ӳ���
*/
void independent_key_scan(void)
{	
/*
**keystu�ĵ�4λΪ��ʱɨ������λ�������Ǳ�����4λ��Ȼ���õ�4λ��1�Ƚϣ���ס���ߵ�������һ���ġ�
*/
	if ((keystu&1<<4)==1<<4)
	{
		/*
		**������������״̬
		*/
		independent_key_scan_in();
		/*
		**���4���������Ŷ��Ǹߵ�ƽ������һ��1��������keystu�ĵ�3λ;���������һ�������ǵ͵�ƽ��������һ��0����<<�������,������keystu�ĵ�3λ��������ͬʱҲ��ɨ���־λ��0
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
		/*����״̬�ж�,��ѡһ����
		**�ɴ˲�����ѡһ�������⣬�Ǿ��ǵ�������ѡһ�������ʱ��Ҫ������Щ�����ǹ��������飬Ҳ�����������߶����
		**�¼���Ҫ�������������keytime��0����ܹؼ�������©��һ���ʧ���ˡ�
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
			keystu&=~0x02;													//��Ϊ000
			if (keytime>=125) 
			{
				keytime=0;
				independent_key_check();
			}
		}
		else if (keystu==0x05) 												//101
		{
			keytime=0;															//����000,Ҳ����010���Ͳ����ڰ����պ�ʱ�䣬��keytime��0
			keystu|=0x02;														//���111,ʲôҲ����
		}
		else if (keystu==0x04)												//100
		{
			keytime=0;
			independent_key_check();
		}
		
	}

}

/*
**��ʱ����ʱ����
**��Ȼ��2ms�ж�һ�Σ����ǣ����Ƽ���ɨ���λ��20ms�ſ���һ�εġ����ң�����������û���ϸ����2ms�ڱ������һ����ѭ����������������й�����20ms���ǿ��Խ��ܵġ�
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
	**���keystu��000�Ļ��ͼ�ʱ�䣬ÿ2ms��һ��
	*/
	if ((keystu&0x07)==0&&keytime<125)
	{
		keytime++;
	}
}


/*
**�������
*/
void independent_key_check(void)
{

		independent_key_scan_in();
		if ((keyscan&0xff)!=0xff) //ֻҪ��һ�����Ų�Ϊ1���ͼ���(�����ںͺ��������ȫ��ͬ��!���ﻻ��==0�Ͳ�ͬ�����)
		{
			if ((keyname_i&1<<7)==0)//�������λΪ0���Ϳ�������ɨ����
			{
				//�ó������°����ļ���
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
**������������״̬������keyscan��
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


