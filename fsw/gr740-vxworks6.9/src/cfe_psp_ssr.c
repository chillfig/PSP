/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/******************************************************************************
** File:  cfe_psp_ssr.c
**
** Purpose:
**   This file contains glue routines between the cFE and the OS Board Support Package ( BSP ).
**   The functions here allow the cFE to interface functions that are board and OS specific
**   and usually dont fit well in the OS abstraction layer.
**
** History:
**   2005/06/05  Alan Cudmore    | Initial version,
**   2020/08/17  J. Pham         | Copied in UT699 implementation for GR740
**
******************************************************************************/


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

