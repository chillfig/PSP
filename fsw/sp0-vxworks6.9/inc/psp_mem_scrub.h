/*******************************************************************************
**
**  \file psp_mem_scrub.h
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

#ifndef _PSP_MEM_SCRUB_H_
#define _PSP_MEM_SCRUB_H_

/**
 * @brief Set the Memory Scrubbing parameters
 * 
 * NOTE: After calling this function, the new settings will be applied in the 
 *       next call to the Activate Memory Scrubbing funtion.
 * 
 * @param[in] newStartAddr Address to start from, usually zero.
 * @param[in] newEndAddr Address to end to, usually end of physical RAM. If this is 
 *            set to a value larger than the actual physical memory limit, the 
 *            function will use the phyisical memory limit.
 * @param[in] memScrubTask_Priority task priority. Priority can only be set between
 *            MEMSCRUB_PRIORITY_UP_RANGE and MEMSCRUB_PRIORITY_DOWN_RANGE.
 *            Default is set by MEMSCRUB_DEFAULT_PRIORITY.
 */
void CFE_PSP_MEM_SCRUB_Set(uint32 newStartAddr, uint32 newEndAddr, osal_priority_t task_priority);

/**
 * @brief Report if the Memory Scrubbing Task is running
 * 
 * @return true if task is running
 * @return false if task not running
 */
bool CFE_PSP_MEM_SCRUB_isRunning(void);

/**
 * @brief Function prints the status of Active Memory Scrubbing
 * 
 */
void CFE_PSP_MEM_SCRUB_Status(void);

/**
 * @brief Initialize the memory scrubbing task
 * 
 */
void CFE_PSP_MEM_SCRUB_Init(void);

/**
 * @brief Starts the Active Memory Scrubbing task
 * 
 */
void CFE_PSP_MEM_SCRUB_Enable(void);

/**
 * @brief Stops the Active Memory Scrubbing task
 * 
 */
void CFE_PSP_MEM_SCRUB_Disable(void);

#endif /* _PSP_MEM_SCRUB_H_ */