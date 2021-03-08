#ifndef _psp_time_sync_
#define _psp_time_sync_

static bool setTime_From_VxWorks = true;

/**
 * Change how often to sync CFE Time Service with VxWorks Local Time.
 * VxWorks local time is synchronized to NTP server(s) automatically from within VxWorks.
 */
static uint16 PSP_VXWORKS_TIME_SYNC_SEC = 30;


/******************************************************************************
**  Function:  CFE_PSP_SetTime_Enable(bool)
**
**  Purpose:
**    Enable or Disable CFE_PSP_SetTime_From_VxWorks
**
**  Arguments:
**    bool - When True, CFE_PSP_SetTime_From_VxWorks will be called to synchronize time
**
**  Return:
**    int32 - CFE_PSP_SUCCESS or CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_SetTime_Enable(bool);

/******************************************************************************
**  Function:  CFE_PSP_SetTime_From_VxWorks()
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
void CFE_PSP_SetTime_From_VxWorks(uint32 timer_id);

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

#endif