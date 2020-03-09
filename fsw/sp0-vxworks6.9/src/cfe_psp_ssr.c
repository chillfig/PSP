/******************************************************************************
**
** File:  cfe_psp_ssr.c
**
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** Purpose:
**   This file contains glue routines between the cFE and the OS Board Support 
**   Package (BSP). The functions here allow the cFE to interface functions 
**   that are board and OS specific and usually dont fit well in the OS 
**   abstraction layer.
**
******************************************************************************/

/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vxWorks.h"
#include "sysLib.h"
#include "taskLib.h"
#include "ramDrv.h"
#include "dosFsLib.h"
#include "errnoLib.h"
#include "usrLib.h"
#include "cacheLib.h"
#include "drv/hdisk/ataDrv.h"
#include "cacheLib.h"
#include "xbdBlkDev.h"
#include "xbdRamDisk.h"

#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */
#include "cfe_mission_cfg.h"   /* for spacecraft ID */

#include "cfe_psp.h"
#include "cfe_psp_memory.h"


/******************************************************************************
**  Function:  CFE_PSP_InitSSR
**
**  Purpose:
**    Initializes the Solid State Recorder device
**    For the MCP750, this simply initializes the Hard disk device.
**
**  Arguments:
**    Input - bus - TODO: describe
**    Input - device - TODO: describe
**    Input - deviceName - TODO: describe
**
**  Return:
**    Status of SSR init
******************************************************************************/
int32 CFE_PSP_InitSSR(uint32 bus, uint32 device, char *deviceName )
{
   int32 retCode = CFE_PSP_ERROR;
   
#if 0
   device_t xbd = NULLDEV;

   xbd = ataXbdDevCreate(bus, device, 0, 0, deviceName);
   if (xbd != NULLDEV)
   {
       retCode = CFE_PSP_SUCCESS;
   }
#endif

   return (retCode);
}

