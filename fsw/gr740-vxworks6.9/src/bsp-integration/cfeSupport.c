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

/*
** cFE Support routines
**
** This module is a collection of support routines needed to run the cFE on vxWorks.
**
*/

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
