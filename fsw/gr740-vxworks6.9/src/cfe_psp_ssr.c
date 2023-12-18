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

