/**
  ******************************************************************************
  * @file    BSP/Src/joystick.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    17-February-2017
  * @brief   This example code shows how to use the joystick feature in the 
  *          stm32412g_discovery driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HEADBAND_HEIGHT                 64
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static JOYState_TypeDef JoyState = JOY_NONE;
/* Private function prototypes -----------------------------------------------*/
static void Joystick_SetHint(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Joystick Gpio demo
  * @param  None
  * @retval None
  */
void Joystick_demo (void)
{ 

  uint8_t status = 0;

  Joystick_SetHint();
  
  status = BSP_JOY_Init(JOY_MODE_GPIO);
  
  if (status != HAL_OK)
  {
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE); 
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 95, (uint8_t *)"ERROR", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 80, (uint8_t *)"Joystick cannot be initialized", CENTER_MODE);
  }
  
  while (1)
  {
    if (status == HAL_OK)
    {
      /* Get the Joystick State */
      JoyState = BSP_JOY_GetState();
      Joystick_SetCursorPosition();
    }
    if(CheckForUserInput() > 0)
    {
      return;
    }
    HAL_Delay(6);
  }
}

/**
  * @brief  Joystick cursor position
  * @param  None
  * @retval None
  */
void Joystick_SetCursorPosition(void)
{
  static uint16_t xPtr = 6;
  static uint16_t yPtr = HEADBAND_HEIGHT + 6;
  static uint16_t old_xPtr = 6;
  static uint16_t old_yPtr = HEADBAND_HEIGHT + 6;

      
  switch(JoyState)
  {
  case JOY_UP:
    if(yPtr > HEADBAND_HEIGHT + 6)
    {
      yPtr--;
    }
    break;
  case JOY_DOWN:
    if(yPtr < (BSP_LCD_GetYSize() - 6 - 11))
    {
      yPtr++;
    }
    break;
  case JOY_LEFT:
    if(xPtr > 6)
    {
      xPtr--;
    }
    break;
  case JOY_RIGHT:
    if(xPtr < (BSP_LCD_GetXSize() - 2 - 11))
    {
      xPtr++;
    }
    break;
  default:
    break;
  }

  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);

  if(JoyState == JOY_SEL)
  {
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DisplayChar(xPtr, yPtr, 'X');

  }
  else if(JoyState == JOY_NONE)
  {
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    BSP_LCD_DisplayChar(xPtr, yPtr, 'X');
  }
  else
  {
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DisplayChar(old_xPtr, old_yPtr, 'X');
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    BSP_LCD_DisplayChar(xPtr, yPtr, 'X');

    old_xPtr = xPtr;
    old_yPtr = yPtr;
  }
}



/**
  * @brief  Display joystick demo hint
  * @param  None
  * @retval None
  */
static void Joystick_SetHint(void)
{
  /* Clear the LCD */ 
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  
  /* Set Joystick Demo description */
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), HEADBAND_HEIGHT);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_SetBackColor(LCD_COLOR_BLUE); 
  BSP_LCD_SetFont(&Font16);
  BSP_LCD_DisplayStringAt(0, 1, (uint8_t *)"Joystick", CENTER_MODE);
  BSP_LCD_SetFont(&Font12);
  BSP_LCD_DisplayStringAt(0, 20, (uint8_t *)"Use the joystick", CENTER_MODE);
  BSP_LCD_DisplayStringAt(0, 35, (uint8_t *)"to move the pointer", CENTER_MODE);
  BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)"inside the rectangle", CENTER_MODE);
  
  /* Set the LCD Text Color */
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);  
  BSP_LCD_DrawRect(5, HEADBAND_HEIGHT + 5, BSP_LCD_GetXSize() - 10, BSP_LCD_GetYSize() - HEADBAND_HEIGHT - 10);
}
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
