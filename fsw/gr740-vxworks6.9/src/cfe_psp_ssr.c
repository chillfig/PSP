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
*   \file cfe_psp_ssr.c
*
*   \brief Implementation for SSR on GR740
*
*   \brief This file contains glue routines between the cFE and the OS Board Support Package (BSP).
*   The functions here allow the cFE to interface functions that are board and OS specific and usually 
*   do not fit well in the OS abstraction layer.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vxWorks.h>

/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h"
#include "cfe_psp_memory.h"


#define  BLOCKSIZE         512
#define  DISKSIZE          (BLOCKSIZE * 2000)


/**********************************************************
 * 
 * Function: CFE_PSP_InitSSR
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_InitSSR(uint32 bus, uint32 device, char *DeviceName )
{
   int32     iReturnCode = CFE_PSP_ERROR;

#ifdef USE_VXWORKS_ATA_DRIVER
   if (DeviceName != NULL)
   {
      device_t  xbd = NULLDEV;
      STATUS error = OK;
      xbd = xbdRamDiskDevCreate (BLOCKSIZE, DISKSIZE, 0, DeviceName);
      if (xbd == NULLDEV)
      {
         iReturnCode = CFE_PSP_ERROR;
      }
      else
      {
         /* 
         ** Format the RAM disk for HRFS. Allow for upto a 1000 files/directories 
         ** and let HRFS determine the logical block size.
         */

         error = hrfsFormat(DeviceName, 0ll, 0, 1000);
         if (error != OK)
         {
            iReturnCode = CFE_PSP_ERROR;
         }
         else
         {
            iReturnCode = CFE_PSP_SUCCESS;
         }
      }
   }
#else
   iReturnCode = CFE_PSP_ERROR_NOT_IMPLEMENTED;
#endif

   return(iReturnCode);
   
}

