 /******************************************************************************
 * Module: Port
 * File Name: Port.h
 * Description: Header file for TM4C123GH6PM Microcontroller - Port Driver.
 * Author: Salma Hamdy
 ******************************************************************************/

#ifndef PORT_H
#define PORT_H

/* Id for the company in the AUTOSAR
 * for example Mohamed Tarek's ID = 1000 :) */
#define PORT_VENDOR_ID    (1000U)

/* Port Module Id */
#define PORT_MODULE_ID    (120U)

/* Port Instance Id */
#define PORT_INSTANCE_ID  (0U)

/*
 * Module Version 1.0.0
 */
#define PORT_SW_MAJOR_VERSION           (1U)
#define PORT_SW_MINOR_VERSION           (0U)
#define PORT_SW_PATCH_VERSION           (0U)

/*
 * AUTOSAR Version 4.0.3
 */
#define PORT_AR_RELEASE_MAJOR_VERSION   (4U)
#define PORT_AR_RELEASE_MINOR_VERSION   (0U)
#define PORT_AR_RELEASE_PATCH_VERSION   (3U)

/*
 * Macros for Port Status
 */
#define PORT_INITIALIZED                (1U)
#define PORT_NOT_INITIALIZED            (0U)

/* Standard AUTOSAR types */
#include "Std_Types.h"

/* AUTOSAR checking between Std Types and Dio Modules */
#if ((STD_TYPES_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Std_Types.h does not match the expected version"
#endif

/* Port Pre-Compile Configuration Header file */
#include "Port_Cfg.h"

/* AUTOSAR Version checking between Port_Cfg.h and Port.h files */
#if ((PORT_CFG_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of PORT_Cfg.h does not match the expected version"
#endif

/* Software Version checking between Port_Cfg.h and Port.h files */
#if ((PORT_CFG_SW_MAJOR_VERSION != PORT_SW_MAJOR_VERSION)\
 ||  (PORT_CFG_SW_MINOR_VERSION != PORT_SW_MINOR_VERSION)\
 ||  (PORT_CFG_SW_PATCH_VERSION != PORT_SW_PATCH_VERSION))
  #error "The SW version of Port_Cfg.h does not match the expected version"
#endif

/* Non AUTOSAR files */
#include "Common_Macros.h"

/******************************************************************************
 *                      API Service Id Macros                                 *
 ******************************************************************************/

/* Service ID for PORT Init */
#define PORT_INIT_SID                             (uint8)0x00

/* Service ID for PORT Set Pin Direction */
#define PORT_SET_PIN_DIRECTION_SID                (uint8)0x01

/* Service ID for PORT Refresh Port Direction */
#define PORT_REFRESH_PORT_DIRECTION_SID           (uint8)0x02

/* Service ID for PORT Get Version Info */
#define PORT_GET_VERSION_INFO_SID                 (uint8)0x03

/* Service ID for PORT Set Pin Mode */
#define PORT_SET_PIN_MODE_SID                     (uint8)0x04

/*******************************************************************************
 *                      DET Error Codes                                        *
 *******************************************************************************/

/* DET code to report Invalid Port Pin ID */
#define PORT_E_PARAM_PIN                           (uint8)0x0A

/* DET code to report Port Pin Not configured as changeable */
#define PORT_E_DIRECTION_UNCHANGEABLE              (uint8)0x0B

/* PORT_Init API service called with wrong parameter */
#define PORT_E_PARAM_CONFIG                        (uint8)0x0C

/*API Port_SetPinMode service called when mode is unchangeable.*/
#define PORT_E_PARAM_INVALID_MODE                  (uint8)0x0D
#define PORT_E_MODE_UNCHANGEABLE                   (uint8)0x0E

/*API service called without module initialization*/
#define PORT_E_UNINIT                              (uint8)0x0F

/*APIs called with a Null Pointer*/
#define PORT_E_PARAM_POINTER                       (uint8)0x10

/*******************************************************************************
 *                              Module Data Types                              *
 *******************************************************************************/

/* Data type for the symbolic name of a port pin */
typedef uint8 Port_PinType;

 /* Different port pin modes */
typedef uint8 Port_PinModeType;

/* Description: Enum to hold PIN direction */
typedef enum
{
    PORT_PIN_IN,PORT_PIN_OUT
}Port_PinDirectionType;

/* Description: Enum to hold internal resistor type for PIN */
typedef enum
{
    OFF,PULL_UP,PULL_DOWN
}Port_InternalResistorType;

/*Initial Value for a port Pin*/
typedef enum
{
    PORT_PIN_LEVEL_LOW, PORT_PIN_LEVEL_HIGH
}Port_PinLevelValueType;

/*Port pin mode from mode list for use with Port_Init() function*/
typedef enum
{
    PORT_PIN_MODE_ADC,
    PORT_PIN_MODE_CAN,
    PORT_PIN_MODE_DIO,
    PORT_PIN_MODE_DIO_GPT,
    PORT_PIN_MODE_DIO_WDG,
    PORT_PIN_MODE_FLEXRAY,
    PORT_PIN_MODE_ICU,
    PORT_PIN_MODE_LIN,
    PORT_PIN_MODE_MEM,
    PORT_PIN_MODE_PWM,
    PORT_PIN_MODE_SPI
}Port_PinInitialModeType;

/*Pin Direction Changeable or not*/
#define PORT_PIN_DIRECTION_CHANGEABLE             (STD_ON)
#define PORT_PIN_DIRECTION_NOT_CHANGEABLE         (STD_OFF)

/*Pin Mode Changeable or not*/
#define PORT_PIN_MODE_CHANGEABLE                  (STD_ON)
#define PORT_PIN_MODE_NOT_CHANGEABLE              (STD_OFF)

/* Description: Structure to configure each individual Pin:
 *	1. the PORT Which the pin belongs to --> 0, 1, 2, 3, 4 or 5
 *	2. the number of the pin in the PORT --> 0, 1, 2, 3, 4, 5, 6, 7
 *  3. the direction of pin --> PORT_PIN_IN or PORT_PIN_OUT
 *  4. the internal resistor --> OFF, PULL_UP up or Pull PULL_DOWN
 *  5. the pin mode --> DIO, ADC, SPI, or...
 *  6. the pin direction changeable during runtime --> STD_ON or STD_OFF
 *  7. the pin mode changeable during runtime --> STD_ON or STD_OFF
 *  8. the pin initial value level --> PORT_PIN_LEVEL_LOW or PORT_PIN_LEVEL_HIGH
 */
typedef struct
{
    uint8 port_num;
    Port_PinType pin_num;
    Port_PinDirectionType pin_direction;
    Port_InternalResistorType pin_resistor;
    Port_PinModeType pin_mode;
    uint8 pin_direction_changeable;
    uint8 pin_mode_changeable;
    Port_PinLevelValueType pin_initial_value;
}Port_ConfigPin;

/* Array of Structures for all configured pins */
typedef struct
{
    Port_ConfigPin Pin[PORT_CONFIGURED_PINS];
}Port_ConfigType;

/*******************************************************************************
 *                      Function Prototypes                                    *
 *******************************************************************************/

/* Function for Port Initialization API
 * * Description: Function to Setup the pin configuration:
*              - Setup the pin as Digital GPIO pin
*              - Setup the direction of the GPIO pin
*              - Provide initial value for o/p pin
*              - Setup the internal resistor for i/p pin*/
void Port_Init(const Port_ConfigType *ConfigPtr);

/* Function for Port Set Pin Direction API */
#if (PORT_SET_PIN_DIRECTION_API == STD_ON)
void Port_SetPinDirection(Port_PinType Pin,Port_PinDirectionType Direction);
#endif

/* Function for Refresh Port Direction API */
void Port_RefreshPortDirection(void);

/* Function for Port Get Version Info API */
#if (PORT_VERSION_INFO_API == STD_ON)
void Port_GetVersionInfo(Std_VersionInfoType* versioninfo);
#endif

/* Function for Port Set Pin Mode API */
#if (PORT_SET_PIN_MODE_API == STD_ON)
void Port_SetPinMode(Port_PinType Pin,Port_PinModeType Mode);
#endif

/*******************************************************************************
 *                       External Variables                                    *
 *******************************************************************************/

/* Extern PB structures to be used by Dio and other modules */
extern const Port_ConfigType Port_Configuration;

#endif /* PORT_H */
