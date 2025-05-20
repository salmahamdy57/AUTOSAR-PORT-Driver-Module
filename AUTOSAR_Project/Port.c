/******************************************************************************
 * Module: Port
 * File Name: Port.c
 * Description: Source file for TM4C123GH6PM Microcontroller - Port Driver.
 * Author: Salma Hamdy
 ******************************************************************************/

#include "Port.h"
#include "Port_Regs.h"
#include "tm4c123gh6pm_registers.h"

#if (PORT_DEV_ERROR_DETECT == STD_ON)

#include "Det.h"
/* AUTOSAR Version checking between Det and Dio Modules */
#if ((DET_AR_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
        || (DET_AR_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
        || (DET_AR_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
#error "The AR version of Det.h does not match the expected version"
#endif

#endif

STATIC const Port_ConfigPin *Port_configPtr = NULL_PTR;
STATIC uint8 Port_Status = PORT_NOT_INITIALIZED;

/************************************************************************************
 * Service Name: Port_Init
 * Service ID[hex]: 0x00
 * Sync/Async: Synchronous
 * Reentrancy: Non Reentrant
 * Parameters (in): ConfigPtr - Pointer to post-build configuration data
 * Parameters (inout): None
 * Parameters (out): None
 * Return value: None
 * Description: Function to initialize the Port Driver Module
 ************************************************************************************/
void Port_Init(const Port_ConfigType * ConfigPtr)
{
    volatile uint32 * Port_Base_Address_Ptr = NULL_PTR; /* point to the required Port Registers base address */
    volatile Port_PinType pinIndex = PORT_PIN0_ID;      /* pin index starting with oin 0 */

    /* Check for development error */
#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* check if the input configuration pointer is not a NULL_PTR */
    if (NULL_PTR == ConfigPtr)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_INIT_SID, PORT_E_PARAM_CONFIG);
    }
    else
#endif
    {
        /*
         * Set the module state to initialized and point to the PB configuration structure using a global pointer.
         * This global pointer is global to be used by other functions to read the PB configuration structures
         */
        Port_Status = PORT_INITIALIZED;
        Port_configPtr = ConfigPtr->Pin; /* address of the first Pin structure --> Pin[0] */
    }

    for(pinIndex = PORT_PIN0_ID; pinIndex < PORT_CONFIGURED_PINS; pinIndex++)
    {
        switch(Port_configPtr[pinIndex].port_num)
        {
        case PORT_PORTA_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
            break;
        case PORT_PORTB_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
            break;
        case PORT_PORTC_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
            break;
        case PORT_PORTD_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
            break;
        case PORT_PORTE_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
            break;
        case PORT_PORTF_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
            break;
        }

        if( ((Port_configPtr[pinIndex].port_num == PORT_PORTD_ID) && (Port_configPtr[pinIndex].pin_num == PORT_PIN7_ID)) || \
                ((Port_configPtr[pinIndex].port_num == PORT_PORTF_ID) && (Port_configPtr[pinIndex].pin_num == PORT_PIN0_ID)) ) /* PD7 or PF0 */
        {
            *(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;                     /* Unlock the GPIOCR register */
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_LOCK_REG_OFFSET) , Port_configPtr[pinIndex].port_num);  /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
        }
        else if( (Port_configPtr[pinIndex].port_num == PORT_PORTC_ID) && (Port_configPtr[pinIndex].pin_num <= PORT_PIN3_ID) ) /* PC0 to PC3 */
        {
            /* Do Nothing ...  this is the JTAG pins */
            continue;
        }
        else
        {
            /* Do Nothing ... No need to unlock the commit register for this pin */
        }

        /* Set Pin Direction */
        if(Port_configPtr[pinIndex].pin_direction == PORT_PIN_OUT)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DIR_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);               /* Set the corresponding bit in the GPIODIR register to configure it as output pin */

            if(Port_configPtr[pinIndex].pin_initial_value == STD_HIGH)
            {
                SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DATA_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);          /* Set the corresponding bit in the GPIODATA register to provide initial value 1 */
            }
            else
            {
                CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DATA_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);        /* Clear the corresponding bit in the GPIODATA register to provide initial value 0 */
            }
        }
        else if(Port_configPtr[pinIndex].pin_direction == PORT_PIN_IN)
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DIR_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);             /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */

            if(Port_configPtr[pinIndex].pin_resistor == PULL_UP)
            {
                SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_PULL_UP_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);       /* Set the corresponding bit in the GPIOPUR register to enable the internal pull up pin */
            }
            else if(Port_configPtr[pinIndex].pin_resistor == PULL_DOWN)
            {
                SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_PULL_DOWN_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);     /* Set the corresponding bit in the GPIOPDR register to enable the internal pull down pin */
            }
            else
            {
                CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_PULL_UP_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);     /* Clear the corresponding bit in the GPIOPUR register to disable the internal pull up pin */
                CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_PULL_DOWN_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);   /* Clear the corresponding bit in the GPIOPDR register to disable the internal pull down pin */
            }
        }
        else
        {
            /* Do Nothing */
        }

        /* Set Pin Mode */
        if(Port_configPtr[pinIndex].pin_mode == PORT_PIN_MODE_DIO) /* DIO Mode */
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);             /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
            *(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_configPtr[pinIndex].pin_num * 4));     /* Clear the PMCx bits for this pin */
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);         /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
        }
        else if(Port_configPtr[pinIndex].pin_mode == PORT_PIN_MODE_ADC) /* ADC Mode */
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);        /* Set the corresponding bit in the GPIOAMSEL register to enable analog functionality on this pin */
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);             /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
            *(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_configPtr[pinIndex].pin_num * 4));     /* Clear the PMCx bits for this pin */
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);       /* Clear the corresponding bit in the GPIODEN register to disable digital functionality on this pin */
        }
        else /* Any other Mode */
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);               /* Enable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
            *(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_configPtr[pinIndex].pin_num * 4));     /* Clear the PMCx bits for this pin first */
            *(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_CTL_REG_OFFSET) |= (0x0000000F << (Port_configPtr[pinIndex].pin_num * 4));      /* Then Set the PMCx bits for this pin */
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);         /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
        }
    }
}

/************************************************************************************
 * Service Name: Port_SetPinDirection
 * Service ID[hex]: 0x01
 * Sync/Async: Synchronous
 * Reentrancy: Reentrant
 * Parameters (in): Pin - Port Pin ID number
 *                  Direction - Port Pin Direction
 * Parameters (inout): None
 * Parameters (out): None
 * Return value: None
 * Description: Function to set the port pin direction
 ************************************************************************************/
#if (PORT_SET_PIN_DIRECTION_API == STD_ON)
void Port_SetPinDirection(Port_PinType Pin,Port_PinDirectionType Direction)
{
    volatile uint32 * Port_Base_Address_Ptr = NULL_PTR;
    boolean error = FALSE;

#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* Check if the Driver is initialized before using this function */
    if (PORT_NOT_INITIALIZED == Port_Status)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID,
                        PORT_SET_PIN_DIRECTION_SID, PORT_E_UNINIT);
        error = TRUE;
    }
    else
    {
        /* No Action Required */
    }
    /* Check if the pin ID is valid */
    if (PORT_CONFIGURED_PINS <= Pin)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID,
                        PORT_SET_PIN_DIRECTION_SID, PORT_E_PARAM_PIN);
        error = TRUE;
    }
    else
    {
        /* No Action Required */
    }

    /* Checks if the pin direction is changeable */
    if (PORT_PIN_DIRECTION_NOT_CHANGEABLE == Port_configPtr[Pin].pin_direction_changeable)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID,
                        PORT_SET_PIN_DIRECTION_SID, PORT_E_DIRECTION_UNCHANGEABLE);

        error = TRUE;
    }
    else
    {
        /* No Action Required */
    }
#endif

    /* In-case there are no errors */
    if(FALSE == error)
    {
        switch(Port_configPtr[Pin].port_num)
        {
        case PORT_PORTA_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
            break;
        case PORT_PORTB_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
            break;
        case PORT_PORTC_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
            break;
        case PORT_PORTD_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
            break;
        case PORT_PORTE_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
            break;
        case PORT_PORTF_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
            break;
        }

        /* Set Pin Direction */
        if(PORT_PIN_OUT == Direction)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DIR_REG_OFFSET) , Port_configPtr[Pin].pin_num);               /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
        }
        else if(PORT_PIN_IN == Direction)
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DIR_REG_OFFSET) , Port_configPtr[Pin].pin_num);             /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        /* Do Nothing */
    }
}
#endif

/************************************************************************************
 * Service Name: Port_RefreshPortDirection
 * Service ID[hex]: 0x02
 * Sync/Async: Synchronous
 * Reentrancy: Non Reentrant
 * Parameters (in): None
 * Parameters (inout): None
 * Parameters (out): None
 * Return value: None
 * Description: Function to refresh port direction
 ************************************************************************************/
void Port_RefreshPortDirection(void)
{
    volatile uint32 *Port_Base_Address_Ptr = NULL_PTR;
    volatile Port_PinType pinIndex = PORT_PIN0_ID;
    boolean error = FALSE;

#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* Check if the Driver is initialized before using this function */
    if (PORT_NOT_INITIALIZED == Port_Status)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID,
                        PORT_REFRESH_PORT_DIRECTION_SID, PORT_E_UNINIT);
        error = TRUE;
    }
    else
    {
        /* No Action Required */
    }
#endif

    /* In-case there are no errors */
    if(FALSE == error)
    {
        switch(Port_configPtr[pinIndex].port_num)
        {
        case PORT_PORTA_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
            break;
        case PORT_PORTB_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
            break;
        case PORT_PORTC_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
            break;
        case PORT_PORTD_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
            break;
        case PORT_PORTE_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
            break;
        case PORT_PORTF_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
            break;
        }

        for(pinIndex = PORT_PIN0_ID; pinIndex < PORT_CONFIGURED_PINS; pinIndex++)
        {
            if(Port_configPtr[pinIndex].pin_direction_changeable == PORT_PIN_DIRECTION_CHANGEABLE)
            {
                /* Set Pin Direction */
                if(Port_configPtr[pinIndex].pin_direction == PORT_PIN_OUT)
                {
                    SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DIR_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);               /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
                }
                else if(Port_configPtr[pinIndex].pin_direction == PORT_PIN_IN)
                {
                    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DIR_REG_OFFSET) , Port_configPtr[pinIndex].pin_num);             /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
                }
                else
                {
                    /* Do Nothing */
                }
            }
            else
            {
                /*Do Nothing*/
            }
        }
    }
    else
    {
        /*Do Nothing*/
    }
}

/************************************************************************************
 * Service Name: Port_GetVersionInfo
 * Service ID[hex]: 0x03
 * Sync/Async: Synchronous
 * Reentrancy: Non Reentrant
 * Parameters (in): None
 * Parameters (inout): None
 * Parameters (out): Version Info - Ptr to where to store the version info of this module.
 * Return value: None
 * Description: Function to return the version info of this module
 ************************************************************************************/
#if (PORT_VERSION_INFO_API == STD_ON)
void Port_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* Check if input pointer is not Null pointer */
    if(NULL_PTR == versioninfo)
    {
        /* Report to DET  */
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID,
                        PORT_GET_VERSION_INFO_SID, PORT_E_PARAM_POINTER);
    }
    else
#endif /* (PORT_DEV_ERROR_DETECT == STD_ON) */
    {
        /* Copy the vendor Id */
        versioninfo->vendorID = (uint16)PORT_VENDOR_ID;
        /* Copy the module Id */
        versioninfo->moduleID = (uint16)PORT_MODULE_ID;
        /* Copy Software Major Version */
        versioninfo->sw_major_version = (uint8)PORT_SW_MAJOR_VERSION;
        /* Copy Software Minor Version */
        versioninfo->sw_minor_version = (uint8)PORT_SW_MINOR_VERSION;
        /* Copy Software Patch Version */
        versioninfo->sw_patch_version = (uint8)PORT_SW_PATCH_VERSION;
    }
}
#endif

/************************************************************************************
 * Service Name: Port_SetPinMode
 * Service ID[hex]: 0x04
 * Sync/Async: Synchronous
 * Reentrancy: Reentrant
 * Parameters (in): Pin - Port Pin ID number
 *                  Mode - New Port Pin mode to be set on port pin
 * Parameters (inout): None
 * Parameters (out): None
 * Return value: None
 * Description: Function to set the port pin mode
 ************************************************************************************/
#if (PORT_SET_PIN_MODE_API == STD_ON)
void Port_SetPinMode(Port_PinType Pin,Port_PinModeType Mode)
{
    volatile uint32 * Port_Base_Address_Ptr = NULL_PTR;
    boolean error = FALSE;

#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* Check if the Driver is initialized before using this function */
    if (PORT_NOT_INITIALIZED == Port_Status)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID,
                        PORT_SET_PIN_MODE_SID, PORT_E_UNINIT);
        error = TRUE;
    }
    else
    {
        /* No Action Required */
    }
    /* Check if the pin ID is valid */
    if (PORT_CONFIGURED_PINS <= Pin)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID,
                        PORT_SET_PIN_MODE_SID, PORT_E_PARAM_PIN);
        error = TRUE;
    }
    else
    {
        /* No Action Required */
    }

    /* Checks if the pin mode is changeable */
    if (PORT_PIN_MODE_NOT_CHANGEABLE == Port_configPtr[Pin].pin_mode_changeable)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID,
                        PORT_SET_PIN_MODE_SID, PORT_E_MODE_UNCHANGEABLE);

        error = TRUE;
    }
    else
    {
        /* No Action Required */
    }
#endif

    /* In-case there are no errors */
    if(FALSE == error)
    {
        switch(Port_configPtr[Pin].port_num)
        {
        case PORT_PORTA_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
            break;
        case PORT_PORTB_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
            break;
        case PORT_PORTC_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
            break;
        case PORT_PORTD_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
            break;
        case PORT_PORTE_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
            break;
        case PORT_PORTF_ID:
            Port_Base_Address_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
            break;
        }

        /* Set Pin Mode */
        if(Mode == PORT_PIN_MODE_DIO) /* DIO Mode */
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_configPtr[Pin].pin_num);      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_configPtr[Pin].pin_num);             /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
            *(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_configPtr[Pin].pin_num * 4));     /* Clear the PMCx bits for this pin */
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_configPtr[Pin].pin_num);         /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
        }
        else if(Mode == PORT_PIN_MODE_ADC) /* ADC Mode */
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_configPtr[Pin].pin_num);        /* Set the corresponding bit in the GPIOAMSEL register to enable analog functionality on this pin */
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_configPtr[Pin].pin_num);             /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
            *(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_configPtr[Pin].pin_num * 4));     /* Clear the PMCx bits for this pin */
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_configPtr[Pin].pin_num);       /* Clear the corresponding bit in the GPIODEN register to disable digital functionality on this pin */
        }
        else /* Any other Mode */
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , Port_configPtr[Pin].pin_num);      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_ALT_FUNC_REG_OFFSET) , Port_configPtr[Pin].pin_num);               /* Enable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
            *(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (Port_configPtr[Pin].pin_num * 4));     /* Clear the PMCx bits for this pin first */
            *(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_CTL_REG_OFFSET) |= (0x0000000F << (Port_configPtr[Pin].pin_num * 4));      /* Then Set the PMCx bits for this pin */
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)Port_Base_Address_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , Port_configPtr[Pin].pin_num);         /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
        }

    }
    else
    {
        /* Do Nothing */
    }
}
#endif
