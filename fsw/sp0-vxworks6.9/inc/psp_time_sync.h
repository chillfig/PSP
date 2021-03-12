#ifndef _psp_time_sync_
#define _psp_time_sync_

/**
 * Boolean variable to control if to synchronize CFE Time Service with VxWorks
 * local time.
 * True, synch will occur
 * False, timer will not be disabled, but sync will not execute
 */
static bool getTime_From_VxWorks_flag = true;

/**
 * Change how often to sync CFE Time Service with VxWorks Local Time.
 * VxWorks local time is synchronized to NTP server(s) automatically from within VxWorks.
 */
static uint16 PSP_VXWORKS_TIME_SYNC_SEC = 30;


/******************************************************************************
**  Function:  CFE_PSP_Sync_From_VxWorks_Ena(bool)
**
**  Purpose:
**    Enable or Disable CFE_PSP_Get_VxWorks_Time
**
**  Arguments:
**    bool - When True, CFE_PSP_Get_VxWorks_Time will be called to synchronize time
**
**  Return:
**    int32 - True if 
******************************************************************************/
int32 CFE_PSP_Sync_From_VxWorks_Ena(bool);

/******************************************************************************
**  Function:  CFE_PSP_Get_VxWorks_Time()
**
**  Purpose:
**    Syncronize CFE Time Service with VxWorks local time. VxWorks time is 
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
void CFE_PSP_Get_VxWorks_Time(uint32 timer_id);

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
**  Function:  CFE_PSP_Update_Sync_Frequency(uint16_t)
**
**  Purpose:
**    Changes the synchronication frequency
**
**  Arguments:
**    uint16 - seconds between updates
**
**  Return:
**    int32 - CFE_PSP_SUCCESS or the current PSP_VXWORKS_TIME_SYNC_SEC value
******************************************************************************/
int32 CFE_PSP_Update_Sync_Frequency(uint16 new_frequency_sec);

/******************************************************************************
**  Function:  CFE_PSP_NTP_Daemon_Get_Status(void)
**
**  Purpose:
**    Changes the synchronication frequency
**
**  Arguments:
**    none
**
**  Return:
**    int32 - True if NTP Daemon is running, False if it is not running
******************************************************************************/
int32 CFE_PSP_NTP_Daemon_Get_Status();

#endif