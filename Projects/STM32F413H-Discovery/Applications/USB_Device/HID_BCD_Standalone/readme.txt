/**
  @page HID_BCD_Standalone USB Device Human Interface BCD application
  
  @verbatim
  ******************** (C) COPYRIGHT 2017 STMicroelectronics *******************
  * @file    USB_Device/HID_BCD_Standalone/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-February-2017 
  * @brief   Description of the USB HID BCD application.
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. All rights reserved.
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
   @endverbatim

@par Application Description 

The the STM32F4xx devices integrated battery charger detection circuitry supports USB-IF Battery 
Charger Detection, BCD,(revision 1.2). The hpcd.Init.battery_charging_enable in the usbd_conf.c 
should be set to 1 to enable the support for BCD.
This example is a part of the USB Device Library package using STM32Cube firmware. It describes how to 
use the BCD feature based on the USB HID device application.

Battery charger detection will begin automatically when VBUS is detected. The STM32F4xx includes the 
capability to detect various USB battery chargers and can detect a range of USB battery chargers including
a Standard Downstream Port (SDP), a Charging Downstream Port (CDP), and a Dedicated Charging Port (DCP).
For more information on USB battery charger detection, please see the USB Battery Charging Specification,
Revision 1.2 (Note 1).

In this example, we use STM32F413H-Discovery board LEDs to indicate the following connection status events:
-  LED3 is On for SDP 
-  LED4 is On for CDP
-  Both LED3 and LED4 Leds are On for DCP

At the beginning of the main program the HAL_Init() function is called to reset all the peripherals,
initialize the Flash interface and the systick. The user is provided with the SystemClock_Config()
function to configure the system clock (SYSCLK) to run at 216 MHz. The Full Speed (FS) USB module uses
internally a 48-MHz clock which is coming from a specific output of two PLLs: main PLL or PLL SAI.

The 48 MHz clock for the USB FS can be derived from one of the two following sources:
  � PLL clock (clocked by the HSE): If the USB uses the PLL as clock source, the PLL clock must be programmed
    to output 48 MHz frequency (USBCLK = PLLVCO/PLLQ).
  � PLLSAI clock (clocked by the HSE): If the USB uses the PLLSAI as clock source, the PLLSAI clock must be programmed
    to output 48 MHz frequency (USBCLK = PLLSAIVCO/PLLSAIP).

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.
      
For more details about the STM32Cube USB Device library, please refer to UM1734 
"STM32Cube USB Device library".
      

@par USB Library Configuration

@par Keywords

Middleware, USB Device, HID, BCD, USB-IF, VBUS, SDP, CDP, DCP

@par Directory contents

  - USB_Device/HID_BCD_Standalone/Src/main.c                  Main program
  - USB_Device/HID_BCD_Standalone/Src/system_stm32f4xx.c      STM32F4xx system clock configuration file
  - USB_Device/HID_BCD_Standalone/Src/stm32f4xx_it.c          Interrupt handlers
  - USB_Device/HID_BCD_Standalone/Src/usbd_conf.c             General low level driver configuration
  - USB_Device/HID_BCD_Standalone/Src/usbd_desc.c             USB device HID descriptor
  - USB_Device/HID_BCD_Standalone/Inc/main.h                  Main program header file
  - USB_Device/HID_BCD_Standalone/Inc/stm32f4xx_it.h          Interrupt handlers header file
  - USB_Device/HID_BCD_Standalone/Inc/stm32f4xx_hal_conf.h    HAL configuration file
  - USB_Device/HID_BCD_Standalone/Inc/usbd_conf.h             USB device driver Configuration file
  - USB_Device/HID_BCD_Standalone/Inc/usbd_desc.h             USB device HID descriptor header file  

	
@par Hardware and Software environment

  - This application runs on STM32F413xx devices.
    
  - This application has been tested with STMicroelectronics STM32F413H-Discovery Rev.B 
    boards and can be easily tailored to any other supported device 
    and development board.

  - STM32F413H-Discovery Set-up
    - Connect the STM32F413H-Discovery board to the PC through 'USB micro A-Male 
      to A-Male' cable to the connector CN17 (OTG-FS)

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the application
 
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
  