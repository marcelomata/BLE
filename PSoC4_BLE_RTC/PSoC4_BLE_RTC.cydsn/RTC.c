/*******************************************************************************
* File Name: RTC.c
*
* Version: 1.0
*
* Description:
* Contians source code for RTC time sync from the CTS server and keeps the time
*
*******************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <BLE Connection.h>
#include <Button.h>
#include <Configuration.h>
#include <project.h>
#include <RTC.h>
#include <stdio.h>

/* Holds the current RTC time */
CYBLE_CTS_CURRENT_TIME_T currentTime;

/* Status & Control Variables */
volatile uint8 RTC_statusDateTime = 0u;

/* RTC tick flag */
uint8 timerTick = 0;

/* Month Day Array - number of days in the months */
const uint8 CYCODE RTC_daysInMonths[RTC_MONTHS_IN_YEAR] = {
    RTC_DAYS_IN_JANUARY,
    RTC_DAYS_IN_FEBRUARY,
    RTC_DAYS_IN_MARCH,
    RTC_DAYS_IN_APRIL,
    RTC_DAYS_IN_MAY,
    RTC_DAYS_IN_JUNE,
    RTC_DAYS_IN_JULY,
    RTC_DAYS_IN_AUGUST,
    RTC_DAYS_IN_SEPTEMBER,
    RTC_DAYS_IN_OCTOBER,
    RTC_DAYS_IN_NOVEMBER,
    RTC_DAYS_IN_DECEMBER};

/* Day of the week lookup table */
const char dayOfTheWeek[][10] = {
"Monday", 
"Tuesday",    
"Wednesday", 
"Thursday", 
"Friday", 
"Saturday",
"Sunday",    
};

/*******************************************************************************
* Function Name: StartTimeServiceDiscovery
********************************************************************************
*
* Summary:
*  Starts service discovery on the connected peer BLE device
*
* Parameters:
*  None
*
* Return:
*  CYBLE_API_RESULT_T - Result of the start discovery API
*
*******************************************************************************/
CYBLE_API_RESULT_T StartTimeServiceDiscovery(void)
{
    return CyBle_GattcStartDiscovery(cyBle_connHandle);
}

/*******************************************************************************
* Function Name: SyncTimeFromBleTimeServer
********************************************************************************
*
* Summary:
*  Read the current time received from the peer time server
*
* Parameters:
*  None
*
* Return:
*  CYBLE_API_RESULT_T - Result of the start discovery API
*
*******************************************************************************/
CYBLE_API_RESULT_T SyncTimeFromBleTimeServer(void)
{
    return CyBle_CtscGetCharacteristicValue(cyBle_connHandle, CYBLE_CTS_CURRENT_TIME);
}

/*******************************************************************************
* Function Name: CtsCallBack
********************************************************************************
*
* Summary:
*  BLE stack callback API for Current Time Service.
*
* Parameters:
*  event - event that triggered this callback
*  eventParam - parameters for the event.
*
* Return:
*  None
*
*******************************************************************************/
void CtsCallBack(uint32 event, void* eventParam)
{
    CYBLE_CTS_CHAR_VALUE_T *timeAttribute;
    CYBLE_GATT_VALUE_T *timeValue;
    
    timeAttribute = (CYBLE_CTS_CHAR_VALUE_T *) eventParam;
    
    /* This is a CTS specific event triggered by the BLE component */
    switch(event)
    {
        /* Only operation that is required for RTC is reading the time. We are not enabling notifications
         * right now */
        case CYBLE_EVT_CTSC_READ_CHAR_RESPONSE:
            if(timeAttribute->charIndex == CYBLE_CTS_CURRENT_TIME)
            {
                uint8 intStatus;
                /* copy the current time received from the time server to local data structure for RTC
                 * operation */
                timeValue = timeAttribute->value;               
                
                intStatus = CyEnterCriticalSection();
                memcpy(&currentTime, timeValue->val, (timeValue->len)-1);
                CyExitCriticalSection(intStatus);
                
                RTC_Start(); /* Update the RTC component with time synced from the BLE time server */
                
#if DISCONNECT_BLE_AFTER_TIME_SYNC               
                BLE_RequestDisconnection();
#endif    
            }
            else if(timeAttribute->charIndex == CYBLE_CTS_LOCAL_TIME_INFO)
            {
                
            }             
        break;
    }
}

/*******************************************************************************
* Function Name: WDT_Handler
********************************************************************************
*
* Summary:
*  Watchdog 1 second interrupt handler for RTC functionality
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
CY_ISR(WDT_Handler)
{
    if(CySysWdtGetInterruptSource() & RTC_INTERRUPT_SOURCE)
    {
        /* Increment seconds counter */
        currentTime.seconds++;

        /* Check if minute elapsed */
        if(currentTime.seconds > RTC_MINUTE_ELAPSED)
        {
            /* Inc Min */
            currentTime.minutes++;

            /* Clear Sec */
            currentTime.seconds = 0u;

            if(currentTime.minutes > RTC_HOUR_ELAPSED)
            {
                /* Inc HOUR */
                currentTime.hours++;

                /* Clear Min */
                currentTime.minutes = 0u;

                /* Day roll over */
                if(currentTime.hours > RTC_DAY_ELAPSED)
                {
                    /* Inc DayOfMonth */
                    currentTime.day++;

                    /* Clear Hour */
                    currentTime.hours = 0u;

                    /* Inc DayOfWeek */
                    currentTime.dayOfWeek++;

                    /* Check DayOfWeek */
                    if(currentTime.dayOfWeek > RTC_WEEK_ELAPSED)
                    {
                        /* start new week */
                        currentTime.dayOfWeek = 1u;
                    }

                    /* Day of month roll over.
                    * Check if day of month greater than 29 in February of leap year or
                    * if day of month greater than 28 NOT in February of NON leap year or
                    * if day of month greater than it should be in every month in non leap year
                    */
                    if(((0u != (RTC_statusDateTime & RTC_STATUS_LY)) &&
                        (currentTime.month == RTC_FEBRUARY)  &&
                        (currentTime.day >
                                 (RTC_daysInMonths[currentTime.month - 1u] + 1u))) ||
                       ((0u != (RTC_statusDateTime & RTC_STATUS_LY))  &&
                        (currentTime.month != RTC_FEBRUARY) &&
                        (currentTime.day >
                                        RTC_daysInMonths[currentTime.month - 1u])) ||
                       ((0u == (RTC_statusDateTime & RTC_STATUS_LY))  &&
                        (currentTime.day >
                                        RTC_daysInMonths[currentTime.month - 1u])))
                    {
                        /* Inc Month */
                        currentTime.month++;

                        /* Set first day of month */
                        currentTime.day = 1u;

                        /* Year roll over */
                        if(currentTime.month > RTC_YEAR_ELAPSED)
                        {
                            /* Inc Year */
                            if(currentTime.yearLow == RTC_YEAR_LOW_MAX)
                            {
                                currentTime.yearHigh++;
                            }
                            currentTime.yearLow++;

                            /* Set first month of year */
                            currentTime.month = 1u;

                            /* Is this year leap */
                            if(1u == RTC_LEAP_YEAR(((uint16)currentTime.yearHigh)<<8 | currentTime.yearLow))
                            {
                                /* Set leap year flag */
                                RTC_statusDateTime |= RTC_STATUS_LY;
                            }
                            else    /* not leap year */
                            {
                                /* Clear leap year */
                                RTC_statusDateTime &= (uint8)(~RTC_STATUS_LY);
                            }
                        } /* Month > 12 */
                    }   /* Day roll over */
                } /* End of day roll over */

                /* Status set PM/AM flag */
                if(currentTime.hours < RTC_HALF_OF_DAY_ELAPSED)
                {
                    /* AM Hour 00:00-11:59, flag zero */
                    RTC_statusDateTime &= (uint8)(~RTC_STATUS_AM_PM);
                }
                else
                {
                    /* PM Hour 12:00-23:59, flag set */
                    RTC_statusDateTime |= RTC_STATUS_AM_PM;
                }
            }
        }
        
        CySysWdtClearInterrupt(RTC_INTERRUPT_SOURCE);

        timerTick = 1;
    }
}

/*******************************************************************************
* Function Name: RTC_Start
********************************************************************************
*
* Summary:
*  Initialize WDT timer 0 for 1 second interrupt mode of operation required for
*  RTC.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void RTC_Start(void)
{
    /* configures WDT to trigger an interrupt every second for RTC operation */
    
    CySysWdtUnlock(); /* Unlock the WDT registers for modification */
    
    CySysWdtWriteMode(RTC_SOURCE_COUNTER, CY_SYS_WDT_MODE_INT);
    
    CySysWdtWriteClearOnMatch(RTC_SOURCE_COUNTER, RTC_COUNTER_ENABLE);
    
    CySysWdtWriteMatch(RTC_SOURCE_COUNTER, RTC_COUNT_PERIOD);
    
    CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);
    
    CySysWdtLock();
}

/*******************************************************************************
* Function Name: RTC_TickExpired
********************************************************************************
*
* Summary:
*  Returns if RTC interrupt has fired or not from the time this API was called
*  last until now.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
uint8 RTC_TickExpired(void)
{
    if(timerTick)
    {
        RTC_Interrupt_Disable();
        
        timerTick = 0;
        
        RTC_Interrupt_Enable();
       
        return 1;
    }
    
    return 0;
}

/*******************************************************************************
* Function Name: RTC_UI_Update
********************************************************************************
*
* Summary:
*  Update the current value of RTC .
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void RTC_UI_Update(void)
{
    if(RTC_TickExpired() == 1) /* one second interrupt expired */
    {
#if(CONSOLE_LOG_ENABLED)
#if DISPLAY_ON_BUTTON_PRESS    
        if(Button_IsPressed())
#endif /* End of DISCONNECT_BLE_AFTER_TIME_SYNC */       
        {
        printf("%s ", dayOfTheWeek[currentTime.dayOfWeek-1]);
        printf("%d\\%d\\%d\\ %d:%d:%d\r\n",
                ((uint16)(currentTime.yearHigh))<< 8 | currentTime.yearLow,
                currentTime.month, currentTime.day, currentTime.hours, currentTime.minutes,
                currentTime.seconds);
        
#endif /* End of CONSOLE_LOG_ENABLED */
        }
	}
}

/* [] END OF FILE */
