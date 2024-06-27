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
*   \file cfeSupport.c
*
*   \brief cFE Support Routines
*
*   \brief This module is a collection of support routines needed to run the cFE on vxWorks.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "vxWorks.h"
#include "taskLib.h"
#include "sysLib.h"
#include "symLib.h"
#include "loadLib.h"
#include "ioLib.h"
#include "dosFsLib.h"
#include "xbdBlkDev.h"
#include "errnoLib.h"
#include "usrLib.h"
#include "cacheLib.h"
#include "drv/hdisk/ataDrv.h"
#include "stdint.h"

/*
** External reference to wrs_kernel_text_start and wrs_kernel_text_end
** The cFE core needs these symbols, but when the cfe-core.o is loaded as a
** dynamic module, these symbols are not in the symbol table.
*/

extern char wrs_kernel_text_start[];
extern char wrs_kernel_text_end[];

/*
** External reference to the vxWorks symbol table
*/
extern SYMTAB_ID sysSymTbl;



/*
** Function: GetWrsKernelTextStart
** Purpose:  This function returns the start address of the kernel code.
**
*/
unsigned int GetWrsKernelTextStart(void)
{
    return (unsigned int)&wrs_kernel_text_start;
}

/*
** Function: GetWrsKernelTextEnd
** Purpose:  This function returns the end address of the kernel code.
**
*/
unsigned int GetWrsKernelTextEnd(void)
{
    return (unsigned int)&wrs_kernel_text_end;
}


void startCfeCore(void)
{
    int    fd;
    STATUS status;
    fd = open("/romfs/core-vic.exe", O_RDONLY, 0);

    if (fd < 0)
    {
        return;
    }

    loadModule(fd, LOAD_ALL_SYMBOLS);
    close(fd);

    SYMBOL_DESC symDesc0 = {SYM_FIND_BY_NAME, "OS_BSPMain"};

    status = symFind(sysSymTbl, &symDesc0);
    if (status == OK)
    {
        void (*cFEFuncPtr)(void) = (void *)symDesc0.value;
        cFEFuncPtr();
    }
    else
    {
        logMsg("Can't start CFE!\n", 1, 2, 3, 4, 5, 6);
    }
}

uint32_t CFE_SUPPORT_GetProcessorIdx(void)
{
    extern uint32_t ngIniFpgaGetCardId(void); // from ngIniFpgaAppHdr.h
    return ngIniFpgaGetCardId() & 0x1;
}
