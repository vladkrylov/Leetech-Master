/**
  ******************************************************************************
  * @file    stm32f107.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   STM32F107 hardware configuration
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32_eth.h"
#include "stm32f107.h"
#include "stm32f10x_rcc.h"
#include "phil_can_functions.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DP83848_PHY        /* Ethernet pins mapped on STM3210C-EVAL Board */
#define PHY_ADDRESS       0x01F /* Relative to STM3210C-EVAL Board */

#define MII_MODE          /* MII mode for STM3210C-EVAL Board (MB784) (check jumpers setting) */
//#define RMII_MODE       /* RMII mode for STM3210C-EVAL Board (MB784) (check jumpers setting) */

/*--------------- LCD Messages ---------------*/
#define MESSAGE1   "     STM32F107      "
#define MESSAGE2   " Connectivity Line  "
#define MESSAGE3   "   * LwIP demos *   "
#define MESSAGE4   "                    "

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void ADC_Configuration(void);
void Ethernet_Configuration(void);

/**
  * @brief  Setup STM32 system (clocks, Ethernet, GPIO, NVIC) and STM3210C-EVAL resources.
  * @param  None
  * @retval None
  */
void System_Setup(void)
{
//	GPIO_InitTypeDef  GPIO_InitStructure1;
  RCC_ClocksTypeDef RCC_Clocks;
	
  /* Setup STM32 clock, PLL and Flash configuration) */
  SystemInit();

  /* Enable USART2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);


  /* Enable ETHERNET clock  */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx |
                        RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);

  /* Enable GPIOs and ADC1 clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO |
						 RCC_APB2Periph_ADC1, ENABLE);

  /* NVIC configuration */
  NVIC_Configuration();  
	
	
  /* ADC configuration */
  ADC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();

//  /* Initialize the STM3210C-EVAL's LCD */
//  STM3210C_LCD_Init();
	
	CAN_NVIC_Config();
	CAN_Config();

//  /* Initialize STM3210C-EVAL's LEDs */
//  STM_EVAL_LEDInit(LED1);
//  STM_EVAL_LEDInit(LED2);
//  STM_EVAL_LEDInit(LED3);
//  STM_EVAL_LEDInit(LED4);

//  /* Turn on leds available on STM3210X-EVAL */
//  STM_EVAL_LEDOn(LED1);
//  STM_EVAL_LEDOn(LED2);
//  STM_EVAL_LEDOn(LED3);
//  STM_EVAL_LEDOn(LED4);
  
//  /* Enable the GPIO_LED Clock */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

//  /* Configure the GPIO_LED pin */
//  GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_9;
//  GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_Out_PP; 
//  GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;

//  GPIO_Init(GPIOB, &GPIO_InitStructure1);

  /* Clear the LCD */
  LCD_Clear(Blue);

  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);

  /* Set the LCD Text Color */
  LCD_SetTextColor(White);

  /* Display message on the LCD*/
  LCD_DisplayStringLine(Line0, MESSAGE1);
  LCD_DisplayStringLine(Line1, MESSAGE2);
  LCD_DisplayStringLine(Line2, MESSAGE3);
  LCD_DisplayStringLine(Line3, MESSAGE4);

  /* Configure the Ethernet peripheral */
  Ethernet_Configuration();

  /* SystTick configuration: an interrupt every 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.SYSCLK_Frequency / 100);

  /* Update the SysTick IRQ priority should be higher than the Ethernet IRQ */
  /* The Localtime should be updated during the Ethernet packets processing */
  NVIC_SetPriority (SysTick_IRQn, 1);
//	NVIC_SetPriority (CAN2_RX0_IRQn, 1);
  
  /* Configure the Key button */ 
  //STM_EVAL_PBInit(Button_KEY, Mode_GPIO);
}

/**
  * @brief  Configures the Ethernet Interface
  * @param  None
  * @retval None
  */
void Ethernet_Configuration(void)
{
	  ETH_InitTypeDef ETH_InitStructure;

	  GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_MII);

	  /* Reset ETHERNET on AHB Bus */
	  ETH_DeInit();

	  /* Software reset */
	  ETH_SoftwareReset();

	  /* Wait for software reset */
	  while(ETH_GetSoftwareResetStatus()==SET);

	  /* ETHERNET Configuration ------------------------------------------------------*/
	  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
	  ETH_StructInit(&ETH_InitStructure);

	  /* Fill ETH_InitStructure parametrs */
	  /*------------------------   MAC   -----------------------------------*/
	  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable  ;
	  //ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
	  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
	  //ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
	  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
	  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
	  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;
	  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Disable;
	  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
	  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
	  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
	  /* Configure ETHERNET */
	  ETH_Init(&ETH_InitStructure, PHY_ADDRESS);

		ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
//void GPIO_Configuration(void)
//{
//  GPIO_InitTypeDef GPIO_InitStructure;

//  /* ETHERNET pins configuration */
//  /* AF Output Push Pull:
//  - ETH_MII_MDIO / ETH_RMII_MDIO: PA2
//  - ETH_MII_MDC / ETH_RMII_MDC: PC1
//  - ETH_MII_TXD2: PC2
//  - ETH_MII_TX_EN / ETH_RMII_TX_EN: PB11
//  - ETH_MII_TXD0 / ETH_RMII_TXD0: PB12
//  - ETH_MII_TXD1 / ETH_RMII_TXD1: PB13
//  - ETH_MII_PPS_OUT / ETH_RMII_PPS_OUT: PB5
//  - ETH_MII_TXD3: PB8 */

//  /* Configure PA2 as alternate function push-pull */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);

//  /* Configure PC1, PC2 and PC3 as alternate function push-pull */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);

//  /* Configure PB5, PB8, PB11, PB12 and PB13 as alternate function push-pull */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_11 |
//                                GPIO_Pin_12 | GPIO_Pin_13;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);

//  /**************************************************************/
//  /*               For Remapped Ethernet pins                   */
//  /*************************************************************/
//  /* Input (Reset Value):
//  - ETH_MII_CRS CRS: PA0
//  - ETH_MII_RX_CLK / ETH_RMII_REF_CLK: PA1
//  - ETH_MII_COL: PA3
//  - ETH_MII_RX_DV / ETH_RMII_CRS_DV: PD8 !!!!! PA7
//  - ETH_MII_TX_CLK: PC3
//  - ETH_MII_RXD0 / ETH_RMII_RXD0: PD9 !!!!! PC4
//  - ETH_MII_RXD1 / ETH_RMII_RXD1: PD10 !!!!! PC5
//  - ETH_MII_RXD2: PD11 !!!!! PB0
//  - ETH_MII_RXD3: PD12 !!!!! PB1
//  - ETH_MII_RX_ER: PB10 */

//  /* ETHERNET pins remapp in STM3210C-EVAL board: RX_DV and RxD[3:0] */
//  GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);

//  /* Configure PA0, PA1 and PA3 as input */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_7;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);

//  /* Configure PB10 as input */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_0 | GPIO_Pin_1;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);

//  /* Configure PC3 as input */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);

//  /* Configure PD8, PD9, PD10, PD11 and PD12 as input */
////  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
////  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
////  GPIO_Init(GPIOD, &GPIO_InitStructure); /**/

//  /* ADC Channel14 config --------------------------------------------------------*/
//  /* Relative to STM3210D-EVAL Board   */
//  /* Configure PC.04 (ADC Channel14) as analog input -------------------------*/
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);

//  /* MCO pin configuration------------------------------------------------- */
//  /* Configure MCO (PA8) as alternate function push-pull */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);

//}
void GPIO_Configuration(void)
{

   //ETH_MII_CRS      PA0    Floating input (reset state)
   //ETH_MII_COL      PA3    Floating input (reset state)
   //ETH_MDIO MDIO    PA2    AF output push-pull highspeed(50 MHz)
   //ETH_MDC          PC1    AF output push-pull highspeed(50 MHz)

   //ETH_MII_TX_EN    PB11   AF output push-pull highspeed(50 MHz)
   //ETH_MII_TXD0     PB12   AF output push-pull highspeed(50 MHz)
   //ETH_MII_TXD1     PB13   AF output push-pull highspeed(50 MHz)
   //ETH_MII_TXD2     PC2    AF output push-pull highspeed(50 MHz)
   //ETH_MII_TXD3     PB8    AF output push-pull highspeed(50 MHz)
   //ETH_MII_TX_CLK   PC3    Floating input (reset state)

   //ETH_MII_RX_DV    PA7    Floating input (reset state)
   //ETH_MII_RX_ER    PB10   Floating input (reset state)
   //ETH_MII_RXD0     PC4    Floating input (reset state)
   //ETH_MII_RXD1     PC5    Floating input (reset state)
   //ETH_MII_RXD2     PB0    Floating input (reset state)
   //ETH_MII_RXD3     PB1    Floating input (reset state)
   //ETH_MII_RX_CLK   PA1    Floating input (reset state)

  GPIO_InitTypeDef GPIO_InitStructure;

  //GPIOA,AF,ppout,50
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //GPIOB,AF,ppout,50
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //GPIOC,AF,ppout,50
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  //GPIOA,float input,50
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //GPIOB,float input,50
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //GPIOC,float input,50
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOB, GPIO_Pin_9);
}


/**
  * @brief  Configures the ADC.
  * @param  None
  * @retval None
  */
void ADC_Configuration(void)
{
  ADC_InitTypeDef ADC_InitStructure;

  /* ADC1 Configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel14 configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_13Cycles5);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
  
  /* Enable the Ethernet global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/