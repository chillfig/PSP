#include <vxWorks.h>
#include <string.h>
#include <ioLib.h>
#include <stdio.h>
#include <bootLib.h>
#include "utstubs.h"
#include "ut_psp_utils.h"

#define MAX_RDWR_SIZE       0x01000000  /* 16MB */

typedef enum {SP0_ORIGINAL = 1, SP0_UPGRADE = 2} SP0_BOARD_GENERATION_TYPE;

char UserReservedMemory[4000] = {'\0'};
char *pURM = UserReservedMemory;
size_t uURM = 4000;
char bString[BOOT_FILE_LEN] = "/ffx0/startup";


void userReservedGet( char **  pUserReservedAddr, size_t * pUserReservedSize )
{
    int iSize;

    iSize = UT_DEFAULT_IMPL(userReservedGet);

    pUserReservedAddr = &pURM;
    uURM = iSize;
    pUserReservedSize = &uURM;
}

int userMemAlloc(uint32 *addr, uint32 size, bool talk)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(userMemAlloc);
    if (iStatus >= 0)
    {
        *addr = (uint32)pURM;
    }

    return iStatus;
}

int aimonGetBITExecuted(uint64 *pulRun, int iValue)
{
    UT_Stub_RegisterContext(UT_KEY(aimonGetBITExecuted), pulRun);
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(aimonGetBITExecuted);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(aimonGetBITExecuted), (uint64 *)pulRun, sizeof(*pulRun));
    }

    return iStatus;
}

int aimonGetBITResults(uint64 *pulResult, int iValue)
{
    UT_Stub_RegisterContext(UT_KEY(aimonGetBITResults), pulResult);
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(aimonGetBITResults);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(aimonGetBITResults), (uint64 *)pulResult, sizeof(*pulResult));
    }

    return iStatus;
}

int32 ReadResetSourceReg(uint32 *puiResetSrc, uint32 uiTalkAtive)
{
    UT_Stub_RegisterContext(UT_KEY(ReadResetSourceReg), puiResetSrc);
    UT_Stub_RegisterContextGenericArg(UT_KEY(ReadResetSourceReg), uiTalkAtive);

    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(ReadResetSourceReg);

    if (iStatus >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(ReadResetSourceReg), (uint32 *)puiResetSrc, sizeof(*puiResetSrc)) < sizeof(*puiResetSrc))
        {
            *puiResetSrc = 0x08;
        }
    }

    return iStatus;
}

/**
 ** \brief UT Read the Safe Mode User Data
 ** \par Description:
 ** iStatus >= 0 --> pSafeModeUserData
 ** iStatus = -1 --> Error
 ** \param[out] pSafeModeUserData 
 ** \param[in] uiTalkAtive 
 ** \return int32 
 */
int32 ReadSafeModeUserData(void *pSafeModeUserData, uint32 uiTalkAtive)
{
    int32 iStatus;

    UT_Stub_RegisterContext(UT_KEY(ReadSafeModeUserData), pSafeModeUserData);

    iStatus = UT_DEFAULT_IMPL(ReadSafeModeUserData);

    if (iStatus >= 0)
    {
        /* 24 is the number of bytes of the USER_SAFE_MODE_DATA_STRUCT structure */
        UT_Stub_CopyToLocal(UT_KEY(ReadSafeModeUserData), (void *)pSafeModeUserData, 24);
        return OK;
    }

    return ERROR;
}



char *sysModel(void)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(sysModel);
    if (iStatus == 1)
    {
        return "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE";
    }
    return "Typical Message";
}

uint32 getCoreClockSpeed(void)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(getCoreClockSpeed);
    if (iStatus == 0)
    {
        return 399;
    }
    return 333;
}

int returnSelectedBootFlash(void)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(returnSelectedBootFlash);

    return iStatus;
}

/*  Watchdog Stubs */
int sysEnableFpgaWdt ( BOOL globalReset )
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(sysEnableFpgaWdt);

    return iStatus;
}
int sysDisableFpgaWdt ( BOOL globalReset )
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(sysDisableFpgaWdt);

    return iStatus;
}
int sysSetFpgaWdt(float secs /* Number of seconds for timeout */)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(sysSetFpgaWdt);

    return iStatus;
}

int sysClkRateGet(void)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(sysClkRateGet);

    return iStatus;
}

/**
 * SP0_ORIGINAL = 1
 * SP0_UPGRADE =2
 */
SP0_BOARD_GENERATION_TYPE sysGetBoardGeneration(bool talkative)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(sysGetBoardGeneration);

    if (Status == 1)
    {
        return SP0_ORIGINAL;
    }
    else if (Status == 2)
    {
        return SP0_UPGRADE;
    }
    else
    {
        return 0;
    }
}

char *PCS_bootStringToStruct (char * bootString, BOOT_PARAMS * pBootParams)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(bootStringToStruct);

    if (Status == OK)
    {
        memcpy(pBootParams->startupScript, bString, BOOT_FILE_LEN);
    }

    return bString;
}

int PCS_OS_BSPMain(void)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_OS_BSPMain);

    return iStatus;
}

/**
 ** \brief Unit Test userNvRamGet
 **
 ** \par Description:
 ** Deferred Return Code = 0 --> Success
 ** Deferred Return Code < 0 --> Error
 ** 
 ** \param[inout] dat_ptr 
 ** \param[inout] nbytes 
 ** \param[inout] offset 
 ** \return STATUS 
 */
STATUS userNvRamGet (char *dat_ptr, int nbytes, int offset)
{
    int32   iStatus;
    int32   ret_code = OK;

    iStatus = UT_DEFAULT_IMPL(userNvRamGet);

    /* Nominal */
    if (iStatus == 0)
    {
        /* Return requested data */
        memcpy(dat_ptr, nvram + offset, nbytes);
    }
    else if (iStatus < 0)
    {
        ret_code = ERROR;
    }

    return ret_code;
}

/**
 ** \brief Unit Test userNvRamSet
 **
 ** \par Description:
 ** Deferred Return Code = 0 --> Success
 ** Deferred Return Code < 0 --> Error
 ** 
 ** \param[inout] dat_ptr 
 ** \param[inout] nbytes 
 ** \param[inout] offset 
 ** \return STATUS 
 */
STATUS userNvRamSet (char *dat_ptr, int nbytes, int offset)
{
    int32   iStatus;
    int32   ret_code = OK;

    iStatus = UT_DEFAULT_IMPL(userNvRamSet);

    /* Nominal */
    if (iStatus == 0)
    {
        memcpy(nvram, dat_ptr + offset, nbytes);
    }
    else if (iStatus < 0)
    {
        ret_code = ERROR;
    }

    return ret_code;
}

int tempSensorRead (int8 sensor, uint8 dataType, float *temperature, bool talkative )
{
    int32 iStatus;
    iStatus = UT_DEFAULT_IMPL(tempSensorRead);
    *temperature = 25.5f;

    return iStatus;
}

int volSensorRead(int8 sensor, uint8 dataType, float *voltage, bool talkative )
{
    int32 iStatus;
    iStatus = UT_DEFAULT_IMPL(volSensorRead);
    *voltage = 5.05f;

    return iStatus;
}

/* GetUsecTime( ) – gets the time in micro-seconds since startup. */
double GetUsecTime(void)
{
    /* Time in microseconds */
    return (double)100.00;
}

STATUS scrubMemory(uint32_t startAddr, uint32_t endAddr, uint32_t *pNumPagesScrubbed)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(scrubMemory);

    return iStatus;
}

STATUS sysNvRamGet(
char *dat_ptr, /* pointer to data location */
int nbytes, /* size of the buffer */
int offset /* byte offset in EEPROM */
)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(sysNvRamGet);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(sysNvRamGet), (char *) dat_ptr, nbytes);
    }

    return iStatus;
}

STATUS sysNvRamSet(
char *dat_ptr, /* pointer to data location */
int nbytes, /* size of the buffer */
int offset /* byte offset in EEPROM */
)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(sysNvRamSet);

    return iStatus;
}