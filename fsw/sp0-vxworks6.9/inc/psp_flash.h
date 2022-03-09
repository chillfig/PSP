/*
** \file  psp_flash.h
**
** \brief API header to control FLASH interaction
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
** This file contains the function prototypes relating to FLASH interactions.
**
** \par Limitations, Assumptions, External Events, and Notes:
** None
*/

#ifndef _PSP_FLASH_H_
#define _PSP_FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_public_api_sp0vx69 PSP Public APIs - SP0-VxWorks6.9 Platform
** \{
*/

/*
** \func Read data from flash
**
** \par Description:
** Read a specific amount of data from flash
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] p_dest - destination buffer
** \param[in] size - Amount of data to read
** \param[in] fname - file name
**
** \return #CFE_PSP_INVALID_POINTER
** \return #CFE_PSP_ERROR
** \return #CFE_PSP_SUCCESS
*/
int32 CFE_PSP_FLASH_ReadFromFLASH(uint32 *p_dest, size_t size, char *fname);

/*
** \func  Write data to flash
**
** \par Description:
** Write a specific amount of data to flash
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] p_src - Data source
** \param[in] size - Amount of data to write
** \param[in] fname - file name
**
** \return #CFE_PSP_INVALID_POINTER
** \return #CFE_PSP_ERROR
** \return #CFE_PSP_SUCCESS
*/
int32 CFE_PSP_FLASH_WriteToFLASH(uint32 *p_src, size_t size, char *fname);

/*
** \func Delete a file
**
** \par Description:
** Delete a file
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] fname - file name
**
** \return #CFE_PSP_INVALID_POINTER
** \return #CFE_PSP_ERROR
** \return #CFE_PSP_SUCCESS
*/
int32 CFE_PSP_FLASH_DeleteFile(char *fname);

/*
** \func Create a file
**
** \par Description:
** Create a file
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] fname - file name
**
** \return #CFE_PSP_INVALID_POINTER
** \return #CFE_PSP_ERROR
** \return #CFE_PSP_SUCCESS
*/
int32 CFE_PSP_FLASH_CreateFile(char *fname);

/*
** \func Check if a file exists
**
** \par Description:
** Check if a file exists
**
** \par Assumptions, External Events, and Notes:
** Will only check if file exists. Will not send file
** send/fill any file information
**
** \param[in] fname - file name
**
** \return false - file does not exist or invalid pointer
** \return true - file does exist
*/
bool CFE_PSP_FLASH_CheckFile(char *fname);

/**
** \} <!-- End of group "psp_public_api_sp0vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_FLASH_H_ */