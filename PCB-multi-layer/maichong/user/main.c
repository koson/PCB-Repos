
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "textcmd.h"
#include "ad5245.h"
#include "stm32f10x_rcc.h"

#ifdef __GNUC__
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
#define ADC1_DR_Address    ((u32)0x4001244C)

char Ver[] = "2019-05-14 frozen soil data logger V1.0";  //20190514

u32 PAS_value[128], PAS_counter;
u8 PAS_down = 4, PAS_up = 126;
vu16 AD_Value[4];
		
volatile u16 ADC_ConvertedValue;
float ADC_ConvertedValueLocal;
float g_adcTotalValue = 0;
float g_adcAverageValue = 0;
float g_adcOldAverageValue = 0;
		
vu16 timer_timeout;
float voltage_feedback, voltage_target = 1600, gain=10, temperature;
float pwm_freq, pwm_duty;
		
RCC_ClocksTypeDef ClockInfo;

float pas_maxvalue_buf[10];
u8 pas_maxvalue_cnt;

void TIM_Configuration(void);
void Tim3_pwm_config(unsigned short arr, unsigned short psc);
void adc1_init(void);
void delay_ms(u16 cnt);
void highvolage_adjust(u16 volt);
void PAS_buf(void);

void RCC_Configuration(void)
{
//    ErrorStatus HSEStartUpStatus;
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

//    /* Enable HSI */
//    RCC_HSICmd(ENABLE);

//    /* Wait till HSE is ready */
//    HSEStartUpStatus = RCC_WaitForHSEStartUp();

//    if (HSEStartUpStatus == SUCCESS)
//    {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div2);

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* ADCCLK = PCLK2/4 */
//    RCC_ADCCLKConfig(RCC_PCLK2_Div8); //     降低频率

    /* PLLCLK = 4MHz * 9 = 36 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {}

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {}
//    }

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
//                           RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3, ENABLE);
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM4, ENABLE);
}
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    //条件编译，flash或者ram运行中断向量表地址设置。
//#ifdef  VECT_TAB_RAM
//    /* Set the Vector Table base location at 0x20000000 */
//    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x20000000);
//#else  /* VECT_TAB_FLASH  */
//    /* Set the Vector Table base location at 0x08000000 */
//    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x08003000);
//#endif

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

//    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

//    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

//    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQChannel;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

//    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

//    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

//    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQChannel;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
}


void TIM_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;


    /* Time base configuration */

    /*每1秒发生一次更新事件(进入中断服务程序)。RCC_Configuration()的SystemInit()的

    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2表明TIM3CLK为72MHz。因此，每次进入中

    断服务程序间隔时间为

    ((1+TIM_Prescaler )/72M)*(1+TIM_Period )=((1+7199)/72M)*(1+9999)=1秒 */

    TIM_TimeBaseStructure.TIM_Period = 9999;
    TIM_TimeBaseStructure.TIM_Prescaler = 3599;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);


    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    /* TIM IT enable */
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    /* TIM2 enable counter */
    TIM_Cmd(TIM2, ENABLE);

    //AD一次(7.5*4)/9M=3.33us 定时器3us  缓冲100组数据
    TIM_TimeBaseStructure.TIM_Period = 2;
    TIM_TimeBaseStructure.TIM_Prescaler = 35;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    /* TIM IT enable */
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    /* TIM2 enable counter */
    TIM_Cmd(TIM4, ENABLE);
}
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA ,ENABLE);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_9|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_5); //开关电源使能
		GPIO_SetBits(GPIOC, GPIO_Pin_9); //运行指示灯
		GPIO_SetBits(GPIOC, GPIO_Pin_12); //485使能开关
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_8); //脉冲指示灯


//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//    GPIO_SetBits(GPIOB, GPIO_Pin_0); //运行灯亮
//    GPIO_ResetBits(GPIOB, GPIO_Pin_1); //脉冲灯灭

}

void USART_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;
    //USART_ClockInitTypeDef USART_ClockInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* USART1 configuration ------------------------------------------------------*/
    /* USART1 configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Hardware flow control disabled (RTS and CTS signals)
          - Receive and transmit enabled
          - USART Clock disabled
          - USART CPOL: Clock is active low
          - USART CPHA: Data is captured on the middle
          - USART LastBit: The clock pulse of the last data bit is not output to
                           the SCLK pin
    */

    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    /* Configure USART3 Rx (PB.11) as input floating */
//    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    /* Configure USART3 Tx (PC.10) as alternate function push-pull */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    //USART_InitStructure.USART_FIFO = USART_FIFO_Disable;

    /* Configure USART1 */
    USART_Init(USART1, &USART_InitStructure);
    /* Configure USART2 */
    USART_Init(USART2, &USART_InitStructure);
//    /* Configure USART3 */
//    USART_Init(USART3, &USART_InitStructure);

    /* Enable USART1 Receive and Transmit interrupts */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

    /* Enable USART2 Receive and Transmit interrupts */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

//    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
//  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);


    /* Enable the USART1 */
    USART_Cmd(USART1, ENABLE);
    /* Enable the USART2 */
    USART_Cmd(USART2, ENABLE);
//    /* Enable the USART3 */
//    USART_Cmd(USART3, ENABLE);

    USART_GetFlagStatus(USART1, USART_FLAG_TC);
    USART_GetFlagStatus(USART2, USART_FLAG_TC);
//    USART_GetFlagStatus(USART3, USART_FLAG_TC);
//    USART_GetFlagStatus(UART4, USART_FLAG_TC);
}

void USART1_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		
		/* config USART1 clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
		
		/* USART1 GPIO config */
		/* Configure USART1 Tx (PA.09) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);    
		/* Configure USART1 Rx (PA.10) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
			
		/* USART1 mode config */
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART1, &USART_InitStructure); 
		USART_Cmd(USART1, ENABLE);
}
void Tim3_pwm_config(unsigned short arr, unsigned short psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM3_CH3 TIM2_CH3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

void Tim2_pwm_config(u16 arr,u16 psc)
{  
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
		
		GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //TIM3_CH3 TIM2_CH3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void adc1_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		DMA_InitTypeDef DMA_InitStructure;
		ADC_InitTypeDef ADC_InitStructure;
		
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1 , ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    //循环模式开启，Buffer写满后，自动回到初始地址开始传输
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    //配置完成后，启动DMA通道
    DMA_Cmd(DMA1_Channel1, ENABLE);
		
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfChannel = 1;
		ADC_Init(ADC1, &ADC_InitStructure);
		
		RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
		ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_7Cycles5);//adc采样时间7.5+12.5=20个机器周期  频率为9兆，adc通道转换时间为20*1/9000000 = 2.22微秒
		
		ADC_DMACmd(ADC1, ENABLE);
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);
    /* Enable ADC1 reset calibaration register */
    ADC_ResetCalibration(ADC1);
    /* Check the end of ADC1 reset calibration register */
    while (ADC_GetResetCalibrationStatus(ADC1))
    {
        ;
    }
    /* Start ADC1 calibaration */
    ADC_StartCalibration(ADC1);
    /* Check the end of ADC1 calibration */
    while (ADC_GetCalibrationStatus(ADC1))
    {
        ;
    }
    /* Start ADC1 Software Conversion */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
void ADC_Read(void)
{
//    u16 i;
//    for(i=0;i<256;i++)voltage_feedback += AD_Value[i*4+2];
//    voltage_feedback = voltage_feedback/256;
    voltage_feedback = (AD_Value[2] * 3.3 * (100066.5) / 66.5  / 4096);//TODO 66.5 改为100的电阻
    temperature = (float)(AD_Value[3] * 3.3 / 4096);
    temperature = (float)((1.43 - temperature) * 10000 / 43 + 25.00);
}
void delay_ms(u16 cnt)
{
    u16 i;
    while (cnt--)
    {
        for (i = 0; i < 3600; i++)
        {
            __nop();
        }
    }
}
void highvolage_adjust(u16 volt)
{
    static u16 timer_temp = 0;
    static float voltage_feedback_last;

    ADC_Read();
    if (volt < 3000 && abs(volt - voltage_feedback) > 5) //电压小于2000 且 电压值 5V以上调整  12位AD分辨率 1.21V
    {
        if (abs(voltage_feedback_last - voltage_feedback) < 5) //稳定之前不调整
        {
            if (volt > voltage_feedback)
            {
                if ((volt - voltage_feedback) > 100)
                {
                    if(volt>1000)pwm_duty += 5;else pwm_duty += 2;
                    TIM_SetCompare2(TIM3, pwm_duty * 10);
                }
                else if ((volt - voltage_feedback) > 50)
                {
                    pwm_duty += 1;
                    TIM_SetCompare2(TIM3, pwm_duty * 10);
                }
                else
                {
                    pwm_duty += 0.1;
                    TIM_SetCompare2(TIM3, pwm_duty * 10);
                }
            }
            else if (volt < voltage_feedback)
            {
                if ((voltage_feedback - volt) > 100)
                {
                    if(volt>1000)pwm_duty -= 5;else pwm_duty -= 2;
                    TIM_SetCompare2(TIM3, pwm_duty * 10);
                }
                else if ((voltage_feedback - volt) > 50)
                {
                    pwm_duty -= 1;
                    TIM_SetCompare2(TIM3, pwm_duty * 10);
                }
                else
                {
                    pwm_duty -= 0.1;
                    TIM_SetCompare2(TIM3, pwm_duty * 10);
                }
            }
            timer_temp = timer_timeout;
        }
    }
    if (pwm_duty > 60 || ((timer_timeout - timer_temp) > 10 && timer_temp != 0))
    {
//        pwm_duty = 5;
//        TIM_SetCompare2(TIM3, pwm_duty*10);
        timer_temp = 0;
    }
    voltage_feedback_last = voltage_feedback;
}
void PAS_buf(void)
{
    static u16 pas_maxvalue=0,pas_signal_flag=0;
    u16 pas_value_tmp[2];

    //AD_Value[1] PA1 原信号 AD_Value[0] PA0 过电阻信号
    pas_value_tmp[0] = AD_Value[0];
    pas_value_tmp[1] = AD_Value[1];
    
    if(pas_value_tmp[1]>(200+pas_value_tmp[0]))
    {
        if(pas_signal_flag==0)
        {
            if(pas_value_tmp[1]>=pas_maxvalue)
            {
                pas_maxvalue = pas_value_tmp[1];
            }else
            {
                pas_maxvalue_buf[pas_maxvalue_cnt++] = (float)pas_maxvalue/4096*3.3;
                if(pas_maxvalue_cnt>9)pas_maxvalue_cnt = 0;
                pas_maxvalue = (pas_maxvalue >> 4) - 0x80;
                if (pas_maxvalue > PAS_down && pas_maxvalue < PAS_up)
                {
                    PAS_value[pas_maxvalue]++;
                    PAS_counter++;
                }
                pas_maxvalue = 0;
                pas_signal_flag = 1;
            }
            GPIO_SetBits(GPIOB, GPIO_Pin_1);
        }
    }else
    {
        pas_signal_flag = 0;
        GPIO_ResetBits(GPIOB, GPIO_Pin_1);
    }
    
    
//    for(i=0;i<256;i++)
//    {
//        if(pas_value_tmp<AD_Value[i*4+1])pas_value_tmp = AD_Value[i*4+1];
//        AD_Value[i*4+1] = 0;
//    }
//    if (pas_value_tmp > 0x7FF)
//    {
//        pas_value_tmp = (pas_value_tmp >> 4) - 0x80;
//        if (pas_value_tmp > PAS_down && pas_value_tmp < PAS_up)
//        {
//            PAS_value[pas_value_tmp]++;
//            PAS_counter++;
//            GPIO_SetBits(GPIOB, GPIO_Pin_1);
//        }
////        PAS_counter = 0;
////        for(i=PAS_down;i<PAS_up;i++)
////        {
////            PAS_counter += PAS_value[i];
////        }
//    }else
//    {
//        GPIO_ResetBits(GPIOB, GPIO_Pin_1);
//    }
}
/****************************************************************
* @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
****************************************************************/

int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART1, (u8) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
	
		return (ch);
}
/******************* (C) COPYRIGHT 2010 CETC27 *****END OF FILE****/
void WaitForAdcDown(void)
{
			GPIO_ResetBits(GPIOC, GPIO_Pin_5);
			TIM_SetCompare3(TIM3, 0);
			delay_ms(1000);
			GPIO_SetBits(GPIOC, GPIO_Pin_5);
			delay_ms(10);
}

void AdjustVoltage(u16 voltage)
{
			u16 percent = 0;
			u16 temp = 0;
			u16 temp1 = 0;
			WaitForAdcDown();
			for(percent = 0; percent < 1000; percent+=10)
			{
				TIM_SetCompare3(TIM3, percent);//999
				delay_ms(30);
				for (temp = 0; temp < 10; temp++)
				{
						ADC_ConvertedValueLocal +=(float) ADC_ConvertedValue/4096*3.3; // 读取转换的AD值
						delay_ms(2);
				}
				g_adcAverageValue = ADC_ConvertedValueLocal/10;			
				ADC_ConvertedValueLocal = 0;
				
				if (percent >= 990)
				{
						GPIO_ResetBits(GPIOC, GPIO_Pin_5);
						TIM_SetCompare3(TIM3, 0);
						delay_ms(5000);
						GPIO_SetBits(GPIOC, GPIO_Pin_5);
						delay_ms(100);
				}
					
					if (((g_adcAverageValue * 1001) > (voltage - 150))&&(g_adcAverageValue * 1001 <= voltage))
					{
							g_adcOldAverageValue = 0;
							do{
									percent++;
									TIM_SetCompare3(TIM3, percent);//999
									delay_ms(10);
									for (temp1 = 0; temp1 < 10; temp1++)
									{
											ADC_ConvertedValueLocal +=(float) ADC_ConvertedValue/4096*3.3; // 读取转换的AD值
											delay_ms(1);
									}
									g_adcAverageValue = ADC_ConvertedValueLocal/10;
									printf("%d %f \r\n", percent, g_adcAverageValue*1001);
									ADC_ConvertedValueLocal = 0;
									if (abs(voltage - g_adcAverageValue * 1001) <= 3)
									{
										break;
									}
							}while((g_adcAverageValue * 1001) <= (voltage +20));
							break;
					}
			}
}

void TestPwmAndInputVoltage(void)
{
			u16 percent = 0;
			u16 temp = 0;
			for(percent = 0; percent < 1000; percent++)
			{
				TIM_SetCompare3(TIM3, percent);//999
				delay_ms(50);
				for (temp = 0; temp < 50; temp++)
				{
						ADC_ConvertedValueLocal +=(float) ADC_ConvertedValue/4096*3.3; // 读取转换的AD值
						delay_ms(1);
				}
				g_adcAverageValue = ADC_ConvertedValueLocal/50;	
				
				if (percent == 10)
				{
						printf("%d  %f \r\n", percent, g_adcAverageValue*1001);
				}
				else if (percent == 990)
				{
						printf("%d  %f \r\n", percent, g_adcAverageValue*1001);
				}
				else 
				{
						printf("%f\r\n",g_adcAverageValue*1001);
				}
				
				ADC_ConvertedValueLocal = 0;
				if (percent >= 990)
				{
						GPIO_ResetBits(GPIOC, GPIO_Pin_5);
						TIM_SetCompare3(TIM3, 0);
						delay_ms(5000);
						GPIO_SetBits(GPIOC, GPIO_Pin_5);
						delay_ms(100);
				}
			}
}

int main(void)
{
    RCC_Configuration();
    NVIC_Configuration();
    USART1_Config();
    GPIO_Configuration();
//    TIM_Configuration();
    Tim3_pwm_config(999, 0); //PWM频率 36000/(999+1)=36 kHz 不分频, 19999和3599的参数，表明了pwm波形2s翻转一次
		Tim2_pwm_config(9999, 4999);
//    TIM_SetCompare2(TIM3, 0);

//    TIM_SetCompare3(TIM3, 29);//999
		TIM_SetCompare3(TIM2, 2999);//999
//    TIM_SetCompare4(TIM3, 99);
//    printf("this is a test!\r\n");
 //    ad5245_init();
    adc1_init();
	
//    RCC_GetClocksFreq(&ClockInfo);
	
//    TIM_Configuration();TODO by myself
//    delay_ms(1000);
//    TIM_SetCompare2(TIM3,30);
    while (1)
		{
				WaitForAdcDown();
			delay_ms(5000);
				TestPwmAndInputVoltage();
				delay_ms(5000);
//			AdjustVoltage(800);
//			printf("数据趋向所需值11111_______________________%f \r\n", g_adcAverageValue*1001);
//			delay_ms(5000);
//			AdjustVoltage(2600);
//			printf("数据趋向所需值_______________________%f \r\n", g_adcAverageValue*1001);
//			delay_ms(5000);
//			
////        PAS_buf();
////        ADC_Read();
//        delay_ms(1);
////        highvolage_adjust(voltage_target);
//        printf("%ld 个,%.1f 伏特,%.1f 摄氏度  ", PAS_counter, voltage_feedback, temperature);
//        printf("%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\r\n",pas_maxvalue_buf[0],pas_maxvalue_buf[1],pas_maxvalue_buf[2],pas_maxvalue_buf[3],pas_maxvalue_buf[4],pas_maxvalue_buf[5],pas_maxvalue_buf[6],pas_maxvalue_buf[7],pas_maxvalue_buf[8],pas_maxvalue_buf[9]);
//        if (termination_flag == 1)text_cmd();
    }
}





