/**
  ******************************************************************************
  * @file    audio_player_win.c
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    17-February-2017
  * @brief   Audio player functions
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
#include "audio_player_res.c"
#include "../Common/audio_if.h" 
#include "audio_player_app.h"

/** @addtogroup AUDIO_PLAYER_MODULE
  * @{
  */

/** @defgroup AUDIO_PLAYER
  * @brief audio player routines
  * @{
  */

/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);

/* Private typedef -----------------------------------------------------------*/
K_ModuleItem_Typedef  audio_player_board =
{  
  1,
  (uint8_t*)"",
  &bmaudio5,
  Startup,
}
;

typedef union
{
  uint32_t d32;
  struct
  {
    uint32_t repeat         : 2;
    uint32_t pause          : 1;
    uint32_t play           : 1;
    uint32_t stop           : 1;    
    uint32_t mute           : 1;
    uint32_t volume         : 8;   
    uint32_t reserved       : 18;
  }b;
}
AudioSettingsTypeDef;

/* Private defines -----------------------------------------------------------*/
#define REPEAT_NONE            0
#define REPEAT_ONCE            1
#define REPEAT_ALL             2


#define PLAY_INACTIVE                 0x00
#define PLAY_ACTIVE                   0x01

#define PAUSE_INACTIVE                0x00
#define PAUSE_ACTIVE                  0x01

#define STOP_INACTIVE                 0x00  
#define STOP_ACTIVE                   0x01  


#define ID_FRAMEWIN_INFO      (GUI_ID_USER + 0x01)

/* Button */
#define ID_VOLUME_UP_BUTTON        (GUI_ID_USER + 0x20)
#define ID_PREVIOUS_BUTTON        (GUI_ID_USER + 0x21)
#define ID_PLAY_BUTTON            (GUI_ID_USER + 0x22)
#define ID_NEXT_BUTTON            (GUI_ID_USER + 0x23)
#define ID_VOLUME_DOWN_BUTTON     (GUI_ID_USER + 0x24)
#define ID_PAUSE_BUTTON           (GUI_ID_USER + 0x25)

#define ID_EXIT_BUTTON            (GUI_ID_USER + 0x2A)

/* Audio information */
#define ID_TITLE              (GUI_ID_USER + 0x40)

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static const GUI_WIDGET_CREATE_INFO _aDialog[] = 
{
  { WINDOW_CreateIndirect, "Audio Player", 0, 0,   0, 240, 240, FRAMEWIN_CF_MOVEABLE },
};

static WM_HTIMER hTimerTime;
uint8_t elapsed = 0;

static WAV_InfoTypedef         WavInfo;
static  FILELIST_FileTypeDef   WavList;
static uint16_t              Audio_FilePos = 0; 
static AudioSettingsTypeDef  PlayerSettings;
long progress;
  
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Add entire folder to play list.
  * @param  Foldername: pointer to folder name.
  * @retval None
  */
static void _AddEntireFolder(char *Foldername, int update)
{
  FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn;
  static char tmp[FILEMGR_FILE_NAME_SIZE]; 
  
#if _USE_LFN
  static char lfn[_MAX_LFN];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif
  
  res = f_opendir(&dir, Foldername);
  
  if (res == FR_OK)
  {
    
    while (1)
    {
      res = f_readdir(&dir, &fno);
      
      if (res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      if (fno.fname[0] == '.')
      {
        continue;
      }
#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif
      
      if (WavList.ptr < FILEMGR_LIST_DEPDTH)
      {
        if ((fno.fattrib & AM_DIR) == 0)
        {
          if(((strstr(fn, ".wav")) || (strstr(fn, ".WAV"))))
          {
            
            strcpy(tmp, Foldername);
            strcat(tmp, "/");
            strcat(tmp, fn);
            strncpy((char *)WavList.file[WavList.ptr].name, (char *)tmp, FILEMGR_FILE_NAME_SIZE);
            WavList.ptr++;
          }
        }
      }   
    }
  }
  f_closedir(&dir);
}

/**
  * @brief  Play wav file.
  * @param  filename: pointer to file name.
  * @retval None
  */
static void _PlayFile(char *filename)
{
  if(haudio.out.state == AUDIOPLAYER_STOP)
  {
    if(AUDIOPLAYER_GetFileInfo(filename, &WavInfo) == 0)
    {            
      /* Open audio file */
      if(AUDIOPLAYER_SelectFile(filename) == 0)
      {
        /* start playing */
        AUDIOPLAYER_Play(WavInfo.SampleRate);
        if(PlayerSettings.b.mute == MUTE_ON)
        {
          AUDIOPLAYER_Mute(MUTE_ON);
        }
      }
    }  
  }
}

/**
  * @brief  Notify the end of wav file.
  * @param  None.
  * @retval Audio state.
  */
AUDIOPLAYER_ErrorTypdef  AUDIOPLAYER_NotifyEndOfFile(void)
{  
  AUDIOPLAYER_Stop();
  
  if(Audio_FilePos < (WavList.ptr - 1))
  {
    Audio_FilePos++;      
  }
  else 
  {        
    Audio_FilePos = 0;          
  }
  
  _PlayFile((char *)WavList.file[Audio_FilePos].name);
  
  return AUDIOPLAYER_ERROR_NONE;
}

/**
  * @brief  Paints Play button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_play(BUTTON_Handle hObj) {
  int Index;
    
  Index = (WIDGET_GetState(hObj) & BUTTON_STATE_PRESSED) ? 1 : 0;
  
  if(Index)
  {
      if(PlayerSettings.b.pause == PAUSE_ACTIVE)
      {
        GUI_DrawBitmap(&bmplay_pressed, 0, 0);
      }
      else if(PlayerSettings.b.pause == PLAY_ACTIVE)
      {
        GUI_DrawBitmap(&bmpause, 0, 0);
      }
  }
  else
  {    
      if(PlayerSettings.b.play == PLAY_INACTIVE)
      {    
        GUI_DrawBitmap(&bmplay_umpressed, 0, 0);
      }
      else if(PlayerSettings.b.play == PLAY_ACTIVE)
      {
        GUI_DrawBitmap(&bmpause, 0, 0);
      }
  }
}

/**
  * @brief  Paints Previous button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_previous(BUTTON_Handle hObj) {
  int Index;
  
  Index = (WIDGET_GetState(hObj) & BUTTON_STATE_PRESSED) ? 1 : 0;

  if(Index)
  {
    GUI_DrawBitmap(&bmleft_pressed, 0, 0);
  }
  else
  {
    GUI_DrawBitmap(&bmleft_umpressed, 0, 0);
  }
}

/**
  * @brief  Paints Next button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_next(BUTTON_Handle hObj) {
  int Index;
  
  Index = (WIDGET_GetState(hObj) & BUTTON_STATE_PRESSED) ? 1 : 0;

  if(Index)
  {
    GUI_DrawBitmap(&bmright_pressed, 0, 0);
  }
  else
  {
    GUI_DrawBitmap(&bmright_umpressed, 0, 0);
  }
}

/**
  * @brief  Paints Next button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_list(BUTTON_Handle hObj) {
  int Index;
  
  Index = (WIDGET_GetState(hObj) & BUTTON_STATE_PRESSED) ? 1 : 0;

  if(Index)
  {
    GUI_DrawBitmap(&bmbot_pressed, 0, 0);
  }
  else
  {
    GUI_DrawBitmap(&bmbot_umpressed, 0, 0);
  }
}

/**
  * @brief  Paints speaker button
  * @param  speaker_status: speaker button status
  * @retval None
  */
static void _OnPaint_speaker(BUTTON_Handle hObj, uint32_t speaker_status) {
  int Index;
  
  Index = (WIDGET_GetState(hObj) & BUTTON_STATE_PRESSED) ? 1 : 0;
  
  if(Index)
  {
    GUI_DrawBitmap(&bmtop_pressed, 0, 0);    
  }
  else
  {
    GUI_DrawBitmap(&bmtop_umpressed, 0, 0);
  }  
}

/**
  * @brief  callback for play button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE 
  * @retval None
  */
static void _cbButton_play(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
    case WM_PAINT:
      _OnPaint_play(pMsg->hWin);
      break;
    default:
      /* The original callback */
      BUTTON_Callback(pMsg);
      break;
  }
}

/**
  * @brief  callback for next button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton_next(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
    case WM_PAINT:
      _OnPaint_next(pMsg->hWin);
      break;
    default:
      /* The original callback */
      BUTTON_Callback(pMsg);
      break;
  }
}

/**
  * @brief  callback for previous button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton_previous(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
    case WM_PAINT:
      _OnPaint_previous(pMsg->hWin);
      break;
    default:
      /* The original callback */
      BUTTON_Callback(pMsg);
      break;
  }
}

/**
  * @brief  callback for previous button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton_list(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
    case WM_PAINT:
      _OnPaint_list(pMsg->hWin);
      break;
    default:
      /* The original callback */
      BUTTON_Callback(pMsg);
      break;
  }
}

/**
  * @brief  callback for speaker button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton_speaker(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
    case WM_PAINT:
      _OnPaint_speaker(pMsg->hWin, PlayerSettings.b.mute);
      break;
    default:
      /* The original callback */
      BUTTON_Callback(pMsg);
      break;
  }
}

/**
* @brief  Paints record button
* @param  hObj: button handle
* @retval None
*/
static void _OnPaint_exit(BUTTON_Handle hObj)
{
	GUI_DrawBitmap(&bmaudio_exit, 20, 0);
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
  * @brief  main callback for Audio Player
  * @param  pMsg: Pointer to Date structure
  * @retval None
  */

static void _cbMainDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int Id, NCode;
  
  switch (pMsg->MsgId) {
    
  case WM_INIT_DIALOG:
    
    hItem = BUTTON_CreateEx(186, 3, 50, 30, pMsg->hWin, WM_CF_SHOW, 0, ID_EXIT_BUTTON);
    WM_SetCallback(hItem, _cbButton_exit);    
    
    hItem = BUTTON_CreateEx(24, 73, 59, 135, pMsg->hWin, WM_CF_SHOW, 0, ID_PREVIOUS_BUTTON);
    WM_SetCallback(hItem, _cbButton_previous);   

    hItem = BUTTON_CreateEx(54, 42, 135, 59, pMsg->hWin, WM_CF_SHOW, 0, ID_VOLUME_UP_BUTTON);
    WM_SetCallback(hItem, _cbButton_speaker);  

    hItem = BUTTON_CreateEx(78, 97, 85, 85, pMsg->hWin, WM_CF_SHOW, 0, ID_PLAY_BUTTON);
    WM_SetCallback(hItem, _cbButton_play);

    hItem = BUTTON_CreateEx(55, 179, 135, 59, pMsg->hWin, WM_CF_SHOW, 0, ID_VOLUME_DOWN_BUTTON);
    WM_SetCallback(hItem, _cbButton_list);     
    
    hItem = BUTTON_CreateEx(161, 72, 59, 135, pMsg->hWin, WM_CF_SHOW, 0, ID_NEXT_BUTTON);
    WM_SetCallback(hItem, _cbButton_next);    
    
    hTimerTime = WM_CreateTimer(pMsg->hWin, 0, 333, 0);
  
    /* Detect if no wav file in the SDCard */
    if(WavList.ptr == 0)
    {
      hItem = TEXT_CreateEx(20,  10,  170,  25, pMsg->hWin, WM_CF_SHOW, 0, ID_TITLE, "No available wav files");
      hItem = WM_GetDialogItem(pMsg->hWin, ID_TITLE);
      TEXT_SetFont(hItem, &GUI_FontLubalGraph20B); 
      TEXT_SetTextColor(hItem,GUI_WHITE); 
    }
    
    break;
    
  case WM_TIMER:
    
    Id = WM_GetTimerId(pMsg->Data.v);
    
    if(AUDIOPLAYER_GetState() == AUDIOPLAYER_PLAY)
    {
      progress = AUDIOPLAYER_GetProgress();
      
      /* Set progress slider position */
      progress = (long)(progress/(WavInfo.FileSize/360));
      
      WM_InvalidateWindow(pMsg->hWin);
 
    }
    else if(AUDIOPLAYER_GetState() == AUDIOPLAYER_EOF)
    {
      AUDIOPLAYER_Process();
    }
    
    AUDIOPLAYER_Process();
    WM_RestartTimer(pMsg->Data.v, 1000);    
   
    break;
    
  case WM_PAINT:
    /* Draw audio player background */
    GUI_SetBkColor(0xFF272223);
    GUI_Clear();   

    GUI_SetColor(0xEAB548);
    GUI_FillRect(0, 0, 240, 40);    
    
      GUI_SetPenSize( 5 );
      GUI_SetColor(0xFF00FF00);
      GUI_AA_DrawArc(120, 140, 45, 45, 90, 90+progress);       
      
    break; 
    
  case WM_DELETE:
    WM_DeleteTimer(hTimerTime);
    AUDIOPLAYER_DeInit();   
    PlayerSettings.b.mute = MUTE_OFF;  
    PlayerSettings.b.pause = PAUSE_INACTIVE;
    PlayerSettings.b.play = PLAY_INACTIVE;
    PlayerSettings.b.stop = STOP_INACTIVE; 
    progress = 0;
    break;    

    
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
    NCode = pMsg->Data.v;               /* Notification code */
    switch(Id) {
      
      
      /* Notifications sent by 'Volume Up' Button */
    case ID_VOLUME_UP_BUTTON: 
      if(NCode == WM_NOTIFICATION_RELEASED)
      {
        if(PlayerSettings.b.mute == MUTE_ON)
        {          
          AUDIOPLAYER_Mute(MUTE_OFF);
          PlayerSettings.b.mute = MUTE_OFF;
        } 
        if(PlayerSettings.b.volume < 90)
        {
          AUDIOPLAYER_SetVolume(PlayerSettings.b.volume + 10);
          PlayerSettings.b.volume = PlayerSettings.b.volume + 10;
        }
        else
        {
          AUDIOPLAYER_SetVolume(100);
          PlayerSettings.b.volume = 100;
        }
      }
      break;
      
      /* Notifications sent by 'Volume Down' Button */
    case ID_VOLUME_DOWN_BUTTON:
      if(NCode == WM_NOTIFICATION_RELEASED)
      {
        if(PlayerSettings.b.volume > 10)
        {
          AUDIOPLAYER_SetVolume(PlayerSettings.b.volume - 10);
          PlayerSettings.b.volume = PlayerSettings.b.volume - 10;
        }
        else
        {
          if(PlayerSettings.b.mute == MUTE_OFF)
          {          
            AUDIOPLAYER_Mute(MUTE_ON);
            PlayerSettings.b.mute = MUTE_ON;
          }          
        }
      }
      break;        

      /* Notifications sent by 'Play' Button */
    case ID_PLAY_BUTTON: 
      if(NCode == WM_NOTIFICATION_RELEASED)
      {        
        if(AUDIOPLAYER_GetState() == AUDIOPLAYER_STOP)
        {
          if(WavList.ptr > 0)
          {
            _PlayFile((char *)WavList.file[Audio_FilePos].name);
            PlayerSettings.b.play = PLAY_ACTIVE;
            hItem = WM_GetDialogItem(pMsg->hWin, ID_PLAY_BUTTON);
            WM_InvalidateWindow(hItem);
            WM_Update(hItem);               
          }
          else
          {
          }
          
          }
        else if(AUDIOPLAYER_GetState() == AUDIOPLAYER_PLAY)
        {
            PlayerSettings.b.pause = PAUSE_ACTIVE;
            PlayerSettings.b.play = PLAY_INACTIVE;
            AUDIOPLAYER_Pause();
            
            hItem = WM_GetDialogItem(pMsg->hWin, ID_PLAY_BUTTON);
            WM_InvalidateWindow(hItem);
            WM_Update(hItem);   
        }
        else if(AUDIOPLAYER_GetState() == AUDIOPLAYER_PAUSE)
        {
            PlayerSettings.b.pause = PAUSE_INACTIVE;
            PlayerSettings.b.play = PLAY_ACTIVE;
            AUDIOPLAYER_Resume();
            
            hItem = WM_GetDialogItem(pMsg->hWin, ID_PLAY_BUTTON);
            WM_InvalidateWindow(hItem);
            WM_Update(hItem);   
        }        
      }
      break;
      
    /* Notifications sent by 'Next' Button */
    case ID_NEXT_BUTTON: 
      if(NCode == WM_NOTIFICATION_RELEASED)
      {
        if( WavList.ptr > 0)
        {        
          if(Audio_FilePos < (WavList.ptr - 1))
          {
            Audio_FilePos++;
          }
          else
          {            
            Audio_FilePos = 0;  
          }
          
          if(AUDIOPLAYER_GetState() == AUDIOPLAYER_PLAY)
          {    
            if(PlayerSettings.b.pause == PAUSE_ACTIVE)
            {  
              PlayerSettings.b.pause = PAUSE_INACTIVE;
              hItem = WM_GetDialogItem(pMsg->hWin, ID_PAUSE_BUTTON);
              WM_InvalidateWindow(hItem);
              WM_Update(hItem);
              
              PlayerSettings.b.play = PLAY_ACTIVE;
              hItem = WM_GetDialogItem(pMsg->hWin, ID_PLAY_BUTTON);
              WM_InvalidateWindow(hItem);
              WM_Update(hItem);              
            }
            
            AUDIOPLAYER_Stop();
            _PlayFile((char *)WavList.file[Audio_FilePos].name); 
          }
        }
      }
      break;
      
      /* Notifications sent by 'Previous' Button */
    case ID_PREVIOUS_BUTTON: 
      if(NCode == WM_NOTIFICATION_RELEASED)
      {
        if( WavList.ptr > 0)
        {
          if(Audio_FilePos > 0)
          {             
            Audio_FilePos--;       
          }
          else if(PlayerSettings.b.repeat == REPEAT_ALL)
          {          
            Audio_FilePos = (WavList.ptr - 1);         
          }          
          
          if(AUDIOPLAYER_GetState() == AUDIOPLAYER_PLAY)
          {  
            if(PlayerSettings.b.pause == PAUSE_ACTIVE)
            {  
              PlayerSettings.b.pause = PAUSE_INACTIVE;
              hItem = WM_GetDialogItem(pMsg->hWin, ID_PAUSE_BUTTON);
              WM_InvalidateWindow(hItem);
              WM_Update(hItem);
              
              PlayerSettings.b.play = PLAY_ACTIVE;
              hItem = WM_GetDialogItem(pMsg->hWin, ID_PLAY_BUTTON);
              WM_InvalidateWindow(hItem);
              WM_Update(hItem);    
            }
            
            AUDIOPLAYER_Stop();            
            _PlayFile((char *)WavList.file[Audio_FilePos].name);              
          }
        }
      }
      break;  
      
    case ID_EXIT_BUTTON:
      if(NCode == WM_NOTIFICATION_RELEASED)
      {
        GUI_EndDialog(pMsg->hWin, 0);
      }
      break;
    }
    break;
    
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/**
  * @brief  Audio player window Startup
  * @param  hWin: pointer to the parent handle.
  * @param  xpos: X position 
  * @param  ypos: Y position
  * @retval None
  */
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos)
{  
  /* Prevent Volume 0 */
  if(PlayerSettings.b.volume == 0)
  {
    PlayerSettings.b.volume = DEFAULT_AUDIO_OUT_VOLUME;
  } 
   
  Audio_FilePos = 0;
  
  WavList.ptr = 0;
  _AddEntireFolder("0:", 0);
  _AddEntireFolder("0:/Audio", 0);
  
  /*Initialize audio Interface */
  AUDIOPLAYER_Init(PlayerSettings.b.volume);
    
  /*audioWin = */GUI_CreateDialogBox(_aDialog, GUI_COUNTOF(_aDialog), _cbMainDialog, hWin, xpos, ypos);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
