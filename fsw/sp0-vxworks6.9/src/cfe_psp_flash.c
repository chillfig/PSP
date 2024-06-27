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
*   \file cfe_psp_flash.c
*
*   \brief Implementation of PSP flash-related functions
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

/*
** System Includes
*/
#include <stdio.h>
#include <vxWorks.h>
#include <unistd.h>
#include <ioLib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

/*
** PSP API Includes
*/
#include "cfe_psp.h"
#include "cfe_psp_flash.h"

/*
** Macro Definitions
*/
/** \brief Define PRINT SCOPES */
#define FLASH_PRINT_SCOPE "PSP FLASH: "

/**********************************************************
 * 
 * Function: CFE_PSP_ReadFromFlash
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_ReadFromFlash(uint32 *p_dest, size_t size, char *fname)
{
    int32 iReturnCode   = CFE_PSP_SUCCESS;
    int32 iFd           = 0;
    ssize_t readBytes   = 0;

    /* Check pointers */
    if ((p_dest == NULL) || (fname == NULL))
    {
        OS_printf(FLASH_PRINT_SCOPE "ReadFromFLASH: NULL pointer(s)\n");
        iReturnCode = CFE_PSP_INVALID_POINTER;
    }
    else
    {
        /* Attempt to open file */
        iFd = open(fname, O_RDONLY, 0);
        if (iFd < 0)
        {
            OS_printf(FLASH_PRINT_SCOPE "ReadFromFLASH: Failed to open file\n");
            OS_printf(FLASH_PRINT_SCOPE "FILE: <%s>\n", fname);
            OS_printf(FLASH_PRINT_SCOPE "ReadFromFLASH: strerror: %s\n", strerror(errno));
            iReturnCode = CFE_PSP_ERROR;
        }
        else
        {
            /* Read from file */
            readBytes = read(iFd, (void *)p_dest, size);

            /* Check read amount */
            if (readBytes != size)
            {
                OS_printf(FLASH_PRINT_SCOPE "ReadFromFLASH: Read incorrect amount of data\n");
                OS_printf(FLASH_PRINT_SCOPE "FILE: <%s>\n", fname);
                OS_printf(FLASH_PRINT_SCOPE "ReadFromFLASH: Read %d of %d bytes. strerror: %s \n", 
                                            (int32)readBytes, (int32)size, strerror(errno));
                iReturnCode = CFE_PSP_ERROR;
            }

            /* Attempt to close file */
            if (close(iFd) != OS_SUCCESS)
            {
                OS_printf(FLASH_PRINT_SCOPE "ReadFromFLASH: Unable to close file\n");
                OS_printf(FLASH_PRINT_SCOPE "FILE: <%s>\n", fname);
            }
        }
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_WriteToFlash
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_WriteToFlash(uint32 *p_src, size_t size, char *fname)
{
    int32 iReturnCode   = CFE_PSP_SUCCESS;
    int32 iFd           = 0;
    ssize_t wroteBytes  = 0;

    /* Check pointers */
    if ((p_src == NULL) || (fname == NULL))
    {
        OS_printf(FLASH_PRINT_SCOPE "WriteToFLASH: NULL pointer(s)\n");
        iReturnCode = CFE_PSP_INVALID_POINTER;
    }
    else
    {
        /* Attempt to open file */
        iFd = open(fname, O_WRONLY | O_CREAT, 0);
        if (iFd < 0)
        {
            OS_printf(FLASH_PRINT_SCOPE "WriteToFLASH: Failed to open file\n");
            OS_printf(FLASH_PRINT_SCOPE "FILE: <%s>\n", fname);
            OS_printf(FLASH_PRINT_SCOPE "WriteToFLASH: strerror: %s\n", strerror(errno));
            iReturnCode = CFE_PSP_ERROR;
        }
        else
        {
            /* Write to file */
            wroteBytes = write(iFd, (void *)p_src, size);

            /* Check write amount */
            if (wroteBytes != size)
            {
                OS_printf(FLASH_PRINT_SCOPE "WriteToFLASH: Wrote incorrect amount of data\n");
                OS_printf(FLASH_PRINT_SCOPE "FILE: <%s>\n", fname);
                OS_printf(FLASH_PRINT_SCOPE "WriteToFLASH: Wrote %ld of %lu bytes. strerror: %s\n", 
                                            (long)wroteBytes, (unsigned long)size, strerror(errno));
                iReturnCode = CFE_PSP_ERROR;
            }

            /* Attempt to close file */
            if (close(iFd) != OS_SUCCESS)
            {
                OS_printf(FLASH_PRINT_SCOPE "WriteToFLASH: Unable to close file\n");
                OS_printf(FLASH_PRINT_SCOPE "FILE: <%s>\n", fname);
                /* Not necessarily a write error here */
            }
        }
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_DeleteFile
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_DeleteFile(char *fname)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    if (fname == NULL)
    {
        OS_printf(FLASH_PRINT_SCOPE "DeleteFile: NULL filename\n");
        iReturnCode = CFE_PSP_INVALID_POINTER;
    }
    else
    {
        if (CFE_PSP_CheckFile(fname) == true)
        {
            if (remove(fname) != OS_SUCCESS)
            {
                OS_printf(FLASH_PRINT_SCOPE "DeleteFile: Failed to remove file\n");
                iReturnCode = CFE_PSP_ERROR;
            }
        }
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_CreateFile
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_CreateFile(char *fname)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;
    int32 iFd = -1;

    if (fname == NULL)
    {
        OS_printf(FLASH_PRINT_SCOPE "CreateFile: NULL filename\n");
        iReturnCode = CFE_PSP_INVALID_POINTER;
    }
    else
    {
        if (CFE_PSP_CheckFile(fname) == false)
        {
            /* File does not exist */
            iFd = creat(fname, 0);

            if (iFd < 0)
            {
                iReturnCode = CFE_PSP_ERROR;
                OS_printf(FLASH_PRINT_SCOPE "CreateFile: Failed to create file\n");
                OS_printf(FLASH_PRINT_SCOPE "FILE: <%s>\n", fname);
                OS_printf(FLASH_PRINT_SCOPE "CreateFile: strerror: %s\n", strerror(errno));
            }
            else
            {
                if (close(iFd) != OS_SUCCESS)
                {
                    OS_printf(FLASH_PRINT_SCOPE "CreateFile: Failed to close file\n");
                    OS_printf(FLASH_PRINT_SCOPE "FILE: <%s>\n", fname);
                }
            }
        }
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_CheckFile
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
bool CFE_PSP_CheckFile(char *fname)
{
    bool bReturnValue = false;
    struct stat buf = {};

    if (fname == NULL)
    {
        OS_printf(FLASH_PRINT_SCOPE "CheckFile: NULL filename\n");
        bReturnValue = false;
    }
    else
    {
        /* Check if file exists */
        if (stat(fname, &buf) == 0)
        {
            bReturnValue = true;
        }
    }

    return bReturnValue;
}

/**********************************************************
 * 
 * Function: CFE_PSP_CreateDirectory
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_CreateDirectory(const char *p_dir)
{
    int32 iStatus = 0;
    int32 iReturnCode = CFE_PSP_SUCCESS;

    if (p_dir == NULL)
    {
        iReturnCode = CFE_PSP_INVALID_POINTER;
        OS_printf(FLASH_PRINT_SCOPE "CreateDirectory: Invalid Pointer\n");
    }
    else
    {
        iStatus = mkdir(p_dir);

        if (iStatus < 0)
        {
            /*
            ** Only report an error if our errno is something other
            ** than errno 17 "File exists"
            */
            if (errno != 17)
            {
                iReturnCode = CFE_PSP_ERROR;
                OS_printf(FLASH_PRINT_SCOPE "CreateDirectory: Failed to create <%s> directory\n", p_dir);
                OS_printf(FLASH_PRINT_SCOPE "CreateDirectory: STATUS: %d\n", iStatus);
                OS_printf(FLASH_PRINT_SCOPE "CreateDirectory: ERRNO: %d, STRERROR: %s\n", errno, strerror(errno));
            }
        }
    }

    return iReturnCode;
}
