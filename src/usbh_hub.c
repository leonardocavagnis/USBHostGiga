/**
  ******************************************************************************
  * @file    usbh_hub.c
  * @author  Leonardo Cavagnis
  * @brief   This file is the HUB Layer Handlers for USB Host HUB class.
  *
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbh_hub.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_HUB_CLASS
  * @{
  */

/** @defgroup USBH_HUB_CORE
  * @brief    This file includes HUB Layer Handlers for USB Host HUB class.
  * @{
  */


/** @defgroup USBH_HUB_CORE_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_HUB_CORE_Private_Defines
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_HUB_CORE_Private_Macros
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_HUB_CORE_Private_Variables
  * @{
  */
static uint8_t  HUB_NumPorts = 0;
static uint16_t HUB_PwrGood  = 0;
/**
  * @}
  */


/** @defgroup USBH_HUB_CORE_Private_FunctionPrototypes
  * @{
  */

static USBH_StatusTypeDef USBH_HUB_InterfaceInit(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_HUB_InterfaceDeInit(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_HUB_ClassRequest(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_HUB_Process(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_HUB_SOFProcess(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef HUB_GetDescriptor(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef HUB_SetPortPower(USBH_HandleTypeDef *phost, uint8_t hub_port);
static USBH_StatusTypeDef HUB_RequestOp(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t dataDirection, uint8_t porta, uint8_t *buffer, uint16_t size);

USBH_ClassTypeDef  HUB_Class =
{
  "HUB",
  USB_HUB_CLASS,
  USBH_HUB_InterfaceInit,
  USBH_HUB_InterfaceDeInit,
  USBH_HUB_ClassRequest,
  USBH_HUB_Process,
  USBH_HUB_SOFProcess,
  NULL,
};
/**
  * @}
  */


/** @defgroup USBH_HUB_CORE_Private_Functions
  * @{
  */

/**
  * @brief  USBH_HUB_InterfaceInit
  *         The function init the HUB class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_HUB_InterfaceInit(USBH_HandleTypeDef *phost)
{
  USBH_StatusTypeDef status;
  uint8_t interface;
  HUB_HandleTypeDef *HUB_Handle;

  interface = USBH_FindInterface(phost, phost->pActiveClass->ClassCode, 0x00, 0x00);
  
  if ((interface == 0xFFU) || (interface >= USBH_MAX_NUM_INTERFACES)) /* Not Valid Interface */
  {
    USBH_DbgLog("Cannot Find the interface for %s class.", phost->pActiveClass->Name);
    return USBH_FAIL;
  }

  status = USBH_SelectInterface(phost, interface);

  if (status != USBH_OK)
  {
    return USBH_FAIL;
  }

  phost->pActiveClass->pData = (HUB_HandleTypeDef *)USBH_malloc(sizeof(HUB_HandleTypeDef));
  HUB_Handle = (HUB_HandleTypeDef *) phost->pActiveClass->pData;

  if (HUB_Handle == NULL)
  {
    USBH_DbgLog("Cannot allocate memory for HUB Handle");
    return USBH_FAIL;
  }

  /* Initialize hub handler */
  (void)USBH_memset(HUB_Handle, 0, sizeof(HUB_HandleTypeDef));

  HUB_Handle->state     = HUB_IDLE;
  HUB_Handle->ctl_state = HUB_REQ_IDLE;
  HUB_Handle->ep_addr   = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress;
  HUB_Handle->length    = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].wMaxPacketSize;
  HUB_Handle->poll      = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bInterval;

  if (HUB_Handle->poll < HUB_MIN_POLL) HUB_Handle->poll = HUB_MIN_POLL;

  USBH_UsrLog("HUB device POLL %d, LEN %d", HUB_Handle->poll, HUB_Handle->length);

  if(phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress & 0x80)
  {
    HUB_Handle->InEp = (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress);
    HUB_Handle->InPipe  = USBH_AllocPipe(phost, HUB_Handle->InEp);

    // Open pipe for IN endpoint
    USBH_OpenPipe(phost, HUB_Handle->InPipe, HUB_Handle->InEp, phost->device.address, phost->device.speed, USB_EP_TYPE_INTR, HUB_Handle->length);

    USBH_LL_SetToggle(phost, HUB_Handle->InPipe, 0);
  }

  return USBH_OK;
}


/**
  * @brief  USBH_HUB_InterfaceDeInit
  *         The function DeInit the Pipes used for the HUB class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_HUB_InterfaceDeInit(USBH_HandleTypeDef *phost)
{
  HUB_HandleTypeDef *HUB_Handle = (HUB_HandleTypeDef *) phost->pActiveClass->pData;

  if ((HUB_Handle->InPipe) != 0U)
  {
    (void)USBH_ClosePipe(phost, HUB_Handle->InPipe);
    (void)USBH_FreePipe(phost, HUB_Handle->InPipe);
    HUB_Handle->InPipe = 0U;     /* Reset the Channel as Free */
  }

  if ((phost->pActiveClass->pData) != NULL)
  {
    USBH_free(phost->pActiveClass->pData);
    phost->pActiveClass->pData = 0U;
  }

	return USBH_OK;
}

/**
  * @brief  USBH_HUB_ClassRequest
  *         The function is responsible for handling Standard requests
  *         for HUB class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_HUB_ClassRequest(USBH_HandleTypeDef *phost)
{
  USBH_StatusTypeDef status = USBH_BUSY;
  HUB_HandleTypeDef *HUB_Handle = (HUB_HandleTypeDef *) phost->pActiveClass->pData;

  static uint8_t port = 1;

  switch (HUB_Handle->ctl_state)
  {
    case HUB_REQ_IDLE:
    case HUB_REQ_GET_DESCRIPTOR:
      port = 1;
        if(HUB_GetDescriptor(phost) == USBH_OK) HUB_Handle->ctl_state = HUB_REQ_SET_POWER;
      break;

    case HUB_REQ_SET_POWER:
      // Turn on power for each hub port...
      if(HUB_SetPortPower(phost, port) == USBH_OK)
      {
        // Reach last port
        if(HUB_NumPorts == port)
          HUB_Handle->ctl_state = HUB_WAIT_PWRGOOD;
        else
          port++;
      }
      break;

    case HUB_WAIT_PWRGOOD:
      USBH_Delay(HUB_PwrGood);
      HUB_Handle->ctl_state = HUB_REQ_DONE;
      break;

    case HUB_REQ_DONE:
      USBH_UsrLog("%d HUB PORTS ENABLED", HUB_NumPorts);
      status = USBH_OK;
      break;
  }
  
  return status;
}

/**
  * @brief  USBH_HUB_Process
  *         The function is for managing state machine for HUB data transfers
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_HUB_Process(USBH_HandleTypeDef *phost)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(phost);

  return USBH_OK;
}

/**
  * @brief  USBH_HUB_SOFProcess
  *         The function is for managing SOF callback
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_HUB_SOFProcess(USBH_HandleTypeDef *phost)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(phost);

  return USBH_OK;
}

static USBH_StatusTypeDef HUB_GetDescriptor(USBH_HandleTypeDef *phost)
{
	USBH_StatusTypeDef status = USBH_BUSY;
	static uint8_t state = 0;

	HUB_HandleTypeDef *HUB_Handle = (HUB_HandleTypeDef *) phost->pActiveClass->pData;

	switch(state)
	{
	case 0:
		phost->Control.setup.b.bmRequestType  = USB_D2H|USB_REQ_RECIPIENT_DEVICE|USB_REQ_TYPE_CLASS;
		phost->Control.setup.b.bRequest  	    = USB_REQ_GET_DESCRIPTOR;
		phost->Control.setup.b.wValue.bw.msb  = 0;
		phost->Control.setup.b.wValue.bw.lsb  = USB_DESCRIPTOR_HUB;
		phost->Control.setup.b.wIndex.w  	    = 0;
		phost->Control.setup.b.wLength.w 	     = sizeof(HUB_UsbDescriptorTypeDef);

		if(USBH_CtlReq(phost, HUB_Handle->buffer, sizeof(HUB_UsbDescriptorTypeDef)) == USBH_OK) state = 1;
    break;

	case 1:
    {
    HUB_UsbDescriptorTypeDef  *HUB_Desc = (HUB_UsbDescriptorTypeDef *) HUB_Handle->buffer;
    HUB_NumPorts = (HUB_Desc->bNbrPorts > MAX_HUB_PORTS) ? MAX_HUB_PORTS : HUB_Desc->bNbrPorts;
    HUB_PwrGood  = (HUB_Desc->bPwrOn2PwrGood * 2);
    state = 0;
    status = USBH_OK;
    }
		break;
	}

	return status;
}

static USBH_StatusTypeDef HUB_SetPortPower(USBH_HandleTypeDef *phost, uint8_t hub_port)
{
	return HUB_RequestOp(phost, USB_REQ_SET_FEATURE, HUB_FEATURE_SEL_PORT_POWER, USB_DEVICE_REQUEST_SET, hub_port, 0, 0);
}

static USBH_StatusTypeDef HUB_RequestOp(USBH_HandleTypeDef *phost, uint8_t request, uint8_t feature, uint8_t dataDirection, uint8_t porta, uint8_t *buffer, uint16_t size)
{
	uint8_t bmRequestType = (dataDirection == USB_DEVICE_REQUEST_GET) ? USB_D2H : USB_H2D;

	phost->Control.setup.b.bmRequestType  = bmRequestType|USB_REQ_RECIPIENT_OTHER|USB_REQ_TYPE_CLASS;
	phost->Control.setup.b.bRequest  	    = request;
	phost->Control.setup.b.wValue.bw.msb  = feature;
	phost->Control.setup.b.wValue.bw.lsb  = 0;
	phost->Control.setup.b.wIndex.bw.msb  = porta;
	phost->Control.setup.b.wIndex.bw.lsb  = 0;
	phost->Control.setup.b.wLength.w      = size;

	return USBH_CtlReq(phost, buffer, size);
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

/**
  * @}
  */

/**
  * @}
  */
