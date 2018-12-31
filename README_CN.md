# STM32便携式串口调试助手


## 概述



## 重温

​	当时这是写的啥呀，害的我又花了两个小时才看懂当时写的啥😂。原来是这样，将接收到string数据写入`W25Q64`Flash的前半部分；再将string数据[转换为hex数据](#stringtohex)，后写入`W25Q64`Flash的后半部分；
### Feature
* [x] key_up: 修改波特率；默认串口9600
* [x] key_1 : hex显示，最多存储7页
* [x] key_2 : string显示，最多存储7页

```c
//计算给定字符串的长度
int my_strlen(const char *str)
{
	const char *p = str;
	while (*p != '\0')
	{
		if (*p == ' ' && *(p + 1) == ' ' && *(p + 2) == ' ')
		{
			break;
		}
		p++;
	}
	return p - str;
}
```

```c
//初始化W25Q64
void Check_W25Q64()
{
	u8 i;
	u32 r = SIZE, secpos;
	while (SPI_Flash_ReadID() != W25Q64) //检测不到W25Q64
	{
		LCD_ShowString(1, 1, 200, 16, 16, "25Q64 Check Failed!");
		delay_ms(500);
		LCD_ShowString(1, 1, 200, 16, 16, "Please Check!      ");
		delay_ms(500);
		LED0 = !LED0; //DS0闪烁
	}
	LCD_ShowString(1, 1, 250, 16, 12, "25Q64 Ready!                    Page:");

	for (i = 0; i < 6; i++) //清楚断电前数据
	{
		secpos = (FLASH_SIZE / 2 - SIZE - (i * 20 * SIZE)) / 4096; //hex区
		SPI_Flash_Erase_Sector(secpos);
		secpos = (FLASH_SIZE - SIZE - (i * 20 * SIZE)) / 4096; //string区
		SPI_Flash_Erase_Sector(secpos);
	}
}
```

### stringtohex

```c
len = my_strlen(USART_RX_BUF);
for (i = 0; i < len * 3; i += 3)//每组3个，第1个为hex高位，第2个为hex低位，第3个为空格；
	{
		transfer_value[i] = (USART_RX_BUF[i / 3] / 16 + 48);   //54
		if (\
			(USART_RX_BUF[i / 3] > 41 && USART_RX_BUF[i / 3] < 48) || \
			(USART_RX_BUF[i / 3] > 57 && USART_RX_BUF[i / 3] < 64) || \
			(USART_RX_BUF[i / 3] > 73 && USART_RX_BUF[i / 3] < 80) || \
			(USART_RX_BUF[i / 3] > 89 && USART_RX_BUF[i / 3] < 96) || \
			(USART_RX_BUF[i / 3] > 105 && USART_RX_BUF[i / 3] < 112) || \
			(USART_RX_BUF[i / 3] > 121 && USART_RX_BUF[i / 3] <= 127))
				transfer_value[i + 1] = (USART_RX_BUF[i / 3] - (USART_RX_BUF[i / 3] / 16) * 16 + 55);
		else
			transfer_value[i + 1] = (USART_RX_BUF[i / 3] - (USART_RX_BUF[i / 3] / 16) * 16 + 48);  //52
				
		transfer_value[i + 2] = 32; //十进制Decimal，'32'='空格'
	}
```





## 2017年5月13日22:45:15

基于正点原子的例程修改

> 实验平台：MiniSTM32F103开发板 V3版本
> 实验目的：学习STM32 SPI接口的使用,驱动W25Q64实现SPI FLASH数据读写.
> 实验现象：本实验通过KEY_UP按键来控制W25Q64的写入，通过另外一个按键KEY0来控制W25Q64的读取。并在LCD模块上面显示相关信息。DS0提示程序正在运行。同时，我们可以通过USMART控制读取W25QXX的ID或者整片擦除。

* [x] DS0(连接在PA8) 
* [x] 串口1(波特率:9600,PA9/PA10连接在板载USB转串口芯片CH340上面)
* [x] ALIENTEK 2.8/3.5/4.3/7寸TFTLCD模块(通过GPIO驱动,连接关系见lcd.h)
* [x] 按键KEY0(PC5)/KEY_UP(PA0,也称之为WK_UP) 
* [x] W25Q64(SPI FLASH芯片,连接在SPI1上)

注意事项:  
4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源供电(5V电源，接VOUT2即可).
本例程在LCD_Init函数里面(在ILI93xx.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!! 
LCD模块在开发板的LCD接口上面，必须靠右插
不要把1820和PA0的跳线帽跳上,否则可能导致按键"不灵".  
W25Q64和NRF24L01/SD卡接口共用SPI1,所以他们必须分时复用,使用一个的时候,必须禁止另外两个的片选.

**接下来是我的工作**

Suvan：
key_up: 修改波特率
key_1 : hex显示，最多7页
key_2 : string显示，最多7页
2017年5月13日22:45:15





