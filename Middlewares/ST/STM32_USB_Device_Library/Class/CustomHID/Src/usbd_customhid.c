/**
  ******************************************************************************
  * @file    usbd_customhid.c
  * @author  MCD Application Team
  * @version V2.4.1
  * @date    19-June-2015
  * @brief   This file provides the CUSTOM_HID core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                CUSTOM_HID Class  Description
  *          =================================================================== 
  *           This module manages the CUSTOM_HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (CUSTOM_HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - Usage Page : Generic Desktop
  *             - Usage : Vendor
  *             - Collection : Application 
  *      
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *           
  *      
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_customhid.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_CUSTOM_HID 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_CUSTOM_HID_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_CUSTOM_HID_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 
/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_CUSTOM_HID_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx);

static uint8_t  USBD_CUSTOM_HID_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx);

static uint8_t  USBD_CUSTOM_HID_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req);

static uint8_t  *USBD_CUSTOM_HID_GetCfgDesc (uint16_t *length);

static uint8_t  *USBD_CUSTOM_HID_GetDeviceQualifierDesc (uint16_t *length);

static uint8_t  USBD_CUSTOM_HID_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_CUSTOM_HID_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  USBD_CUSTOM_HID_EP0_RxReady (USBD_HandleTypeDef  *pdev);
/**
  * @}
  */ 

/** @defgroup USBD_CUSTOM_HID_Private_Variables
  * @{
  */ 


// (ALTERAR)
USBD_ClassTypeDef  USBD_CUSTOM_HID = 
{
  USBD_CUSTOM_HID_Init,
  USBD_CUSTOM_HID_DeInit,
  USBD_CUSTOM_HID_Setup,
  NULL, /*EP0_TxSent*/  
  USBD_CUSTOM_HID_EP0_RxReady, /*EP0_RxReady*/ /* STATUS STAGE IN */		// (NAO TEM)
  USBD_CUSTOM_HID_DataIn, /*DataIn*/
  USBD_CUSTOM_HID_DataOut,																							//(INCLUIR)
  NULL, /*SOF */
  NULL,
  NULL,      
  USBD_CUSTOM_HID_GetCfgDesc,
  USBD_CUSTOM_HID_GetCfgDesc, 
  USBD_CUSTOM_HID_GetCfgDesc,
  USBD_CUSTOM_HID_GetDeviceQualifierDesc,
};

/* USB CUSTOM_HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CUSTOM_HID_CfgDesc[USB_CUSTOM_HID_CONFIG_DESC_SIZ] __ALIGN_END =
{
		9,                          // bLength;
    2,                          // bDescriptorType;
    LOBYTE(CONFIG_DESC_SIZE),      // wTotalLength
    HIBYTE(CONFIG_DESC_SIZE),
    NUM_INTERFACE,              // bNumInterfaces
    1,                          // bConfigurationValue
		0,                          // iConfiguration
		DEVICE_ATTRIBUTES,					// bmAttributes
		DEVICE_POWER,								// bMaxPower
	
		//Interface 0
		9,													//bLength (length of interface descriptor 9 bytes)
		4,													//bDescriptorType (4 is interface)
		0,													//bInterfaceNumber (This is interface 0)
		0,													//bAlternateSetting (used to select alternate setting.  notused)
		2,													//bNumEndpoints (this interface has 2 endpoints)
		0xFF,												//bInterfaceClass (Vendor Defined is 255)
		0x5D,												//bInterfaceSubClass
		0x01,												//bInterfaceProtocol
		0,													//iInterface (Index of string descriptor for describing this notused)
		//Some sort of common descriptor? I pulled this from Message Analyzer dumps of an actual controller
		17,33,0,1,1,37,129,20,0,0,0,0,19,2,8,0,0,
		//Endpoint 1 IN
		7,													//bLength (length of ep1in in descriptor 7 bytes)
		5,													//bDescriptorType (5 is endpoint)
		0x81,												//bEndpointAddress (0x81 is IN1)
		0x03,												//bmAttributes (0x03 is interrupt no synch, usage type data)
		0x20, 0x00,									//wMaxPacketSize (0x0020 is 1x32 bytes)
		4,													//bInterval (polling interval in frames 4 frames)
		//Endpoint 2 OUT
		7,													//bLength (length of ep2out in descriptor 7 bytes)
		5,													//bDescriptorType (5 is endpoint)
		0x02,												//bEndpointAddress (0x02 is OUT2)
		0x03,												//bmAttributes (0x03 is interrupt no synch, usage type data)
		0x20, 0x00,									//wMaxPacketSize (0x0020 is 1x32 bytes)
		8,													//bInterval (polling interval in frames 8 frames)

		//Interface 1
		9,													//bLength (length of interface descriptor 9 bytes)
		4,													//bDescriptorType (4 is interface)
		1,													//bInterfaceNumber (This is interface 1)
		0,													//bAlternateSetting (used to select alternate setting.  notused)
		4,													//bNumEndpoints (this interface has 4 endpoints)
		0xFF,												//bInterfaceClass (Vendor Defined is 255)
		0x5D,												//bInterfaceSubClass (93)
		0x03,												//bInterfaceProtocol (3)
		0,													//iInterface (Index of string descriptor for describing this notused)
		//A different common descriptor? I pulled this from Message Analyzer dumps of an actual controller
		27,33,0,1,1,1,131,64,1,4,32,22,133,0,0,0,0,0,0,22,5,0,0,0,0,0,0,
		//Endpoint 3 IN
		7,													//bLength (length of ep3in descriptor 7 bytes)
		5,													//bDescriptorType (5 is endpoint)
		0x83,												//bEndpointAddress (0x83 is IN3)
		0x03,												//bmAttributes (0x03 is interrupt no synch, usage type data)
		0x20, 0x00,									//wMaxPacketSize (0x0020 is 1x32 bytes)
		2,													//bInterval (polling interval in frames 2 frames)
		//Endpoint 4 OUT
		7,													//bLength (length of ep4out descriptor 7 bytes)
		5,													//bDescriptorType (5 is endpoint)
		0x04,												//bEndpointAddress (0x04 is OUT4)
		0x03,												//bmAttributes (0x03 is interrupt no synch, usage type data)
		0x20, 0x00,									//wMaxPacketSize (0x0020 is 1x32 bytes)
		4,													//bInterval (polling interval in frames 4 frames)
		//Endpoint 5 IN
		7,													//bLength (length of ep5in descriptor 7 bytes)
		5,													//bDescriptorType (5 is endpoint)
		0x85,												//bEndpointAddress (0x85 is IN5)
		0x03,												//bmAttributes (0x03 is interrupt no synch, usage type data)
		0x20, 0x00,									//wMaxPacketSize (0x0020 is 1x32 bytes)
		64,													//bInterval (polling interval in frames 64 frames)
		//Endpoint 5 OUT (shares endpoint number with previous)
		7,													//bLength (length of ep5out descriptor 7 bytes)
		5,													//bDescriptorType (5 is endpoint)
		0x05,												//bEndpointAddress (0x05 is OUT5)
		0x03,												//bmAttributes (0x03 is interrupt no synch, usage type data)
		0x20, 0x00,									//wMaxPacketSize (0x0020 is 1x32 bytes)
		16,													//bInterval (polling interval in frames 16 frames)

		//Interface 2
		9,													//bLength (length of interface descriptor 9 bytes)
		4,													//bDescriptorType (4 is interface)
		2,													//bInterfaceNumber (This is interface 2)
		0,													//bAlternateSetting (used to select alternate setting.  notused)
		1,													//bNumEndpoints (this interface has 4 endpoints)
		0xFF,												//bInterfaceClass (Vendor Defined is 255)
		0x5D,												//bInterfaceSubClass (93)
		0x02,												//bInterfaceProtocol (3)
		0,													//iInterface (Index of string descriptor for describing this notused)
		//Common Descriptor.  Seems that these come after every interface description?
		9,33,0,1,1,34,134,7,0,
		//Endpoint 6 IN
		7,													//bLength (length of ep6in descriptor 7 bytes)
		5,													//bDescriptorType (5 is endpoint)
		0x86,												//bEndpointAddress (0x86 is IN6)
		0x03,												//bmAttributes (0x03 is interrupt no synch, usage type data)
		0x20, 0x00,									//wMaxPacketSize (0x0020 is 1x32 bytes)
		16,													//bInterval (polling interval in frames 64 frames)+
		//Interface 3
		//This is the interface on which all the security handshaking takes place
		//We don't use this but it could be used for man-in-the-middle stuff
		9,													//bLength (length of interface descriptor 9 bytes)
		4,													//bDescriptorType (4 is interface)
		3,													//bInterfaceNumber (This is interface 3)
		0,													//bAlternateSetting (used to select alternate setting.  notused)
		0,													//bNumEndpoints (this interface has 0 endpoints ???)
		0xFF,												//bInterfaceClass (Vendor Defined is 255)
		0xFD,												//bInterfaceSubClass (253)
		0x13,												//bInterfaceProtocol (19)
		4,				//iInterface (Computer never asks for this, but an x360 would. so include one day?)
		//Another interface another Common Descriptor
		6,65,0,1,1,3
} ;

/* USB CUSTOM_HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CUSTOM_HID_Desc[USB_CUSTOM_HID_DESC_SIZ] __ALIGN_END =
{
  /* 18 */
  0x09,         /*bLength: CUSTOM_HID Descriptor size*/
  CUSTOM_HID_DESCRIPTOR_TYPE, /*bDescriptorType: CUSTOM_HID*/
  0x11,         /*bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of CUSTOM_HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  USBD_CUSTOM_HID_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CUSTOM_HID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */ 

/** @defgroup USBD_CUSTOM_HID_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         Initialize the CUSTOM_HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_CUSTOM_HID_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx)
{
  uint8_t ret = 0;
  USBD_CUSTOM_HID_HandleTypeDef     *hhid;
  
	
	// Open EP IN 
  USBD_LL_OpenEP(pdev,
                 0x81,
                 USBD_EP_TYPE_INTR,
                 CUSTOM_HID_EPIN_SIZE);  
  
  // Open EP OUT 
  USBD_LL_OpenEP(pdev,
                 CUSTOM_HID_EPOUT_ADDR,
                 USBD_EP_TYPE_INTR,
                 0x20);
	
  
	USBD_LL_OpenEP(pdev,
                 0x04,
                 USBD_EP_TYPE_INTR,
                 0x20);
	
	//USBD_LL_OpenEP(pdev, XINPUT_RX_ENDPOINT, USBD_EP_TYPE_BULK, XINPUT_RX_SIZE );
	//USBD_LL_OpenEP(pdev, XINPUT_TX_ENDPOINT, USBD_EP_TYPE_INTR, XINPUT_TX_SIZE );
	
  pdev->pClassData = USBD_malloc(sizeof (USBD_CUSTOM_HID_HandleTypeDef));
  
  if(pdev->pClassData == NULL)
  {
    ret = 1; 
  }
  else
  {
    hhid = (USBD_CUSTOM_HID_HandleTypeDef*) pdev->pClassData;
      
    hhid->state = CUSTOM_HID_IDLE;
    ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->Init();
          /* Prepare Out endpoint to receive 1st packet */ 
    USBD_LL_PrepareReceive(pdev, CUSTOM_HID_EPOUT_ADDR, hhid->Report_buf, 
                           USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
		
		USBD_LL_PrepareReceive(pdev, XINPUT_RX_ENDPOINT, hhid->Report_buf, XINPUT_RX_SIZE );
		
		USBD_LL_PrepareReceive(pdev, 0x04, hhid->Report_buf, 
                           USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
		
		
  }
    
  return ret;
}

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         DeInitialize the CUSTOM_HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_CUSTOM_HID_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx)
{
  /* Close CUSTOM_HID EP IN */
  USBD_LL_CloseEP(pdev,
                  CUSTOM_HID_EPIN_ADDR);
  
  /* Close CUSTOM_HID EP OUT */
  USBD_LL_CloseEP(pdev,
                  CUSTOM_HID_EPOUT_ADDR);
  
  /* FRee allocated memory */
  if(pdev->pClassData != NULL)
  {
    ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->DeInit();
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }
  return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_Setup
  *         Handle the CUSTOM_HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_CUSTOM_HID_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req)
{
  uint16_t len = 0;
  uint8_t  *pbuf = NULL;
  USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)pdev->pClassData;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :  
    switch (req->bRequest)
    {
      
      
    case CUSTOM_HID_REQ_SET_PROTOCOL:
      hhid->Protocol = (uint8_t)(req->wValue);
      break;
      
    case CUSTOM_HID_REQ_GET_PROTOCOL:
      USBD_CtlSendData (pdev, 
                        (uint8_t *)&hhid->Protocol,
                        1);    
      break;
      
    case CUSTOM_HID_REQ_SET_IDLE:
      hhid->IdleState = (uint8_t)(req->wValue >> 8);
      break;
      
    case CUSTOM_HID_REQ_GET_IDLE:
      USBD_CtlSendData (pdev, 
                        (uint8_t *)&hhid->IdleState,
                        1);        
      break;      
    
    case CUSTOM_HID_REQ_SET_REPORT:
      hhid->IsReportAvailable = 1;
      USBD_CtlPrepareRx (pdev, hhid->Report_buf, (uint8_t)(req->wLength));
      
      break;
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL; 
    }
    break;
    
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR: 
      if( req->wValue >> 8 == CUSTOM_HID_REPORT_DESC)
      {
        len = MIN(USBD_CUSTOM_HID_REPORT_DESC_SIZE , req->wLength);
        pbuf =  ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->pReport;
      }
      else if( req->wValue >> 8 == CUSTOM_HID_DESCRIPTOR_TYPE)
      {
        pbuf = USBD_CUSTOM_HID_Desc;   
        len = MIN(USB_CUSTOM_HID_DESC_SIZ , req->wLength);
      }
      
      USBD_CtlSendData (pdev, 
                        pbuf,
                        len);
      
      break;
      
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        (uint8_t *)&hhid->AltSetting,
                        1);
      break;
      
    case USB_REQ_SET_INTERFACE :
      hhid->AltSetting = (uint8_t)(req->wValue);
      break;
    }
  }
  return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_SendReport 
  *         Send CUSTOM_HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_CUSTOM_HID_SendReport     (USBD_HandleTypeDef  *pdev, 
                                 uint8_t *report,
                                 uint16_t len)
{
  USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)pdev->pClassData;
  
  if (pdev->dev_state == USBD_STATE_CONFIGURED )
  {
    if(hhid->state == CUSTOM_HID_IDLE)
    {
      hhid->state = CUSTOM_HID_BUSY;
      USBD_LL_Transmit (pdev, 
                        0x81,                                      
                        report,
                        len);
			
			
    }
  }
  return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_GetCfgDesc 
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_CUSTOM_HID_GetCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_CUSTOM_HID_CfgDesc);
  return USBD_CUSTOM_HID_CfgDesc;
}

/**
  * @brief  USBD_CUSTOM_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_CUSTOM_HID_DataIn (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{
  
  /* Ensure that the FIFO is empty before a new transfer, this condition could 
  be caused by  a new transfer before the end of the previous transfer */
  ((USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassData)->state = CUSTOM_HID_IDLE;

  return USBD_OK;
}

extern uint8_t USB_RX_Buffer[8];

/**
  * @brief  USBD_CUSTOM_HID_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_CUSTOM_HID_DataOut (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{
  
  USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)pdev->pClassData;  
  
  //((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->OutEvent(hhid->Report_buf[0], 
   //                                                         hhid->Report_buf[1]);
    
  //USBD_LL_PrepareReceive(pdev, CUSTOM_HID_EPOUT_ADDR , hhid->Report_buf, 
     //                    USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
	
	//USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)pdev->pClassData;
	
	for( uint8_t i = 0 ; i < 8 ; i++ ){
		USB_RX_Buffer[i] = hhid->Report_buf[i];
	}

  return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_EP0_RxReady
  *         Handles control request data.
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_CUSTOM_HID_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)pdev->pClassData;  

  if (hhid->IsReportAvailable == 1)
  {
    ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->OutEvent(hhid->Report_buf[0], 
                                                              hhid->Report_buf[1]);
    hhid->IsReportAvailable = 0;      
  }

  return USBD_OK;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static uint8_t  *USBD_CUSTOM_HID_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_CUSTOM_HID_DeviceQualifierDesc);
  return USBD_CUSTOM_HID_DeviceQualifierDesc;
}

/**
* @brief  USBD_CUSTOM_HID_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CUSTOMHID Interface callback
  * @retval status
  */
uint8_t  USBD_CUSTOM_HID_RegisterInterface  (USBD_HandleTypeDef   *pdev, 
                                             USBD_CUSTOM_HID_ItfTypeDef *fops)
{
  uint8_t  ret = USBD_FAIL;
  
  if(fops != NULL)
  {
    pdev->pUserData= fops;
    ret = USBD_OK;    
  }
  
  return ret;
}
/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
