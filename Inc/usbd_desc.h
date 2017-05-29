/**
  ******************************************************************************
  * @file           : usbd_desc.h
  * @version        : v1.0_Cube
  * @brief          : Header for usbd_desc file.
  ******************************************************************************
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  * 1. Redistributions of source code must retain the above copyright notice,
  * this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  * this list of conditions and the following disclaimer in the documentation
  * and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of its contributors
  * may be used to endorse or promote products derived from this software
  * without specific prior written permission.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_DESC__H__
#define __USBD_DESC__H__

#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "usbd_def.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USB_DESC
  * @brief general defines for the usb device library file
  * @{
  */ 

/** @defgroup USB_DESC_Exported_Defines
  * @{
  */
	 
	 // from msf_xinput (TROCAR) de lugar

	#define ENDPOINT_UNUSED			0x00
	#define ENDPOINT_TRANSIMIT_ONLY		0x15
	#define ENDPOINT_RECEIVE_ONLY		0x19
	#define ENDPOINT_TRANSMIT_AND_RECEIVE	0x1D

	#define DEVICE_CLASS	0xFF
  #define DEVICE_SUBCLASS	0xFF
  #define DEVICE_PROTOCOL	0xFF
  #define DEVICE_VERSION 0x0114
  #define DEVICE_ATTRIBUTES 0xA0
  #define DEVICE_POWER	0xFA
  #define VENDOR_ID		0x045e
  #define PRODUCT_ID		0x028e
  #define MANUFACTURER_NAME	{'©','M','i','c','r','o','s','o','f','t'}
  #define MANUFACTURER_NAME_LEN	10
  #define PRODUCT_NAME		{'C','o','n','t','r','o','l','l','e','r'}
  #define PRODUCT_NAME_LEN	10
  #define EP0_SIZE	8
  #define NUM_ENDPOINTS	6
  #define NUM_USB_BUFFERS	24
  #define NUM_INTERFACE	4
  #define XINPUT_INTERFACE	0
  #define XINPUT_RX_ENDPOINT	2
  #define XINPUT_RX_SIZE 8
  #define XINPUT_TX_ENDPOINT	1
  #define XINPUT_TX_SIZE 20
  #define CONFIG_DESC_SIZE 153
  #define ENDPOINT1_CONFIG ENDPOINT_TRANSIMIT_ONLY
  #define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_ONLY
  #define ENDPOINT3_CONFIG ENDPOINT_TRANSIMIT_ONLY
  #define ENDPOINT4_CONFIG ENDPOINT_RECEIVE_ONLY
  #define ENDPOINT5_CONFIG ENDPOINT_TRANSMIT_AND_RECEIVE
  #define ENDPOINT6_CONFIG ENDPOINT_TRANSIMIT_ONLY
	 

/**
  * @}
  */ 

/** @defgroup USBD_DESC_Exported_TypesDefinitions
  * @{
  */
/**
  * @}
  */ 

/** @defgroup USBD_DESC_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBD_DESC_Exported_Variables
  * @{
  */ 
extern USBD_DescriptorsTypeDef FS_Desc;
/**
  * @}
  */ 

/** @defgroup USBD_DESC_Exported_FunctionsPrototype
  * @{
  */ 
  
/**
  * @}
  */ 
#ifdef __cplusplus
}
#endif

#endif /* __USBD_DESC_H */

/**
  * @}
  */ 

/**
* @}
*/ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
