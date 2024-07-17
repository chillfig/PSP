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
*   \file cfe_psp_timer.h
*
*   \brief Main PSP configuration file for GR740
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
