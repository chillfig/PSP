/*********************************************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
*   Export Administration Regulations (EAR) Notice
*   This document contains information which falls under the purview of the Export Administration Regulations (EAR),
*   15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
*   or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
*   furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
*   or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
*   of the Gateway MOUs and IA requires prior U.S. Government authorization.
*
*   \file cfe_psp_memscrub.h
*
*   \brief API header for Memory Scrubbing
*
*   \brief This is specific to the LEON4 processor on GR740 running VxWorks 6.9 OS.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef PSP_MEM_SCRUB_H
#define PSP_MEM_SCRUB_H

#include "common_types.h"
#include "osapi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_public_api_gr740vx69 PSP Public APIs - GR740-VxWorks6.9 Platform
** \{
*/

/**
 ** \brief Default Memory Scrubbing pre-print string 
 ** \par Description:
 ** This string is printed before every print related to Memory Scrubbing API.
*/
#define MEMSCRUB_PRINT_SCOPE       "PSP MEM SCRUB: "

/**
** \brief MEMSCRUB Run Modes
*/
typedef enum
{
    /**
     ** \brief Automatic Mode
     **
     ** \par Description
     ** No task is created, the Memory Scrub feature is fully supported by hardware
     ** and it will run in a continuous loop scanning from Start to End addresses.
     ** Only available in some platforms.
     */
    MEMSCRUB_AUTOMATIC_MODE = 1,
    /**
     ** \brief Idle Mode
     **
     ** \par Description
     ** A task is created that runs continuously in a while loop with low priority
     */
    MEMSCRUB_IDLE_MODE = 2,
    /**
     ** \brief Timed Mode
     **
     ** \par Description
     ** A task is created that runs every X number of seconds. The start and
     ** end address are scrubbed through in blocks.
     */
    MEMSCRUB_TIMED_MODE = 3,
    /**
     ** \brief Manual Mode
     **
     ** \par Description
     ** End user calls the mem scrub function when appropriate. The start and
     ** end address are scrubbed through in blocks.
     */
    MEMSCRUB_MANUAL_MODE = 4
} MEMSCRUB_RunMode_t;


/**
 ** \brief Memory Scrubbing configuration and information
 **
 */
typedef struct
{
    /**
     ** \brief Defines the Memory Scrub Run Method
     ** \par Description:
     ** Mem Scrub can be run in multiple modes depending on the PSP config.
     ** - Idle Mode
     **   - A task is created that runs continuously in a while loop with
     **     low priority
     ** - Timed Mode
     **   - A task is created that runs every X number of seconds. The start and
     **     end address are scrubbed through in blocks.
     ** - Manual Mode
     **   - End user calls the mem scrub function when appropriate. The start and
     **     end address are scrubbed through in blocks.
     */
    MEMSCRUB_RunMode_t  RunMode;
    /**
     ** \brief Defines the Start Address to scrub
     ** \par Description:
     ** The start address can be any value in the address space starting from 0
     */
    uint32              uiMemScrubStartAddr;
    /**
     ** \brief Defines the end Address to scrub
     ** \par Description:
     ** End Address cannot be larger than the maximum RAM
     */
    uint32              uiMemScrubEndAddr;

} CFE_PSP_MemScrubStatus_t;

/**
 ** \brief Memory Error Statistics struct
 **
 ** \par Description:
 ** Returns a structure containing memory error statistics
 **
 ** \par Assumptions, External Events, and Notes:
 **
 */
typedef struct
{
	/* Zero when inactive */
	uint32	uiCurrentPosition;
	/* Run error count */
	uint32  uiRUNCOUNT;
	/* Block error count */
	uint32  uiBLKCOUNT;
	/* Global correctable error count */
	uint32  uiCECNT;
	/* Global uncorrectable error count */
	uint32  uiUECNT;

} CFE_PSP_MemScrubErrStats_t;

/**
** \func Set the Memory Scrubbing parameters
** 
** \par Description:
** This functions set the memory scrubbing parameters.
**
** \par Assumptions, External Events, and Notes:
** After calling this function, the new settings will be applied in the 
** next call to the Activate Memory Scrubbing function.
** If newEndAddr is set to a value larger than the actual physical memory limit,
** the function will use the physical memory limit.
** 
** \param[in] pNewConfiguration - pointer to a Mem Scrub Configuration structure with new values
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_MemScrubSet(CFE_PSP_MemScrubStatus_t *pNewConfiguration);

/**
** \func Check if the Memory Scrubbing is running
** 
** \par Description:
** This function provides the status whether the Memory Scrubbing  is running.
**
** \par Assumptions, External Events, and Notes:
** This applies only for Idle and Timed Mode.
** See #uiRunMode for more details
**
** \param None
**
** \return true - If Memory Scrubbing is running
** \return false - If Memory Scrubbing is not running
*/
bool  CFE_PSP_MemScrubIsRunning(void);

/**
** \func Stop the memory scrubbing
**
** \par Description:
** This function stops the memory scrubber and reset the configuration.
** You still need to run the #CFE_PSP_MemScrubInit function to fully restart.
** 
** \par Assumptions, External Events, and Notes:
**
** \param - None
**
** \return #CFE_PSP_SUCCESS
*/
int32 CFE_PSP_MemScrubDelete(void);

/**
** \func Get the Memory Scrubbing parameters
** 
** \par Description:
** Copies the content of the configuration structure to pConfig.
** Talkative prints: Run Mode, Start memory address, End memory address
**
** \par Assumptions, External Events, and Notes:
** Start memory address is usually 0. End memory address is usually set to the 
** last value of RAM address.
** 
** \param[out] pConfig - pointer to a Mem Scrub Configuration structure
** \param[in] iConfigSize - Size of the memory array pointed by pConfig
** \param[in] talk - Print out the status values
**
** \return #CFE_PSP_SUCCESS - No errors
** \return #CFE_PSP_ERROR - Size of iStatusSize too small
*/
int32 CFE_PSP_MemScrubGet(CFE_PSP_MemScrubStatus_t *pConfig, size_t iConfigSize, bool talk);

int32 CFE_PSP_MemScrubValidate(CFE_PSP_MemScrubStatus_t *pNewValues);

/**
** \func Initialize the Memory Scrubbing process
**
** \par Description:
** Initialize the memory scrubber register with default values. If automatic
** startup is true, starts memory scrubbing process
**
** \par Assumptions, External Events, and Notes:
** 
**
** \param None
**
** \return #CFE_PSP_SUCCESS - If successful initialization
** \return #CFE_PSP_ERROR - If unsuccessful initialization
*/
int32 CFE_PSP_MemScrubInit(void);

/**
** \func Enable the Memory Scrubbing
**
** \par Description:
** This function enables (starts) Memory Scrubbing
**
** \par Assumptions, External Events, and Notes:
**
** \param None
**
** \return #CFE_PSP_SUCCESS - If successfully disabled memory scrub
** \return #CFE_PSP_ERROR - If unsuccessfully disabled memory scrub
*/
int32  CFE_PSP_MemScrubEnable(void);

/**
** \func Disable the Memory Scrubbing
**
** \par Description:
** This function disables (stops) Memory Scrubbing.
**
** \par Assumptions, External Events, and Notes:
**
** \param None
**
** \return #CFE_PSP_SUCCESS - If successfully disabled memory scrub
** \return #CFE_PSP_ERROR - If unsuccessfully disabled memory scrub
*/
int32  CFE_PSP_MemScrubDisable(void);

/**
 ** \func Get the memory error statistics
 **
 ** \par Description:
 ** This function will fill the provided CFE_PSP_MemScrubErrStats_t pointer with
 ** memory error statistics
 **
 ** \par Assumptions, External Events, and Notes:
 ** TBD what these individual values truly represent
 **
 ** \param pErrStats - Pointer to CFE_PSP_MemScrubErrStats_t structure
 ** \param[in] iErrSize - Size of the memory array pointed by pStatus
 ** \param[in] talk - Print out the statistics values
 **
 ** \return #CFE_PSP_SUCCESS - No errors
 ** \return #CFE_PSP_ERROR - Size of iStatusSize too small
 */
int32 CFE_PSP_MemScrubErrStats(CFE_PSP_MemScrubErrStats_t *pErrStats, size_t iErrSize, bool talkative);

/**
** \} <!-- End of group "psp_public_api_gr740vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_MEM_SCRUB_H_ */
