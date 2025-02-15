/**
  @page DFSDM_PulseSkipper Description
  
  @verbatim
  ******************** (C) COPYRIGHT 2017 STMicroelectronics *******************
  * @file    DFSDM/DFSDM_PulseSkipper/readme.txt
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-February-2017
  * @brief   Description of the DFSDM audio record example.
  ******************************************************************************
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
  @endverbatim

@par Example Description 

This example shows how to use the DFSDM HAL API to perform stereo audio recording.
For this audio record we will use two MP34DT01 digital microphones mounted on the board(2micros) or
those integrated on extension boards( 2 micro from MB1299). 

MP34DT01 runs with a clock frequency from 1MHz to 3,25MHz.
This clock frequency has to be delivered by DFSDM. So first channel of DFSDM will be
configurated to enable output clock generation, to choose audio clock as output clock source,
and output clock divider will be set to generate output clock on MP34DT01 frequency clock range.
Audio clock will be configurated to 49.142MHz, so DFSDM output clock divider will be set to 24.

The user can select 2 microphones  at the same time from the extention board MB1299
if it is fitted otherwise the 2 microphones embeded on STM32F413H-Discovery will be used.

//#define PLAY_DFSDM2_DATIN01     /* Use Mics: U2 and U3 from extention board */
//#define PLAY_DFSDM2_DATIN06     /* Use Mics: U2 and U4 from extention board */
//#define PLAY_DFSDM2_DATIN07     /* Use Mics: U2 and U5 from extention board */
//#define PLAY_DFSDM2_DATIN16     /* Use Mics: U3 and U4 from extention board */
//#define PLAY_DFSDM2_DATIN17     /* Use Mics: U3 and U5 from extention board */
//#define PLAY_DFSDM2_DATIN67     /* Use Mics: U4 and U5 from extention board */
#define PLAY_DFSDM12_DATIN10    /* Use Mics: U1 and U2 from extention board or U16 and U17 from discovery */
//#define PLAY_DFSDM12_DATIN11    /* Use Mics: U1 and U3 from extention board */
//#define PLAY_DFSDM12_DATIN16    /* Use Mics: U1 and U4 from extention board */
//#define PLAY_DFSDM12_DATIN17    /* Use Mics: U1 and U5 from extention board */

In addition, this example shows how to implement a pulse skipper to generate a delay on a selected channel.
In main.h, uncomment #define USE_CHANNEL_DELAY to enable pulse skipper.

Purpose of pulses skipper is to implement delay line for given input channel(s).
Given number of samples from input serial data stream will be discarded before they enter into the filter. 
The implementation of the Clock skipping is based on block MultiChannelDelay, Tim3 and TIM4.
The clock injected to digital microphones is provided by DFSDM2 clock out. It is distributed to 
OR gates, Trigger inputs of two timers and DFSDM1_CKOUT and DFSDM2_CKOUT to generate output clock 
signal for DFSDM pins.

@note Refer to the application note AN4957 "How to synchronize the DFSDMs filters 
and how to program the pulse skipper on STM32F413/423 line devices" for more details.

For each two Microphones selected, the user can select in which channel to generate pulse delay. 

#if defined(PLAY_DFSDM12_DATIN10)
/* Select channel to generate delay: either DFSDM1 CH1 or  DFSDM2 CH0 */
//#define GENERATE_DELAY_DFSDM1_CH1
//#define GENERATE_DELAY_DFSDM2_CH0
#elif defined(PLAY_DFSDM12_DATIN11)
/* Select channel to generate delay: either DFSDM1 CH1 or  DFSDM2 CH1 */
//#define GENERATE_DELAY_DFSDM1_CH1
//#define GENERATE_DELAY_DFSDM2_CH1
...
#endif

Playback of the recorded data will be performed on headset using HAL_I2S and WM8994 audio codec.
A circular playback buffer will be filled as soon as recorded data are available. 
When half left and right buffers will be filled, we put first parts of left and right channels data 
on first half of playback buffer. 
When left and right buffers will be full filled, we put second parts of left and right channels data 
on second half of playback buffer.


@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents  

  - DFSDM/DFSDM_PulseSkipper/Src/main.c                  Main program
  - DFSDM/DFSDM_PulseSkipper/Src/system_stm32f4xx.c      STM32F4xx system source file
  - DFSDM/DFSDM_PulseSkipper/Src/stm32f4xx_it.c          Interrupt handlers
  - DFSDM/DFSDM_PulseSkipper/Inc/main.h                  Main program header file
  - DFSDM/DFSDM_PulseSkipper/Inc/stm32f4xx_hal_conf.h    HAL configuration file
  - DFSDM/DFSDM_PulseSkipper/Inc/stm32f4xx_it.h          Interrupt handlers header file


@par Hardware and Software environment

  - This example runs on STM32F413xx devices.
    
  - This example has been tested with STMicroelectronics STM32F413ZH-Discovery revB
    board and can be easily tailored to any other supported device
    and development board.      

  - STM32F413H-Discovery Set-up :
    - Plug headset on AUDIO JACK connector CN5.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example 

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
