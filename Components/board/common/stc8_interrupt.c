#include <ioSTC8.h>

/***********************说明*****************************
*这里面罗列了STC8单片机在IAR平台下的的所有中断向量入口地址，
*格式如下面所示;一旦中断产生，就会进入相应中断的函数入口。
********************************************************/


#pragma vector=INT0_VECTOR //外部中断0中断向量入口地址
__interrupt void INT0_Server(void)
{

}


#pragma vector=Timer0_VECTOR //定时器0中断向量入口地址
__interrupt void Timer0_Server(void)
{

}


#pragma vector=INT1_VECTOR //外部中断1中断向量入口地址
__interrupt void INT1_Server(void)
{

}


#pragma vector=Timer1_VECTOR //定时器01中断向量入口地址
__interrupt void Timer1_Server(void)
{

}


#pragma vector=UART1_VECTOR //串口1中断向量入口地址
__interrupt void UART1_Server(void)
{

}


#pragma vector=ADC_VECTOR //ADC中断向量入口地址
__interrupt void ADC_Server(void)
{

}


#pragma vector=LVD_VECTOR //LVD中断向量入口地址
__interrupt void LVD_Server(void)
{

}


#pragma vector=PCA_VECTOR //PCA中断向量入口地址
__interrupt void PCA_Server(void)
{

}


#pragma vector=UART2_VECTOR //串口2中断向量入口地址
__interrupt void UART2_Server(void)
{

}


#pragma vector=SPI_VECTOR //SPI中断向量入口地址
__interrupt void SPI_Server(void)
{

}


#pragma vector=INT2_VECTOR //外部中断2中断向量入口地址
__interrupt void INT2_Server(void)
{

}


#pragma vector=INT3_VECTOR //外部中断3中断向量入口地址
__interrupt void INT3_Server(void)
{

}


#pragma vector=Timer2_VECTOR //定时器2中断向量入口地址
__interrupt void Timer2_Server(void)
{

}


#pragma vector=INT4_VECTOR //外部中断4中断向量入口地址
__interrupt void INT4_Server(void)
{

}


#pragma vector=UART3_VECTOR //串口3中断向量入口地址
__interrupt void UART3_Server(void)
{

}


#pragma vector=UART4_VECTOR //串口4中断向量入口地址
__interrupt void UART4_Server(void)
{

}


#pragma vector=Timer3_VECTOR //定时器3中断向量入口地址
__interrupt void Timer3_Server(void)
{

}


#pragma vector=Timer4_VECTOR //定时器4中断向量入口地址
__interrupt void Timer4_Server(void)
{

}


#pragma vector=CMP_VECTOR //CMP中断向量入口地址
__interrupt void CMP_Server(void)
{

}


#pragma vector=PWM_VECTOR //PWM中断向量入口地址
__interrupt void PWM_Server(void)
{

}


#pragma vector=PWMFD_VECTOR //PWMFD中断向量入口地址
__interrupt void PWMFD_Server(void)
{

}


#pragma vector=I2C //I2C中断向量入口地址
__interrupt void I2C_Server(void)
{

}


#pragma vector=CAN //CAN中断向量入口地址
__interrupt void CAN_Server(void)
{

}
