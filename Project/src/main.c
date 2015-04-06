/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   Main program body
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
#include "netconf.h"
#include "main.h"
#include "helloworld.h"
#include "httpd.h"
#include "tftpserver.h"
#include "phil_aux.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  10

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay;

/* Private function prototypes -----------------------------------------------*/
void System_Periodic_Handle(void);
void RS_Init(void);

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	GPIO_InitTypeDef GPIO_InitStructure1;
	
	/* Setup STM32 system (clocks, Ethernet, GPIO, NVIC) and STM3210C-EVAL resources */
	System_Setup();
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure1);
	GPIO_SetBits(GPIOB, GPIO_Pin_9);
	
	RS_Init();
//	SerialPutString(USART2, "Hello world!");
             
  /* Initilaize the LwIP satck */
  LwIP_Init();
  
  /* Initilaize the HelloWorld module */
  //HelloWorld_init();

  /* Initilaize the webserver module */
  httpd_init();
	
	InitResponce();

//	STM_EVAL_LEDToggle(LED1);
//	SerialPutString(USART2, "123");

  /* Infinite loop */
	SerialPutString(USART2, "Initialization done.");
  while (1)
  {    
	/* Periodic tasks */
	System_Periodic_Handle();
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 10ms periods to wait for.
  * @retval None
  */
void Delay(uint32_t nCount)
{
  /* Capture the current local time */
  timingdelay = LocalTime + nCount;  

  /* wait until the desired delay finish */  
  while(timingdelay > LocalTime)
  {     
  }
}

/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
void Time_Update(void)
{
  LocalTime += SYSTEMTICK_PERIOD_MS;
}

/**
  * @brief  Handles the periodic tasks of the system
  * @param  None
  * @retval None
  */
void System_Periodic_Handle(void)
{
  /* Update the LCD display and the LEDs status */
  /* Manage the IP address setting */
//  Display_Periodic_Handle(LocalTime);
  
  /* LwIP periodic services are done here */
  LwIP_Periodic_Handle(LocalTime);
}

void RS_Init(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;

        /* Clock configuration -------------------------------------------------------*/

        /* Configure the GPIO ports( USART2 Transmit and Receive Lines) */
        /* Configure the USART2_Tx as Alternate function Push-Pull */
        /* Configure USART2_Tx as alternate function push-pull */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOD, &GPIO_InitStructure);

        /* Configure USART2_Rx as input floating        */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOD, &GPIO_InitStructure);  


        /* USART2 configuration ------------------------------------------------------*/
        /* USART2 configured as follow:
        - BaudRate = 960000 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        */
        USART_InitStructure.USART_BaudRate = 128000;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

        /* Configure the USART2 */
        USART_Init(USART2, &USART_InitStructure);

        /* Enable the USART2 */
        USART_Cmd(USART2, ENABLE);
}



void SerialPutChar(USART_TypeDef* USARTx, uint8_t c)
{
        USART_SendData(USART2, c);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void SerialPutString(USART_TypeDef* USARTx, uint8_t *s)
{
        while (*s != '\0')
        {
        SerialPutChar(USARTx, *s);
        s ++;
        }
				SerialPutChar(USARTx, '\r');
				SerialPutChar(USARTx, '\n');
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
