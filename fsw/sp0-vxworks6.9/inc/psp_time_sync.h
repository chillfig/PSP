/*******************************************************************************
**
**  \file psp_time_sync.h
**
**  \copyright
**  Copyright (c) 2004-2011, United States Government as represented by
**  Administrator for The National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  This is governed by the NASA Open Source Agreement and may be used,
**  distributed and modified only pursuant to the terms of that agreement.
**
**  \brief Header file for PSP Time Service support
**
*******************************************************************************/
#ifndef _PSP_TIME_SYNC_H_
#define _PSP_TIME_SYNC_H_

/******************************************************************************
**  Function:  CFE_PSP_TIME_Init()
**
**  Purpose:
**    Initialize the CFE PSP Time Task synchronizing with the NTP server
**
**  Arguments:
**    timer_frequency_sec is the update frequency in seconds
**
**  Return:
**    int32 - CFE_PSP_SUCCESS or CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_TIME_Init(uint16 timer_frequency_sec);

/******************************************************************************
**  Function:  CFE_PSP_Sync_From_OS_Enable(bool)
**
**  Purpose:
**    Enable or Disable CFE_PSP_Get_OS_Time
**
**  Arguments:
**    bool - When True, CFE_PSP_Get_OS_Time will be called to synchronize time
**
**  Return:
**    int32 - input argument
******************************************************************************/
int32 CFE_PSP_Sync_From_OS_Enable(bool enable);

/******************************************************************************
**  Function:  CFE_PSP_NTP_Daemon_Get_Status(void)
**
**  Purpose:
**    Get status of NTP daemon
**
**  Arguments:
**    none
**
**  Return:
**    int32 - True if NTP Daemon is running, False if it is not running
******************************************************************************/
int32 CFE_PSP_NTP_Daemon_Get_Status(void);

/******************************************************************************
**  Function:  CFE_PSP_Sync_From_OS_Freq(uint16_t)
**
**  Purpose:
**    Changes the synchronication frequency
**
**  Arguments:
**    uint16 - seconds between updates. If zero, returns the current value
**
**  Return:
**    int - CFE_PSP_SUCCESS or the current cfe_OS_Time_Sync_Sec value
******************************************************************************/
int32 CFE_PSP_Sync_From_OS_Freq(uint16 new_frequency_sec);

/******************************************************************************
**  Function:  CFE_PSP_Set_OS_Time()
**
**  Purpose:
**    Set the OS CLOCK_REALTIME to a specified timestamp
**    IMPORTANT: If NTP daemon is enabled and it is capable of contacting an
**               NTP server, setting time does not work.
**
**  Arguments:
**    uint32 ts_sec - Seconds since Epoch 1/1/1970
**    uint32 ts_nsec - NanoSecond timestamp
**
**  Return:
**    int32 - CFE_PSP_SUCCESS or CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_Set_OS_Time(const uint32 ts_sec, const uint32 ts_nsec);

/******************************************************************************
**  Function:  CFE_PSP_Get_OS_Time()
**
**  Purpose:
**    Syncronize CFE Time Service with OS local time. OS time is 
**    automatically syncronized to NTP server
**    Declaration is dictated by OSAL, see osapi-os-timer.h OS_TimerCreate info
**
**    IMPORTANT: Function declaration is dictated by OS_TimerCreate function.
**
**  Arguments:
**    timer_id
**
**  Return:
**    void
******************************************************************************/
void CFE_PSP_Get_OS_Time(uint32 timer_id);

/******************************************************************************
**  Function:  CFE_PSP_StartNTPDaemon()
**
**  Purpose:
**    Start the NTP daemon on OS that sync with NTP server
**
**  Arguments:
**    none
**
**  Return:
**    int32 - Task ID or CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_StartNTPDaemon(void);

/******************************************************************************
**  Function:  CFE_PSP_StopNTPDaemon()
**
**  Purpose:
**    Stop the NTP daemon on OS that sync with NTP server
**
**  Arguments:
**    none
**
**  Return:
**    int32 - CFE_PSP_SUCCESS or CFE_PSP_ERROR
**            Note: NTP task already stopped, return CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_StopNTPDaemon(void);

/******************************************************************************
**  Function:  CFE_PSP_NTP_Daemon_Enable(bool)
**
**  Purpose:
**    Enable or Disable OS NTP dameon process
**
**  Arguments:
**    bool - When True starts NTP daemon, else stop process
**
**  Return:
**    int32 - Task ID or CFE_PSP_SUCCESS or CFE_PSP_ERROR
**            Task ID is returned when successfully starting NTP process
**            CFE_PSP_SUCCESS is returned when successfully stopping NTP process
******************************************************************************/
int32 CFE_PSP_NTP_Daemon_Enable(bool enable);

#endif /* _PSP_TIME_SYNC_H_ */
