#ifndef _psp_time_sync_
#define _psp_time_sync_

/*
** Function Prototypes
*/

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

#endif