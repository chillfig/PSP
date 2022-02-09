/**
 ** \file cfe_psp_flash.c
 **
 ** \brief cFE PSP Flash related functions
 **
 ** \copyright
 ** Copyright (c) 2019-2022 United States Government as represented by
 ** the Administrator of the National Aeronautics and Space Administration.
 ** All Rights Reserved.
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **
 ** \par Description:
 ** Provide API for interacting with FLASH
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 */

/*
** System Includes
*/
#include <stdio.h>
#include <vxWorks.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

/*
** PSP API Includes
*/
#include "cfe_psp.h"
#include "psp_flash.h"

/*
** Macro Definitions
*/
#define FLASH_PRINT_SCOPE "PSP FLASH: "

/**********************************************************
 * 
 * Function: CFE_PSP_FLASH_ReadFromFLASH
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_FLASH_ReadFromFLASH(uint32 *p_dest, size_t size, char *fname)
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
            OS_printf(FLASH_PRINT_SCOPE "ReadFromFLASH: strerror: %s", strerror(errno));
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
                OS_printf(FLASH_PRINT_SCOPE "ReadFromFLASH: Read %d of %d bytes. strerror: %s \n", 
                                            (int)readBytes, (int)size, strerror(errno));
                iReturnCode = CFE_PSP_ERROR;
            }

            /* Attempt to close file */
            if (close(iFd) != OS_SUCCESS)
            {
                OS_printf(FLASH_PRINT_SCOPE "ReadFromFLASH: Unable to close file\n");
            }
        }
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_FLASH_WriteToFLASH
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_FLASH_WriteToFLASH(uint32 *p_src, size_t size, char *fname)
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
        iFd = open(fname, O_WRONLY, 0);
        if (iFd < 0)
        {
            OS_printf(FLASH_PRINT_SCOPE "WriteToFLASH: Failed to open file\n");
            OS_printf(FLASH_PRINT_SCOPE "WriteToFLASH: strerror: %s", strerror(errno));
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
                OS_printf(FLASH_PRINT_SCOPE "WriteToFLASH: Wrote %lu of %lu bytes. strerror: %s\n", 
                                            wroteBytes, size, strerror(errno));
                iReturnCode = CFE_PSP_ERROR;
            }

            /* Attempt to close file */
            if (close(iFd) != OS_SUCCESS)
            {
                OS_printf(FLASH_PRINT_SCOPE "WriteToFLASH: Unable to close file\n");
                /* Not necessarily a write error here */
            }
        }
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_FLASH_DeleteFile
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_FLASH_DeleteFile(char *fname)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    if (fname == NULL)
    {
        OS_printf(FLASH_PRINT_SCOPE "DeleteFile: NULL filename\n");
        iReturnCode = CFE_PSP_INVALID_POINTER;
    }
    else
    {
        if (CFE_PSP_FLASH_CheckFile(fname) == true)
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
 * Function: CFE_PSP_FLASH_CreateFile
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_FLASH_CreateFile(char *fname)
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
        if (CFE_PSP_FLASH_CheckFile(fname) == false)
        {
            /* File does not exist */
            iFd = creat(fname, 0);

            if (iFd < 0)
            {
                iReturnCode = CFE_PSP_ERROR;
                OS_printf(FLASH_PRINT_SCOPE "CreateFile: Failed to create file\n");
                OS_printf(FLASH_PRINT_SCOPE "CreateFile: strerror: %s", strerror(errno));
            }
            else
            {
                if (close(iFd) != OS_SUCCESS)
                {
                    OS_printf(FLASH_PRINT_SCOPE "CreateFile: Failed to close file\n");
                }
            }
        }
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_FLASH_CheckFile
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
bool CFE_PSP_FLASH_CheckFile(char *fname)
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