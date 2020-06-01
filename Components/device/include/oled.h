#ifndef OLED_H
#define OLED_H

#include "hal_types.h"

static void OLED_WR_Byte(uint8 dat, uint8 cmd);
static void OLED_WR_Bytes(uint8 *dat, uint32 len);
extern void OLED_Clear(void);
extern void OLED_Init(void);
void OLED_Set_Pos(uint8 x, uint8 y);
extern void OLED_Display_On(void);
extern void OLED_Display_Off(void);

extern void OLED_SetPixel(int32 x, int32 y, int32 PixelValue);
extern int32 OLED_GetPixel(int32 x, int32 y);
extern void OLED_ReverseAllPixel(void);
extern void OLED_Refresh_Display(void);
extern void OLED_Clear_Display(void);
extern void OLED_Clear_Ram(void);

extern void OLED_ShowChinese(uint8 x, uint8 y, uint8 *code,uint16 size);
extern void OLED_ShowChar(uint8 iStartX, uint8 iStartY,uint8 ascii_code,uint16 size);
extern void OLED_ShowNum(uint8 iStartX, uint8 iStartY,uint32 num ,uint16 size);
extern void OLED_ShowString(uint8 iStartX, uint8 iStartY,uint8 *str ,uint16 size);
extern uint32 OLED_Pow(uint8 base,uint8 index);

extern void OLED_DrawLine(int16 iStartX, int16 iStartY, int16 iEndX, int16 iEndY, int16 fill);
extern void OLED_DrawDottedLine(int16 iStartX, int16 iStartY, int16 iEndX, int16 iEndY);
extern void OLED_DrawCircle(uint8 uiCx, uint8 uiCy, uint8 uiRadius, uint8 eEdgeColor, uint8 eFillColor);
extern void OLED_DrawRectangle(uint16 uiStartX, uint16 uiStartY, uint16 uiWidth, uint16 uiHeight, uint8 eEdgeColor, uint8 eFillColor);
extern void OLED_DrawEllipse(uint8 uiCx,uint8 uiCy,int a,int b,uint8 fill);
extern void OLED_Drawcos(uint8 uiStartX,uint8 uiStartY,uint8 EnlargeX,uint8 EnlargeY,uint8 fill);
extern void OLED_DrawSin(uint8 uiStartX,uint8 uiStartY,uint8 EnlargeX,uint8 EnlargeY,uint8 fill);
 
#endif

