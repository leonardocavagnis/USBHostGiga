/**
  ******************************************************************************
  * @file    usbh_hub.h
  * @author  Leonardo Cavagnis
  * @brief   This file contains all the prototypes for the usbh_hub.c
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

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_HUB_H
#define __USBH_HUB_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"


/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_HUB_CLASS
  * @{
  */

/** @defgroup USBH_HUB_CLASS
  * @brief This file is the Header file for usbh_core.c
  * @{
  */

#define USB_HUB_CLASS	                        0x09

#define HUB_MIN_POLL                          200
#define MAX_HUB_PORTS 						            4

#define USB_DESCRIPTOR_HUB                    0x29
#define USB_REQUEST_GET_DESCRIPTOR            0x06
#define HUB_FEATURE_SEL_PORT_POWER            0x08

#define USB_DEVICE_REQUEST_SET   			        0x00
#define USB_DEVICE_REQUEST_GET   			        0x01
#define USB_REQUEST_CLEAR_FEATURE   		      0x01
#define USB_REQUEST_SET_FEATURE     		      0x03
#define USB_REQUEST_GET_STATUS          	    0x00

/**
  * @}
  */

/** @defgroup USBH_HUB_CLASS_Exported_Types
  * @{
  */

typedef enum
{
	HUB_IDLE= 0,
	HUB_SYNC,
	HUB_BUSY,
	HUB_GET_DATA,
	HUB_POLL,
	HUB_LOOP_PORT_CHANGED,
	HUB_LOOP_PORT_WAIT,
	HUB_PORT_CHANGED,
	HUB_C_PORT_CONNECTION,
	HUB_C_PORT_RESET,
	HUB_RESET_DEVICE,
	HUB_DEV_ATTACHED,
	HUB_DEV_DETACHED,
	HUB_C_PORT_OVER_CURRENT,
	HUB_C_PORT_SUSPEND,
	HUB_ERROR,
} HUB_StateTypeDef;

typedef enum
{
	USBH_HUB_REQ_IDLE = 0,
	USBH_HUB_REQ_GET_DESCRIPTOR,
	USBH_HUB_REQ_SET_POWER,
	USBH_HUB_WAIT_PWRGOOD,
	USBH_HUB_REQ_DONE,
}
HUB_CtlStateTypeDef;

typedef struct __attribute__ ((packed)) _HUBDescriptor
{
  uint8_t  bLength;               // Length of this descriptor.
  uint8_t  bDescriptorType;       // Descriptor Type, value: 29H for hub descriptor
  uint8_t  bNbrPorts;             // Number of downstream facing ports that this hub supports
  uint16_t wHubCharacteristics;   //
  uint8_t  bPwrOn2PwrGood;        // Time (in 2 ms intervals) from the time the power-on sequence begins on a port until power is good on that port
  uint8_t  bHubContrCurrent;      // Maximum current requirements of the Hub Controller electronics in mA
  uint8_t  DeviceRemovable;       // Indicates if a port has a removable device attached.
  uint8_t  PortPwrCtrlMask;       // This field exists for reasons of compatibility with software written for 1.0 compliant devices.
} HUB_DescTypeDef;

/* Structure for HUB process */
typedef struct _HUB_Process
{
  uint8_t              InPipe;
  HUB_StateTypeDef     state;
  uint8_t              InEp;
  HUB_CtlStateTypeDef  ctl_state;
  uint16_t             length;
  uint8_t              ep_addr;
  uint16_t             poll;
  uint32_t             timer;
  uint8_t              DataReady;
  HUB_DescTypeDef      HUB_Desc;
  uint8_t              current_port;
} HUB_HandleTypeDef;

/**
  * @}
  */

/** @defgroup USBH_HUB_CLASS_Exported_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup USBH_HUB_CLASS_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_HUB_CLASS_Exported_Variables
  * @{
  */
extern USBH_ClassTypeDef  HUB_Class;
#define USBH_HUB_CLASS    &HUB_Class

/**
  * @}
  */

/** @defgroup USBH_HUB_CLASS_Exported_FunctionsPrototype
  * @{
  */
USBH_StatusTypeDef USBH_HUB_IOProcess(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_HUB_Init(USBH_HandleTypeDef *phost);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBH_HUB_H */

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
