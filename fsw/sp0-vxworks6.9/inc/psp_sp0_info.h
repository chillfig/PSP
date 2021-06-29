
#define SP0_TEXT_BUFFER_MAX_SIZE    1000

/* This is the ascii buffer to write to stdout or file */
char sp0_data_dump[SP0_TEXT_BUFFER_MAX_SIZE];
int sp0_data_dump_length;

/*
** Typedef for the sp0 info structure
NOTE: Move this to cfe_psp_config.h
*/
struct
{
    /* Values that changes once during boot */
    char * systemModel;
    char * systemBspRev;
    uint32 systemPhysMemTop;
    int systemProcNum;
    int systemSlotId;
    bool systemCpciSysCtrl;
    uint32 systemCoreClockSpeed;
    uint8 systemLastResetReason;
    uint8 active_boot;
    int systemClkRateGet;
    int systemAuxClkRateGet;
    uint64 bitExecuted;
    uint64 bitResult;
    char safeModeUserData[256];

    /* Values that changes over time after initial boot */
    double systemStartupUsecTime;
    float temperatures[4];
    float voltages[6];
} sp0_info_table;


/**
 * @brief Collects SP0 Hardware and Firmware information
 * 
 *        Data is saved in structure CFE_PSP_SP0_Info_t sp0_info_table and
 *        saved as string in sp0_data_dump.
 * 
 * @return int32 CFE_PSP_SUCCESS or CFE_PSP_ERROR
 * 
 * TODO: Fix the return code
 */
int32 getSP0Info(void);

/**
 * @brief Print the data collected to console and return a string containing
 *        the printout
 * 
 */
void printSP0_info_table(void);

