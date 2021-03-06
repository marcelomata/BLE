/******************************************************************************
* Project Name		: Long Characteristic Read Write
* File Name			: Configuration.h
* Version 			: 1.0
* Device Used		: CY8C4247LQI-BL483
* Software Used		: PSoC Creator 3.2
* Compiler    		: ARM GCC 4.8.4
* Related Hardware	: CY8CKIT-042-BLE Bluetooth Low Energy Pioneer Kit 
* Owner				: kris@cypress.com
*
********************************************************************************
* Copyright (2015), Cypress Semiconductor Corporation. All Rights Reserved.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international treaty
* provisions. Cypress hereby grants to licensee a personal, non-exclusive,
* non-transferable license to copy, use, modify, create derivative works of,
* and compile the Cypress Source Code and derivative works for the sole
* purpose of creating custom software in support of licensee product to be
* used only in conjunction with a Cypress integrated circuit as specified in
* the applicable agreement. Any reproduction, modification, translation,
* compilation, or representation of this software except as specified above 
* is prohibited without the express written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. 
*
* Use of this Software may be limited by and subject to the applicable Cypress
* software license agreement. 
*******************************************************************************/

/********************************************************************************
* Description - Contains all the configuration flags that you could use to 
* enable/disable firmware features.
********************************************************************************/

#if !defined(CONFIGURATION_H)
#define CONFIGURATION_H
    
/*****************************************************
*                  Configuration flags
*****************************************************/
#define DEBUG_ENABLE                        1  /* Enables GPIO toggling on different power modes. Poor man's power profiler */
#define CONSOLE_LOG_ENABLE                  1  /* Enable UART console logging */
#define SEND_PREPARE_WRITE_ERROR_RESPONSE   0  /* Enable this to respond to Prepare Write request with an error response */
#define SEND_EXECUTE_WRITE_ERROR_RESPONSE   0  /* Enable this to respond to Execute Write request with an error response */
    
    
/* ADD ADDITIONAL FIRMWARE CONFIGURATION FLAGS HERE */    
    
#endif /* End of #if !defined(CONFIGURATION_H) */

/* [] END OF FILE */
