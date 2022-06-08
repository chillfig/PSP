/**
 ** file
 **
 ** brief API header for collecting SP0(s) hardware and software information
 **
 ** copyright
 ** Copyright (c) 2019-2021 United States Government as represented by
 ** the Administrator of the National Aeronautics and Space Administration.
 ** All Rights Reserved.
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **
 ** par Description:
 ** Functions here allow CFS to provide a method to probe 
 ** SP0 hardware for information from POST, Temperatures, Voltages, Active 
 ** Boot EEPROM, etc. In addition, this module has a function to save a 
 ** dump_core text file before aborting CFS execution. 
 **
 ** par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
*/

#ifndef PSP_SP0_INFO_H
#define PSP_SP0_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

/* For supporting REALTIME clock */
// #include <timers.h>
#include "osapi.h"
struct timespec
{
    time_t  tv_sec;
    long    tv_nsec;
};

/**
 ** brief SP0_TEXT_BUFFER_MAX_SIZE
 ** par Description:
 ** This is the maximum size of the SP0 char array table.
 */
#define SP0_TEXT_BUFFER_MAX_SIZE            1000

/**
 ** brief SP0_SAFEMODEUSERDATA_BUFFER_SIZE
 ** par Description:
 ** This is the maximum size of the safeModeUserData char array.
 */
#define SP0_SAFEMODEUSERDATA_BUFFER_SIZE    256

/**
 ** brief Default SP0 Info pre-print string 
 ** par Description:
 ** This string is printed before every print related to SP0 Info API.
*/
#define SP0_PRINT_SCOPE                     "PSP SP0: "

/**
** brief SP0 info structure
** par Description:
** The table includes values that changes only once during boot and others
** that changes at a regular interval.
** 
** Variables that changes at regular intervals are:
** - systemStartupUsecTime
** - temperatures
** - voltages
*/
typedef struct
{
    /** brief UTC date time when the data was collected */
    struct timespec lastUpdatedUTC;
    /** brief Pointer to the string identifing the System Model */
    char * systemModel;
    /** brief Pointer to the string identifing the system BSP Revision */
    char * systemBspRev;
    /** brief Top of the System Physical Memory */
    uint32 systemPhysMemTop;
    /** brief Number of Processors */
    int systemProcNum;
    /** brief Slod ID in the chassis */
    int systemSlotId;
    /** brief Identifies if the SP0 is the cPCI main system controller */
    bool systemCpciSysCtrl;
    /** brief System Core Clock Speed in MHz */
    uint32 systemCoreClockSpeed;
    /** brief Reason for last SP0 computer reset */
    uint8 systemLastResetReason;
    /** brief Identifies the EEPROM to successfully booted the kernel */
    uint8 active_boot;
    /** brief System Clock Rate */
    int systemClkRateGet;
    /** brief System Aux Clock Rate */
    int systemAuxClkRateGet;
    /** brief Identifies the POST Test Bit Executed */
    uint64 bitExecuted;
    /** brief Identifies the POST Test Results */
    uint64 bitResult;
    /** brief Safe Mode User Data */
    char safeModeUserData[SP0_SAFEMODEUSERDATA_BUFFER_SIZE];
    
    /** brief Number of usec since startup */
    float systemStartupUsecTime;
    /** brief Array of 4 temperatures on the SP0 computer */
    float temperatures[4];
    /** brief Array of 6 voltages powering the SP0 */
    float voltages[6];
} CFE_PSP_SP0InfoTable_t;


/**
 ** func Collect SP0 Hardware and Firmware data
 **
 ** par Description:
 ** This function collects the SP0 hardware and firmware data and saves it
 ** in the sp0_info_table object, as well as a string in the sp0_data_dump object.
 ** 
 ** par Assumptions, External Events, and Notes:
 ** None
 **
 ** param None
 **
 ** return #CFE_PSP_SUCCESS
 ** return #CFE_PSP_ERROR
 */
int32  CFE_PSP_SP0GetInfo(void);

/**
 ** func Get the structure containing the SP0 Hardware and Firmware data
 **
 ** par Description:
 ** This function returns and print the structure containing the SP0 Hardware and Firmware
 ** data.
 ** 
 ** par Assumptions, External Events, and Notes:
 ** None
 **
 ** param print_to_console Print string buffer to console if larger than 0
 ** param sp0_info Pointer to an CFE_PSP_SP0InfoTable_t structure
 **
 ** return #CFE_PSP_SUCCESS
 ** return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0GetInfoTable(CFE_PSP_SP0InfoTable_t *sp0_info, uint8_t print_to_console);

/**
 ** func Function dumps the collected data to file
 **
 ** par Description:
 ** Saves data dump to location defined by #SP0_DATA_DUMP_FILEPATH
 ** 
 ** par Assumptions, External Events, and Notes:
 ** None
 **
 ** param None
 **
 ** return #CFE_PSP_SUCCESS
 ** return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0DumpData(void);

/**
 ** func Get disk free disk space
 ** 
 ** par Description:
 ** Function uses the statfs64 to gather statistics about the file system.
 ** It works with both RAM and FLASH file systems such as "/ram0" and "/ffx0"
 **
 ** par Assumptions, External Events, and Notes:
 ** None
 **
 ** param[in] ram_disk_root_path 
 **
 ** return int64_t - Size of free space in disk in bytes
 ** return CFE_PSP_ERROR - If statfs returned error
 */
int64_t CFE_PSP_SP0GetDiskFreeSize(char *ram_disk_root_path);

/**
 ** brief Lock ROM1
 **
 ** par Description:
 ** Function that will lock ROM1
 **
 ** par Assumptions, External Events, and Notes:
 ** This function will still attempt to lock ROM1
 ** even if it is currently locked
 **
 ** param None
 **
 ** return #CFE_PSP_SUCCESS
 ** return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0ROM1Lock(void);

/**
 ** brief Unlock ROM1
 **
 ** par Description:
 ** Function that will unlock ROM1
 **
 ** par Assumptions, External Events, and Notes:
 ** This function will still attempt to unlock ROM1
 ** even if it is currently unlocked
 **
 ** param None
 **
 ** return #CFE_PSP_SUCCESS
 ** return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0ROM1Unlock(void);

/**
 ** brief Lock ROM2
 **
 ** par Description:
 ** Function that will lock ROM2
 **
 ** par Assumptions, External Events, and Notes:
 ** This function will still attempt to lock ROM2
 ** even if it is currently locked
 **
 ** param None
 **
 ** return #CFE_PSP_SUCCESS
 ** return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0ROM2Lock(void);

/**
 ** brief Unlock ROM2
 **
 ** par Description:
 ** Function that will unlock ROM2
 **
 ** par Assumptions, External Events, and Notes:
 ** This function will still attempt to unlock ROM2
 ** even if it is currently unlocked
 **
 ** param None
 **
 ** return #CFE_PSP_SUCCESS
 ** return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0ROM2Unlock(void);

/**
 ** brief Get ROM1 Lock/Unlock Status
 **
 ** par Description:
 ** Function will return LOCK/UNLOCK status of ROM1
 **
 ** par Assumptions, External Events, and Notes:
 ** None
 **
 ** param None
 **
 ** return false - ROM1 is UNLOCKED
 ** return true - ROM1 is LOCKED
 */
bool CFE_PSP_SP0ROM1Status(void);

/**
 ** brief Get ROM2 Lock/Unlock Status
 **
 ** par Description:
 ** Function will return LOCK/UNLOCK status of ROM2
 **
 ** par Assumptions, External Events, and Notes:
 ** None
 **
 ** param None
 **
 ** return false - ROM2 is UNLOCKED
 ** return true - ROM2 is LOCKED
 */
bool CFE_PSP_SP0ROM2Status(void);

#ifdef __cplusplus
}
#endif

#endif /* _PSP_SP0_INFO_H_ */

