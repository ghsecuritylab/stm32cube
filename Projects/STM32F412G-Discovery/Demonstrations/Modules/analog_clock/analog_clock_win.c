/**
  ******************************************************************************
  * @file    analog_clock_win.c
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    17-February-2017   
  * @brief   analog clcok functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "dialog.h"
#include "k_module.h"
#include "k_rtc.h"
#include "analog_clock_res.c"

/** @addtogroup ANALOG_CLOCK_MODULE
  * @{
  */

/** @defgroup ANALOG_CLOCK
  * @brief analog clock routines 
  * @{
  */


/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);

/* Private typedef -----------------------------------------------------------*/
K_ModuleItem_Typedef  analog_clock_board =
{
  2,
  (uint8_t*)"",
  &bmclock5,
  Startup,
}
;

/* Private defines -----------------------------------------------------------*/
#define ID_FRAMEWIN_INFO        (GUI_ID_USER + 0x01)
#define ID_IMAGE_INFO           (GUI_ID_USER + 0x1F)

#define ID_BUTTON_EXIT          (GUI_ID_USER + 0x20)
#define ID_BUTTON_SETTINGS      (GUI_ID_USER + 0x21)

#define ID_BUTTON_MINUS     (GUI_ID_USER + 0x22)
#define ID_BUTTON_PLUS      (GUI_ID_USER + 0x23)

#define ID_BUTTON_REFRESH        (GUI_ID_USER + 0x25)

#define  PI                                3.14
#define  AA_FACTOR                         3
  
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
  RTC_TimeTypeDef          Time;
  RTC_DateTypeDef          Date;
  
  uint32_t settings = 0;

/* Dialog resource using a WINDOW widget*/

  static const GUI_WIDGET_CREATE_INFO _aDialogClockSettings[] = {
  { WINDOW_CreateIndirect, "Clock Settings",                   0,   0,   0, 240, 240, FRAMEWIN_CF_MOVEABLE },
  { IMAGE_CreateIndirect,     "Image",        ID_IMAGE_INFO,   0,   40, 240, 200,   0,   0,   0 },
};

static const GUI_POINT aPoints[4][4] = {
   
  /* Hour Needle */
  {{ 0, 0 * AA_FACTOR},
   { 3, -5 * AA_FACTOR},
   {0, -18 * AA_FACTOR},
   {-3, -5 * AA_FACTOR}},  
  
  /* Min Needle */
  {{ 1, 0 * AA_FACTOR},
   { 1, -20 * AA_FACTOR},
   {-1, -20 * AA_FACTOR},
   {-1, 0 * AA_FACTOR}},
   
   /* Sec Needle */
  {{1, 1 * AA_FACTOR},
   {-1, 1 * AA_FACTOR},
   {-1,-88 * AA_FACTOR},
   {1,-88 * AA_FACTOR}},
   
  {{2, -1 * AA_FACTOR},
   {-2, -1 * AA_FACTOR},
   {-2, 20 * AA_FACTOR},
   {2, 20 * AA_FACTOR}},   
};


GUI_POINT aPointsDest[4][4];

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Draw Needle
  * @param  index: Needle index  
  * @param  x0:    x position
  * @param  y0:    y position      
  * @retval None
  */
static void DrawNeedle(uint32_t index, uint16_t x0, uint16_t y0) 
{
  
  /* draw Needles */
  if(index >= 2)
  {
    GUI_SetColor(GUI_RED);
    GUI_AA_FillPolygon(aPointsDest[index], 4, AA_FACTOR * x0, AA_FACTOR * y0);
  }
  else if(index == 1)
  {
    if(settings == 1)
    {
      GUI_SetColor(0xFF0000);
    }
    else
    {
      GUI_SetColor(0xFF5B3E20);
    }
    
    GUI_AA_FillPolygon(aPointsDest[index], 4, AA_FACTOR * x0, AA_FACTOR * y0);
  }
  else
  {
    if(settings == 2)
    {
      GUI_SetColor(0x0000FF);
    }
    else
    {
      GUI_SetColor(0xFF5B3E20);
    }    
    GUI_AA_FillPolygon(aPointsDest[index], 4, AA_FACTOR * x0, AA_FACTOR * y0);
  }  
}

/**
  * @brief  Update clock
  * @param  x0:   x position
  * @param  y0:   y position
  * @param  hour: updated hour
  * @param  min:  updated minute
  * @param  sec:  updated second      
  * @retval None
  */
static void GUI_UpdateClock (uint16_t x0, uint16_t y0, uint8_t hour, uint8_t min, uint8_t sec)
{ 
  GUI_AA_EnableHiRes();
  GUI_AA_SetFactor(AA_FACTOR);
  
  
  GUI_MagnifyPolygon(aPointsDest[0], aPoints[0], 4, 4);
  GUI_RotatePolygon(aPointsDest[0], aPointsDest[0], 4, - 2 * PI * (int)((int)hour + (int)min /60) / 12);    
  DrawNeedle(0, x0, y0);
  
  GUI_MagnifyPolygon(aPointsDest[1], aPoints[1], 4, 4);
  GUI_RotatePolygon(aPointsDest[1], aPointsDest[1], 4, - 2 * PI * (int)((int)min + (int)sec / 60) / 60);
  DrawNeedle(1, x0, y0);  
  
  GUI_MagnifyPolygon(aPointsDest[2], aPoints[2], 4, 1);
  GUI_RotatePolygon(aPointsDest[2], aPointsDest[2], 4, - 2 * PI * sec / 60);    
  DrawNeedle(2, x0, y0); 
  
  GUI_MagnifyPolygon(aPointsDest[3], aPoints[3], 4, 1);
  GUI_RotatePolygon(aPointsDest[3], aPointsDest[3], 4, - 2 * PI * sec / 60);    
  DrawNeedle(3, x0, y0); 
  
  GUI_AA_DisableHiRes();
}

/**
* @brief  Paints record button
* @param  hObj: button handle
* @retval None
*/
static void _OnPaint_exit(BUTTON_Handle hObj)
{
  GUI_DrawBitmap(&bmclock_exit, 20, 0);
}

/**
* @brief  callback for exit button
* @param  pMsg: pointer to data structure of type WM_MESSAGE
* @retval None
*/
static void _cbButton_exit(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_exit(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}

/**
* @brief  Paints plus button
* @param  hObj: button handle
* @retval None
*/
static void _OnPaint_plus(BUTTON_Handle hObj)
{
  if(settings == 1)
  {
    GUI_DrawBitmap(&bmplus_mn, 0, 0);
  }
  else if(settings == 2)
  {
    GUI_DrawBitmap(&bmplus_hour, 0, 0);
  }
}

/**
* @brief  Paints minus button
* @param  hObj: button handle
* @retval None
*/
static void _OnPaint_minus(BUTTON_Handle hObj)
{
  if(settings == 1)
  {
    GUI_DrawBitmap(&bmminus_mn, 0, 0);
  }
  else if(settings == 2)
  {
    GUI_DrawBitmap(&bmminus_hour, 0, 0);
  }
}

/**
* @brief  callback for plus button
* @param  pMsg: pointer to data structure of type WM_MESSAGE
* @retval None
*/
static void _cbButton_plus(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_plus(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}
/**
* @brief  callback for minus button
* @param  pMsg: pointer to data structure of type WM_MESSAGE
* @retval None
*/
static void _cbButton_minus(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_minus(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}

/**
* @brief  Paints settings button
* @param  hObj: button handle
* @retval None
*/
static void _OnPaint_settings(BUTTON_Handle hObj)
{
  GUI_DrawBitmap(&bmsettings, 0, 0);
}

/**
* @brief  callback for exit button
* @param  pMsg: pointer to data structure of type WM_MESSAGE
* @retval None
*/
static void _cbButton_settings(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_settings(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}


/**
* @brief  Paints refresh button
* @param  hObj: button handle
* @retval None
*/
static void _OnPaint_refresh(BUTTON_Handle hObj)
{
  if(settings == 1)
  {
    GUI_SetColor(0xFF0000);
  }
  else if(settings == 2)
  {
    GUI_SetColor(0x0000FF);
  }
  else
  {
    GUI_SetColor(0x2CCB5);
  }   
  
  GUI_FillRect(0, 0, 240, 40);  
}

/**
* @brief  callback for refresh button
* @param  pMsg: pointer to data structure of type WM_MESSAGE
* @retval None
*/
static void _cbButton_refresh(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_refresh(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}

/**
  * @brief  callback for Clock Settings
  * @param  pMsg: Pointer to Date structure
  * @retval None
  */

static void _cbClockSettings(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int Id, NCode;    
 
  static WM_HTIMER hTimerTime;
  
  hItem = pMsg->hWin;
  switch (pMsg->MsgId) {
    
  case WM_TIMER:
    WM_InvalidateWindow(pMsg->hWin);
    
    WM_RestartTimer(pMsg->Data.v, 100);
    break; 
    
  case WM_DELETE:
    WM_DeleteTimer(hTimerTime);
    break;
    
  case WM_INIT_DIALOG:
    
    hTimerTime = WM_CreateTimer(pMsg->hWin, 0, 100, 0);
    hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_INFO);
    IMAGE_SetBitmap(hItem, &bmClock); 

    hItem = BUTTON_CreateEx(0, 0, 240, 40, pMsg->hWin, WM_CF_SHOW, 0, ID_BUTTON_REFRESH);
    WM_SetCallback(hItem, _cbButton_refresh);    

    hItem = BUTTON_CreateEx(186, 3, 50, 50, pMsg->hWin, WM_CF_SHOW, 0, ID_BUTTON_EXIT);
    WM_SetCallback(hItem, _cbButton_exit);
    
    hItem = BUTTON_CreateEx(4, 3, 60, 50, pMsg->hWin, WM_CF_SHOW, 0, ID_BUTTON_SETTINGS);
    WM_SetCallback(hItem, _cbButton_settings);        
    
    hItem = BUTTON_CreateEx(0, 149, 91, 91, pMsg->hWin, WM_CF_SHOW, 0, ID_BUTTON_MINUS);
    WM_SetCallback(hItem, _cbButton_minus);      
    
    hItem = BUTTON_CreateEx(149, 149, 91, 91, pMsg->hWin, WM_CF_SHOW, 0, ID_BUTTON_PLUS);
    WM_SetCallback(hItem, _cbButton_plus);
 
    break;
    
  case WM_PAINT:
    
    /* Draw analog clock background */
    GUI_SetColor(0x0);
    GUI_AA_FillCircle(120, 140, 5);
    
    k_GetTime(&Time); 
    k_GetDate(&Date);
    
    GUI_UpdateClock (120, 140, Time.Hours, Time.Minutes, Time.Seconds);
    
    break;
    
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
    NCode = pMsg->Data.v;               /* Notification code */
    switch (Id) {  

    case ID_BUTTON_SETTINGS:
      
      if(NCode == WM_NOTIFICATION_RELEASED)
      {
        if (++settings > 2)
        {
          settings = 0;
        } 
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MINUS);
        WM_InvalidateWindow(hItem);
        WM_Update(hItem);  
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_PLUS);
        WM_InvalidateWindow(hItem);
        WM_Update(hItem);  

        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_REFRESH);
        WM_InvalidateWindow(hItem);
        WM_Update(hItem);  
        
      }
      break;
    case ID_BUTTON_EXIT:
      if(NCode == WM_NOTIFICATION_RELEASED)
      {
        GUI_EndDialog(pMsg->hWin, 0);
      }     
      break;
      
    case ID_BUTTON_PLUS:
      if (settings == 2)
      {         
        if(NCode == WM_NOTIFICATION_CLICKED)
        {
          if(Time.Hours < 12)
          {
            Time.Hours++;
          }
          else
          {
            Time.Hours = 1;
          }
          k_SetTime(&Time);  
          k_SetDate(&Date);          
        }  
      }
      else if (settings == 1)
      { 
        if(NCode == WM_NOTIFICATION_RELEASED)
        {
          if(Time.Minutes < 60)
          {
            Time.Minutes++;
          }
          else
          {
            Time.Minutes = 1;
          }
          k_SetTime(&Time);  
          k_SetDate(&Date);
        }
      }      
      break; 
      
    case ID_BUTTON_MINUS:
      if (settings == 2)
      { 
        if(NCode == WM_NOTIFICATION_CLICKED)
        {
          if(Time.Hours > 0)
          {
            Time.Hours--;
          }
          else
          {
            Time.Hours = 11;
          }
          k_SetTime(&Time);  
          k_SetDate(&Date);          
        }  
      }
      if (settings == 1)
      { 
        if(NCode == WM_NOTIFICATION_RELEASED)
        {
          if(Time.Minutes > 0)
          {
            Time.Minutes--;
          }
          else
          {
            Time.Minutes = 59;
          }
          k_SetTime(&Time);  
          k_SetDate(&Date);
        } 
      }      
      break;   
    }          
    break;    
default:
    WM_DefaultProc(pMsg);
  }
}

/**
  * @brief  Analog clock window Startup
  * @param  hWin: pointer to the parent handle.
  * @param  xpos: X position 
  * @param  ypos: Y position
  * @retval None
  */
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos)
{
  settings = 0;
  GUI_CreateDialogBox(_aDialogClockSettings, GUI_COUNTOF(_aDialogClockSettings), _cbClockSettings, hWin, xpos, ypos);
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

