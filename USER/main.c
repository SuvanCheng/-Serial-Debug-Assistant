#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"
#include "spi.h"
#include "flash.h"

#define SIZE 50 
#define FLASH_SIZE 8388608 
void show_string(void);
void show_hex(void);
void baud_rate(void);
void show(void);
int my_strlen(const char *str)  
{  
    const char *p = str;  
    while(*p!='\0')  
    {
			if(*p==' '&&*(p+1)==' '&&*(p+2)==' ')
			{
					break;
			}
			p++;  
    }  
    return p-str;  
}    
int main(void)
{ 
	u8 t,datatemp[SIZE],transfer_value[SIZE];
	u16 i;
	int len=0;
	u32 r=SIZE,My_flag=15,secpos;
	delay_init();
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	LED_Init();
 	LCD_Init();
	KEY_Init();
	SPI_Flash_Init();  	//SPI FLASH ��ʼ�� 
  LCD_Fill(0,0,240,320,BLACK);//�������	
 	POINT_COLOR=RED;    //��������Ϊ��ɫ 
	while(SPI_Flash_ReadID()!=W25Q64)    //��ⲻ��W25Q64
	{
		LCD_ShowString(1,1,200,16,16,"25Q64 Check Failed!");
		delay_ms(500);
		LCD_ShowString(1,1,200,16,16,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
		}LCD_ShowString(1,1,250,16,12,"25Q64 Ready!                    Page:");
		for(i=0;i<6;i++)  //����ϵ�ǰ����
		{
			secpos=(FLASH_SIZE/2-SIZE-(i*20*SIZE))/4096;   //hex��
			SPI_Flash_Erase_Sector(secpos);
			secpos=(FLASH_SIZE-SIZE-(i*20*SIZE))/4096;     //string��
			SPI_Flash_Erase_Sector(secpos);
		}
  show();
	POINT_COLOR=GREEN;    //��������Ϊ��ɫ	
	while(1)
	{
		t=KEY_Scan(0);
		switch(t)
		{				 
			case KEY0_PRES:{
				show_string();
				LED0=!LED0;
				break;
			}
			case KEY1_PRES:{
				show_hex();
				LED1=!LED1;
				break;
			}
			case WKUP_PRES:{
				baud_rate();
				break;
			}
			default:delay_ms(10);	
		}
		if(USART_RX_STA&0x8000)    //Determine whether to receive data
		{
			SPI_Flash_Write((u8*)USART_RX_BUF,FLASH_SIZE-r,SIZE);      //tail_flash��r����ַ����ʼ,д��SIZE���ȵ�����   string
			
			len=my_strlen(USART_RX_BUF);    //��ȡ�����ַ�������
			for(i=0;i<len*3;i+=3)
			{
				transfer_value[i]=(USART_RX_BUF[i/3]/16+48);
				if((USART_RX_BUF[i/3]>41&&USART_RX_BUF[i/3]<48)
				 ||(USART_RX_BUF[i/3]>57&&USART_RX_BUF[i/3]<64)
				 ||(USART_RX_BUF[i/3]>73&&USART_RX_BUF[i/3]<80)
				 ||(USART_RX_BUF[i/3]>89&&USART_RX_BUF[i/3]<96)
				 ||(USART_RX_BUF[i/3]>105&&USART_RX_BUF[i/3]<112)
				 ||(USART_RX_BUF[i/3]>121&&USART_RX_BUF[i/3]<=127))
					transfer_value[i+1]=(USART_RX_BUF[i/3]-(USART_RX_BUF[i/3]/16)*16+55);
				else
					transfer_value[i+1]=(USART_RX_BUF[i/3]-(USART_RX_BUF[i/3]/16)*16+48);
					transfer_value[i+2]=32; //�ո�
			}
			SPI_Flash_Write((u8*)transfer_value,FLASH_SIZE/2-r,SIZE);  //midd_flash��r����ַ����ʼ,д��SIZE���ȵ�����   hex
			USART_RX_STA=0;    //Update receive flag
			for(i=0;i<USART_REC_LEN;i++)
                USART_RX_BUF[i]=0xff;   //�����ܵ��ַ������
			for(i=0;i<USART_REC_LEN;i++)
                transfer_value[i]=' '; //��ת�����ַ������
			
			SPI_Flash_Read(datatemp,FLASH_SIZE-r,SIZE);
		  LCD_ShowString(1,My_flag,230,12,12,datatemp);    //��ʾ�������ַ���
			My_flag+=12;
			if(My_flag>252)
			{
				My_flag=15;
				LCD_Fill(0,15,239,259,BLACK);
			}
			r+=SIZE;
		}
	}
}
void show_string()
{
	static int page=0;
	u8 datatemp[SIZE];
	u16 i;
	u32 s=SIZE;
	LCD_Fill(0,15,239,259,BLACK);
	for(i=15;i<252;i+=12)
	{
		SPI_Flash_Read(datatemp,FLASH_SIZE-s-(page*20*SIZE),SIZE);
		LCD_ShowxNum(215,2,page+1,3,12,0);         //��ʾ��ǰҳ��
		LCD_ShowString(1,i,230,12,12,datatemp);    //��ʾ�������ַ���
		s+=SIZE;
		if(s>i*SIZE)s=SIZE;
		delay_ms(100);
	}page++;
	if(page>6)page=0;
}
void show_hex()
{
	static int page=0;
	u8 datatemp[SIZE];
	u16 i;
	u32 s=SIZE;
	LCD_Fill(0,15,239,259,BLACK);
	for(i=15;i<252;i+=12)
	{
		SPI_Flash_Read(datatemp,FLASH_SIZE/2-s-(page*20*SIZE),SIZE);
		LCD_ShowxNum(215,2,page+1,3,12,0);         //��ʾ��ǰҳ��
		LCD_ShowString(1,i,230,12,12,datatemp);    //��ʾ�������ַ���
		s+=SIZE;
		if(s>i*SIZE)s=SIZE;
		delay_ms(100);
	}page++;
	if(page>6)page=0;
}
void show()
{
	LCD_DrawRectangle(0,1,240,260);
	LCD_ShowString(3,262,230,12,12,"SET Baudrate:9600   (by KEY_UP)");
	POINT_COLOR=GREEN;    //��������ΪLVSE
	LCD_ShowString(3,275,230,12,12,"CurrentMode:Maxrecord range is 7 pages");
	LCD_ShowString(3,288,230,12,12,"KEY_0:Displays STRING from serial");
	LCD_ShowString(3,302,230,12,12,"KEY_1:Display data in HEXadecimal form");
}
void baud_rate()
{
	static int i=0;
	i++;
	switch(i)
	{
    case 0 :uart_init(1200);   LCD_ShowString(80,262,230,12,12,"1200   ");break;
		case 1 :uart_init(4800);   LCD_ShowString(80,262,230,12,12,"4800   ");break;
		case 2 :uart_init(9600);   LCD_ShowString(80,262,230,12,12,"9600   ");break;
		case 3 :uart_init(14400);  LCD_ShowString(80,262,230,12,12,"14400  ");break;
		case 4 :uart_init(19200);  LCD_ShowString(80,262,230,12,12,"19200  ");break;
		case 5 :uart_init(38400);  LCD_ShowString(80,262,230,12,12,"38400  ");break;
		case 6 :uart_init(43000);  LCD_ShowString(80,262,230,12,12,"43000  ");break;
    case 7 :uart_init(57600);  LCD_ShowString(80,262,230,12,12,"57600  ");break;
		case 8 :uart_init(76800);  LCD_ShowString(80,262,230,12,12,"76800  ");break;
		case 9 :uart_init(115200); LCD_ShowString(80,262,230,12,12,"115200 ");break;
		case 10:uart_init(128000); LCD_ShowString(80,262,230,12,12,"128000 ");break;
		case 11:uart_init(230400); LCD_ShowString(80,262,230,12,12,"230400 ");break;
		case 12:uart_init(256000); LCD_ShowString(80,262,230,12,12,"256000 ");break;
    case 13:uart_init(460800); LCD_ShowString(80,262,230,12,12,"460800 ");break;
    case 14:uart_init(921600); LCD_ShowString(80,262,230,12,12,"921600 ");break;
    case 15:uart_init(1382400);LCD_ShowString(80,262,230,12,12,"1382400");break;
    case 16:i=0;LCD_ShowString(80,262,230,12,12,"Max....");break;
		default:i=0;
	}
}




