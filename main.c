#include <reg52.h>
#include "key.h"
#include "key2.h"

void tim0_initial();
void show();
void debug_on();
void debug_off();
unsigned char feedback_start0=0, feedback_start1=0;
unsigned int num=0, second=0, feedback0=0, feedback1=0, feedback2=0, feedback3=0, feedback4=0, feedback5=0;
unsigned char LED=0, SW=0;

sbit OE = P3^4;			//ʹ�ܶˣ��͵�ƽ��Ч�����Ƶƺͼ̵���
sbit LE_Led = P3^7;		//����ˣ��͵�ƽ���棬���Ƶ�
sbit LE_Relay = P3^6;	//����ˣ��͵�ƽ���棬���Ƽ̵���

sbit input1=P2^1;

void main()
{
	OE = 0;
	LED = 0;
	show();
	SW = 0;
	show();

	independent_key_initial();
	// independent_key_initial_j();
	tim0_initial();

	while(1)
	{
		independent_key_scan();
		// independent_key_scan_j();

		/* ���ã��ж��ĸ��󰴼�������, ͬʱ�������󰴼���Ӧ�ź�ͨ·�ķ���, ��ʱ��ÿ2ms�������������� */
		/* ����1����������ˣ�GPO(P0.0~P0.5)��Ӧ�İ����������̵�����ͨ */
		/* ����2�����԰��С�ƻ��� */
		if ((keyname_i&0x80) == 0x80)
		{
			keyname_i&=~0x80;
			switch(keyname_i)
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
					feedback_start0 = 1;//�����������
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
					feedback_start0 = 2;
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
					feedback_start0 = 3;
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
					feedback_start1 = 1;
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
					feedback_start1 = 2;
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
					feedback_start1 = 3;
					break;
				default:
					break;
			}
		}

		/* ���ã����԰��ϵ�С�������Գ��� */
		/* �ж�С�����Ƿ񱻰���,��������ˣ�������·��ͨ*/
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


/* ���ã���12Mhz�µľ����£�ÿ2ms����Ķ�ʱ0�����ڷ�ʽ0�ĳ�ʼ������*/
void tim0_initial()
{
	TMOD = 0x01;//�����ڷ�ʽ1��Ҳ����16λ��ʱ��
	TH0 = (65536-2000)/256;//12MHz, 2ms
	TL0 = (65536-2000)%256;
	EA = 1;//�����ж�
	ET0 = 1;//����ʱ��0�ж�
	TR0 = 1;//������ʱ��0
}

/* ���ã���ʱ0�����ڷ�ʽ0������жϳ���*/
void tim0() interrupt 1
{
	//��ΪӲ����������־��Ҳ���ù��жϱ�־λ
	TH0 = (65536-2000)/256;//��װ��ֵ����Ϊ�����Զ���װ
	TL0 = (65536-2000)%256;

	independent_tim0_key_scan();
	// independent_tim0_key_scan_j();

	/*��·ֻ������һ·����*/
	/* ���ã������·���ˣ�����GPI(P2.0~P2.2)����һ·��·������ص�Ƭ������Ƭ����⵽��·��·���𰴼���*/
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
			// if(P2&(1<<1) == 0){//==���ȼ���&���ߣ�����ܿӵ���
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

	/*��·ֻ������һ·����*/
	/*���ã������·���ˣ�����GPI(P2.3~P2.5)����һ·��·������ص�Ƭ������Ƭ����⵽��·��·���𰴼���*/
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
			// if(P2&(1<<4) == 0){//==���ȼ���&���ߣ�����ܿӵ���
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
}

/* ���ã���ʾ������� */
/* ����������������������ʱ��ͻ�Ӱ������P0����˲��ܿ�һ��������������һ��λ�ĵ�ƽ*/
/* ����Ҫ�ȿ���������Ȼ���������P0������������*/
/* ����������ÿ��λ�����м����LED����SW������LED��SW�����������ֱ��¼�����ƺͼ̵�����״̬�������迼��573��Ȼ���ٴ��͵�P0 */
void show()
{
	//�����������
	LE_Led = 1;
	P0 = LED;
	LE_Led = 0;
	P0 = 0x00;
	//�����������
	LE_Led = 1;
	P0 = LED;
	LE_Led = 0;
	P0 = 0x00;
	//������̵�����С��
	LE_Relay = 1;
	P0 = SW;
	LE_Relay = 0;
	P0 = 0x00;
	//������̵�����С��
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