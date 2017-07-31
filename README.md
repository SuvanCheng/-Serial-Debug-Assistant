# STM32_Notes-01.Matrix_Key
Matrix Key uesd on STM32ZE kits

矩陣按鍵 Matrix Key
MCU   : STM32103ZE
I/O口 ：Pin_All_D
頭文件：MatrixKey.h
-----------------------------------------
#ifndef __MATRIXKEY_H
#define __MATRIXKEY_H
#include "sys.h"

void HL_Init_HScan(void);
void HL_Init_LScan(void);
u8 MatrixKey_Scan(void);

#endif
-----------------------------------------
