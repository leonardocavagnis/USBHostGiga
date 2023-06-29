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

#define HUB_FEAT_SEL_PORT_CONNECTION          0x00
#define HUB_FEAT_SEL_C_HUB_LOCAL_POWER        0x00
#define HUB_FEAT_SEL_C_HUB_OVER_CURRENT       0x01

#define HUB_FEAT_SEL_PORT_CONN                0x00
#define HUB_FEAT_SEL_PORT_ENABLE              0x01
#define HUB_FEAT_SEL_PORT_SUSPEND             0x02
#define HUB_FEAT_SEL_PORT_OVER_CURRENT        0x03
#define HUB_FEAT_SEL_PORT_RESET               0x04
#define HUB_FEAT_SEL_PORT_POWER               0x08
#define HUB_FEAT_SEL_PORT_LOW_SPEED           0x09
#define HUB_FEAT_SEL_C_PORT_CONNECTION        0x10
#define HUB_FEAT_SEL_C_PORT_ENABLE            0x11
#define HUB_FEAT_SEL_C_PORT_SUSPEND           0x12
#define HUB_FEAT_SEL_C_PORT_OVER_CURRENT      0x13
#define HUB_FEAT_SEL_C_PORT_RESET             0x14
#define HUB_FEAT_SEL_PORT_INDICATOR           0x16

/**
  * @}
  */

/** @defgroup USBH_HUB_CLASS_Exported_Types
  * @{
  */

typedef enum
{
	USBH_HUB_IDLE = 0,
	USBH_HUB_SYNC,
	USBH_HUB_BUSY,
	USBH_HUB_GET_DATA,
	USBH_HUB_POLL,
	USBH_HUB_LOOP_PORT_CHANGED,
	USBH_HUB_LOOP_PORT_WAIT,
	USBH_HUB_PORT_CHANGED,
	USBH_HUB_C_PORT_CONNECTION,
	USBH_HUB_C_PORT_RESET,
	USBH_HUB_RESET_DEVICE,
	USBH_HUB_DEV_ATTACHED,
	USBH_HUB_DEV_DETACHED,
	USBH_HUB_C_PORT_OVER_CURRENT,
	USBH_HUB_C_PORT_SUSPEND,
	USBH_HUB_ERROR,
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

typedef struct __attribute__ ((packed)) _HUB_PortStatus
{
    union
    {
        struct
        {
        	uint8_t     PORT_CONNECTION      : 1;
        	uint8_t     PORT_ENABLE          : 1;
        	uint8_t     PORT_SUSPEND         : 1;
        	uint8_t     PORT_OVER_CURRENT    : 1;
        	uint8_t     PORT_RESET           : 1;
        	uint8_t     RESERVED_1           : 3;
        	uint8_t     PORT_POWER           : 1;
        	uint8_t     PORT_LOW_SPEED       : 1;
        	uint8_t     PORT_HIGH_SPEED      : 1;
        	uint8_t     PORT_TEST            : 1;
        	uint8_t     PORT_INDICATOR       : 1;
        	uint8_t     RESERVED_2           : 3;
        };
        uint16_t val;
    } wPortStatus;

    union
    {
        struct
        {
        	uint8_t     C_PORT_CONNECTION    : 1;
        	uint8_t     C_PORT_ENABLE        : 1;
        	uint8_t     C_PORT_SUSPEND       : 1;
        	uint8_t     C_PORT_OVER_CURRENT  : 1;
        	uint8_t     C_PORT_RESET         : 1;
        	uint16_t    RESERVED_1           : 11;
        };
        uint16_t val;
    } wPortChange;
} HUB_PortStatusTypeDef;

/* Structure for HUB process */
typedef struct _HUB_Process
{
  uint8_t               InPipe;
  HUB_StateTypeDef      state;
  uint8_t               InEp;
  HUB_CtlStateTypeDef   ctl_state;
  uint8_t              buffer[20];
  uint16_t              length;
  uint8_t               ep_addr;
  uint16_t              poll;
  uint32_t              timer;
  uint8_t               DataReady;
  HUB_DescTypeDef       HUB_Desc;
  uint8_t               current_port;
  HUB_PortStatusTypeDef port_status;
} HUB_HandleTypeDef;

typedef struct __attribute__ ((packed)) _HUB_PortChange
{
    union
    {
        struct
        {
          uint8_t     PORT_1    : 1;
          uint8_t     PORT_2    : 1;
          uint8_t     PORT_3    : 1;
          uint8_t     PORT_4    : 1;
          uint8_t     PORT_5    : 1;
          uint8_t     PORT_6    : 1;
          uint8_t     PORT_7    : 1;
          uint8_t     PORT_8    : 1;
        } bPorts;
        uint8_t val;
    };
} HUB_PortChangeTypeDef;

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
