/**
  @page CustomHID_Standalone USB Device Custom HID application
  
  @verbatim
  ******************** (C) COPYRIGHT 2017 STMicroelectronics *******************
  * @file    USB_Device/CustomHID_Standalone/readme.txt 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    17-February-2017
  * @brief   Description of the USB Custom HID application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
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
  @endverbatim
  
@par Application Description

This application is a part of the USB Device Library package using STM32Cube firmware. It describes how to 
use USB device application based on the Custom HID Class on the STM32F4xx devices.

This is a typical application on how to use the STM32F4xx USB OTG Device peripheral, where the STM32 MCU is 
enumerated as a HID compliant device using the native PC Host HID driver to which the STM324x9I-EVAL
board is connected.

At the beginning of the main program the HAL_Init() function is called to reset all the peripherals,
initialize the Flash interface and the systick. The user is provided with the SystemClock_Config()
function to configure the system clock (SYSCLK) to run at 168 MHz. The Full Speed (FS) USB module uses
internally a 48-MHz clock, which is generated from an integrated PLL. In the High Speed (HS) mode the
USB clock (60 MHz) is driven by the ULPI.

It's worth noting that the system clock (SYSCLK) can be configured, depending on the used USB Core:
 - SYSCLK is set to 168 MHz: for FS Core (FS or HS-IN-FS), because used embedded PHY
                             requires 48 MHz clock, achieved only when system clock
                             is set to 168 MHz.
 - SYSCLK is set to 180 MHz: for only HS Core, since no embedded PHY is used.

To test the application, user must proceed as follows:
 - Start the "USB HID Demonstrator" PC applet, available for download from www.st.com, 
   and connect STM32 USB to PC
 - The device should be detected and shown in the USB HID target box
 - Press Graphic View button
 - Select "use SET_FEATURE" or "use SET_REPORT" in order to use SET_REPORT request to send HID Report
   for LED control
 - Use the potentiometer of the STM324x9I-EVAL board to transfer the result of the converted voltage
  (via the ADC) the to the PC host (these values are sent to the PC using the endpoint1 IN)   
 - Make sure that following report ID are configured: LED1 ID (0x1), LED2 ID(0x2), LED3 ID(0x3),
   LED4 ID(0x4), BUTTON1_ID(0x6) and Potentiometer_ID(0x7).
 - Select LEDs to switch on/off on the STM324x9I-EVAL board: a SET_REPORT request will be sent

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

To select the appropriate USB Core to work with, user must add the following macro defines within the
compiler preprocessor (already done in the preconfigured projects provided with this application):
      - "USE_USB_HS" when using USB High Speed (HS) Core
      - "USE_USB_FS" when using USB Full Speed (FS) Core 
      - "USE_USB_HS" and "USE_USB_HS_IN_FS" when using USB High Speed (HS) Core in FS mode


@par Directory contents

  - USB_Device/CustomHID_Standalone/Src/main.c                  Main program
  - USB_Device/CustomHID_Standalone/Src/system_stm32f4xx.c      STM32F4xx system clock configuration file
  - USB_Device/CustomHID_Standalone/Src/stm32f4xx_it.c          Interrupt handlers
  - USB_Device/CustomHID_Standalone/Src/stm32f4xx_hal_msp.c     HAL MSP module
  - USB_Device/CustomHID_Standalone/Src/usbd_conf.c             General low level driver configuration
  - USB_Device/CustomHID_Standalone/Src/usbd_desc.c             USB device descriptor
  - USB_Device/CustomHID_Standalone/Inc/main.h                  Main program header file
  - USB_Device/CustomHID_Standalone/Inc/stm32f4xx_it.h          Interrupt handlers header file
  - USB_Device/CustomHID_Standalone/Inc/stm32f4xx_hal_conf.h    HAL configuration file
  - USB_Device/CustomHID_Standalone/Inc/usbd_conf.h             USB device driver Configuration file
  - USB_Device/CustomHID_Standalone/Inc/usbd_desc.h             USB device descriptor header file

	
@par Hardware and Software environment

  - This application runs on STM32F429xx/STM32F439xx devices.
    
  - This application has been tested with STMicroelectronics STM324x9I-EVAL RevB 
    evaluation boards and can be easily tailored to any other supported device 
    and development board.

  - STM324x9I-EVAL RevB Set-up
    - Connect the STM324x9I-EVAL board to the PC through 'USB micro A-Male 
      to A-Male' cable to the connector:
      - CN9 : to use USB High Speed (HS) 
      - CN14: to use USB Full Speed (FS) with embedded PHY(U7)
              Please ensure that jumper JP16 is not fitted.
      - CN15: to use USB HS-IN-FS.
              Note that some FS signals are shared with the HS ULPI bus, so some PCB rework is needed.
              For more details, refer to section 2.8 USB OTG2 HS & FS in UM1667
		  
    
@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - In the workspace toolbar select the project configuration:
   - STM324x9I-EVAL_USBH-HS: to configure the project for STM32F4xx devices using USB OTG HS peripheral
   - STM324x9I-EVAL_USBH-FS: to configure the project for STM32F4xx devices using USB OTG FS peripheral
   - STM324x9I-EVAL_USBH-HS-IN-FS: to configure the project for STM32F4xx devices and use USB OTG HS 
                                   peripheral In FS (using embedded PHY).
 - Run the application
 
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
 