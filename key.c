#include "reg52.h"
	
#define keyscan_mask 0x00//0������Ϊ�͵�ƽ��0xff������Ϊ�ߵ�ƽ

sfr in0=0x90;//P1
#define in0_mask (1<<0)
sfr in1=0x90;
#define in1_mask (1<<1)
sfr in2=0x90;
#define in2_mask (1<<2)

sfr in3=0x90;
#define in3_mask (1<<3)
sfr in4=0x90;
#define in4_mask (1<<4)
sfr in5=0x90;
#define in5_mask (1<<5)

static void independent_key_check(void);
static void independent_key_scan_in(void);
//const+������÷��������flashrom��
static const unsigned char keytab[] = {0x01,0x02,0x03,0x04,0x05,0x06};//����

// static unsigned char key20ms,keytime,keystu;
static unsigned char key20ms,keystu;
static unsigned int keytime;
//ֻ����������������ŵ�״̬���ѣ����Դ��8�����ŵĵ�ƽ������һ��ʼȫ���ǵ͵�ƽ��
static unsigned char  keyscan=0x00;				

unsigned char keyname_i;


/*
**��Ҫ������ʼ��
*/
void independent_key_initial(void)
{
	// key20ms=10;
	key20ms=30;
	keystu=0x07;			//����0000 0111;���Ǳ�����3λ����������ԭ����ʲô����ʲô����
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
		** ���6���������Ŷ��ǵ͵�ƽ������û�а��������£�û�����ǵ͵�ƽ��, ����һ��1��������keystu�ĵ�3λ;
		** ���������һ�������ǵ͵�ƽ��������һ��0����<<�������,������keystu�ĵ�3λ��������ͬʱҲ��ɨ���־λ��0
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
		/* ����״̬�ж�,��ѡһ����
		** �ɴ˲�����ѡһ�������⣬�Ǿ��ǵ�������ѡһ�������ʱ��Ҫ������Щ�����ǹ��������飬Ҳ�����������߶����
		** �¼���Ҫ�������������keytime��0����ܹؼ�������©��һ���ʧ���ˡ�
		*/
		if (keystu==0x00)					//000
		{
			if (keytime>=500) 
			{
				keytime=0;
				independent_key_check();
			}
		}
		else if (keystu==0x02)				//010
		{
			keystu&=~0x02;					//��Ϊ000
			if (keytime>=500) 
			{
				keytime=0;
				independent_key_check();
			}
		}
		else if (keystu==0x05) 				//101
		{
			keytime=0;						//����000,Ҳ����010���Ͳ����ڰ����պ�ʱ�䣬��keytime��0
			keystu|=0x02;					//���111,ʲôҲ����
		}
		else if (keystu==0x04)				//100
		{
			keytime=0;
			independent_key_check();
		}
		
	}

}

/*
**��ʱ����ʱ����
**��Ȼ��2ms�ж�һ�Σ����ǣ����Ƽ���ɨ���λ��100ms�ſ���һ�εġ����ң�����������û���ϸ����2ms�ڱ������һ����ѭ����������������й�����20ms���ǿ��Խ��ܵġ�
*/
void independent_tim0_key_scan(void)
{
	key20ms--;
	if (key20ms==0)
	{
		key20ms=30;
		keystu|=(1<<4);
	}
	/*
	**���keystu��000�Ļ��ͼ�ʱ�䣬ÿ2ms��һ��
	*/
	if ((keystu&0x07)==0&&keytime<500)
	{
		keytime++;
	}
}

/*
**�������
*/
static void independent_key_check(void)
{

		independent_key_scan_in();//��ȷ�ϰ����Ƿ��Դ��ڰ���״̬
		if ((keyscan&0xff)!=keyscan_mask) //ֻҪ��һ�����Ų�Ϊ0���ͼ���(�����ںͺ��������ȫ��ͬ��!���ﻻ��==0�Ͳ�ͬ�����)
		{
			if ((keyname_i&(1<<7))==0)//�������λΪ0���Ϳ�������ɨ����
			{
				// �ó������°����ļ���
				if ((in0&in0_mask)==in0_mask)
					keyname_i=keytab[0]|0x80;
				else if ((in1&in1_mask)==in1_mask)
					keyname_i=keytab[1]|0x80;
				else if ((in2&in2_mask)==in2_mask)
					keyname_i=keytab[2]|0x80;
				else if ((in3&in3_mask)==in3_mask)
					keyname_i=keytab[3]|0x80;
				else if ((in4&in4_mask)==in4_mask)
					keyname_i=keytab[4]|0x80;
				else if ((in5&in5_mask)==in5_mask)
					keyname_i=keytab[5]|0x80;
			}
		}


}



/*
**������������״̬������keyscan��
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


