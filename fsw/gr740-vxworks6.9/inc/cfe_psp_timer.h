/*********************************************************************************************************/
/** \export_control EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
*   This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
*   15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government
*   and are authorized for export by NASA only to fulfill responsibilities of the parties or of a
*   Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using
*   License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and
*   Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
*   or to any person other than the authorized ultimate consignee or end-user(s), either in their
*   original form or after being incorporated into other items, without first obtaining approval from
*   the U.S. government or as otherwise authorized by U.S. law and regulations.
*
*   \file cfe_psp_timer.h
*
*   \brief Main PSP configuration file for GRP740
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef CFE_PSP_TIMER_H
#define CFE_PSP_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_gr740vx69_platcfg PSP Platform Configurations - GR740-VxWorks6.9
** \{
*/

/**
 ** \brief Default Timer pre-print string 
 ** \par Description:
 ** This string is printed before every print related to Timer API.
*/
#define TIMER_PRINT_SCOPE       "PSP Timer: "

/**
 ** \brief Initializes the vxWorks Aux and TimeStamp timers
 **
 ** \par Description:
 ** Function sets up a 64 bit timer. The timer value will be used for the 
 ** function #vxTimeBaseGet.
 **  
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return None
 */
void CFE_PSP_InitLocalTime(void);

/**
 ** \brief Reads value of VxWorks timer
 **
 ** \par Description:
 ** Function replicates the behaviour of the VxWorks function vxTimeBaseGet,
 ** which is missing from the GR740 BSP.
 **  
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 ** \param Tbu - Upper part of the 64 bit timer 
 ** \param Tbl - Lower part of the 64 bit timer
 **
 ** \return None
 */
void vxTimeBaseGet(uint32 *Tbu, uint32 *Tbl);

/**
** \} <!-- End of group "psp_gr740vx69_platcfg" -->
*/

#ifdef __cplusplus
}
#endif

#endif  /* _CFE_PSP_TIMER_ */
